//При изграждане на система за пренасяне на сериен асинхронен сигнал върху радиопреносна мрежа се оказало, че големи поредици от битове само нули или само единици смущават
//        сигнала, поради нестабилно ниво. Инженерите решили проблема, като:
//• в моментите, в които няма сигнал от серийният порт, вкарвали изкуствено байт 0x55 в потока;
//• реалните байтове 0x00, 0xFF, 0x55 и 0x7D се кодирали посредством XOR-ване (побитова обработка с изключващо-или) с 0x20, като полученият байт се изпращал през потока, предхождан
//        от 0x7D, който играе ролята на escape character.
//Разполагате със запис от такъв поток. Напишете програма на C, която приема два параметъра -
//имена на файлове. Примерно извикване:
//$ ./main input.lfld output.bin
//        Програмата трябва да обработва записа и да генерира output.bin, който да съдържа оригиналните данни. Четенето на входните данни трябва да става посредством изпълнение на външна shell
//команда.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <err.h>

#define ESC 0x7D

int main(int argc, char *argv[]) {
    if (argc != 3) {
        errx(1, "Usage: %s input_file output_file", argv[0]);
    }

    int fd[2];
    if (pipe(fd) == -1) err(2, "pipe");

    pid_t pid = fork();
    if (pid < 0) err(3, "fork");

    if (pid == 0) {
        // дете -> извикваме shell команда за четене на input.lfld
        dup2(fd[1], STDOUT_FILENO); // stdout -> pipe
        close(fd[0]); close(fd[1]);
        execlp("cat", "cat", argv[1], (char*)NULL);
        err(4, "exec cat");
    } else {
        // родител -> чете от pipe и декодира
        close(fd[1]);
        int out_fd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0666);
        if (out_fd < 0) err(5, "open output file");

        int escape = 0;
        unsigned char byte;
        while (read(fd[0], &byte, 1) == 1) {
            if (escape) {
                byte ^= 0x20;        // декодираме байта
                write(out_fd, &byte, 1);
                escape = 0;
            } else if (byte == ESC) {
                escape = 1;          // следващият байт е “escaped”
            } else if (byte != 0x55) {
                // нормален байт (0x55 е изкуствено добавен сигнал за нули/единици)
                write(out_fd, &byte, 1);
            }
            // ако byte == 0x55 -> игнорираме
        }

        close(out_fd);
        close(fd[0]);
        waitpid(pid, NULL, 0);
    }

    return 0;
}
