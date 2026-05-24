#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <err.h>
#include <errno.h>

const char* fifo = "myfifo";

int main(int argc, char* argv[])
{
    if (argc != 2) {
        errx(1, "usage: %s <file>", argv[0]);
    }

    // създаваме FIFO (ако не съществува)
    if (mkfifo(fifo, 0666) < 0 && errno != EEXIST) {
        err(2, "mkfifo");
    }

    int pid = fork();
    if (pid < 0) {
        err(3, "fork");
    }

    if (pid == 0) {
        // child

        int fd = open(fifo, O_WRONLY);
        if (fd < 0) {
            err(4, "open fifo write");
        }

        dup2(fd, 1); // stdout → FIFO
        close(fd);

        execlp("cat", "cat", argv[1], NULL);
        err(5, "exec cat");
    }

    wait(NULL);
    return 0;
}
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <err.h>
#include <errno.h>

const char* fifo = "myfifo";

int main(int argc, char* argv[])
{
    if (argc != 2) {
        errx(1, "usage: %s <command>", argv[0]);
    }

    // създаваме FIFO (ако не съществува)
    if (mkfifo(fifo, 0666) < 0 && errno != EEXIST) {
        err(2, "mkfifo");
    }

    int pid = fork();
    if (pid < 0) {
        err(3, "fork");
    }

    if (pid == 0) {
        // child

        int fd = open(fifo, O_RDONLY);
        if (fd < 0) {
            err(4, "open fifo read");
        }

        dup2(fd, 0); // stdin ← FIFO
        close(fd);

        execl(argv[1], argv[1], NULL);
        err(5, "exec command");
    }

    wait(NULL);
    return 0;
}
