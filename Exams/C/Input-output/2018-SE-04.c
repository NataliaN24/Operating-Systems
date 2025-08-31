//Напишете програма на C, която приема два параметъра – имена на файлове:
//• примерно извикване: ./main input.bin output.bin
//• файловете input.bin и output.bin се третират като двоични файлове, състоящи се от uint16_t
//        числа
//• файлът input.bin може да съдържа максимум 65535 числа
//• файлът output.bin трябва да бъде създаден от програмата и да съдържа числата от input.bin,
//сортирани във възходящ ред
//• endianness-ът на машината, създала файла input.bin е същият, като на текущата машина
//• ограничения на ресурси: програмата трябва да работи с употреба на максимум 256 KB RAM
//и 2 MB дисково пространство.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

int compare_uint16(const void *a, const void *b) {
    uint16_t x = *(uint16_t*)a;
    uint16_t y = *(uint16_t*)b;
    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        errx(1, "Usage: %s input.bin output.bin", argv[0]);
    }

    const char *input_name = argv[1];
    const char *output_name = argv[2];

    int fd_in = open(input_name, O_RDONLY);
    if (fd_in < 0) err(2, "Cannot open %s", input_name);

    int fd_out = open(output_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_out < 0) err(3, "Cannot create %s", output_name);

    // Разбираме размера на файла
    off_t file_size = lseek(fd_in, 0, SEEK_END);
    if (file_size < 0) err(4, "lseek failed");

    if (file_size % sizeof(uint16_t) != 0) {
        errx(5, "Invalid input file size");
    }

    size_t n = file_size / sizeof(uint16_t);
    if (n > 65535) {
        errx(6, "Too many numbers in input file");
    }

    // Връщаме указателя в началото
    if (lseek(fd_in, 0, SEEK_SET) < 0) err(7, "lseek failed");

    uint16_t *numbers = malloc(n * sizeof(uint16_t));
    if (!numbers) errx(8, "Memory allocation failed");

    ssize_t bytes_read = read(fd_in, numbers, n * sizeof(uint16_t));
    if (bytes_read < 0) err(9, "Read error");
    if ((size_t)bytes_read != n * sizeof(uint16_t)) {
        errx(10, "Could not read full input");
    }

    close(fd_in);

    qsort(numbers, n, sizeof(uint16_t), compare_uint16);

    ssize_t bytes_written = write(fd_out, numbers, n * sizeof(uint16_t));
    if (bytes_written < 0) err(11, "Write error");
    if ((size_t)bytes_written != n * sizeof(uint16_t)) {
        errx(12, "Could not write full output");
    }

    close(fd_out);
    free(numbers);

    return 0;
}
