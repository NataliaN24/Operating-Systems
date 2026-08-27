#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <err.h>

typedef struct {
    uint64_t id;
    uint8_t len;
    char text[256];
    char role[256];
} rec;

int compare(const void* a, const void* b)
{
    const rec* r1 = a;
    const rec* r2 = b;

    if (r1->id < r2->id) {
        return -1;
    }

    if (r1->id > r2->id) {
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 21) {
        errx(1, "usage: ./main file1 file2 ...");
    }

    int n = argc - 1;

    /*
     * Максимумът е 20 файла.
     * Не знаем колко records има във всеки,
     * затова динамично увеличаваме масива.
     */
    rec* records = NULL;
    int count = 0;
    int capacity = 0;

    for (int i = 0; i < n; i++) {

        int fd = open(argv[i + 1], O_RDONLY);

        if (fd < 0) {
            err(1, "open");
        }

        /*
         * Четем header.
         */
        rec header;

        if (read(fd, &header.id, sizeof(header.id))
            != sizeof(header.id)) {
            errx(1, "cannot read header id");
        }

        if (read(fd, &header.len, sizeof(header.len))
            != sizeof(header.len)) {
            errx(1, "cannot read header length");
        }

        if (read(fd, header.text, header.len)
            != header.len) {
            errx(1, "cannot read role");
        }

        header.text[header.len] = '\0';

        /*
         * Проверяваме header ID.
         */
        if (header.id != 133742) {
            errx(1, "invalid header");
        }

        /*
         * Четем всички реплики от този файл.
         */
        while (1) {

            rec current;

            ssize_t bytes = read(fd,
                                 &current.id,
                                 sizeof(current.id));

            if (bytes == 0) {
                break;
            }

            if (bytes != sizeof(current.id)) {
                errx(1, "invalid record");
            }

            if (read(fd,
                     &current.len,
                     sizeof(current.len))
                != sizeof(current.len)) {
                errx(1, "invalid record");
            }

            if (read(fd,
                     current.text,
                     current.len)
                != current.len) {
                errx(1, "invalid record");
            }

            current.text[current.len] = '\0';

            /*
             * Записваме ролята към репликата.
             */
            strcpy(current.role, header.text);

            /*
             * Ако масивът е пълен,
             * увеличаваме размера му.
             */
            if (count == capacity) {

                if (capacity == 0) {
                    capacity = 10;
                } else {
                    capacity *= 2;
                }

                records = realloc(records,
                                  capacity * sizeof(rec));

                if (records == NULL) {
                    err(1, "realloc");
                }
            }

            records[count] = current;
            count++;
        }

        close(fd);
    }

    /*
     * Сортираме всички реплики по време (id).
     */
    qsort(records,
          count,
          sizeof(rec),
          compare);

    /*
     * Извеждаме резултата.
     */
    for (int i = 0; i < count; i++) {

        if (write(1,
                  records[i].role,
                  strlen(records[i].role))
            != (ssize_t)strlen(records[i].role)) {
            err(1, "write");
        }

        if (write(1, ": ", 2) != 2) {
            err(1, "write");
        }

        if (write(1,
                  records[i].text,
                  records[i].len)
            != records[i].len) {
            err(1, "write");
        }

        if (write(1, "\n", 1) != 1) {
            err(1, "write");
        }
    }

    free(records);

    return 0;
}


//////////////////////////////////////////////
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

typedef struct
{
    uint64_t id;
    uint8_t N;
} rec;

typedef struct
{
    uint64_t id;
    char role[256];
    char text[256];
} dialogue;


int compare(const void *a, const void *b)
{
    const dialogue *d1 = a;
    const dialogue *d2 = b;

    if (d1->id < d2->id)
        return -1;

    if (d1->id > d2->id)
        return 1;

    return 0;
}


int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 21)
    {
        errx(1, "usage: %s file1 ... file20", argv[0]);
    }

    dialogue dialogues[10000];
    int count = 0;

    for (int i = 1; i < argc; i++)
    {
        int fd = open(argv[i], O_RDONLY);

        if (fd < 0)
        {
            err(1, "open");
        }

        int isFirst = 1;

        rec r;

        char role[256];

        while (
            read(fd, &r.id, sizeof(r.id)) == sizeof(r.id) &&
            read(fd, &r.N, sizeof(r.N)) == sizeof(r.N)
        )
        {
            if (isFirst)
            {
                /*
                    Първият запис е HEADER.
                */

                isFirst = 0;

                if (r.id != 133742)
                {
                    errx(1, "invalid header");
                }

                if (r.N >= sizeof(role))
                {
                    errx(1, "role name too long");
                }

                if (read(fd, role, r.N) != r.N)
                {
                    errx(1, "invalid file");
                }

                role[r.N] = '\0';
            }
            else
            {
                /*
                    Тук имаме реплика.
                */

                if (r.N >= sizeof(dialogues[count].text))
                {
                    errx(1, "text too long");
                }

                if (read(fd,
                        dialogues[count].text,
                        r.N) != r.N)
                {
                    errx(1, "invalid file");
                }

                dialogues[count].text[r.N] = '\0';

                /*
                    Запазваме момента.
                */
                dialogues[count].id = r.id;

                /*
                    Запазваме името на ролята.
                */
                strcpy(dialogues[count].role, role);

                count++;
            }
        }

        close(fd);
    }


    /*
        Сортираме всички реплики
        глобално по времето.
    */
    qsort(
        dialogues,
        count,
        sizeof(dialogue),
        compare
    );


    /*
        Извеждаме резултата.
    */
    for (int i = 0; i < count; i++)
{
    char buff[1024];

    int len = snprintf(
        buff,
        sizeof(buff),
        "%s: %s\n",
        dialogues[i].role,
        dialogues[i].text
    );

    if (len < 0 || len >= sizeof(buff))
    {
        errx(1, "buffer too small");
    }

    if (write(STDOUT_FILENO, buff, len) != len)
    {
        err(1, "write");
    }
}

    return 0;
}



///////////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <string.h>

#define MAX_FILES 20
#define MAX_LINES 10000
#define HEADER_ID 133742

struct Line {
    uint64_t time;
    char role[256];
    char text[256];
};

int cmp(const void* a, const void* b)
{
    const struct Line* x = a;
    const struct Line* y = b;

    if (x->time < y->time) {
        return -1;
    }

    if (x->time > y->time) {
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > MAX_FILES + 1) {
        errx(1, "usage: %s file1 ... file20", argv[0]);
    }

    struct Line lines[MAX_LINES];
    int linesCount = 0;

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);

        if (fd < 0) {
            err(1, "open");
        }

        uint64_t id;
        uint8_t len;
        char role[256];

        if (read(fd, &id, sizeof(id)) != sizeof(id)) {
            errx(1, "empty file");
        }

        if (read(fd, &len, sizeof(len)) != sizeof(len)) {
            errx(1, "bad header");
        }

        if (read(fd, role, len) != len) {
            errx(1, "bad header text");
        }

        role[len] = '\0';

        if (id != HEADER_ID) {
            errx(1, "invalid header");
        }

        while (1) {
            uint64_t time;
            uint8_t textLen;
            char text[256];

            int r = read(fd, &time, sizeof(time));

            if (r == 0) {
                break;
            }

            if (r != sizeof(time)) {
                errx(1, "bad record");
            }

            if (read(fd, &textLen, sizeof(textLen)) != sizeof(textLen)) {
                errx(1, "bad record");
            }

            if (read(fd, text, textLen) != textLen) {
                errx(1, "bad record text");
            }

            text[textLen] = '\0';

            if (linesCount >= MAX_LINES) {
                errx(1, "too many lines");
            }

            lines[linesCount].time = time;
            strcpy(lines[linesCount].role, role);
            strcpy(lines[linesCount].text, text);

            linesCount++;
        }

        close(fd);
    }

    qsort(lines, linesCount, sizeof(struct Line), cmp);

    for (int i = 0; i < linesCount; i++) {
        char output[600];

        int size = snprintf(
            output,
            sizeof(output),
            "%s: %s\n",
            lines[i].role,
            lines[i].text
        );

        if (write(1, output, size) != size) {
            err(1, "write");
        }
    }

    exit(0);
}
