//Напишете програма на C, която да работи подобно на командата cat, реализирайки
//само следната функционалност:
//• програмата извежда на STDOUT
//• ако няма подадени параметри, програмата чете от STDIN
//• ако има подадени параметри – файлове, програмата последователно ги извежда
//• ако някой от параметрите започва с тире (-), програмата да го третира като специално име
//        за STDIN
//        Примерно извикване:
//$ ./main f - g
//– извежда съдържанието на файла f, после STDIN, след това съдържанието на файла g.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void copy_file(FILE *fp) {
    char buffer[4096];
    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if (fwrite(buffer, 1, n, stdout) != n) {
            perror("Write error");
            exit(1);
        }
    }

    if (ferror(fp)) {
        perror("Read error");
        exit(1);
    }
}

int main(int argc, char **argv) {
    if (argc == 1) {
        // няма аргументи, четем от STDIN
        copy_file(stdin);
    } else {
        for (int i = 1; i < argc; i++) {
            FILE *fp;
            if (strcmp(argv[i], "-") == 0) {
                fp = stdin;
            } else {
                fp = fopen(argv[i], "rb");
                if (!fp) {
                    perror(argv[i]);
                    continue;
                }
            }

            copy_file(fp);

            if (fp != stdin) fclose(fp);
        }
    }

    return 0;
}
