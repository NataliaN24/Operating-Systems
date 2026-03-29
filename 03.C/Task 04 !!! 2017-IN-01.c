#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <err.h>
#include <stdio.h>

ssize_t getSizeOfFile(const char* filename)
{
    struct stat st;
    if (stat(filename, &st) == -1)
    {
        err(2, "stat");
    }
    return st.st_size;
}

void readSafe(int fd, uint8_t* buf, size_t count, const char* msg)
{
    size_t total = 0;
    while (total < count)
    {
        ssize_t bytesRead = read(fd, buf + total, count - total);
        if (bytesRead == -1) {
            err(3, "%s", msg);
        }
        if (bytesRead == 0) {
            errx(4, "%s: unexpected EOF", msg);
        }
        total += bytesRead;
    }
}

void writeSafe(int fd, const uint8_t* ptr, size_t count, const char* msg)
{
    size_t total = 0;
    while (total < count)
    {
        ssize_t bytesWritten = write(fd, ptr + total, count - total);
        if (bytesWritten == -1)
        {
            err(5, "%s", msg);
        }
        total += bytesWritten;
    }
}

int main(int argc, const char* argv[])
{
    if (argc != 5) {
        errx(1, "Usage: %s f1.dat f1.idx f2.dat f2.idx", argv[0]);
    }

    const char* f1Data = argv[1];
    const char* f1Idx  = argv[2];
    const char* f2Data = argv[3];
    const char* f2Idx  = argv[4];

    int fd1 = open(f1Data, O_RDONLY);
    if (fd1 < 0) {
        err(2, "Could not open %s", f1Data);
    }

    int fd2 = open(f1Idx, O_RDONLY);
    if (fd2 < 0) {
        close(fd1);
        err(2, "Could not open %s", f1Idx);
    }

    int fd3 = open(f2Data, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd3 < 0) {
        close(fd1);
        close(fd2);
        err(2, "Could not create %s", f2Data);
    }

    int fd4 = open(f2Idx, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd4 < 0) {
        close(fd1);
        close(fd2);
        close(fd3);
        err(2, "Could not create %s", f2Idx);
    }

    ssize_t f1DataSize = getSizeOfFile(f1Data);
    ssize_t f1IdxSize  = getSizeOfFile(f1Idx);

    if (f1IdxSize % 4 != 0)
    {
        close(fd1);
        close(fd2);
        close(fd3);
        close(fd4);
        errx(3, "Inconsistent files: idx file size is not divisible by 4");
    }

    uint16_t outOffset = 0;
    ssize_t recordsCount = f1IdxSize / 4;

    for (ssize_t i = 0; i < recordsCount; i++)
    {
        uint16_t offset;
        uint8_t length;
        uint8_t reserved;

        readSafe(fd2, (uint8_t*)&offset, sizeof(offset), "Could not read offset from idx");
        readSafe(fd2, &length, sizeof(length), "Could not read length from idx");
        readSafe(fd2, &reserved, sizeof(reserved), "Could not read reserved from idx");

        if ((ssize_t)offset + (ssize_t)length > f1DataSize) {
            close(fd1);
            close(fd2);
            close(fd3);
            close(fd4);
            errx(4, "Inconsistent files: invalid offset/length in idx");
        }

        if (length == 0) {
            continue;
        }

        if (lseek(fd1, offset, SEEK_SET) == -1)
        {
            close(fd1);
            close(fd2);
            close(fd3);
            close(fd4);
            err(5, "Could not seek in %s", f1Data);
        }

        uint8_t firstByte;
        readSafe(fd1, &firstByte, sizeof(firstByte), "Could not read first byte from dat");

        if (!(firstByte >= 'A' && firstByte <= 'Z'))
        {
            continue;
        }

        if (lseek(fd1, offset, SEEK_SET) == -1) {
            close(fd1);
            close(fd2);
            close(fd3);
            close(fd4);
            err(5, "Could not seek in %s", f1Data);
        }

        uint8_t buffer[255];
        readSafe(fd1, buffer, length, "Could not read string from dat");

        writeSafe(fd3, buffer, length, "Could not write to output dat");

        writeSafe(fd4, (const uint8_t*)&outOffset, sizeof(outOffset), "Could not write offset to output idx");
        writeSafe(fd4, &length, sizeof(length), "Could not write length to output idx");

        uint8_t newReserved = 0;
        writeSafe(fd4, &newReserved, sizeof(newReserved), "Could not write reserved to output idx");

        if ((uint32_t)outOffset + (uint32_t)length > UINT16_MAX) {
            close(fd1);
            close(fd2);
            close(fd3);
            close(fd4);
            errx(6, "Output dat too large for uint16_t offsets");
        }

        outOffset += length;
    }

    if (close(fd1) == -1) {
        err(7, "Could not close %s", f1Data);
    }
    if (close(fd2) == -1) {
        err(7, "Could not close %s", f1Idx);
    }
    if (close(fd3) == -1) {
        err(7, "Could not close %s", f2Data);
    }
    if (close(fd4) == -1) {
        err(7, "Could not close %s", f2Idx);
    }

    return 0;
}
