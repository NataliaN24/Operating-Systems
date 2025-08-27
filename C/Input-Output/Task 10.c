//Модифицирайте предната програма, така че да намерите най-големия байт.
//to be reviewd

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <string.h>

/* Helper function to print an unsigned byte as characters */
void write_number(unsigned char num) {
    char buf[4]; // max byte 255 -> 3 digits + newline
    int i = 3;
    buf[i--] = '\n';

    if (num == 0) {
        buf[i] = '0';
        write(1, &buf[i], 2); // '0' + '\n'
        return;
    }

    while (num > 0) {
        buf[i--] = (num % 10) + '0';
        num /= 10;
    }

    write(1, &buf[i+1], 3 - i);
}

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

    unsigned char byte;
    ssize_t bytes;
    int first = 1;
    unsigned char min_val = 0, max_val = 0;

    /* Read file byte by byte */
    while ((bytes = read(fd, &byte, sizeof(byte))) == sizeof(byte)) {
        if (first) {
            min_val = max_val = byte;
            first = 0;
        } else {
            if (byte > max_val) max_val = byte;
            if (byte < min_val) min_val = byte;
        }

        if (strcmp(option, "--print") == 0) {
            write_number(byte);
        }
    }

    if (bytes == -1) {
        close(fd);
        err(3, "Error reading from %s", filename);
    }

    close(fd);

    if (strcmp(option, "--min") == 0) {
        write_number(min_val);
    } else if (strcmp(option, "--max") == 0) {
        write_number(max_val);
    }

    return 0;
}
