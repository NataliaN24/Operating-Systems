2 Двоичните файлове f1 и f2 съдържат 32 битови числа без знак (uint32_t). Файлът f1 е
съдържа 𝑛 двойки числа, нека 𝑖-тата двойка е < 𝑥𝑖
, 𝑦𝑖 >. Напишете програма на C, която извлича
интервалите с начало 𝑥𝑖 и дължина 𝑦𝑖
от файла f2 и ги записва залепени в изходен файл f3.
Пример:
• f1 съдържа 4 числа (2 двойки): 30000, 20, 19000, 10
• програмата записва в f3 две поредици 32-битови числа, взети от f2 както следва:
• най-напред се записват числата, които са на позиции 30000, 30001, 30002, ... 30019.
• след тях се записват числата от позиции 19000, 19001, ... 19009.
Забележка: С пълен брой точки ще се оценяват решения, които работят със скорост, пропорционална
на размера на изходния файл f3

  #include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        errx(1, "Usage: %s <f1> <f2> <f3>", argv[0]);
    }

    const char *f1_name = argv[1];
    const char *f2_name = argv[2];
    const char *f3_name = argv[3];

    // Отваряме файловете
    int f1 = open(f1_name, O_RDONLY);
    if (f1 < 0) err(2, "Cannot open file %s", f1_name);

    int f2 = open(f2_name, O_RDONLY);
    if (f2 < 0) err(3, "Cannot open file %s", f2_name);

    int f3 = open(f3_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (f3 < 0) err(4, "Cannot create file %s", f3_name);

    uint32_t xi, yi;
    while (read(f1, &xi, sizeof(uint32_t)) == sizeof(uint32_t)) {
        if (read(f1, &yi, sizeof(uint32_t)) != sizeof(uint32_t)) {
            errx(5, "f1 contains incomplete pair");
        }

        // Преместване до позицията xi в f2
        off_t offset = (off_t)xi * sizeof(uint32_t);
        if (lseek(f2, offset, SEEK_SET) < 0) {
            err(6, "lseek failed on f2");
        }

        // Четене и писане на интервала
        uint32_t buffer[1024]; // буфер за по-голяма ефективност
        uint32_t remaining = yi;
        while (remaining > 0) {
            uint32_t chunk = remaining > 1024 ? 1024 : remaining;
            ssize_t n = read(f2, buffer, chunk * sizeof(uint32_t));
            if (n <= 0) err(7, "Error reading from f2");

            ssize_t written = write(f3, buffer, n);
            if (written != n) err(8, "Error writing to f3");

            remaining -= n / sizeof(uint32_t);
        }
    }

    close(f1);
    close(f2);
    close(f3);
    return 0;
}
