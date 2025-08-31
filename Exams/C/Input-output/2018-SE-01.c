//Напишете програма на C, която да работи подобно на командата tr, реализирайки
//само следната функционалност:
//• програмата чете от стандартния вход и пише на стандартния изход
//33
//• общ вид на изпълнение: ./main [OPTION] SET1 [SET2]
//• OPTION би могъл да бъде или -d, или -s, или да липсва
//• SET1 и SET2 са низове, в които знаците нямат специално значение, т.е. всеки знак “означава”
//съответния знак. SET2, ако е необходим, трябва да е същата дължина като SET1
//• ако е подаден като първи параметър -d, програмата трие от входа всяко срещане на знак µ
//        от SET1; SET2 не е необходим
//• ако е подаден като първи параметър -s, програмата заменя от входа всяка поредица от повторения знак µ от SET1 с
//еднократен знак µ; SET2 не е необходим
//• в останалите случаи програмата заменя от входа всеки знак µ от SET1 със съответстващият
//му позиционно знак ν от SET2.
//Примерно извикване:
//$ echo asdf | ./main -d ’d123’ | ./main ’sm’ ’fa’ | ./main -s ’f’
//af

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [OPTION] SET1 [SET2]\n", argv[0]);
        return 1;
    }

    int mode_delete = 0;
    int mode_squeeze = 0;
    char *set1 = NULL;
    char *set2 = NULL;

    if (strcmp(argv[1], "-d") == 0) {
        mode_delete = 1;
        if (argc != 3) {
            fprintf(stderr, "-d requires SET1\n");
            return 1;
        }
        set1 = argv[2];
    } else if (strcmp(argv[1], "-s") == 0) {
        mode_squeeze = 1;
        if (argc != 3) {
            fprintf(stderr, "-s requires SET1\n");
            return 1;
        }
        set1 = argv[2];
    } else {
        if (argc != 3) {
            fprintf(stderr, "Replacement mode requires SET1 and SET2\n");
            return 1;
        }
        set1 = argv[1];
        set2 = argv[2];
        if (strlen(set1) != strlen(set2)) {
            fprintf(stderr, "SET1 and SET2 must have same length\n");
            return 1;
        }
    }

    int last_written[256] = {0}; // за -s
    int c;
    while ((c = getchar()) != EOF) {
        unsigned char uc = (unsigned char)c;

        if (mode_delete) {
            if (strchr(set1, uc)) continue;
            putchar(uc);
        } else if (mode_squeeze) {
            if (strchr(set1, uc)) {
                if (!last_written[uc]) {
                    putchar(uc);
                    last_written[uc] = 1;
                }
            } else {
                putchar(uc);
                last_written[uc] = 0;
            }
        } else {
            char *p = strchr(set1, uc);
            if (p) {
                size_t idx = p - set1;
                putchar(set2[idx]);
            } else {
                putchar(uc);
            }
        }
    }

    return 0;
}
