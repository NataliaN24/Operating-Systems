//Ваши колеги - асистенти по ОС имат нужда от демострационна програма на C, която
//да служи като пример за конкурентност и синхронизация на процеси. Напишете такава програма,
//приемаща два задължителни позиционни параметъра – едноцифрени числа. Примерно извикване:
//./main N D
//        Общ алгоритъм на програмата:
//• началният (родителски) процес създава процес-наследник
//• N на брой пъти се изпълнява:
//– родителският процес извежда на stdout низа “DING ”
//– процесът-наследник извежда на stdout низа “DONG”
//– родителският процес изчаква D секунди
//        Гарантирайте, че:
//• процесът-наследник винаги извежда “DONG” след като родителския процес е извел “DING ”
//• родителският процес винаги започва изчакването след като процеса-наследник е извел “DONG”
//Забележка: За изчакването погледнете sleep(3).

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s N D\n", argv[0]);
        return 1;
    }

    int N = argv[1][0] - '0'; // брой повторения
    int D = argv[2][0] - '0'; // изчакване в секунди

    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // child -> наследник
        close(pipe1[1]); // child чете от pipe1
        close(pipe2[0]); // child пише в pipe2

        char buf;
        for (int i = 0; i < N; i++) {
            // чака сигнал от родителя (DING)
            read(pipe1[0], &buf, 1);
            printf("DONG\n");
            fflush(stdout);

            // сигнализира на родителя
            write(pipe2[1], "x", 1);
        }

        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    } else {
        // parent
        close(pipe1[0]); // parent пише в pipe1
        close(pipe2[1]); // parent чете от pipe2

        char buf;
        for (int i = 0; i < N; i++) {
            printf("DING ");
            fflush(stdout);

            // сигнализираме на child
            write(pipe1[1], "x", 1);

            // чакаме DONG от child
            read(pipe2[0], &buf, 1);

            // изчакваме D секунди
            sleep(D);
        }

        close(pipe1[1]);
        close(pipe2[0]);

        wait(NULL); // чакаме child да завърши
    }

    return 0;
}
