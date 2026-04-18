#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        errx(1, "usage: %s input.lfld output.bin", argv[0]);
    }

    int outfd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outfd < 0) {
        err(2, "open output");
    }

    int p[2];
    if (pipe(p) < 0) {
        err(3, "pipe");
    }

    pid_t pid = fork();
    if (pid < 0) {
        err(4, "fork");
    }

    if (pid == 0) {
        close(p[0]);

        if (dup2(p[1], 1) < 0) {
            err(5, "dup2");
        }
        close(p[1]);

        char cmd[1024];
        int n = snprintf(cmd, sizeof(cmd), "cat '%s'", argv[1]);
        if (n < 0 || n >= (int)sizeof(cmd)) {
            errx(6, "command too long");
        }

        execl("/bin/sh", "sh", "-c", cmd, (char*)NULL);
        err(7, "execl");
    }

    close(p[1]);

    uint8_t byte;
    while (1) {
        ssize_t r = read(p[0], &byte, sizeof(byte));
        if (r < 0) {
            err(8, "read");
        }
        if (r == 0) { //EOF
            break;
        }

        if (byte == 0x55) {
            continue;
        }

        if (byte == 0x7D) {
            uint8_t escaped;
            ssize_t rr = read(p[0], &escaped, sizeof(escaped));//READ ONE OTHER BYTE IN ORDER TO RETURN THE ORIGNAL ONE
            if (rr < 0) {
                err(9, "read escaped byte");
            }
            if (rr == 0) {
                errx(10, "invalid stream: escape byte at end");
            }

            uint8_t original = escaped ^ 0x20;
            if (write(outfd, &original, sizeof(original)) != sizeof(original)) {
                err(11, "write decoded byte");
            }
        } else {
            if (write(outfd, &byte, sizeof(byte)) != sizeof(byte)) {
                err(12, "write normal byte");
            }
        }
    }

    close(p[0]);
    close(outfd);

    int status;
    if (wait(&status) < 0) {
        err(13, "wait");
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        errx(14, "shell command failed");
    }

    return 0;
}
