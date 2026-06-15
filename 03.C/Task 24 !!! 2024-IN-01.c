#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    uint32_t ctime;
    uint16_t opt;
    uint16_t parent_id;
    uint32_t size;
    uint32_t ssize;
} object;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        errx(1, "usage: %s file", argv[0]);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        err(1, "open");
    }

    uint16_t magic, ver, cp, co;
    if (read(fd, &magic, sizeof(magic)) != sizeof(magic) ||
        read(fd, &ver, sizeof(ver)) != sizeof(ver) ||
        read(fd, &cp, sizeof(cp)) != sizeof(cp) ||
        read(fd, &co, sizeof(co)) != sizeof(co))
    {
        err(1, "read header");
    }

    if (magic != 0x6963 || ver != 0x6e73) {
        errx(1, "invalid format");
    }

    uint16_t v1, v2;
    uint32_t v3;
    for (int i = 0; i < cp; i++) {
        if (read(fd, &v1, sizeof(v1)) != sizeof(v1) ||
            read(fd, &v2, sizeof(v2)) != sizeof(v2) ||
            read(fd, &v3, sizeof(v3)) != sizeof(v3))
        {
            err(1, "read preamble");
        }
    }

    object* objects = malloc(co * sizeof(object));
    if (objects == NULL) {
        err(1, "malloc");
    }

    for (int i = 0; i < co; i++) {
        if (read(fd, &objects[i], sizeof(object)) != sizeof(object)) {
            err(1, "read object");
        }
    }

    uint64_t sum_size = 0;
    uint64_t sum_ssize = 0;

    for (int i = 0; i < co; i++) {
        object current = objects[i];

        uint16_t type = current.opt >> 14;
        if (type != 2) {
            continue;
        }

        if (current.parent_id == 0) {
            continue;
        }

        if (current.parent_id >= co) {
            continue;
        }

      //  object parent = objects[current.parent_id];
        uint16_t pid = current.parent_id;
    Object parent = objects[pid];

        if (current.ctime < parent.ctime) {
            continue;
        }

        uint32_t diff = current.ctime - parent.ctime;

        if (diff >= 85800 && diff <= 87000) {
            sum_size += current.size;
            sum_ssize += current.ssize;
        }
    }

   char buf[32];
int len;

if (sum_size == 0) {
    len = snprintf(buf, sizeof(buf), "0.000000\n");
} else {
    double result = (double)sum_ssize / (double)sum_size;
    len = snprintf(buf, sizeof(buf), "%.6f\n", result);
}
write(1, buf, len);

    free(objects);
    close(fd);
    return 0;
}
