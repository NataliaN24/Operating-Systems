Напишете програма на C, която да работи подобно на командата cat, реализирайки само
следната функционалност:
• общ вид на изпълнение: ./main [OPTION] [FILE]...
• ако е подаден като първи параметър -n, то той да се третира като опция, което кара програмата
ви да номерира (глобално) всеки изходен ред (започвайки от 1).
• програмата извежда на STDOUT
• ако няма подадени параметри (имена на файлове), програмата чете от STDIN
• ако има подадени параметри – файлове, програмата последователно ги извежда
• ако някой от параметрите е тире (-), програмата да го третира като специално име за STDIN
Примерно извикване:
$ cat a.txt
a1
a2
$ cat b.txt
b1
b2
b3
$ echo -e "s1\ns2" | ./main -n a.txt - b.txt
1 a1
2 a2
3 s1
4 s2
5 b1
6 b2
7 b3
  Забележка: Погледнете setbuf(3) и strcmp(3)

  #include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_file(FILE *fp, int *line_number, int number_lines) {
    char buffer[4096];

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (number_lines) {
            printf("%d %s", (*line_number)++, buffer);
            if (buffer[strlen(buffer)-1] != '\n') {
                printf("\n"); // ако редът няма нов ред в края
            }
        } else {
            fputs(buffer, stdout);
        }
    }
}

int main(int argc, char **argv) {
    int line_numbering = 0;
    int line_count = 1;
    int first_arg = 1;

    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        line_numbering = 1;
        first_arg = 2;
    }

    if (argc <= first_arg) {
        // няма файлове, четем от STDIN
        print_file(stdin, &line_count, line_numbering);
    } else {
        for (int i = first_arg; i < argc; i++) {
            FILE *fp;

            if (strcmp(argv[i], "-") == 0) {
                fp = stdin;
            } else {
                fp = fopen(argv[i], "r");
                if (!fp) {
                    fprintf(stderr, "Cannot open file: %s\n", argv[i]);
                    continue;
                }
            }

            print_file(fp, &line_count, line_numbering);

            if (fp != stdin) fclose(fp);
        }
    }

    return 0;
}

