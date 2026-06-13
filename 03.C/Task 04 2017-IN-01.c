#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

int main(int argc, char* argv[])
{
    if (argc != 5) {
        errx(1, "usage: %s f1.dat f1.idx f2.dat f2.idx", argv[0]);
    }

    int f1dat = open(argv[1], O_RDONLY);
    if (f1dat < 0) err(2, "open f1.dat");

    int f1idx = open(argv[2], O_RDONLY);
    if (f1idx < 0) err(2, "open f1.idx");

    int f2dat = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (f2dat < 0) err(2, "open f2.dat");

    int f2idx = open(argv[4], O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (f2idx < 0) err(2, "open f2.idx");

    off_t datSize = lseek(f1dat, 0, SEEK_END);
    if (datSize < 0) err(3, "lseek dat size");

    off_t idxSize = lseek(f1idx, 0, SEEK_END);
    if (idxSize < 0) err(3, "lseek idx size");

    if (idxSize % 4 != 0) {
        errx(4, "invalid idx size");
    }

    if (lseek(f1idx, 0, SEEK_SET) < 0) err(3, "lseek idx start");

    uint16_t off;
    uint8_t len;
    uint8_t reserved;
    uint16_t newOff = 0;

    while (read(f1idx, &off, sizeof(off)) == sizeof(off)) {
        if (read(f1idx, &len, sizeof(len)) != sizeof(len)) {
            err(5, "read len");
        }

        if (read(f1idx, &reserved, sizeof(reserved)) != sizeof(reserved)) {
            err(5, "read reserved");
        }

        if (len == 0) {
            errx(6, "invalid length");
        }

        if ((off_t)off + len > datSize) {
            errx(6, "invalid offset/length");
        }

        if (lseek(f1dat, off, SEEK_SET) < 0) {
            err(7, "lseek dat");
        }

        uint8_t first;
        if (read(f1dat, &first, sizeof(first)) != sizeof(first)) {
            err(7, "read first");
        }

        if (first >= 'A' && first <= 'Z') {
            if (lseek(f1dat, off, SEEK_SET) < 0) {
                err(7, "lseek dat again");
            }

            for (uint8_t i = 0; i < len; i++) {
                uint8_t byte;

                if (read(f1dat, &byte, sizeof(byte)) != sizeof(byte)) {
                    err(8, "read byte");
                }

                if (write(f2dat, &byte, sizeof(byte)) != sizeof(byte)) {
                    err(8, "write byte");
                }
            }

            if (write(f2idx, &newOff, sizeof(newOff)) != sizeof(newOff)) {
                err(8, "write new offset");
            }

            if (write(f2idx, &len, sizeof(len)) != sizeof(len)) {
                err(8, "write len");
            }

            reserved = 0;

            if (write(f2idx, &reserved, sizeof(reserved)) != sizeof(reserved)) {
                err(8, "write reserved");
            }

            newOff += len;
        }
    }

    close(f1dat);
    close(f1idx);
    close(f2dat);
    close(f2idx);

    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////


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
