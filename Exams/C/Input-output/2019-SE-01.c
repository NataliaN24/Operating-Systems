 В приложната статистика често се използват следните описателни статистики за дадена
извадка:
47
• средна стойност ̄𝑥 = 1
𝑁
𝑁
∑
𝑖=1
𝑥𝑖
• дисперсия 𝐷 = 1
𝑁
𝑁
∑
𝑖=1
(𝑥𝑖 − ̄𝑥)2
където {𝑥1
, 𝑥2
, … , 𝑥𝑁} са стойностите на извадката, а 𝑁 е техния брой.
Напишете програма на C, която приема задължителен параметър – име на двоичен файл. Файлът
съдържа информация за потребители, които са влизали в системата, и се състои от наредени петорки
от следните елементи и техните типове:
• уникален потребителски идентификатор (UID) uint32_t
• запазено1 uint16_t – не се използва в задачата
• запазено2 uint16_t – не се използва в задачата
• време1 uint32_t – момент на започване на сесията (Unix time)
• време2 uint32_t – момент на завършване на сесията (Unix time)
За потребители, които са имали сесии, квадратът на продължителността на които е по-голям от дисперсията 𝐷 на продължителността на всички сесии във файла, програмата трябва да изведе на STDOUT
потребителският им идентификатор и продължителността на най-дългата им сесия.

    #include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <math.h>

typedef struct {
    uint32_t uid;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t time1;
    uint32_t time2;
} record_t;

int main(int argc, char **argv) {
    if (argc != 2) {
        errx(1, "Usage: %s <file>", argv[0]);
    }

    const char *file_name = argv[1];
    int fd = open(file_name, O_RDONLY);
    if (fd < 0) err(2, "Cannot open %s", file_name);

    // Четем целия файл в памет (максимум 100 000 записа)
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size < 0) err(3, "lseek failed");
    if (lseek(fd, 0, SEEK_SET) < 0) err(4, "lseek failed");

    size_t n = file_size / sizeof(record_t);
    if (n == 0) {
        close(fd);
        return 0;
    }

    record_t *records = malloc(n * sizeof(record_t));
    if (!records) errx(5, "Memory allocation failed");

    ssize_t bytes_read = read(fd, records, n * sizeof(record_t));
    if (bytes_read < 0) err(6, "Read error");
    if ((size_t)bytes_read != n * sizeof(record_t)) errx(7, "Could not read full file");

    close(fd);

    // Изчисляваме продължителностите
    double sum = 0.0;
    double sum_sq = 0.0;
    uint32_t *durations = malloc(n * sizeof(uint32_t));
    if (!durations) errx(8, "Memory allocation failed");

    for (size_t i = 0; i < n; i++) {
        uint32_t dur = records[i].time2 - records[i].time1;
        durations[i] = dur;
        sum += dur;
    }

    double mean = sum / n;

    double variance = 0.0;
    for (size_t i = 0; i < n; i++) {
        double diff = durations[i] - mean;
        variance += diff * diff;
    }
    variance /= n;

    // Намираме най-дългата сесия за всеки UID и проверяваме условието
    typedef struct {
        uint32_t uid;
        uint32_t max_dur;
    } user_t;

    user_t *users = malloc(n * sizeof(user_t));
    if (!users) errx(9, "Memory allocation failed");
    size_t user_count = 0;

    for (size_t i = 0; i < n; i++) {
        uint32_t uid = records[i].uid;
        uint32_t dur = durations[i];
        int found = 0;
        for (size_t j = 0; j < user_count; j++) {
            if (users[j].uid == uid) {
                if (dur > users[j].max_dur) users[j].max_dur = dur;
                found = 1;
                break;
            }
        }
        if (!found) {
            users[user_count].uid = uid;
            users[user_count].max_dur = dur;
            user_count++;
        }
    }

    for (size_t i = 0; i < user_count; i++) {
        if ((double)(users[i].max_dur) * (double)(users[i].max_dur) > variance) {
            printf("%u %u\n", users[i].uid, users[i].max_dur);
        }
    }

    free(records);
    free(durations);
    free(users);

    return 0;
}

Можете да разчитате на това, че във файла ще има не повече о
