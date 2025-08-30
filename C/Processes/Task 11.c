/Да се напише програма на C, която изпълнява последователно
  подадените ѝ като параметри команди,
//като реализира следната функционалност постъпково:
//main cmd1 ... cmdN Изпълнява всяка от командите в отделен дъщерен процес.
//... при което се запазва броя на изпълнените команди, които са
  дали грешка и броя на завършилите успешно.

  #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>

int main(int argc, char *argv[]) {
    if(argc < 2) {
        errx(1, "Usage: %s cmd1 [cmd2 ... cmdN]", argv[0]);
    }

    int success = 0;
    int failed = 0;

    for(int i = 1; i < argc; i++) {
        pid_t pid = fork();
        if(pid < 0) errx(2, "fork failed");

        if(pid == 0) {
            // Дете изпълнява командата
            execlp(argv[i], argv[i], (char*)NULL);
            errx(3, "failed to exec %s", argv[i]);
        }

        // Родителят чака детето
        int status;
        if(wait(&status) == -1) errx(4, "wait failed");

        if(WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            success++;
        } else {
            failed++;
        }
    }

    printf("Successful commands: %d\n", success);
    printf("Failed commands: %d\n", failed);

    return 0;
}
