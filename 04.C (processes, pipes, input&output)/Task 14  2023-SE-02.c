#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <err.h>

#define ERR 26

int line_is_found(char* line)
{
    return strcmp(line, "found it!") == 0;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        exit(ERR);
    }

    int p[2];

    if (pipe(p) < 0) {
        exit(ERR);
    }

    int n = argc - 1;
    pid_t pids[1024];

    if (n > 1024) {
        exit(ERR);
    }

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            exit(ERR);
        }

        if (pid == 0) {
            close(p[0]);

            if (dup2(p[1], 1) < 0) {
                exit(ERR);
            }

            close(p[1]);

            execl(argv[i + 1], argv[i + 1], NULL);
            exit(ERR);
        }

        pids[i] = pid;
    }

    close(p[1]);

    char buf[1024];
    char line[1024];
    int pos = 0;
    int found = 0;

    ssize_t r;

    while ((r = read(p[0], buf, sizeof(buf))) > 0) {
        for (int i = 0; i < r; i++) {
            if (buf[i] == '\n') {
                line[pos] = '\0';

                if (line_is_found(line)) {
                    found = 1;
                    break;
                }

                pos = 0;
            } else {
                if (pos < 1023) {
                    line[pos++] = buf[i];
                }
            }
        }

        if (found) {
            break;
        }
    }

    if (r < 0) {
        exit(ERR);
    }

    close(p[0]);

    if (found) {
        for (int i = 0; i < n; i++) {
            kill(pids[i], SIGTERM);
        }

        for (int i = 0; i < n; i++) {
            waitpid(pids[i], NULL, 0);
        }

        exit(0);
    }

    for (int i = 0; i < n; i++) {
        waitpid(pids[i], NULL, 0);
    }

    exit(1);
}
