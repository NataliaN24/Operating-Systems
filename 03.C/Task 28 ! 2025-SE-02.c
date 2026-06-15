#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        errx(1, "Usage: %s index values name", argv[0]);
    }

    int index = open(argv[1], O_RDONLY);
    if (index < 0)
    {
        err(1, "open index");
    }

    int val = open(argv[2], O_RDONLY);
    if (val < 0)
    {
        err(1, "open values");
    }

    uint8_t header;
    int recIndex = 0;

    while (read(index, &header, sizeof(header)) == sizeof(header))
    {
        uint8_t len = header & 0x7F;
        uint8_t type = (header >> 7) & 1;

        char text[len + 1];

        if (read(index, text, len) != len)
        {
            errx(1, "bad index file");
        }

        text[len] = '\0';

        if (strcmp(text, argv[3]) == 0)
        {
            if (lseek(val, recIndex * 4, SEEK_SET) < 0)
            {
                err(1, "lseek");
            }

           if (type == 0)
{
    int32_t x;

    if (read(val, &x, sizeof(x)) != sizeof(x))
    {
        errx(1, "bad values file");
    }

    char buff[32];

    int len = snprintf(buff, sizeof(buff), "%d\n", x);

    if (write(1, buff, len) != len)
    {
        err(1, "write");
    }
}
else
{
    float f;

    if (read(val, &f, sizeof(f)) != sizeof(f))
    {
        errx(1, "bad values file");
    }

    char buff[64];

    int len = snprintf(buff, sizeof(buff), "%.3f\n", f);

    if (write(1, buff, len) != len)
    {
        err(1, "write");
    }
}
            close(index);
            close(val);
            return 0;
        }

        recIndex++;
    }

    errx(1, "record not found");
}





#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <err.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc != 4) {
        errx(1, "Usage: %s index values name", argv[0]);
    }

    int fd_idx = open(argv[1], O_RDONLY);
    if (fd_idx < 0) {
        err(2, "open index");
    }

    int fd_val = open(argv[2], O_RDONLY);
    if (fd_val < 0) {
        err(3, "open values");
    }

    struct stat st;
    if (fstat(fd_val, &st) < 0) {
        err(4, "fstat values");
    }

    if (st.st_size % 4 != 0) {
        errx(5, "bad values size");
    }

    uint32_t values_count = st.st_size / 4;
    uint32_t record_index = 0;

    while (1)
    {
        uint8_t control;

        ssize_t r = read(fd_idx, &control, 1);
        if (r < 0) {
            err(6, "read index");
        }

        if (r == 0) {
            break;
        }

        uint8_t len = control & 0x7F;
        uint8_t type = control >> 7;

        char name[128];

        if (read(fd_idx, name, len) != len) {
            errx(7, "bad index file");
        }

        name[len] = '\0';

        if (strcmp(name, argv[3]) == 0)
        {
            if (record_index >= values_count) {
                errx(8, "missing value");
            }

            if (lseek(fd_val, record_index * 4, SEEK_SET) < 0) {
                err(9, "lseek values");
            }

            if (type == 0)
            {
                int32_t x;

                if (read(fd_val, &x, sizeof(x)) != sizeof(x)) {
                    err(10, "read int");
                }

                printf("%d\n", x);
            }
            else
            {
                float f;

                if (read(fd_val, &f, sizeof(f)) != sizeof(f)) {
                    err(11, "read float");
                }

                printf("%.3f\n", f);
            }

            close(fd_idx);
            close(fd_val);
            return 0;
        }

        record_index++;
    }

    errx(12, "name not found");
}
