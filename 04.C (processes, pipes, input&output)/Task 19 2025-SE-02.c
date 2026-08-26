#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/wait.h>
#include <err.h>

int main(int argc, const char *argv[])
{
    if (argc < 3 || argc > 18)
    {
        errx(1, "Usage");
    }

    int cnt = argc - 2;

    int pipefd[2];
    if (pipe(pipefd) < 0)
    {
        err(1, "pipe");
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        err(1, "fork");
    }

    if (pid == 0)
    {
        close(pipefd[0]);

        if (dup2(pipefd[1], STDOUT_FILENO) < 0)
        {
            err(1, "dup2");
        }

        close(pipefd[1]);

        execlp(argv[1], argv[1], (char *)NULL);
        err(1, "execlp");
    }

    close(pipefd[1]);

    int files[16];

    for (int i = 0; i < cnt; i++)
    {
        files[i] = open(argv[i + 2], O_WRONLY | O_APPEND);
        if (files[i] < 0)
        {
            err(1, "open");
        }
    }

    uint8_t package[64];
    int i = 0;

    while (read(pipefd[0], package, sizeof(package)) == sizeof(package))
    {
        if (write(files[i % cnt], package, sizeof(package)) != sizeof(package))
        {
            err(1, "write");
        }

        i++;
    }

    close(pipefd[0]);

    for (int j = 0; j < cnt; j++)
    {
        close(files[j]);
    }

    if (wait(NULL) < 0)
    {
        err(1, "wait");
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAXFILE 16
#define PACKET 64

int main(int argc, char* argv[])
{
    if (argc < 3 || argc > MAXFILE + 2) {
        errx(1, "usage: %s producer file1 [file2 ...]", argv[0]);
    }

    int n = argc - 2;

    int files[MAXFILE];

    for (int i = 0; i < n; i++) {
        files[i] = open(argv[i + 2], O_WRONLY | O_APPEND);
        if (files[i] < 0) {
            err(2, "open consumer");
        }
    }

    int pipefd[2];

    if (pipe(pipefd) < 0) {
        err(3, "pipe");
    }

    pid_t pid = fork();

    if (pid < 0) {
        err(4, "fork");
    }

    if (pid == 0) {
        close(pipefd[0]);

        if (dup2(pipefd[1], 1) < 0) {
            err(5, "dup2");
        }

        close(pipefd[1]);

        execl(argv[1], argv[1], (char*)NULL);
        err(6, "exec");
    }

    close(pipefd[1]);

    uint8_t packet[PACKET];
    int index = 0;

    while (1) {
        ssize_t bytesRead = read(pipefd[0], packet, sizeof(packet));

        if (bytesRead == 0) {
            break;
        }

        if (bytesRead < 0) {
            err(7, "read");
        }

        if (bytesRead != sizeof(packet)) {
            errx(8, "incomplete packet");
        }

        if (write(files[index], packet, sizeof(packet)) != sizeof(packet)) {
            err(9, "write");
        }

        index++;

        if (index == n) {
            index = 0;
        }
    }

    close(pipefd[0]);

    for (int i = 0; i < n; i++) {
        close(files[i]);
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        err(10, "waitpid");
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        errx(11, "producer failed");
    }

    exit(0);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAXFILE 16
#define PACKET 64

int main(int argc, char* argv[])
{
    if (argc < 3 || argc > MAXFILE + 2) {
        errx(1, "wrong arguments");
    }

    int n = argc - 2;

    char* files[MAXFILE];

    for (int i = 0; i < n; i++) {
        files[i] = argv[i + 2];
    }

    int client[2];

    if (pipe(client) < 0) {
        err(1, "pipe");
    }

    pid_t clientPID = fork();

    if (clientPID < 0) {
        err(1, "fork");
    }

    if (clientPID == 0) {
        close(client[0]);

        if (dup2(client[1], 1) < 0) {
            err(1, "dup2");
        }

        close(client[1]);

        execl(argv[1], argv[1], (char*)NULL);
        err(1, "execl");
    }

    close(client[1]);

    uint8_t packet[PACKET];
    int index = 0;

    while (1) {
        ssize_t bytesRead = read(client[0], packet, sizeof(packet));

        if (bytesRead == 0) {
            break;
        }

        if (bytesRead < 0) {
            err(1, "read");
        }

        if (bytesRead != sizeof(packet)) {
            errx(1, "incomplete packet");
        }

        pid_t pid = fork();

        if (pid < 0) {
            err(1, "fork");
        }

        if (pid == 0) {
            int fd = open(files[index], O_WRONLY | O_APPEND);

            if (fd < 0) {
                err(1, "open");
            }

            if (write(fd, packet, sizeof(packet)) != sizeof(packet)) {
                err(1, "write");
            }

            close(fd);
            exit(0);
        }

        waitpid(pid, NULL, 0);

        index++;

        if (index == n) {
            index = 0;
        }
    }

    close(client[0]);

    waitpid(clientPID, NULL, 0);

    exit(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

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
