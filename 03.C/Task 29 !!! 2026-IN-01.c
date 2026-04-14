#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>

typedef struct {
    uint8_t min;
    uint8_t max;
} Pair;

int main(int argc, char* argv[])
{
    if (argc < 4 || argc > 10) {
        errx(1, "usage: %s output input1 input2 ...", argv[0]);
    }

    int n = argc - 2;   // брой входни файлове

    int out = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) {
        err(2, "open output");
    }

    int fds[8];
    uint32_t counts[8];
    uint32_t maxCount = 0;

    for (int i = 0; i < n; i++) {
        fds[i] = open(argv[i + 2], O_RDONLY);
        if (fds[i] < 0) {
            err(3, "open input");
        }

        off_t size = lseek(fds[i], 0, SEEK_END);
        if (size < 0) {
            err(4, "lseek");
        }

        if (size % sizeof(float) != 0) {
            errx(5, "invalid file");
        }

        counts[i] = size / sizeof(float);

        if (counts[i] > maxCount) {
            maxCount = counts[i];
        }
    }

    uint32_t magic = 0x0F0110F0;
    uint32_t size = maxCount;
    uint32_t files = n;

    if (write(out, &magic, sizeof(magic)) != sizeof(magic) ||
        write(out, &size, sizeof(size)) != sizeof(size) ||
        write(out, &files, sizeof(files)) != sizeof(files)) {
        err(6, "write header");
    }

    for (uint32_t pos = 0; pos < maxCount; pos++) {
        float minVal = 0;
        float maxVal = 0;
        uint8_t minIndex = 0;
        uint8_t maxIndex = 0;
        int first = 1;

        for (int i = 0; i < n; i++) {
            if (counts[i] <= pos) {
                continue;
            }

            float x;
            if (lseek(fds[i], pos * sizeof(float), SEEK_SET) < 0) {
                err(7, "lseek input");
            }

            if (read(fds[i], &x, sizeof(x)) != sizeof(x)) {
                err(8, "read");
            }

            if (first) {
                minVal = x;
                maxVal = x;
                minIndex = i;
                maxIndex = i;
                first = 0;
            } else {
                if (x < minVal) {
                    minVal = x;
                    minIndex = i;
                }

                if (x > maxVal) {
                    maxVal = x;
                    maxIndex = i;
                }
            }
        }

        Pair p;
        p.min = minIndex;
        p.max = maxIndex;

        if (write(out, &p, sizeof(p)) != sizeof(p)) {
            err(9, "write pair");
        }
    }

    for (int i = 0; i < n; i++) {
        close(fds[i]);
    }
    close(out);

    return 0;
}
