//Напишете програма на C, която приема незадължителен параметър – име на команда. Ако не е зададена команда като параметър, да се ползва командата echo. Максималната
//        допустима дължина на командата е 4 знака.
//Програмата чете низове (с максимална дължина 4 знака) от стандартния си вход, разделени с
//интервали (0x20) или знак за нов ред (0x0A). Ако някой низ е с дължина по-голяма от 4 знака, то
//        програмата да терминира със съобщение за грешка.
//Подадените на стандартния вход низове програмата трябва да третира като множество от параметри за дефинираната команда. Програмата ви трябва да изпълни командата колкото пъти е
//        необходимо с максимум два низа като параметри, като изчаква изпълнението да приключи, преди
//да започне ново изпълнение.
//Примерни вход, извиквания и изходи:
//$ cat f1
//        a1
//$ cat f2
//        a2
//$ cat f3
//        a3
//$ echo -e "f1\nf2 f3" | ./main cat
//a1
//a2
//a3
//$ echo -e "f1\nf2 f3" | ./main
//f1 f2
//f3

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LEN 4        // максимална дължина на низ
#define BUF_SIZE 1024    // буфер за stdin
#define MAX_ARGS 2       // максимум 2 параметъра

int main(int argc, char *argv[]) {
    char *cmd;

    // определяме команда
    if (argc == 2) {
        if (strlen(argv[1]) > MAX_LEN) {
            const char *msg = "Command too long\n";
            write(2, msg, strlen(msg));
            return 1;
        }
        cmd = argv[1];
    } else {
        cmd = "echo";
    }

    char buf[BUF_SIZE];
    int n;

    while ((n = read(0, buf, BUF_SIZE - 1)) > 0) {  // четем stdin
        buf[n] = '\0';

        int i = 0;
        while (i < n) {
            char *args[MAX_ARGS + 2]; // команда + 2 аргумента + NULL
            args[0] = cmd;
            int arg_count = 1;

            // взимаме до 2 низа
            while (arg_count <= MAX_ARGS && i < n) {
                // пропускаме интервали и нов ред
                while (i < n && (buf[i] == ' ' || buf[i] == '\n')) i++;
                if (i >= n) break;

                // намираме края на текущия низ
                int start = i;
                while (i < n && buf[i] != ' ' && buf[i] != '\n') i++;
                int len = i - start;
                if (len > MAX_LEN) {
                    const char *msg = "String too long\n";
                    write(2, msg, strlen(msg));
                    exit(1);
                }

                // създаваме низа за exec
                buf[start + len] = '\0'; // затваряме с '\0'
                args[arg_count++] = &buf[start];
            }
            args[arg_count] = NULL;

            if (arg_count > 1) { // ако има поне един параметър
                pid_t pid = fork();
                if (pid < 0) {
                    const char *msg = "fork error\n";
                    write(2, msg, strlen(msg));
                    exit(1);
                }
                if (pid == 0) {
                    execvp(cmd, args);
                    const char *msg = "exec error\n";
                    write(2, msg, strlen(msg));
                    exit(1);
                } else {
                    int status;
                    waitpid(pid, &status, 0);
                }
            }
        }
    }

    return 0;
}
