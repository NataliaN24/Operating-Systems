#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <sys/stat.h>

#define MAX_NUMS 4194304
#define HALF_NUMS 2097152

int cmp(const void* a, const void* b)
{
    uint32_t x = *(uint32_t*)a;
    uint32_t y = *(uint32_t*)b;

    if (x < y) {
        return -1;
    }

    if (x > y) {
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        errx(1, "invalid args");
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        err(1, "open input");
    }

    struct stat st;

    if (fstat(fd, &st) < 0) {
        err(1, "stat");
    }

    if (st.st_size % sizeof(uint32_t) != 0) {
        errx(1, "bad file");
    }

    uint32_t nums = st.st_size / sizeof(uint32_t);

    if (nums > MAX_NUMS) {
        errx(1, "too many nums");
    }

    uint32_t first = nums / 2;
    uint32_t second = nums - first;

    uint32_t arr[HALF_NUMS];

    int tmp1 = open("tmp1.bin", O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (tmp1 < 0) {
        err(1, "tmp1");
    }

    ssize_t r = read(fd, arr, first * sizeof(uint32_t));

    if (r < 0) {
        err(1, "read");
    }

    if (r != first * sizeof(uint32_t)) {
        errx(1, "short read");
    }

    qsort(arr, first, sizeof(uint32_t), cmp);

    ssize_t w = write(tmp1, arr, first * sizeof(uint32_t));

    if (w < 0) {
        err(1, "write");
    }

    if (w != first * sizeof(uint32_t)) {
        errx(1, "short write");
    }

    close(tmp1);

    int tmp2 = open("tmp2.bin", O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (tmp2 < 0) {
        err(1, "tmp2");
    }

    r = read(fd, arr, second * sizeof(uint32_t));

    if (r < 0) {
        err(1, "read");
    }

    if (r != second * sizeof(uint32_t)) {
        errx(1, "short read");
    }

    qsort(arr, second, sizeof(uint32_t), cmp);

    w = write(tmp2, arr, second * sizeof(uint32_t));

    if (w < 0) {
        err(1, "write");
    }

    if (w != second * sizeof(uint32_t)) {
        errx(1, "short write");
    }

    close(tmp2);
    close(fd);

    tmp1 = open("tmp1.bin", O_RDONLY);
    if (tmp1 < 0) {
        err(1, "open tmp1");
    }

    tmp2 = open("tmp2.bin", O_RDONLY);
    if (tmp2 < 0) {
        err(1, "open tmp2");
    }

    int out = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (out < 0) {
        err(1, "output");
    }

    uint32_t a;
    uint32_t b;

    int has_a = 0;
    int has_b = 0;

    r = read(tmp1, &a, sizeof(uint32_t));

    if (r < 0) {
        err(1, "read");
    }

    if (r == sizeof(uint32_t)) {
        has_a = 1;
    }

    r = read(tmp2, &b, sizeof(uint32_t));

    if (r < 0) {
        err(1, "read");
    }

    if (r == sizeof(uint32_t)) {
        has_b = 1;
    }

    while (has_a && has_b) {

        if (a <= b) {

            w = write(out, &a, sizeof(uint32_t));

            if (w != sizeof(uint32_t)) {
                err(1, "write");
            }

            r = read(tmp1, &a, sizeof(uint32_t));

            if (r < 0) {
                err(1, "read");
            }

            if (r == sizeof(uint32_t)) {
                has_a = 1;
            } else {
                has_a = 0;
            }

        } else {

            w = write(out, &b, sizeof(uint32_t));

            if (w != sizeof(uint32_t)) {
                err(1, "write");
            }

            r = read(tmp2, &b, sizeof(uint32_t));

            if (r < 0) {
                err(1, "read");
            }

            if (r == sizeof(uint32_t)) {
                has_b = 1;
            } else {
                has_b = 0;
            }
        }
    }

    while (has_a) {

        w = write(out, &a, sizeof(uint32_t));

        if (w != sizeof(uint32_t)) {
            err(1, "write");
        }

        r = read(tmp1, &a, sizeof(uint32_t));

        if (r < 0) {
            err(1, "read");
        }

        if (r == sizeof(uint32_t)) {
            has_a = 1;
        } else {
            has_a = 0;
        }
    }

    while (has_b) {

        w = write(out, &b, sizeof(uint32_t));

        if (w != sizeof(uint32_t)) {
            err(1, "write");
        }

        r = read(tmp2, &b, sizeof(uint32_t));

        if (r < 0) {
            err(1, "read");
        }

        if (r == sizeof(uint32_t)) {
            has_b = 1;
        } else {
            has_b = 0;
        }
    }

    close(tmp1);
    close(tmp2);
    close(out);

    unlink("tmp1.bin");
    unlink("tmp2.bin");

    return 0;
}
