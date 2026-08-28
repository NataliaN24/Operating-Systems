#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        errx(1, "usage: %s input.bin output.bin", argv[0]);
    }

    int input = open(argv[1], O_RDONLY);

    if (input < 0)
    {
        err(1, "open input");
    }

    int output = open(argv[2],
                      O_WRONLY | O_CREAT | O_TRUNC,
                      0644);

    if (output < 0)
    {
        err(1, "open output");
    }

    uint8_t bytes[2];

    // Четем по 2 байта = 16 кодирани бита
    while (read(input, bytes, 2) == 2)
    {
        uint8_t result = 0;

        // Обхождаме двата байта
        for (int j = 0; j < 2; j++)
        {
            // Вземаме битовете отляво надясно
            for (int i = 7; i >= 1; i -= 2)
            {
                uint8_t first = (bytes[j] >> i) & 1;
                uint8_t second = (bytes[j] >> (i - 1)) & 1;

                uint8_t decoded;

                if (first == 1 && second == 0)
                {
                    // 10 -> 1
                    decoded = 1;
                }
                else if (first == 0 && second == 1)
                {
                    // 01 -> 0
                    decoded = 0;
                }
                else
                {
                    errx(1, "invalid encoding");
                }

                // Добавяме декодирания бит към result
                result = (result << 1) | decoded;
            }
        }

        if (write(output, &result, 1) != 1)
        {
            err(1, "write");
        }
    }

    close(input);
    close(output);

    return 0;
}

//////////////////////////////////////
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        errx(1, "usage: %s input output", argv[0]);
    }

    int input = open(argv[1], O_RDONLY);
    if (input < 0) {
        err(1, "open input");
    }

    int output = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (output < 0) {
        err(1, "open output");
    }

    uint8_t in;
    uint8_t out = 0;

    ssize_t n;

    while ((n = read(input, &in, sizeof(in))) == sizeof(in)) {

        for (int i = 7; i >= 0; i -= 2) {

            uint8_t pair = (in >> (i - 1)) & 3;

            uint8_t bit;

            if (pair == 2) {
                // 10 -> 1
                bit = 1;
            }
            else if (pair == 1) {
                // 01 -> 0
                bit = 0;
            }
            else {
                errx(1, "invalid Manchester code");
            }

            out = (out << 1) | bit;


            // всеки 8 декодирани бита записваме
            static int count = 0;
            count++;

            if (count == 8) {
                if (write(output, &out, 1) != 1) {
                    err(1, "write");
                }

                out = 0;
                count = 0;
            }
        }
    }

    if (n != 0) {
        err(1, "read");
    }


    close(input);
    close(output);

    return 0;
}
