#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <err.h>


int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        errx(1, "arguments");
    }


    if (strcmp(argv[1], "-c") == 0)
    {
        if (argc != 3)
        {
            errx(1, "arguments");
        }


        int a;
        int b;


        if (strlen(argv[2]) == 1)
        {
            a = argv[2][0] - '0';
            b = a;
        }
        else
        {
            a = argv[2][0] - '0';
            b = argv[2][2] - '0';
        }


        uint8_t byte;
        int pos = 1;


        while (read(0, &byte, sizeof(byte)) == sizeof(byte))
        {
            if (byte == '\n')
            {
                pos = 1;
            }


            if (pos >= a && pos <= b)
            {
                if (write(1, &byte, sizeof(byte)) != sizeof(byte))
                {
                    err(1, "write");
                }
            }


            pos++;
        }
    }


    else if (strcmp(argv[1], "-d") == 0)
    {
        if (argc != 5)
        {
            errx(1, "arguments");
        }


        char delimiter = argv[2][0];


        if (strcmp(argv[3], "-f") != 0)
        {
            errx(1, "arguments");
        }


        int a;
        int b;


        if (strlen(argv[4]) == 1)
        {
            a = argv[4][0] - '0';
            b = a;
        }
        else
        {
            a = argv[4][0] - '0';
            b = argv[4][2] - '0';
        }


        uint8_t byte;
        int field = 1;


        while (read(0, &byte, sizeof(byte)) == sizeof(byte))
        {
            if (byte == '\n')
            {
                field = 1;

                if (write(1, &byte, sizeof(byte)) != sizeof(byte))
                {
                    err(1, "write");
                }

                continue;
            }


            if (byte == delimiter)
            {
                field++;
                continue;
            }


            if (field >= a && field <= b)
            {
                if (write(1, &byte, sizeof(byte)) != sizeof(byte))
                {
                    err(1, "write");
                }
            }
        }
    }


    return 0;
}
///////////////////////////////////////////////
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
