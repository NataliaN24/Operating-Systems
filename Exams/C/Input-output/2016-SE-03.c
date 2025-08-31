//Напишете програма на C приемаща параметър – име на (двоичен) файл с uint32_t
//  числа. Програмата трябва да сортира файла. Ограничения:
//• Числата биха могли да са максимум 100 000 000 на брой.
//• Програмата трябва да работи на машина със същия endianness, както машината, която е
//        създала файла.
//• Програмата трябва да работи на машина с 256 MB RAM и 8 GB свободно дисково пространство.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

#define BLOCK_SIZE 25000000 // ~100 MB, uint32_t = 4 B

int compare_uint32(const void *a, const void *b) {
    uint32_t ua = *(uint32_t*)a;
    uint32_t ub = *(uint32_t*)b;
    return (ua > ub) - (ua < ub);
}

int main(int argc, char **argv) {
    if (argc != 2) errx(1, "Usage: %s <file>", argv[0]);
    const char *filename = argv[1];

    int fd = open(filename, O_RDWR);
    if (fd < 0) err(2, "Cannot open file %s", filename);

    // Определяне на размера на файла
    off_t filesize = lseek(fd, 0, SEEK_END);
    if (filesize < 0) err(3, "lseek failed");
    lseek(fd, 0, SEEK_SET);

    uint32_t *buffer = malloc(BLOCK_SIZE * sizeof(uint32_t));
    if (!buffer) err(4, "Memory allocation failed");

    size_t read_count;
    off_t offset = 0;
    while ((read_count = read(fd, buffer, BLOCK_SIZE * sizeof(uint32_t))) > 0) {
        size_t num = read_count / sizeof(uint32_t);
        qsort(buffer, num, sizeof(uint32_t), compare_uint32);

        // Връщаме се в началото на блока
        if (lseek(fd, offset, SEEK_SET) < 0) err(5, "lseek failed");
        if (write(fd, buffer, num * sizeof(uint32_t)) != num * sizeof(uint32_t))
            err(6, "write failed");

        offset += num * sizeof(uint32_t);
        lseek(fd, offset, SEEK_SET);
    }

    free(buffer);
    close(fd);
    return 0;
}
