//Напишете програма на C, която да работи подобно на командата cut, реализирайки
//само следната функционалност:
//• програмата трябва да чете текст от стандартния вход и да извежда избраната част от всеки
//        ред на стандартния изход;
//• ако първият параметър на програмата е низът -c, тогава вторият параметър е или едноцифрено число (от 1 до 9),
//или две едноцифрени числа N и M (N ≤ M), разделени с тире (напр. 3-5).
//В този случай програмата трябва да изведе избраният/избраните символи от реда: или само
//символа на указаната позиция, или няколко последователни символи на посочените позиции.
//• ако първият параметър на програмата е низът -d, тогава вторият параметър е низ, от който е
//        важен само първият символ; той се използва като разделител между полета на реда. Третият
//        параметър трябва да бъде низът -f, а четвъртият - или едноцифрено число (от 1 до 9),
//или две едноцифрени числа N и M (N ≤ M), разделени с тире (напр. 3-5). В този случай
//        програмата трябва да разглежда реда като съставен от няколко полета (може би празни),
//разделени от указания символ (първият символ от низа, следващ парметъра -d), и да изведе
//        или само указаното поле, или няколко последователни полета на указаните позиции, разделени
//        от същия разделител.
//• ако някой ред няма достатъчно символи/полета, за него програмата трябва да изведе каквото
//(докъдето) е възможно (или дори празен ред).

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 4096

// Парсираме диапазон N или N-M без sscanf
void parse_range(const char *s, int *start, int *end) {
    int i = 0;
    *start = 0;
    *end = 0;

    // Превръщаме първото число
    while (s[i] >= '0' && s[i] <= '9') {
        *start = *start * 10 + (s[i] - '0');
        i++;
    }

    if (s[i] == '-') {
        i++;
        while (s[i] >= '0' && s[i] <= '9') {
            *end = *end * 10 + (s[i] - '0');
            i++;
        }
    } else {
        *end = *start;
    }
}

// Режим по символи
void cut_chars(const char *range_str) {
    int start, end;
    parse_range(range_str, &start, &end);

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        if (line[len - 1] == '\n') len--; // махаме newline
        int i;
        for (i = start - 1; i < len && i < end; i++) {
            putchar(line[i]);
        }
        putchar('\n');
    }
}

// Режим по полета
void cut_fields(char sep, const char *range_str) {
    int start, end;
    parse_range(range_str, &start, &end);

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; // премахваме newline
        char *fields[256];
        int count = 0;
        char *token = strtok(line, &sep);
        while (token && count < 256) {
            fields[count++] = token;
            token = strtok(NULL, &sep);
        }

        int i;
        for (i = start - 1; i < count && i < end; i++) {
            if (i > start - 1) putchar(sep);
            fputs(fields[i], stdout);
        }
        putchar('\n');
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "%s -c N[-M]\n", argv[0]);
        fprintf(stderr, "%s -d SEP -f N[-M]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-c") == 0) {
        cut_chars(argv[2]);
    } else if (strcmp(argv[1], "-d") == 0) {
        if (argc != 5 || strcmp(argv[3], "-f") != 0) {
            fprintf(stderr, "Usage: %s -d SEP -f N[-M]\n", argv[0]);
            return 1;
        }
        cut_fields(argv[2][0], argv[4]);
    } else {
        fprintf(stderr, "Unknown option %s\n", argv[1]);
        return 1;
    }

    return 0;
}
