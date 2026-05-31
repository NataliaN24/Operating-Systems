#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <err.h>

int main(int argc, char* argv[])
{
    if (argc != 5) {
        errx(1, "Usage: %s f1.dat f1.idx f2.dat f2.idx", argv[0]);
    }

    int fd_dat1 = open(argv[1], O_RDONLY);
    if (fd_dat1 < 0) {
        err(2, "open f1.dat");
    }

    int fd_idx1 = open(argv[2], O_RDONLY);
    if (fd_idx1 < 0) {
        err(3, "open f1.idx");
    }

    int fd_dat2 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dat2 < 0) {
        err(4, "open f2.dat");
    }

    int fd_idx2 = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_idx2 < 0) {
        err(5, "open f2.idx");
    }

    struct stat st_dat;
    struct stat st_idx;

    if (fstat(fd_dat1, &st_dat) < 0) {
        err(6, "fstat dat");
    }

    if (fstat(fd_idx1, &st_idx) < 0) {
        err(7, "fstat idx");
    }

    if (st_idx.st_size % 4 != 0) {
        errx(8, "bad idx size");
    }

    uint16_t outOffset = 0;

    while (1)
    {
        uint16_t offset;
        uint8_t length;
        uint8_t reserved;

        ssize_t r = read(fd_idx1, &offset, sizeof(offset));
        if (r < 0) {
            err(9, "read offset");
        }
        if (r == 0) {
            break;
        }
        if (r != sizeof(offset)) {
            errx(10, "bad idx record");
        }

        if (read(fd_idx1, &length, sizeof(length)) != sizeof(length)) {
            errx(11, "bad idx length");
        }

        if (read(fd_idx1, &reserved, sizeof(reserved)) != sizeof(reserved)) {
            errx(12, "bad idx reserved");
        }

        if ((uint32_t)offset + (uint32_t)length > (uint32_t)st_dat.st_size) {
            errx(13, "inconsistent dat/idx");
        }

        if (length == 0) {
            continue;
        }

        if (lseek(fd_dat1, offset, SEEK_SET) < 0) {
            err(14, "lseek");
        }

        uint8_t first;
        if (read(fd_dat1, &first, 1) != 1) {
            errx(15, "cannot read first byte");
        }

        if (!(first >= 'A' && first <= 'Z')) {
            continue;
        }

        if ((uint32_t)outOffset + (uint32_t)length > UINT16_MAX) {
            errx(16, "output too large");
        }

        if (lseek(fd_dat1, offset, SEEK_SET) < 0) {
            err(17, "lseek again");
        }

        uint8_t buffer[255];

        if (read(fd_dat1, buffer, length) != length) {
            errx(18, "cannot read string");
        }

        if (write(fd_dat2, buffer, length) != length) {
            err(19, "write dat2");
        }

        if (write(fd_idx2, &outOffset, sizeof(outOffset)) != sizeof(outOffset)) {
            err(20, "write offset");
        }

        if (write(fd_idx2, &length, sizeof(length)) != sizeof(length)) {
            err(21, "write length");
        }

        uint8_t newReserved = 0;
        if (write(fd_idx2, &newReserved, sizeof(newReserved)) != sizeof(newReserved)) {
            err(22, "write reserved");
        }

        outOffset += length;
    }

    close(fd_dat1);
    close(fd_idx1);
    close(fd_dat2);
    close(fd_idx2);

    return 0;
}
