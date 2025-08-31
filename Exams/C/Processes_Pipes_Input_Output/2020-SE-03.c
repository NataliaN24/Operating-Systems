//Напишете програма на C, която приема един задължителен позиционен параметър
//- име на файл. Файлът се състои от не повече от 8 наредени тройки елементи:
//• име на файл – точно 8 байта, последният от които задължително е 0x00. Ако името е по-късо
//        от 7 знака, излишните байтове са 0x00;
//• offset – uint32_t, който дава пореден номер на елемент (спрямо N0) във файла;
//• length – uint32_t, който дава брой елементи.
//За всяка наредена тройка програмата трябва да пусне child процес, който да XOR-не (обработи с
//        изключващо-или) елементите (uint16_t) от съответния файл един със друг, и да върне резултата
//        на parent процеса, който от своя страна трябва да XOR-не всички получените резултати и да изведе
//        полученото число в следния формат:
//result: 573B
//Забележка: С пълен брой точки се оценяват решения, в които child процесите работят паралелно

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <err.h>
#include <string.h>

#define MAX_TRIPLE 8

typedef struct {
    char name[8];     // име на файл, последният байт е 0x00
    uint32_t offset;  // пореден номер на елемент
    uint32_t length;  // брой елементи
} triple_t;

uint16_t xor_file_range(const char *filename, uint32_t offset, uint32_t length) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    // пропускаме offset uint16_t елементи
    if (lseek(fd, offset * sizeof(uint16_t), SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }

    uint16_t buf;
    uint16_t result = 0;
    for (uint32_t i = 0; i < length; i++) {
        if (read(fd, &buf, sizeof(uint16_t)) != sizeof(uint16_t)) {
            perror("read");
            exit(1);
        }
        result ^= buf;
    }

    close(fd);
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <triples_file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) err(1, "open triples file");

    triple_t triples[MAX_TRIPLE];
    ssize_t read_bytes = read(fd, triples, sizeof(triples));
    if (read_bytes < 0) err(2, "read triples");
    int triple_count = read_bytes / sizeof(triple_t);

    int pipes[MAX_TRIPLE][2];

    for (int i = 0; i < triple_count; i++) {
        if (pipe(pipes[i]) == -1) err(3, "pipe");

        pid_t pid = fork();
        if (pid < 0) err(4, "fork");

        if (pid == 0) {
            // child
            close(pipes[i][0]); // затваряме read end

            uint16_t res = xor_file_range(triples[i].name, triples[i].offset, triples[i].length);
            if (write(pipes[i][1], &res, sizeof(res)) != sizeof(res)) {
                perror("write pipe");
                exit(1);
            }
            close(pipes[i][1]);
            exit(0);
        } else {
            close(pipes[i][1]); // родителят затваря write end
        }
    }

    // родителят събира резултатите
    uint16_t final_res = 0;
    for (int i = 0; i < triple_count; i++) {
        uint16_t child_res;
        if (read(pipes[i][0], &child_res, sizeof(child_res)) != sizeof(child_res)) {
            perror("read pipe");
            exit(1);
        }
        close(pipes[i][0]);
        final_res ^= child_res;
    }

    // чакаме всички деца
    for (int i = 0; i < triple_count; i++) wait(NULL);

    printf("result: %04X\n", final_res);

    close(fd);
    return 0;
}
