#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

const char DING[] = "DING ";
const char DONG[] = "DONG\n";

int main(int argc, char* argv[])
{
    if (argc != 3) {
        errx(1, "invalid args");
    }

    if (strlen(argv[1]) != 1 || strlen(argv[2]) != 1) {
        errx(1, "args must be one digit");
    }

    if (*argv[1] < '1' || *argv[1] > '9' || *argv[2] < '1' || *argv[2] > '9') {
        errx(1, "args must be digits 1-9");
    }

    int n = *argv[1] - '0';
    int d = *argv[2] - '0';

    int parentToChild[2];
    if (pipe(parentToChild) < 0) {
        err(1, "pipe");
    }

    int childToParent[2];
    if (pipe(childToParent) < 0) {
        err(1, "pipe");
    }

    pid_t child = fork();
    if (child < 0) {
        err(1, "fork");
    }

    if (child == 0) {
        close(parentToChild[1]);
        close(childToParent[0]);

        char x;

        for (int i = 0; i < n; i++) {
            if (read(parentToChild[0], &x, 1) != 1) {
                err(1, "read");
            }

            if (write(1, DONG, 5) != 5) {
                err(1, "write");
            }

            if (write(childToParent[1], "x", 1) != 1) {
                err(1, "write");
            }
        }

        close(parentToChild[0]);
        close(childToParent[1]);
        _exit(0);
    }

    close(parentToChild[0]);
    close(childToParent[1]);

    char x;

    for (int i = 0; i < n; i++) {
        if (write(1, DING, 5) != 5) {
            err(1, "write");
        }

        if (write(parentToChild[1], "x", 1) != 1) {
            err(1, "write");
        }

        if (read(childToParent[0], &x, 1) != 1) {
            err(1, "read");
        }

        sleep(d);
    }

    close(parentToChild[1]);
    close(childToParent[0]);

    int status;
    if (wait(&status) < 0) {
        err(1, "wait");
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        errx(1, "child failed");
    }

    return 0;
}
    return 0;

}
