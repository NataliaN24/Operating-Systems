#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        errx(1, "usage");
    }

    int input = open(argv[1], O_RDONLY);
    if (input < 0) {
        err(1, "open input");
    }

    int out = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (out < 0) {
        err(1, "open output");
    }

    char header[] = "#include <stdint.h>\n\n";
    if (write(out, header, sizeof(header) - 1) != (sizeof(header) - 1)) {
        err(1, "write");
    }

    uint16_t arr[524288];
    uint16_t num;
    ssize_t n;
    uint32_t size = 0;

    while ((n = read(input, &num, sizeof(num))) == sizeof(num)) {
        if (size == 524288) {
            errx(1, "too many elements");
        }
        arr[size++] = num;
    }

    if (n != 0) {
        err(1, "read");
    }

    dprintf(out, "const uint16_t arr[] = {");

    for (uint32_t i = 0; i < size; i++) {
        if (i != 0) {
            dprintf(out, ", ");
        }
        dprintf(out, "%u", arr[i]);
    }

    dprintf(out, "};\n");
    dprintf(out, "const uint32_t arrN = %u;\n", size);

    close(input);
    close(out);

    return 0;
}
////////////////////////////////////////////////
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdlib.h>
#include <sys/stat.h>

void writeStr(int fd, const char* str)
{
    int len = 0;

    while (str[len] != '\0') {
        len++;
    }

    if (write(fd, str, len) < 0) {
        err(1, "write");
    }
}

void writeNum(int fd, uint32_t num)
{
    char buf[20];

    int i = 0;

    if (num == 0) {
        buf[i++] = '0';
    }

    while (num > 0) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }

    for (int j = i - 1; j >= 0; j--) {
        if (write(fd, &buf[j], 1) < 0) {
            err(1, "write");
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        errx(1, "usage");
    }

    int fdIn = open(argv[1], O_RDONLY);
    if (fdIn < 0) {
        err(1, "open input");
    }

    struct stat st;

    if (fstat(fdIn, &st) < 0) {
        err(1, "stat");
    }

    if (st.st_size % sizeof(uint16_t) != 0) {
        errx(1, "invalid file");
    }

    uint32_t count = st.st_size / sizeof(uint16_t);

    if (count > 524288) {
        errx(1, "too many elements");
    }

    int fdOut = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fdOut < 0) {
        err(1, "open output");
    }

    writeStr(fdOut, "#include <stdint.h>\n\n");
    writeStr(fdOut, "const uint16_t arr[] = {\n");

    uint16_t num;

    for (uint32_t i = 0; i < count; i++) {

        ssize_t r = read(fdIn, &num, sizeof(num));

        if (r < 0) {
            err(1, "read");
        }

        if (r != sizeof(num)) {
            errx(1, "broken file");
        }

        writeStr(fdOut, "    ");

        writeNum(fdOut, num);

        if (i + 1 != count) {
            writeStr(fdOut, ",");
        }

        writeStr(fdOut, "\n");
    }

    writeStr(fdOut, "};\n\n");
    writeStr(fdOut, "const uint32_t arrN = ");

    writeNum(fdOut, count);

    writeStr(fdOut, ";\n");

    close(fdIn);
    close(fdOut);

    exit(0);
}


////////////////////////////////////////////////////////////

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
