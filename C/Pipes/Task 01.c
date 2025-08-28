
//T1 - Да се напише програма на C, която приема аргумент - име на файл. Програмата да:
//
//записва във файла 'fo'
//създава child процес, който записва 'bar\n'
//parent-а, след като изчака child процеса, записва 'o\n'

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <err.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if(argc != 2) {
        errx(1,"Usage: %s filename", argv[0]);
    }

    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd == -1) {
        errx(2,"Error opening file");
    }

    const char* str = "fo";
    const char* str2 = "bar\n";
    const char* str3 = "o\n";

    // parent writes "fo"
    if(write(fd, str, strlen(str)) == -1) {
        close(fd);
        errx(3,"Error writing fo");
    }

    pid_t pidOFchild = fork();
    if(pidOFchild == -1) {
        close(fd);
        errx(4,"Error forking");
    }

    if(pidOFchild == 0) {
        // child process
        if(write(fd, str2, strlen(str2)) == -1) {
            close(fd);
            errx(5,"Error writing bar");
        }
        close(fd);
        exit(0);
    } else {
        // parent process
        wait(NULL); // wait for child process
        if(write(fd, str3, strlen(str3)) == -1) {
            close(fd);
            errx(6,"Error writing o");
        }
        close(fd);
    }

    return 0;
}
