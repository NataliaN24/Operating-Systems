#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <sys/stat.h>

#define MAX_NUMS 65535

int cmp(const void* a, const void* b)
{
    uint16_t x = *(uint16_t*)a;
    uint16_t y = *(uint16_t*)b;

    if (x < y) return -1;
    if (x > y) return 1;
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

    if (st.st_size % sizeof(uint16_t) != 0) {
        errx(1, "bad file size");
    }

    uint32_t nums = st.st_size / sizeof(uint16_t);

    if (nums > MAX_NUMS) {
        errx(1, "too many numbers");
    }

    uint16_t arr[MAX_NUMS];

    ssize_t r = read(fd, arr, nums * sizeof(uint16_t));
    if (r < 0) {
        err(1, "read");
    }

    if (r != nums * sizeof(uint16_t)) {
        errx(1, "short read");
    }

    close(fd);

    qsort(arr, nums, sizeof(uint16_t), cmp);

    int out = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (out < 0) {
        err(1, "open output");
    }

    ssize_t w = write(out, arr, nums * sizeof(uint16_t));
    if (w < 0) {
        err(1, "write");
    }

    if (w != nums * sizeof(uint16_t)) {
        errx(1, "short write");
    }

    close(out);

    return 0;
}
