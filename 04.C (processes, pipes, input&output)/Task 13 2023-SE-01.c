#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <err.h>

int ends_with_hash(char* s)
{
    int len = strlen(s);

    if (len < 5) {
        return 0;
    }

    return strcmp(s + len - 5, ".hash") == 0;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        errx(1, "usage: %s dir", argv[0]);
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
        close(p[0]);

        dup2(p[1], 1);
        close(p[1]);

        execlp("find", "find", argv[1], "-type", "f", NULL);
        err(1, "exec find");
    }

    close(p[1]);

    char buf[1024];
    char path[4096];
    int pos = 0;

    ssize_t n;

    while ((n = read(p[0], buf, sizeof(buf))) > 0) {
        for (int i = 0; i < n; i++) {
            if (buf[i] == '\n') {
                path[pos] = '\0';

                if (!ends_with_hash(path)) {
                    pid_t c = fork();

                    if (c < 0) {
                        err(1, "fork");
                    }

                    if (c == 0) {
                        char hashfile[4096];

                        int len = strlen(path);

                        if (len + 5 >= 4096) {
                            errx(1, "path too long");
                        }

                        strcpy(hashfile, path);
                        strcat(hashfile, ".hash");

                        int fd = open(hashfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

                        if (fd < 0) {
                            err(1, "open hash file");
                        }

                        dup2(fd, 1);
                        close(fd);

                        execlp("md5sum", "md5sum", path, NULL);
                        err(1, "exec md5sum");
                    }
                }

                pos = 0;
            } else {
                if (pos >= 4095) {
                    errx(1, "path too long");
                }

                path[pos] = buf[i];
                pos++;
            }
        }
    }

    if (n < 0) {
        err(1, "read");
    }

    close(p[0]);

    wait(NULL);

    while (wait(NULL) > 0) {
    }

    return 0;
}
