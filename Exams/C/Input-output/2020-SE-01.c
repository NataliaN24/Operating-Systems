Напишете програма на C, която приема шест задължителни позиционни параметъра –
имена на файлове. Примерно извикване:
$ ./main affix postfix prefix infix suffix crucifixus
Всички файлове започват с хедър с фиксирана дължина от 16 байта. Пети и шести (спрямо ℤ
+) байт
от хедъра дефинират uint16_t число count, което описва броя на елементите във файла. Файловете
affix и infix се състоят от елементи от тип uint16_t, файловете prefix и crucifixus – от елементи
от тип uint8_t, postfix – от uint32_t, а suffix – от uint64_t.
Интервал наричаме наредена двойка числa, която дефинира номер (спрямо ℤ) на начален елемент
и брой елементи от даден файл. Комплект наричаме наредена четворка от интервали, които позиционно се отнасят спрямо файловете {post,pre,in,suf}fix.
Елементите на файлa affix дефинират серия от комплекти, на чиято база програмата трябва да генерира изходния файл crucifixus.

	#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

#define HEADER_SIZE 16

int read_count(int fd) {
    uint8_t header[HEADER_SIZE];
    if (lseek(fd, 0, SEEK_SET) < 0) err(1, "lseek failed");
    if (read(fd, header, HEADER_SIZE) != HEADER_SIZE) err(2, "read header failed");
    return header[4] | (header[5] << 8); // little-endian uint16_t
}

void copy_interval(int fd_src, int fd_dst, off_t start, size_t count, size_t elem_size) {
    char buf[4096];
    off_t offset = HEADER_SIZE + start * elem_size;
    if (lseek(fd_src, offset, SEEK_SET) < 0) err(3, "lseek src failed");
    for (size_t i = 0; i < count; ) {
        size_t to_read = (count - i) * elem_size;
        if (to_read > sizeof(buf)) to_read = sizeof(buf);
        ssize_t r = read(fd_src, buf, to_read);
        if (r <= 0) err(4, "read src failed");
        if (write(fd_dst, buf, r) != r) err(5, "write dst failed");
        i += r / elem_size;
    }
}

int main(int argc, char **argv) {
    if (argc != 7) errx(1, "Usage: ./main affix postfix prefix infix suffix crucifixus");

    const char *affix_file = argv[1];
    const char *postfix_file = argv[2];
    const char *prefix_file = argv[3];
    const char *infix_file = argv[4];
    const char *suffix_file = argv[5];
    const char *crucifixus_file = argv[6];

    int fd_affix = open(affix_file, O_RDONLY);
    int fd_post = open(postfix_file, O_RDONLY);
    int fd_pre = open(prefix_file, O_RDONLY);
    int fd_in = open(infix_file, O_RDONLY);
    int fd_suf = open(suffix_file, O_RDONLY);
    int fd_out = open(crucifixus_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (fd_affix < 0 || fd_post < 0 || fd_pre < 0 || fd_in < 0 || fd_suf < 0 || fd_out < 0)
        err(2, "cannot open input/output files");

    int affix_count = read_count(fd_affix);

    // Структура на един комплект: четворка от интервали
    typedef struct { uint16_t start; uint16_t len; } interval_t;
    interval_t *sets = malloc(sizeof(interval_t) * 4 * affix_count);
    if (!sets) errx(3, "malloc failed");

    // Четем всички комплекти от affix
    if (lseek(fd_affix, HEADER_SIZE, SEEK_SET) < 0) err(4, "lseek affix");
    if (read(fd_affix, sets, sizeof(interval_t) * 4 * affix_count) != sizeof(interval_t) * 4 * affix_count)
        err(5, "read affix failed");

    // Генерираме crucifixus
    for (int i = 0; i < affix_count; i++) {
        interval_t post = sets[4*i + 0];
        interval_t pre  = sets[4*i + 1];
        interval_t in   = sets[4*i + 2];
        interval_t suf  = sets[4*i + 3];

        copy_interval(fd_post, fd_out, post.start, post.len, sizeof(uint32_t));
        copy_interval(fd_pre, fd_out, pre.start, pre.len, sizeof(uint8_t));
        copy_interval(fd_in, fd_out, in.start, in.len, sizeof(uint16_t));
        copy_interval(fd_suf, fd_out, suf.start, suf.len, sizeof(uint64_t));
    }

    free(sets);
    close(fd_affix); close(fd_post); close(fd_pre);
    close(fd_in); close(fd_suf); close(fd_out);

    return 0;
}

