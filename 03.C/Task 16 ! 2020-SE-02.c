#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        errx(1, "usage: %s scl sdl", argv[0]);
    }

    int fd1 = open(argv[1], O_RDONLY); // SCL
    int fd2 = open(argv[2], O_RDONLY); // SDL
    int fd3 = open("result.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd1 < 0 || fd2 < 0 || fd3 < 0) {
        err(1, "open");
    }

    uint8_t mask;
    uint16_t num;

    while (read(fd1, &mask, 1) == 1) {

        for (int i = 7; i >= 0; i--) {

            if (read(fd2, &num, sizeof(num)) != sizeof(num)) {
                return 0; // свършили са числата
            }

            if (mask & (1 << i)) {
                if (write(fd3, &num, sizeof(num)) != sizeof(num)) {
                    err(1, "write");
                }
            }
        }
    }

    close(fd1);
    close(fd2);
    close(fd3);

    return 0;
}
