#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
    char name[9];
    uint32_t offset;
    uint32_t length;
} record;

int main(int argc, const char *argv[])
{
    if (argc != 2) {
        errx(1, "usage: %s spec_file", argv[0]);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        err(1, "open");
    }

    record records[8];
    int pipes[8][2];
    pid_t pids[8];
    int count = 0;

    for (int i = 0; i < 8; i++) {
        char rawname[8];
        ssize_t r = read(fd, rawname, sizeof(rawname));

        if (r == 0) {
            break; // край на файла
        }
        if (r != sizeof(rawname)) {
            errx(2, "invalid input file");
        }

        memcpy(records[i].name, rawname, 8);
        records[i].name[8] = '\0';

        if (read(fd, &records[i].offset, sizeof(records[i].offset)) != sizeof(records[i].offset) ||
            read(fd, &records[i].length, sizeof(records[i].length)) != sizeof(records[i].length)) {
            errx(2, "invalid input file");
        }

        if (pipe(pipes[i]) < 0) {
            err(1, "pipe");
        }

        pid_t pid = fork();
        if (pid < 0) {
            err(1, "fork");
        }

        if (pid == 0) {
            close(pipes[i][0]);

            int fileFd = open(records[i].name, O_RDONLY);
            if (fileFd < 0) {
                err(1, "open child file");
            }

            off_t start = (off_t)records[i].offset * sizeof(uint16_t);
            if (lseek(fileFd, start, SEEK_SET) < 0) {
                err(1, "lseek");
            }

            uint16_t result = 0;
            for (uint32_t j = 0; j < records[i].length; j++) {
                uint16_t element;
                if (read(fileFd, &element, sizeof(element)) != sizeof(element)) {
                    err(1, "read element");
                }
                result ^= element;
            }

            if (write(pipes[i][1], &result, sizeof(result)) != sizeof(result)) {
                err(1, "write to pipe");
            }

            close(fileFd);
            close(pipes[i][1]);
            exit(0);
        }

        close(pipes[i][1]);
        pids[i] = pid;
        count++;
    }

    close(fd);

    uint16_t finalResult = 0;

    for (int i = 0; i < count; i++) {
        uint16_t childResult;
        if (read(pipes[i][0], &childResult, sizeof(childResult)) != sizeof(childResult)) {
            err(1, "read from pipe");
        }
        finalResult ^= childResult;
        close(pipes[i][0]);
    }

    for (int i = 0; i < count; i++) {
        int status;
        if (waitpid(pids[i], &status, 0) < 0) {
            err(1, "waitpid");
        }
    }

    printf("result: %04X\n", finalResult);

    return 0;
}
