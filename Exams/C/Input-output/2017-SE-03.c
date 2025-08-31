//Напишете програма на C, която приема три параметъра, имена на двоични файлове.
//Примерно извикване:
//$ ./main patch.bin f1.bin f2.bin
//        Файловете f1.bin и f2.bin се третират като двоични файлове, състоящи се от байтове (uint8_t).
//Файлът patch.bin е двоичен файл, състоящ се от наредени тройки от следните елементи (и техните
//        типове):
//• отместване uint16_t
//• оригинален байт uint8_t
//• нов байт uint8_t
//        Програмата да създава файла f2.bin като копие на файла f1.bin, но с отразени промени на базата
//на файла patch.bin, при следния алгоритъм:
//• за всяка наредена тройка от patch.bin, ако на съответното отместване (в байтове) спрямо
//        началото на файла е записан байта оригинален байт, в изходния файл се записва нов байт.
//Ако не е записан такъв оригинален байт или такова отместване не съществува, програмата
//да прекратява изпълнението си по подходящ начин;
//• всички останали байтове се копират директно.
//Забележка: Наредените тройки във файла patch.bin да се обработват последователно.
//Примерен f1.bin:
//00000000: f5c4 b159 cc80 e2ef c1c7 c99a 2fb0 0d8c ...Y......../...
//00000010: 3c83 6fed 6b46 09d2 90df cf1e 9a3c 1f05 <.o.kF.......<..
//00000020: 05f9 4c29 fd58 a5f1 cb7b c9d0 b234 2398 ..L).X...{...4#.
//00000030: 35af 6be6 5a71 b23a 0e8d 08de def2 214c 5.k.Zq.:......!L
//        Примерен patch.bin:
//00000000: 0200 b159 3000 35af ...Y0.5.
//Примерен f2.bin:
//00000000: f5c4 5959 cc80 e2ef c1c7 c99a 2fb0 0d8c ..YY......../...
//00000010: 3c83 6fed 6b46 09d2 90df cf1e 9a3c 1f05 <.o.kF.......<..
//00000020: 05f9 4c29 fd58 a5f1 cb7b c9d0 b234 2398 ..L).X...{...4#.
//00000030: afaf 6be6 5a71 b23a 0e8d 08de def2 214c ..k.Zq.:......!L
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

typedef struct {
    uint16_t offset;
    uint8_t orig;
    uint8_t new;
} patch_entry;

int main(int argc, char **argv) {
    if (argc != 4) {
        errx(1, "Usage: %s patch.bin f1.bin f2.bin", argv[0]);
    }

    const char *patch_name = argv[1];
    const char *f1_name = argv[2];
    const char *f2_name = argv[3];

    // Отваряме оригиналния файл
    int fd1 = open(f1_name, O_RDONLY);
    if (fd1 < 0) err(2, "Cannot open %s", f1_name);

    // Определяме размера на f1.bin
    off_t filesize = lseek(fd1, 0, SEEK_END);
    if (filesize < 0) err(3, "lseek failed");
    lseek(fd1, 0, SEEK_SET);

    // Четем f1.bin в памет
    uint8_t *buffer = malloc(filesize);
    if (!buffer) errx(4, "Memory allocation failed");

    if (read(fd1, buffer, filesize) != filesize) err(5, "Read failed");
    close(fd1);

    // Отваряме patch.bin
    int fd_patch = open(patch_name, O_RDONLY);
    if (fd_patch < 0) err(6, "Cannot open %s", patch_name);

    patch_entry entry;
    while (read(fd_patch, &entry.offset, 2) == 2) {
        if (read(fd_patch, &entry.orig, 1) != 1 || read(fd_patch, &entry.new, 1) != 1)
            err(7, "Invalid patch format");

        if (entry.offset >= filesize)
            errx(8, "Patch offset out of range");

        if (buffer[entry.offset] != entry.orig)
            errx(9, "Original byte mismatch at offset %u", entry.offset);

        buffer[entry.offset] = entry.new;
    }
    close(fd_patch);

    // Записваме резултата във f2.bin
    int fd2 = open(f2_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd2 < 0) err(10, "Cannot create %s", f2_name);

    if (write(fd2, buffer, filesize) != filesize) err(11, "Write failed");
    close(fd2);

    free(buffer);
    return 0;
}

