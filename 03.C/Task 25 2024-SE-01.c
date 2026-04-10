#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stdlib.h>

#define RECORD_SIZE 512

int main(int argc, char* argv[])
{
    if (argc != 2) {
        errx(1, "usage: %s file", argv[0]);
    }

    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        err(1, "open");
    }

    // размер на файла
    off_t size = lseek(fd, 0, SEEK_END);
    if (size < 0) {
        err(1, "lseek");
    }

    if (size % RECORD_SIZE != 0) {
        errx(1, "invalid file");
    }

    uint64_t count = size / RECORD_SIZE;

    // временен файл
    char tmp_name[] = "tmpXXXXXX";
    int tmp = mkstemp(tmp_name);
    if (tmp < 0) {
        err(1, "mkstemp");
    }

    unlink(tmp_name); // да се изтрие автоматично

    // 🔹 ФАЗА 1: намираме живите
    uint64_t curr = 0;

    while (1) {
        // записваме текущия индекс като "жив"
        if (write(tmp, &curr, sizeof(curr)) != sizeof(curr)) {
            err(1, "write tmp");
        }

        // отиваме на записа
        off_t pos = curr * RECORD_SIZE;
        if (lseek(fd, pos, SEEK_SET) < 0) {
            err(1, "lseek");
        }

        // четем next
        uint64_t next;
        if (read(fd, &next, sizeof(next)) != sizeof(next)) {
            err(1, "read next");
        }

        if (next == 0) {
            break;
        }

        if (next >= count) {
            errx(1, "invalid next index");
        }

        curr = next;
    }

    // 🔹 ФАЗА 2: зануляваме неизползваните
    uint8_t zeros[RECORD_SIZE] = {0};

    for (uint64_t i = 0; i < count; i++) {

        // проверка дали е жив
        int alive = 0;

        if (lseek(tmp, 0, SEEK_SET) < 0) {
            err(1, "lseek tmp");
        }

        uint64_t x;
        while (read(tmp, &x, sizeof(x)) == sizeof(x)) {
            if (x == i) {
                alive = 1;
                break;
            }
        }

        if (!alive) {
            off_t pos = i * RECORD_SIZE;

            if (lseek(fd, pos, SEEK_SET) < 0) {
                err(1, "lseek write");
            }

            if (write(fd, zeros, RECORD_SIZE) != RECORD_SIZE) {
                err(1, "write zeros");
            }
        }
    }

    close(tmp);
    close(fd);
}
