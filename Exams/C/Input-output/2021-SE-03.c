//Напишете програма на C, която приема два позиционни параметъра – имена на файлове. Примерно
//        извикване:
//$ ./main input.bin output.h
//        Файлът input.bin е двоичен файл с елементи uint16_t числа, създаден на little-endian машина.
//Вашата програма трябва да генерира C хедър файл, дефиниращ масив с име arr, който:
//• съдържа всички елементи от входния файл;
//• няма указана големина;
//• не позволява промяна на данните.
//Генерираният хедър файл трябва да:
//• съдържа и uint32_t променлива arrN, която указва големината на масива;
//• бъде валиден и да може да се #include-ва без проблеми от C файлове, очакващи да “виждат”
//arr и arrN.
//За да е валиден един входен файл, той трябва да съдържа не повече от 524288 елемента.

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdio.h>

#define MAX_ELEMENTS 524288

int main(int argc, char **argv) {
    if (argc != 3)
        errx(1, "Usage: ./main input.bin output.h");

    const char *in_file = argv[1];
    const char *out_file = argv[2];

    int fd_in = open(in_file, O_RDONLY);
    if (fd_in < 0) err(2, "Cannot open input file");

    int fd_out = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_out < 0) err(3, "Cannot open output file");

    uint16_t buffer[MAX_ELEMENTS];
    ssize_t n_read;
    size_t total_elements = 0;

    // Четене на всички uint16_t числа
    while ((n_read = read(fd_in, &buffer[total_elements], sizeof(uint16_t))) > 0) {
        if (n_read != sizeof(uint16_t))
            errx(4, "Incomplete read");
        total_elements++;
        if (total_elements > MAX_ELEMENTS)
            errx(5, "Input file has too many elements (>524288)");
    }

    if (n_read < 0)
        err(6, "Read error");

    close(fd_in);

    // Записване на хедър файл
    const char *header_start = "#ifndef ARR_H\n#define ARR_H\n\n#include <stdint.h>\n\n";
    if (write(fd_out, header_start, strlen(header_start)) < 0)
        err(7, "Write error");

    // Запис на броя елементи
    char arrN_line[64];
    int len = snprintf(arrN_line, sizeof(arrN_line), "const uint32_t arrN = %zu;\n\n", total_elements);
    if (write(fd_out, arrN_line, len) < 0)
        err(8, "Write error");

    // Запис на масива
    if (write(fd_out, "const uint16_t arr[] = {", 25) < 0)
        err(9, "Write error");

    for (size_t i = 0; i < total_elements; i++) {
        char num_str[16];
        len = snprintf(num_str, sizeof(num_str), "%u", buffer[i]);
        if (write(fd_out, num_str, len) < 0)
            err(10, "Write error");

        if (i != total_elements - 1) {
            if (write(fd_out, ", ", 2) < 0)
                err(11, "Write error");
        }
    }

    if (write(fd_out, "};\n\n#endif\n", 12) < 0)
        err(12, "Write error");

    close(fd_out);
    return 0;
}
