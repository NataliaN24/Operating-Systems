#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>

typedef struct {
    uint32_t uid;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t start;
    uint32_t end;
} record;

int main(int argc, char *argv[])
{
    if (argc != 2)
        errx(1, "wrong arguments");

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
        err(1, "open");

    record records[16384];
    int n = 0;

    uint64_t sum = 0;

    // Четене на файла
    while (read(fd, &records[n], sizeof(record)) == sizeof(record)) {
        if (records[n].end < records[n].start)
            errx(2, "invalid time");

        sum += records[n].end - records[n].start;
        n++;
    }

    close(fd);

    if (n == 0)
        return 0;

    // Средна стойност
    double x = (double)sum / n;

    // Дисперсия
    double D = 0;

    for (int i = 0; i < n; i++) {
        double t = records[i].end - records[i].start;
        D += (t - x) * (t - x);
    }

    D /= n;


    // Масив за различните UID-та
    uint32_t ids[2048];
    uint32_t maxSession[2048];
    int count = 0;


    // Намиране на най-дългата сесия за всеки UID
    for (int i = 0; i < n; i++) {

        uint32_t duration = records[i].end - records[i].start;

        int found = -1;

        for (int j = 0; j < count; j++) {
            if (ids[j] == records[i].uid) {
                found = j;
                break;
            }
        }

        if (found == -1) {
            ids[count] = records[i].uid;
            maxSession[count] = duration;
            count++;
        }
        else {
            if (duration > maxSession[found])
                maxSession[found] = duration;
        }
    }


    // Извеждане
    for (int i = 0; i < count; i++) {

        double square = (double)maxSession[i] * maxSession[i];

        if (square > D) {
            char buf[64];
            int len = 0;

            // прост print без printf
            len += sprintf(buf + len, "%u %u\n",
                           ids[i], maxSession[i]);

            write(1, buf, len);
        }
    }

    return 0;
}
/////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>

typedef struct {
    uint32_t uid;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t start;
    uint32_t end;
} record;

typedef struct {
    uint32_t uid;
    uint32_t maxDuration;
    int shouldPrint;
} userInfo;

int findUser(userInfo *users, size_t usersCount, uint32_t uid)
{
    for (size_t i = 0; i < usersCount; i++) {
        if (users[i].uid == uid) {
            return i;
        }
    }
    return -1;
}

double avg(record *arr, size_t count)
{
    uint64_t sum = 0;

    for (size_t i = 0; i < count; i++) {
        uint32_t duration = arr[i].end - arr[i].start;
        sum += duration;
    }

    return (double)sum / count;
}

double getVariance(double mean, record *arr, size_t count)
{
    double var = 0;

    for (size_t i = 0; i < count; i++) {
        double duration = (double)(arr[i].end - arr[i].start);
        double diff = duration - mean;
        var += diff * diff;
    }

    return var / count;
}

void getUsers(record *arr, size_t count, double var)
{
    userInfo users[2048];
    size_t usersCount = 0;

    for (size_t i = 0; i < count; i++) {

        uint32_t uid = arr[i].uid;
        uint32_t duration = arr[i].end - arr[i].start;

        int idx = findUser(users, usersCount, uid);

        if (idx == -1) {
            users[usersCount].uid = uid;
            users[usersCount].maxDuration = duration;
            users[usersCount].shouldPrint = 0;
            idx = usersCount;
            usersCount++;
        }

        if (duration > users[idx].maxDuration) {
            users[idx].maxDuration = duration;
        }

        if ((double)duration * duration > var) {
            users[idx].shouldPrint = 1;
        }
    }

    for (size_t i = 0; i < usersCount; i++) {
        if (users[i].shouldPrint) {
            dprintf(1, "%u %u\n", users[i].uid, users[i].maxDuration);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        errx(1, "Usage: %s file", argv[0]);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        err(1, "open");
    }

    struct stat st;
    if (stat(argv[1], &st) < 0) {
        err(1, "stat");
    }

    if (st.st_size % sizeof(record) != 0) {
        errx(1, "invalid file");
    }

    size_t count = st.st_size / sizeof(record);

    record *arr = malloc(count * sizeof(record));
    if (!arr) {
        err(1, "malloc");
    }

    ssize_t bytes = read(fd, arr, count * sizeof(record));
    if (bytes < 0) {
        err(1, "read");
    }

    double mean = avg(arr, count);
    double variance = getVariance(mean, arr, count);

    getUsers(arr, count, variance);

    free(arr);
    close(fd);

    return 0;
}
