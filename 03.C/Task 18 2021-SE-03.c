#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

off_t getFileSize(int fd)
{
    off_t size = lseek(fd, 0, SEEK_END);
    if (size < 0)
    {
        err(1, "lseek");
    }

    if (lseek(fd, 0, SEEK_SET) < 0)
    {
        err(1, "lseek");
    }

    return size;
}

void writeHeader(int output)
{
    const char* buffer =
        "#include <stdint.h>\n"
        "static const uint16_t arr[] = {\n";

    if (write(output, buffer, strlen(buffer)) < 0)
    {
        err(1, "write");
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        errx(1, "usage: %s input.bin output.h", argv[0]);
    }

    int input = open(argv[1], O_RDONLY);
    int output = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (input < 0 || output < 0)
    {
        err(1, "open");
    }

    off_t inputSize = getFileSize(input);

    if (inputSize % sizeof(uint16_t) != 0)
    {
        close(input);
        close(output);
        errx(1, "invalid file");
    }

    size_t elementsNumber = inputSize / sizeof(uint16_t);

    if (elementsNumber > 524288)
    {
        close(input);
        close(output);
        errx(1, "too many elements");
    }

    writeHeader(output);

    uint16_t number;

    for (size_t i = 0; i < elementsNumber; i++)
    {
        if (read(input, &number, sizeof(number)) != sizeof(number))
        {
            close(input);
            close(output);
            err(1, "read");
        }

        char line[64];
        int len;

        if (i + 1 == elementsNumber)
        {
            len = snprintf(line, sizeof(line), "    %u\n", number);
        }
        else
        {
            len = snprintf(line, sizeof(line), "    %u,\n", number);
        }

        if (len < 0)
        {
            err(1, "snprintf");
        }

        if (write(output, line, len) < 0)
        {
            err(1, "write");
        }
    }

    // затваряме масива
    if (write(output, "};\n", 3) < 0)
    {
        err(1, "write");
    }

    // пишем размера
    char buffer[128];
    int len = snprintf(buffer, sizeof(buffer),
        "static const uint32_t arrN = %u;\n",
        (uint32_t)elementsNumber);

    if (write(output, buffer, len) < 0)
    {
        err(1, "write");
    }

    close(input);
    close(output);
}
