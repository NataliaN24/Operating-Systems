//Напишете програма-наблюдател P, която изпълнява друга програма Q и я рестартира, когато Q завърши изпълнението си. На командния ред на P се подават следните параметри:
//• праг за продължителност в секунди – едноцифрено число от 1 до 9
//• Q
//• незадължителни параметри на Q
//P работи по следния алгоритъм:
//• стартира Q с подадените параметри
//• изчаква я да завърши изпълнението си
//• записва в текстов файл run.log един ред с три полета - цели числа (разделени с интервал):
//– момент на стартиране на Q (Unix time)
//– момент на завършване на Q (Unix time)
//– код за грешка, с който Q е завършила (exit code)
//• проверява дали е изпълнено условието за спиране и ако не е, преминава отново към стартирането на Q
//Условие за спиране: Ако наблюдателят P установи, че при две последователни изпълнения на Q са
//        били изпълнени и двете условия:
//1. кодът за грешка на Q е бил различен от 0;
//2. разликата между момента на завършване и момента на стартиране на Q е била по-малка от
//подадения като първи параметър на P праг;
//то P спира цикъла от изпълняване на Q и сам завършва изпълнението си.
//Текущото време във формат Unix time (секунди от 1 януари 1970 г.) можете да вземете с извикване
//на системната функция time() с параметър NULL; функцията е дефинирана в time.h. Ако изпълнената програма е била прекъсната от подаден сигнал, това се приема за завършване с код за грешка
//129.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <threshold> <Q> [args...]\n", argv[0]);
        return 1;
    }

    int threshold = argv[1][0] - '0'; // едноцифрен праг
    if (threshold < 1 || threshold > 9) {
        fprintf(stderr, "Threshold must be 1-9\n");
        return 1;
    }

    char **q_args = &argv[2]; // Q и параметри

    // Предишен run
    int prev_exit = 0;
    time_t prev_start = 0, prev_end = 0;

    FILE *logfile = fopen("run.log", "a");
    if (!logfile) {
        perror("fopen");
        return 1;
    }

    while (1) {
        time_t start = time(NULL); // момент на стартиране

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }

        int exit_code;
        if (pid == 0) {
            // дете -> изпълнява Q
            execvp(q_args[0], q_args);
            // ако exec не успее:
            perror("exec");
            exit(1);
        } else {
            // родител -> чака Q
            int status;
            waitpid(pid, &status, 0);

            time_t end = time(NULL); // момент на завършване

            if (WIFEXITED(status)) {
                exit_code = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                exit_code = 129; // прекъснато от сигнал
            } else {
                exit_code = 1; // друг случай
            }

            // логваме изпълнението
            fprintf(logfile, "%ld %ld %d\n", start, end, exit_code);
            fflush(logfile);

            // проверка на условието за спиране
            if (prev_exit != 0 && exit_code != 0) {
                if ((end - start) < threshold && (prev_end - prev_start) < threshold) {
                    // двете последователни изпълнения са кратки и с код != 0
                    break;
                }
            }

            // запазваме за следващата итерация
            prev_start = start;
            prev_end = end;
            prev_exit = exit_code;
        }
    }

    fclose(logfile);
    return 0;
}
