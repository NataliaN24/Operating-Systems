#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdint.h>
#include <err.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        errx(1, "usage: %s input output", argv[0]);
    }

    int out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) {
        err(1, "open output");
    }

    int fd[2];
    if (pipe(fd) < 0) {
        err(1, "pipe");
    }

    pid_t pid = fork();
    if (pid < 0) {
        err(1, "fork");
    }

    if (pid == 0) {
        close(fd[0]);

        if (dup2(fd[1], 1) < 0) {
            err(1, "dup2");
        }

        close(fd[1]);

        execlp("cat", "cat", argv[1], (char*)NULL);
        err(1, "execlp");
    }

    close(fd[1]);

    uint8_t ch;
    ssize_t bytesRead;

    while ((bytesRead = read(fd[0], &ch, sizeof(ch))) == sizeof(ch)) {
        if (ch == 0x55) {
            continue;
        }

        if (ch == 0x7D) {
            uint8_t next;

            if (read(fd[0], &next, sizeof(next)) != sizeof(next)) {
                errx(1, "invalid input");
            }

            uint8_t originalByte = next ^ 0x20;

            if (write(out, &originalByte, sizeof(originalByte)) != sizeof(originalByte)) {
                err(1, "write");
            }
        } else {
            if (write(out, &ch, sizeof(ch)) != sizeof(ch)) {
                err(1, "write");
            }
        }
    }

    if (bytesRead < 0) {
        err(1, "read");
    }

    close(fd[0]);
    close(out);

    if (wait(NULL) < 0) {
        err(1, "wait");
    }

    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    if(argc != 3) { err(1, "Not enough argumenets"); }

    int data[2];
    if(pipe(data) < 0) { err(2, "Couldn't pipe"); }

    int pid = fork();
    if(pid < 0) { err(3, "Fork"); }
    if(pid == 0)
    {
        if(close(data[0]) < 0) { err(4, "Close"); }
        if(dup2(data[1], 1) < 0) { err(5, "Dup2"); }
        if(close(data[1]) < 0) { err(6, "Close"); }

        execlp("cat", "cat", argv[1], (char*)NULL);
        err(7, "Couldn't exec cat on %s", argv[1]);
    }

    int outp = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(outp < 0) { err(8, "Open"); }

    int readBytes;
    uint8_t byte;
    uint8_t mask = 0x20;
    uint8_t prev = 0;

    while((readBytes = read(data[0], &byte, sizeof(byte))) > 0)
    {
        if(prev == 0x7D)
        {
            byte = byte ^ mask;
        }
        else
        {
            if(byte == 0x55)
            {
                continue;
            }
            else if(byte == 0x7D)
            {
                prev = byte;
                continue;
            }
        }

        if(write(outp, &byte, sizeof(byte)) < 0) { err(9, "Write"); }
        prev = byte;
    }

    if(close(outp) < 0) { err(10, "Close"); }

    return 0;
}
