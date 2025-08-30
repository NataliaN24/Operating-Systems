//Да се напише програма на C, която получава като 
//командни параметри две команди (без параметри). Изпълнява ги
//едновременно и извежда на стандартния изход номера на процеса на първата завършила 
//успешно.
//Ако нито една не завърши успешно извежда -1.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>

int main(int argc, char* argv[]) {
    if(argc != 3) {
        errx(1, "Usage: %s <cmd1> <cmd2>", argv[0]);
    }

    pid_t pid1 = fork();
    if(pid1 < 0) errx(2, "fork failed");

    if(pid1 == 0) {
        execlp(argv[1], argv[1], (char*)NULL);
        errx(3, "failed to exec %s", argv[1]);
    }

    pid_t pid2 = fork();
    if(pid2 < 0) errx(4, "fork failed");

    if(pid2 == 0) {
        execlp(argv[2], argv[2], (char*)NULL);
        errx(5, "failed to exec %s", argv[2]);
    }

    int status;
    int success_pid = -1;

    // Чакаме първото дете, което завърши успешно
    for(int i = 0; i < 2; i++) {
        pid_t finished = wait(&status);
        if(finished == -1) errx(6, "wait failed");

        if(WIFEXITED(status) && WEXITSTATUS(status) == 0 && success_pid == -1) {
            success_pid = finished;
        }
    }

    printf("%d\n", success_pid);

    return 0;
}
