#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <err.h>
#include <stdio.h>

off_t getSizeOfFile(const char *filename)
{
    struct stat st;
    if (stat(filename, &st) == -1)
    {
        err(1, "stat failed for %s", filename);
    }
    return st.st_size;
}

void readSafe(int fd, void *buf, size_t count, const char *msg)
{
    size_t total = 0;
    while (total < count)
    {
        ssize_t bytesRead = read(fd, (char*)buf + total, count - total); //char* because of 1 byte
        if (bytesRead == -1)
        {
            err(2, "%s", msg);
        }
        if (bytesRead == 0)
        {
            errx(3, "%s: unexpected EOF", msg);
        }
        total += bytesRead;
    }
}

void writeSafe(int fd, const void *buf, size_t count, const char *msg)
{
    size_t total = 0;
    while (total < count)
    {
        ssize_t bytesWritten = write(fd, (const char*)buf + total, count - total);
        if (bytesWritten == -1)
        {
            err(4, "%s", msg);
        }
        total += bytesWritten;
    }
}

int main(int argc, const char* argv[])
{
    if (argc != 4)
    {
        errx(1, "Usage: %s f1.bin f2.bin patch.bin", argv[0]);
    }

    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0)
    {
        err(2, "Could not open %s", argv[1]);
    }

    int fd2 = open(argv[2], O_RDONLY);
    if (fd2 < 0)
    {
        close(fd1);
        err(2, "Could not open %s", argv[2]);
    }

    int fd3 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd3 < 0)
    {
        close(fd1);
        close(fd2);
        err(2, "Could not create %s", argv[3]);
    }

    off_t size1 = getSizeOfFile(argv[1]);
    off_t size2 = getSizeOfFile(argv[2]);

    if (size1 != size2)
    {
        close(fd1);
        close(fd2);
        close(fd3);
        errx(5, "Files have different sizes");
    }

    if (size1 > UINT16_MAX + 1ULL)
    {
        close(fd1);
        close(fd2);
        close(fd3);
        errx(6, "File too large for uint16_t offsets");
    }

    for (uint16_t i = 0; i < size1; i++)
    {
        uint8_t byte1;
        uint8_t byte2;

        readSafe(fd1, &byte1, sizeof(byte1), "read from f1.bin failed");
        readSafe(fd2, &byte2, sizeof(byte2), "read from f2.bin failed");

        if (byte1 == byte2)
        {
            continue;
        }

        writeSafe(fd3, &i, sizeof(i), "write offset failed");
        writeSafe(fd3, &byte1, sizeof(byte1), "write original byte failed");
        writeSafe(fd3, &byte2, sizeof(byte2), "write new byte failed");
    }

    close(fd1);
    close(fd2);
    close(fd3);

    return 0;
}
