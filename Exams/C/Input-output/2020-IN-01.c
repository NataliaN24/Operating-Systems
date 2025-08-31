//Напишете програма на C, която приема три параметъра – имена на двоични файлове.
//Примерно извикване:
//$ ./main patch.bin f1.bin f2.bin
//        Файловете patch.bin и f1.bin съществуват, и на тяхна база програмата трябва да създаде f2.bin.
//Файлът patch.bin се състои от две секции – 16 байтов хедър и данни. На базата на хедъра програмата трябва да може да интерпретира съдържанието на файла. Структурата на хедъра е:
//• uint32_t, magic – магическа стойност 0xEFBEADDE, която дефинира, че файлът следва тази
//        спецификация
//• uint8_t, header version – версия на хедъра, с единствена допустима стойност за момента 0x01,
//която дефинира останалите байтове от хедъра както следва:
//– uint8_t, data version – версия (описание) на използваните структури в секцията за данни
//        на файла
//– uint16_t, count – брой записи в секцията за данни
//– uint32_t, reserved 1 – не се използва
//– uint32_t, reserved 2 – не се използва
//        Възможни структури в секцията за данни на файла спрямо data version:
//• при версия 0x00
//– uint16_t, offset
//– uint8_t, original byte
//– uint8_t, new byte
//• при версия 0x01
//– uint32_t, offset
//– uint16_t, original word
//– uint16_t, new word
//• забележка: и при двете описани версии offset е отместване в брой елементи спрямо началото
//        на файла
//        Двоичните файлове f1.bin и f2.bin се третират като състоящи се от елементи спрямо data version
//        в patch.bin.
//Програмата да създава файла f2.bin като копие на файла f1.bin, но с отразени промени на базата
//на файла patch.bin, при следния алгоритъм:
//• за всяка наредена тройка от секцията за данни на patch.bin, ако на съответният offset в
//        оригиналния файл f1.bin е записан елементът original byte/word, в изходният файл се записва new byte/word. Ако не е записан такъв елемент или той не съществува, програмата да
//        прекратява изпълнението си по подходящ начин;
//• всички останали елементи се копират директно.
//Наредените тройки в секцията за данни на файла patch.bin да се обработват последователно.
//Обърнете внимание на обработката за грешки и съобщенията към потребителя – искаме програмата
//да бъде удобен и валиден инструмент.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

#define PATCH_MAGIC 0xEFBEADDE

int main(int argc, char **argv) {
    if (argc != 4) {
        errx(1, "Usage: %s patch.bin f1.bin f2.bin", argv[0]);
    }

    const char *patch_file = argv[1];
    const char *f1_file = argv[2];
    const char *f2_file = argv[3];

    int fd_patch = open(patch_file, O_RDONLY);
    if (fd_patch < 0) err(2, "Cannot open %s", patch_file);

    // Четем хедъра (16 байта)
    uint32_t magic;
    uint8_t header_ver;
    uint8_t data_ver;
    uint16_t count;
    uint32_t reserved1, reserved2;

    if (read(fd_patch, &magic, 4) != 4) err(3, "Failed to read magic");
    if (magic != PATCH_MAGIC) errx(4, "Invalid magic in patch file");

    if (read(fd_patch, &header_ver, 1) != 1) err(5, "Failed to read header version");
    if (header_ver != 0x01) errx(6, "Unsupported header version");

    if (read(fd_patch, &data_ver, 1) != 1) err(7, "Failed to read data version");
    if (read(fd_patch, &count, 2) != 2) err(8, "Failed to read count");
    if (read(fd_patch, &reserved1, 4) != 4) err(9, "Failed to read reserved1");
    if (read(fd_patch, &reserved2, 4) != 4) err(10, "Failed to read reserved2");

    // Отваряме f1 и f2
    int fd1 = open(f1_file, O_RDONLY);
    if (fd1 < 0) err(11, "Cannot open %s", f1_file);
    int fd2 = open(f2_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd2 < 0) err(12, "Cannot create %s", f2_file);

    // Копираме f1 -> f2
    char buf[4096];
    ssize_t r;
    while ((r = read(fd1, buf, sizeof(buf))) > 0) {
        if (write(fd2, buf, r) != r) err(13, "Write error");
    }
    if (r < 0) err(14, "Read error during copy");

    // Сега прилагаме пача
    off_t fsize;
    if ((fsize = lseek(fd2, 0, SEEK_END)) < 0) err(15, "lseek failed on f2");
    lseek(fd2, 0, SEEK_SET); // за да можем да пишем по offset

    for (uint16_t i = 0; i < count; i++) {
        if (data_ver == 0x00) {
            uint16_t offset;
            uint8_t original, newbyte;
            if (read(fd_patch, &offset, 2) != 2) err(16, "Read patch data failed");
            if (read(fd_patch, &original, 1) != 1) err(17, "Read patch data failed");
            if (read(fd_patch, &newbyte, 1) != 1) err(18, "Read patch data failed");

            off_t pos = (off_t)offset;
            if (pos >= fsize) errx(19, "Offset out of range");

            uint8_t cur;
            if (lseek(fd2, pos, SEEK_SET) < 0) err(20, "lseek failed");
            if (read(fd2, &cur, 1) != 1) err(21, "Read f2 failed");
            if (cur != original) errx(22, "Original byte mismatch at offset %u", offset);

            if (lseek(fd2, pos, SEEK_SET) < 0) err(23, "lseek failed");
            if (write(fd2, &newbyte, 1) != 1) err(24, "Write patch failed");
        } else if (data_ver == 0x01) {
            uint32_t offset;
            uint16_t original, newword;
            if (read(fd_patch, &offset, 4) != 4) err(25, "Read patch data failed");
            if (read(fd_patch, &original, 2) != 2) err(26, "Read patch data failed");
            if (read(fd_patch, &newword, 2) != 2) err(27, "Read patch data failed");

            off_t pos = (off_t)offset * 2; // защото е word
            if (pos + 1 >= fsize) errx(28, "Offset out of range");

            uint16_t cur;
            if (lseek(fd2, pos, SEEK_SET) < 0) err(29, "lseek failed");
            if (read(fd2, &cur, 2) != 2) err(30, "Read f2 failed");
            if (cur != original) errx(31, "Original word mismatch at offset %u", offset);

            if (lseek(fd2, pos, SEEK_SET) < 0) err(32, "lseek failed");
            if (write(fd2, &newword, 2) != 2) err(33, "Write patch failed");
        } else {
            errx(34, "Unsupported data version");
        }
    }

    close(fd_patch);
    close(fd1);
    close(fd2);

    return 0;
}
