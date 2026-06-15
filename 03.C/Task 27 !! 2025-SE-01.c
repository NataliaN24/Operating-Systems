#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <string.h>

#define MAX_FILES 20
#define MAX_LINES 10000
#define HEADER_ID 133742

struct Line {
    uint64_t time;
    char role[256];
    char text[256];
};

int cmp(const void* a, const void* b)
{
    const struct Line* x = a;
    const struct Line* y = b;

    if (x->time < y->time) {
        return -1;
    }

    if (x->time > y->time) {
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > MAX_FILES + 1) {
        errx(1, "usage: %s file1 ... file20", argv[0]);
    }

    struct Line lines[MAX_LINES];
    int linesCount = 0;

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);

        if (fd < 0) {
            err(1, "open");
        }

        uint64_t id;
        uint8_t len;
        char role[256];

        if (read(fd, &id, sizeof(id)) != sizeof(id)) {
            errx(1, "empty file");
        }

        if (read(fd, &len, sizeof(len)) != sizeof(len)) {
            errx(1, "bad header");
        }

        if (read(fd, role, len) != len) {
            errx(1, "bad header text");
        }

        role[len] = '\0';

        if (id != HEADER_ID) {
            errx(1, "invalid header");
        }

        while (1) {
            uint64_t time;
            uint8_t textLen;
            char text[256];

            int r = read(fd, &time, sizeof(time));

            if (r == 0) {
                break;
            }

            if (r != sizeof(time)) {
                errx(1, "bad record");
            }

            if (read(fd, &textLen, sizeof(textLen)) != sizeof(textLen)) {
                errx(1, "bad record");
            }

            if (read(fd, text, textLen) != textLen) {
                errx(1, "bad record text");
            }

            text[textLen] = '\0';

            if (linesCount >= MAX_LINES) {
                errx(1, "too many lines");
            }

            lines[linesCount].time = time;
            strcpy(lines[linesCount].role, role);
            strcpy(lines[linesCount].text, text);

            linesCount++;
        }

        close(fd);
    }

    qsort(lines, linesCount, sizeof(struct Line), cmp);

    for (int i = 0; i < linesCount; i++) {
        char output[600];

        int size = snprintf(
            output,
            sizeof(output),
            "%s: %s\n",
            lines[i].role,
            lines[i].text
        );

        if (write(1, output, size) != size) {
            err(1, "write");
        }
    }

    exit(0);
}
