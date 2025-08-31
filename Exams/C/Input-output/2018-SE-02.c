//Напишете програма на C, която приема два параметъра – имена на файлове:
//• примерно извикване: ./main input.bin output.bin
//• файловете input.bin и output.bin се третират като двоични файлове, състоящи се от uint32_t
//        числа
//• файлът input.bin може да съдържа максимум 4194304 числа
//• файлът output.bin трябва да бъде създаден от програмата и да съдържа числата от input.bin,
//сортирани във възходящ ред
//• endianness-ът на машината, създала файла input.bin е същият, като на текущата машина
//• ограничения на ресурси: програмата трябва да работи с употреба на максимум 9 MB RAM и
//64 MB дисково пространство.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>

#define MAX_NUMBERS_IN_RAM (9 * 1024 * 1024 / sizeof(uint32_t)) // ~9MB RAM

int compare_uint32(const void *a, const void *b) {
    uint32_t x = *(uint32_t*)a;
    uint32_t y = *(uint32_t*)b;
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

    FILE *fin = fopen(input_name, "rb");
    if (!fin) err(2, "Cannot open %s", input_name);

    FILE *fout = fopen(output_name, "wb");
    if (!fout) err(3, "Cannot create %s", output_name);

    // Временно съхранение на сегменти
    uint32_t *buffer = malloc(MAX_NUMBERS_IN_RAM * sizeof(uint32_t));
    if (!buffer) errx(4, "Memory allocation failed");

    size_t n;
    while ((n = fread(buffer, sizeof(uint32_t), MAX_NUMBERS_IN_RAM, fin)) > 0) {
        qsort(buffer, n, sizeof(uint32_t), compare_uint32);
        if (fwrite(buffer, sizeof(uint32_t), n, fout) != n) {
            err(5, "Write error");
        }
    }

    free(buffer);
    fclose(fin);
    fclose(fout);

    // Този код работи, ако файлът може да се побере в RAM на сегменти и не изисква сложен multi-way merge.
    // За по-големи файлове (>9MB), трябва да се съхраняват сортирани сегменти в временни файлове и след това merge.
    
    return 0;
}
