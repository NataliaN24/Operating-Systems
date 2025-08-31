//Напишете две програми на C (foo и bar), които си комуникират през наименована
//        тръба. Програмата foo приема параметър - име на файл, програмата bar приема параметър -
//команда като абсолютен път до изпълним файл.
//Примерни извиквания и ред на изпълнение (в отделни терминали):
//./foo a.txt
//./bar /usr/bin/sort
//        Програмата foo трябва да изпълнява външна команда cat с аргумент името на подадения файл,
//така че съдържанието му да се прехвърли през тръбата към програмата bar, която от своя страна
//        трябва да изпълни подадената и като аргумент команда (без параметри; /usr/bin/sort в примера),
//която да обработи получените през тръбата данни, четейки от стандартен вход. Еквивалент на
//горния пример би било следното изпълнение:
//cat a.txt | /usr/bin/sort

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <sys/wait.h>

#define FIFO_NAME "myfifo"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        errx(1, "Usage: %s <file>", argv[0]);
    }

    // създаваме именована тръба, ако не съществува
    mkfifo(FIFO_NAME, 0666);

    pid_t pid = fork();
    if (pid < 0) errx(2, "fork error");

    if (pid == 0) {
        // дете -> изпълнява cat <file>
        int fd = open(FIFO_NAME, O_WRONLY); // писане във fifo
        if (fd < 0) err(3, "open fifo");
        dup2(fd, STDOUT_FILENO); // stdout -> fifo
        close(fd);
        execlp("cat", "cat", argv[1], (char*)NULL);
        err(4, "exec cat failed");
    } else {
        // родител -> чака детето
        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/wait.h>

#define FIFO_NAME "myfifo"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        errx(1, "Usage: %s <command>", argv[0]);
    }

    // отваряме fifo за четене
    int fd = open(FIFO_NAME, O_RDONLY);
    if (fd < 0) err(2, "open fifo");

    pid_t pid = fork();
    if (pid < 0) err(3, "fork error");

    if (pid == 0) {
        // дете -> изпълнява командата
        dup2(fd, STDIN_FILENO); // stdin <- fifo
        close(fd);
        execl(argv[1], argv[1], (char*)NULL);
        err(4, "exec command failed");
    } else {
        close(fd);
        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}


