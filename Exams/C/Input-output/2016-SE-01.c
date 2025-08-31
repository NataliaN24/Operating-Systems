Напишете програма на C, която приема параметър - име на (двоичен) файл с байтове.
Програмата трябва да сортира файла.

#include "stdio.h"
#include "stdint.h"
#include "fcntl.h"
#include "unistd.h"
#include "err.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        errx(1, "Invalid number of arguments! Usage: %s <file_name>", argv[0]);
    }

    const char *file_name = argv[1];
    int fd;

    if ((fd = open(file_name, O_RDWR)) < 0) {
        err(2, "Cannot open file %s", file_name);
    }

    // Масив за броене на всички възможни байтове (0-255)
    uint32_t counts[256] = {0};

    uint8_t buf;
    ssize_t bytes_read;

    // Четене и броене на всеки байт
    while ((bytes_read = read(fd, &buf, sizeof(buf))) > 0) {
        counts[buf]++;
    }

    if (bytes_read < 0) {
        err(3, "Cannot read from file %s", file_name);
    }

    // Връщане в началото на файла
    if (lseek(fd, 0, SEEK_SET) < 0) {
        err(4, "Cannot lseek file %s", file_name);
    }

    // Презаписване на файла със сортирани байтове
    for (int i = 0; i < 256; i++) {
        uint8_t byte_val = i;
        for (uint32_t j = 0; j < counts[i]; j++) {
            if (write(fd, &byte_val, sizeof(byte_val)) != sizeof(byte_val)) {
                err(5, "Cannot write to file %s", file_name);
            }
        }
    }

    close(fd);
    return 0;
}

