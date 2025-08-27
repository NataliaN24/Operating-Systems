//Напишете програма, която приема точно 2 аргумента. Първият може да бъде само --min, --max или --print
//(вижте man 3 strcmp). Вторият аргумент е двоичен файл, в който има записани цели неотрицателни двубайтови числа
//(uint16_t - вижте man stdint.h). Ако първият аргумент е:
//
//--min - програмата отпечатва кое е най-малкото число в двоичния файл.
//--max - програмата отпечатва кое е най-голямото число в двоичния файл.
//--print - програмата отпечатва на нов ред всяко число.
//Използвайте двоичния файл binary/dump или генерирайте сами такъв (правилно подравнен)
.
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        errx(1, "Usage: %s --min|--max|--print filename", argv[0]);
    }

    const char *option = argv[1];
    const char *filename = argv[2];

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        err(2, "Cannot open %s", filename);
    }

    uint16_t num;
    ssize_t bytes;
    int first = 1;
    uint16_t min_val = 0, max_val = 0;

    while ((bytes = read(fd, &num, sizeof(num))) == sizeof(num)) {
        if (first) {
            min_val = max_val = num;
            first = 0;
        } else {
            if (num < min_val) min_val = num;
            if (num > max_val) max_val = num;
        }

        if (strcmp(option, "--print") == 0) {
            char buf[16];
            int n = snprintf(buf, sizeof(buf), "%u\n", num);
            if (write(1, buf, n) != n) {
                close(fd);
                err(3, "Error writing to stdout");
            }
        }
    }

    if (bytes == -1) {
        close(fd);
        err(4, "Error reading from %s", filename);
    }

    close(fd);

    if (strcmp(option, "--min") == 0) {
        char buf[16];
        int n = snprintf(buf, sizeof(buf), "%u\n", min_val);
        if (write(1, buf, n) != n) err(5, "Error writing min");
    } else if (strcmp(option, "--max") == 0) {
        char buf[16];
        int n = snprintf(buf, sizeof(buf), "%u\n", max_val);
        if (write(1, buf, n) != n) err(6, "Error writing max");
    }

    return 0;
}

