//Да се напише програма на C, която получава като параметри
от команден ред две команди (без параметри) и име на файл
//в текущата директория. Ако файлът не съществува, го създава.
  Програмата изпълнява командите последователно,
//по реда на подаването им. Ако първата команда завърши успешно,
  програмата добавя нейното име към съдържанието на файла,
//подаден като команден параметър. Ако командата завърши неуспешно,
  програмата уведомява потребителя чрез подходящо съобщение.

  #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[]) {
    if(argc != 4) {
        errx(1, "Usage: %s <cmd1> <cmd2> <filename>", argv[0]);
    }

    char* cmd1 = argv[1];
    char* cmd2 = argv[2];
    char* filename = argv[3];

    int status;
    pid_t pid = fork();
    if(pid < 0) errx(2, "fork failed");

    if(pid == 0) {
        // Детето изпълнява първата команда
        execlp(cmd1, cmd1, (char*)NULL);
        errx(3, "failed to exec %s", cmd1);
    }

    // Родителят чака първата команда
    if(wait(&status) == -1) errx(4, "wait failed");

    if(WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        // Успешно завършване → добавяме името на командата във файла
        int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if(fd == -1) errx(5, "cannot open file %s", filename);

        if(write(fd, cmd1, strlen(cmd1)) != (ssize_t)strlen(cmd1))
            errx(6, "write failed");

        if(write(fd, "\n", 1) != 1)
            errx(7, "write newline failed");

        close(fd);
        printf("Command '%s' executed successfully and added to file '%s'.\n", cmd1, filename);
    } else {
        printf("Command '%s' failed. File not modified.\n", cmd1);
    }

    // Изпълняваме втората команда
    pid = fork();
    if(pid < 0) errx(8, "fork failed");

    if(pid == 0) {
        execlp(cmd2, cmd2, (char*)NULL);
        errx(9, "failed to exec %s", cmd2);
    }

    if(wait(&status) == -1) errx(10, "wait failed");

    return 0;
}
