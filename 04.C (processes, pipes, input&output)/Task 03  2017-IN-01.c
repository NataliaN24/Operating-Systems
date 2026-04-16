#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void) {
    int p1[2], p2[2], p3[2], p4[2];

    if (pipe(p1) < 0) err(1, "pipe p1");
    if (pipe(p2) < 0) err(1, "pipe p2");
    if (pipe(p3) < 0) err(1, "pipe p3");
    if (pipe(p4) < 0) err(1, "pipe p4");

    pid_t pid;

    // 1) cat /etc/passwd
    pid = fork();
    if (pid < 0) err(1, "fork");
    if (pid == 0) {
        dup2(p1[1], 1);

        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        close(p4[0]); close(p4[1]);

        execlp("cat", "cat", "/etc/passwd", (char *)NULL);
        err(1, "exec cat");
    }

    // 2) cut -d: -f7
    pid = fork();
    if (pid < 0) err(1, "fork");
    if (pid == 0) {
        dup2(p1[0], 0);
        dup2(p2[1], 1);

        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        close(p4[0]); close(p4[1]);

        execlp("cut", "cut", "-d:", "-f7", (char *)NULL);
        err(1, "exec cut");
    }

    // 3) sort
    pid = fork();
    if (pid < 0) err(1, "fork");
    if (pid == 0) {
        dup2(p2[0], 0);
        dup2(p3[1], 1);

        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        close(p4[0]); close(p4[1]);

        execlp("sort", "sort", (char *)NULL);
        err(1, "exec sort");
    }

    // 4) uniq -c
    pid = fork();
    if (pid < 0) err(1, "fork");
    if (pid == 0) {
        dup2(p3[0], 0);
        dup2(p4[1], 1);

        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        close(p4[0]); close(p4[1]);

        execlp("uniq", "uniq", "-c", (char *)NULL);
        err(1, "exec uniq");
    }

    // 5) sort -n
    pid = fork();
    if (pid < 0) err(1, "fork");
    if (pid == 0) {
        dup2(p4[0], 0);

        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        close(p4[0]); close(p4[1]);

        execlp("sort", "sort", "-n", (char *)NULL);
        err(1, "exec sort -n");
    }

    // parent: затваря всичко
    close(p1[0]); close(p1[1]);
    close(p2[0]); close(p2[1]);
    close(p3[0]); close(p3[1]);
    close(p4[0]); close(p4[1]);

    // чака всички деца
    for (int i = 0; i < 5; i++) {
        wait(NULL);
    }

    return 0;
}
