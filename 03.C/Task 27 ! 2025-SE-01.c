#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char* role;
    uint64_t timestamp;
    uint8_t textLen;
    char* textBytes;
} RoleLine;

void sortAndPrint(RoleLine* arr, int arrSize)
{
    for (int i = 0; i < arrSize - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < arrSize; j++) {
            if (arr[j].timestamp < arr[minIndex].timestamp) {
                minIndex = j;
            }
        }

        if (minIndex != i) {
            RoleLine temp = arr[i];
            arr[i] = arr[minIndex];
            arr[minIndex] = temp;
        }
    }

    for (int i = 0; i < arrSize; i++) {
        if (printf("%s: %s\n", arr[i].role, arr[i].textBytes) < 0) {
            err(5, "printf");
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 21) {
        errx(1, "usage: %s file1 [file2 ... file20]", argv[0]);
    }

    int capacity = 1024;
    int count = 0;
    RoleLine* array = malloc(sizeof(RoleLine) * capacity);
    if (array == NULL) {
        err(1, "malloc");
    }

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            err(1, "open");
        }

        uint64_t headerId;
        uint8_t roleLen;

        if (read(fd, &headerId, sizeof(headerId)) != sizeof(headerId)) {
            err(2, "read header id");
        }

        if (read(fd, &roleLen, sizeof(roleLen)) != sizeof(roleLen)) {
            err(2, "read role length");
        }

        if (headerId != 133742) {
            errx(3, "invalid header");
        }

        char* roleName = malloc(roleLen + 1);
        if (roleName == NULL) {
            err(1, "malloc");
        }

        if (read(fd, roleName, roleLen) != roleLen) {
            err(2, "read role name");
        }
        roleName[roleLen] = '\0';

        while (1) {
            uint64_t timestamp;
            uint8_t textLen;

            ssize_t bytesRead = read(fd, &timestamp, sizeof(timestamp));
            if (bytesRead == 0) {
                break;  // EOF
            }
            if (bytesRead != sizeof(timestamp)) {
                err(4, "read timestamp");
            }

            if (read(fd, &textLen, sizeof(textLen)) != sizeof(textLen)) {
                err(4, "read text length");
            }

            char* text = malloc(textLen + 1);
            if (text == NULL) {
                err(1, "malloc");
            }

            if (read(fd, text, textLen) != textLen) {
                err(4, "read text");
            }
            text[textLen] = '\0';

            if (count == capacity) {
                capacity *= 2;
                array = realloc(array, sizeof(RoleLine) * capacity);
                if (array == NULL) {
                    err(1, "realloc");
                }
            }

            array[count].role = roleName;
            array[count].timestamp = timestamp;
            array[count].textLen = textLen;
            array[count].textBytes = text;
            count++;
        }

        close(fd);
    }

    sortAndPrint(array, count);

    for (int i = 0; i < count; i++) {
        free(array[i].textBytes);
    }
    free(array);

    return 0;
}
