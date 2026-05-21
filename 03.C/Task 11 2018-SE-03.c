#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>

void processC(int beg, int end)
{
    char c;
    ssize_t readBytes;
    int count = 1;

    while ((readBytes = read(0, &c, 1)) > 0) {
        if (c == '\n') {
            if (write(1, &c, 1) != 1) {
                err(1, "write");
            }
            count = 1;
            continue;
        }

        if (count >= beg && count <= end) {
            if (write(1, &c, 1) != 1) {
                err(1, "write");
            }
        }

        count++;
    }

    if (readBytes < 0) {
        err(1, "read");
    }
}

void processD(char sep, int beg, int end)
{
    char c;
    ssize_t bytesRead;
    int field = 1;
    int printedSomething = 0;

    while ((bytesRead = read(0, &c, 1)) > 0) {
        if (c == '\n') {
            if (write(1, &c, 1) != 1) {
                err(1, "write");
            }

            field = 1;
            printedSomething = 0;
            continue;
        }

        if (c == sep) {
            if (field >= beg && field < end) {
                if (write(1, &c, 1) != 1) {
                    err(1, "write");
                }
            }

            field++;
            continue;
        }

        if (field >= beg && field <= end) {
            if (write(1, &c, 1) != 1) {
                err(1, "write");
            }
            printedSomething = 1;
        }
    }

    if (bytesRead < 0) {
        err(1, "read");
    }
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        errx(1, "usage");
    }

    if (strcmp(argv[1], "-c") == 0) {
        if (argc != 3) {
            errx(1, "usage: ./main -c N or ./main -c N-M");
        }

        if (strlen(argv[2]) == 1) {
            processC(argv[2][0] - '0', argv[2][0] - '0');
        } else {
            processC(argv[2][0] - '0', argv[2][2] - '0');
        }
    } else if (strcmp(argv[1], "-d") == 0) {
        if (argc != 5 || strcmp(argv[3], "-f") != 0) {
            errx(1, "usage: ./main -d SEP -f N or ./main -d SEP -f N-M");
        }

        if (strlen(argv[4]) == 1) {
            processD(argv[2][0], argv[4][0] - '0', argv[4][0] - '0');
        } else {
            processD(argv[2][0], argv[4][0] - '0', argv[4][2] - '0');
        }
    } else {
        errx(1, "unknown option");
    }

    exit(0);
}
