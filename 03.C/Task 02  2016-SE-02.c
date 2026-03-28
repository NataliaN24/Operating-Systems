#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, const char* argv[])
{
    if (argc != 4) { // cmd f1 f2 f3
        errx(1, "Usage: %s f1 f2 f3", argv[0]);
    }

    int fd1 = open(argv[1], O_RDONLY);
    int fd2 = open(argv[2], O_RDONLY);
    int fd3 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd1 < 0 || fd2 < 0 || fd3 < 0) {
        err(1, "open error");
    }

    uint32_t x, y;
    ssize_t bytesRead;

    while ((bytesRead = read(fd1, &x, sizeof(x))) == sizeof(x)) {
        if (read(fd1, &y, sizeof(y)) != sizeof(y)) {
            errx(1, "incomplete pair in f1");
        }

        // here we have <x, y>
        off_t offset = x * sizeof(uint32_t);

        if (lseek(fd2, offset, SEEK_SET) < 0) {
            err(1, "lseek error");
        }

        uint32_t num;
        for (uint32_t i = 0; i < y; i++) {
            if (read(fd2, &num, sizeof(num)) != sizeof(num)) {
                errx(1, "not enough data in f2");
            }

            if (write(fd3, &num, sizeof(num)) != sizeof(num)) {
                err(1, "write error");
            }
        }
    }

    if (bytesRead < 0) {
        err(1, "read error from f1");
    }

    if (close(fd1) < 0) {
        err(1, "close fd1");
    }
    if (close(fd2) < 0) {
        err(1, "close fd2");
    }
    if (close(fd3) < 0) {
        err(1, "close fd3");
    }

    return 0;
}
