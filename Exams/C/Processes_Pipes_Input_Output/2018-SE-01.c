//Напишете програма на C, която приема параметър – име на директория. Програмата
//трябва да извежда името на най-скоро променения (по съдържание) файл в тази директория и
//нейните под-директории, чрез употреба на външни шел команди през pipe().

//find <директория> -type f -printf "%T@ %p\n" | sort -n | tail -1
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <err.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        errx(1, "Usage: %s <directory>", argv[0]);
    }

    int fd1[2], fd2[2];

    // pipe1: find -> sort
    if (pipe(fd1) == -1) err(2, "pipe1");
    // pipe2: sort -> tail
    if (pipe(fd2) == -1) err(3, "pipe2");

    pid_t pid;

    // ----------------- 1. find -----------------
    pid = fork();
    if (pid == -1) err(4, "fork");
    if (pid == 0) {
        dup2(fd1[1], 1); // stdout -> pipe1 write
        close(fd1[0]); close(fd1[1]);
        close(fd2[0]); close(fd2[1]);
        execlp("find", "find", argv[1], "-type", "f", "-printf", "%T@ %p\n", (char*)NULL);
        err(5, "exec find");
    }

    // ----------------- 2. sort -n -----------------
    pid = fork();
    if (pid == -1) err(6, "fork");
    if (pid == 0) {
        dup2(fd1[0], 0);   // stdin <- find
        dup2(fd2[1], 1);   // stdout -> pipe2 write
        close(fd1[0]); close(fd1[1]);
        close(fd2[0]); close(fd2[1]);
        execlp("sort", "sort", "-n", (char*)NULL);
        err(7, "exec sort");
    }

    // ----------------- 3. tail -1 -----------------
    pid = fork();
    if (pid == -1) err(8, "fork");
    if (pid == 0) {
        dup2(fd2[0], 0); // stdin <- sort
        close(fd1[0]); close(fd1[1]);
        close(fd2[0]); close(fd2[1]);
        execlp("tail", "tail", "-1", (char*)NULL);
        err(9, "exec tail");
    }

    // ----------------- Родителят -----------------
    close(fd1[0]); close(fd1[1]);
    close(fd2[0]); close(fd2[1]);

    // чакаме 3-те деца
    for (int i = 0; i < 3; i++) wait(NULL);

    return 0;
}
