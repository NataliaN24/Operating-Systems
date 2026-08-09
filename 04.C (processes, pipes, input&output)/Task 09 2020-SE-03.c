#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        errx(1, "usage: %s file", argv[0]);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        err(1, "open input");
    }

    int wire[2];
    if (pipe(wire) < 0) {
        err(1, "pipe");
    }

    char filename[8];
    uint32_t offset;
    uint32_t len;

    while (read(fd, filename, sizeof(filename)) == sizeof(filename) &&
           read(fd, &offset, sizeof(offset)) == sizeof(offset) &&
           read(fd, &len, sizeof(len)) == sizeof(len)) {

        pid_t pid = fork();
        if (pid < 0) {
            err(1, "fork");
        }

        if (pid == 0) {
            close(wire[0]);
            close(fd);

            int currFd = open(filename, O_RDONLY);
            if (currFd < 0) {
                err(1, "open data file");
            }

            if (lseek(currFd, offset * sizeof(uint16_t), SEEK_SET) < 0) {
                err(1, "lseek");
            }

            uint16_t res = 0;
            uint16_t element;

            for (uint32_t i = 0; i < len; i++) {
                if (read(currFd, &element, sizeof(element)) != sizeof(element)) {
                    err(1, "read element");
                }

                res ^= element;
            }

            if (write(wire[1], &res, sizeof(res)) != sizeof(res)) {
                err(1, "write pipe");
            }

            close(currFd);
            close(wire[1]);

            exit(0);
        }
    }

    close(fd);
    close(wire[1]);

    uint16_t finalRes = 0;
    uint16_t currRes;
    ssize_t readBytes;

    while ((readBytes = read(wire[0], &currRes, sizeof(currRes))) > 0) {
        if (readBytes != sizeof(currRes)) {
            errx(1, "partial read from pipe");
        }

        finalRes ^= currRes;
    }

    if (readBytes < 0) {
        err(1, "read pipe");
    }

    close(wire[0]);

    while (wait(NULL) > 0) {
        // чакаме всички child процеси
    }

    char buff[64];
    int n = snprintf(buff, sizeof(buff), "result: %04X\n", finalRes);

    if (write(1, buff, n) != n) {
        err(1, "write stdout");
    }

    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////


#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char* argv[])
{
    if(argc != 2) { err(1, "Not enough argumenets"); }

    int fd = open(argv[1], O_RDONLY);
    if(fd < 0) { err(2, "Open"); }

    int wire[2];
    if(pipe(wire) < 0) { err(3, "Pipe"); }

    struct stat f_info;
    if(fstat(fd, &f_info) < 0) { err(4, "Fstat"); }
    int count = f_info.st_size / 16;

    uint16_t finalRes = 0;

    for(int i = 0; i < count; i++)
    {
        char filename[8];
        uint32_t offset, len;

        if(read(fd, filename, sizeof(filename)) < 0 || read(fd, &offset, sizeof(offset)) < 0 || read(fd, &len, sizeof(len)) < 0) { err(5, "Read"); }

        int pid = fork();
        if(pid < 0) { err(6, "Fork"); }
        if(pid == 0)
        {
            int currFd = open(filename, O_RDONLY);
            if(currFd < 0) { err(7, "Open"); }

            if(lseek(currFd, offset * sizeof(uint16_t), SEEK_SET) < 0) { err(8, "Lseek"); }
            uint16_t res = 0;

            //let's say that the format is guaranteed so I don't need to check if the current file size is less than the offset + len * 2
            for(int j = 0; j < (int)len; j++)
            {
                uint16_t byte;
                if(read(currFd, &byte, sizeof(byte)) < 0) { err(9, "Read"); }
                res ^= byte;
            }

            if(close(wire[0]) < 0) { err(10, "Close"); }
            if(write(wire[1], &res, sizeof(res)) < 0) { err(11, "Write"); }
            if(close(wire[1]) < 0) { err(12, "Close"); }
            exit(0);
        }
    }

    if(close(wire[1]) < 0) { err(13, "Close"); }

    for(int i = 0; i < count; i++){
        wait(NULL);
    }

    int readBytes = 0;
    uint16_t currRes;
    while((readBytes = read(wire[0], &currRes, sizeof(currRes))) > 0)
    {
        finalRes ^= currRes;
    }

    if(readBytes < 0) { err(14, "Read"); }

    char buff[64];
    snprintf(buff, sizeof(buff), "result: %04X\n", finalRes);
    if(write(1, buff, strlen(buff)) < 0) { err(15, "Write"); }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////



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
