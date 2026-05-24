#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <err.h>
#include <sys/wait.h>

#define PACKET_SIZE 64
#define MAX_FILES 16

int main(int argc, char* argv[])
{
    if (argc < 3 || argc > 18) {
        errx(1, "usage: %s producer file1 ... fileN", argv[0]);
    }

    int n = argc - 2;

    int fds[MAX_FILES];

    // Отваряме файловете консуматори
    for (int i = 0; i < n; i++) {
        fds[i] = open(argv[i + 2], O_WRONLY | O_APPEND);

        if (fds[i] < 0) {
            err(1, "open consumer file");
        }
    }

    int p[2];

    if (pipe(p) < 0) {
        err(1, "pipe");
    }

    pid_t pid = fork();

    if (pid < 0) {
        err(1, "fork");
    }

    if (pid == 0) {
        // child = producer

        close(p[0]);        // не чете от pipe
        dup2(p[1], 1);      // stdout на producer -> pipe
        close(p[1]);

        execl(argv[1], argv[1], NULL);
        err(1, "exec");
    }

    // parent = main програмата

    close(p[1]); // parent не пише в pipe

    char buf[PACKET_SIZE];
    int index = 0;

    while (1) {
        ssize_t r = read(p[0], buf, PACKET_SIZE);

        if (r < 0) {
            err(1, "read");
        }

        if (r == 0) {
            break; // producer е приключил
        }

        if (r != PACKET_SIZE) {
            errx(1, "invalid packet size");
        }

        if (write(fds[index], buf, PACKET_SIZE) != PACKET_SIZE) {
            err(1, "write");
        }

        index++;

        if (index == n) {
            index = 0;
        }
    }

    close(p[0]);

    for (int i = 0; i < n; i++) {
        close(fds[i]);
    }

    wait(NULL);

    return 0;
}
