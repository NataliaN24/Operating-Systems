#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>

int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        errx(1, "usage: input output");
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


    uint32_t P;
    uint32_t N;
    uint8_t data[16384];


    while (read(input, &P, sizeof(P)) == sizeof(P) &&
           read(input, &N, sizeof(N)) == sizeof(N))
    {
        if (N > sizeof(data))
        {
            errx(1, "record too large");
        }


        if (read(input, data, N) != N)
        {
            errx(1, "invalid input");
        }


        if (lseek(output, P, SEEK_SET) < 0)
        {
            err(1, "lseek");
        }


        if (write(output, data, N) != N)
        {
            err(1, "write");
        }
    }


    close(input);
    close(output);

    return 0;
}
