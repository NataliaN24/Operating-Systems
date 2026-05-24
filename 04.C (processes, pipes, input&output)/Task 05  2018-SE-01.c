
//find DIR -type f -printf "%T@ %p\n" \
| sort -n \
| tail -n 1 \
| cut -d ' ' -f 2-

#include <unistd.h>
#include <sys/wait.h>
#include <err.h>
#include <stdlib.h>

void close_all(int p1[2], int p2[2], int p3[2])
{
    close(p1[0]);
    close(p1[1]);
    close(p2[0]);
    close(p2[1]);
    close(p3[0]);
    close(p3[1]);
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        errx(1, "usage: %s dir", argv[0]);
    }

    int p1[2], p2[2], p3[2];

    if(pipe(p1) == -1) err(1, "pipe");
    if(pipe(p2) == -1) err(1, "pipe");
    if(pipe(p3) == -1) err(1, "pipe");

    pid_t pid;

    // find DIR -type f -printf "%T@ %p\n"
    pid = fork();
    if(pid == -1) err(1, "fork");

    if(pid == 0)
    {
        dup2(p1[1], 1);
        close_all(p1, p2, p3);

        execlp("find", "find", argv[1], "-type", "f",
               "-printf", "%T@ %p\n", (char*)NULL);

        err(1, "exec find");
    }

    // sort -n
    pid = fork();
    if(pid == -1) err(1, "fork");

    if(pid == 0)
    {
        dup2(p1[0], 0);
        dup2(p2[1], 1);
        close_all(p1, p2, p3);

        execlp("sort", "sort", "-n", (char*)NULL);

        err(1, "exec sort");
    }

    // tail -n 1
    pid = fork();
    if(pid == -1) err(1, "fork");

    if(pid == 0)
    {
        dup2(p2[0], 0);
        dup2(p3[1], 1);
        close_all(p1, p2, p3);

        execlp("tail", "tail", "-n", "1", (char*)NULL);

        err(1, "exec tail");
    }

    // cut -d ' ' -f 2-
    pid = fork();
    if(pid == -1) err(1, "fork");

    if(pid == 0)
    {
        dup2(p3[0], 0);
        close_all(p1, p2, p3);

        execlp("cut", "cut", "-d", " ", "-f", "2-", (char*)NULL);

        err(1, "exec cut");
    }

    close_all(p1, p2, p3);

    for(int i = 0; i < 4; i++)
    {
        wait(NULL);
    }

    return 0;
}
