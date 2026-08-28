#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        errx(1, "usage");
    }

    int fdIn = open(argv[1], O_RDONLY);
    if (fdIn < 0) {
        err(1, "open input");
    }

    int fdOut = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fdOut < 0) {
        err(1, "open output");
    }

    uint8_t byte;
    ssize_t readBytes;

    while ((readBytes = read(fdIn, &byte, sizeof(byte))) > 0) {

        uint16_t result = 0;

        for (int bit = 7; bit >= 0; bit--) {

            result <<= 2;

            if (byte & (1 << bit)) {
                result |= 2;   // 10
            }
            else {
                result |= 1;   // 01
            }
        }

        write(fdOut, &result, sizeof(result));
    }

    if (readBytes < 0) {
        err(1, "read");
    }

    close(fdIn);
    close(fdOut);

    exit(0);
}
///////////////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        errx(1, "usage: %s scl sdl", argv[0]);
    }

    int input = open(argv[1], O_RDONLY);
    int output = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (input < 0 || output < 0) {
        err(1, "error");
    }

    uint8_t bits;
    ssize_t bytesRead;

    while ((bytesRead = read(input, &bits, sizeof(bits))) > 0) {
        uint16_t encoded = 0;

        for (int i = 7; i >= 0; i--) {
            uint8_t bit = (bits >> i) & 1;

            encoded <<= 2;

            if (bit == 1) {
                encoded |= 2;   // 10
            } else {
                encoded |= 1;   // 01
            }
        }

        uint8_t out[2];
        out[0] = (encoded >> 8) & 255;
        out[1] = encoded & 255;

        if (write(output, out, 2) != 2) {
            close(input);
            close(output);
            err(1, "Error");
        }
    }

    if (bytesRead < 0) {
        close(input);
        close(output);
        err(1, "Error");
    }

    close(input);
    close(output);
    return 0;
}
