#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#define MAX_FILES 20
#define HEADER_ID 133742

typedef struct {
    int fd;

    char role[256];

    uint64_t time;
    uint8_t len;
    char text[256];

    int active;
} File;

int readElement(int fd, uint64_t* id, uint8_t* len, char* text)
{
    int r = read(fd, id, sizeof(*id));

    if(r < 0){
        err(1, "read");
    }

    if(r == 0){
        return 0;
    }

    if(r != sizeof(*id)){
        errx(1, "bad file");
    }

    if(read(fd, len, sizeof(*len)) != sizeof(*len)){
        err(1, "read len");
    }

    if(read(fd, text, *len) != *len){
        err(1, "read text");
    }

    text[*len] = '\0';

    return 1;
}

int main(int argc, char* argv[])
{
    if(argc < 2 || argc > 21){
        errx(1, "usage");
    }

    int n = argc - 1;

    File files[MAX_FILES];

    for(int i = 0; i < n; i++)
    {
        files[i].fd = open(argv[i + 1], O_RDONLY);

        if(files[i].fd < 0){
            err(1, "open");
        }

        uint64_t id;
        uint8_t len;

        if(!readElement(files[i].fd, &id, &len, files[i].role)){
            errx(1, "empty file");
        }

        if(id != HEADER_ID){
            errx(1, "invalid header");
        }

        if(readElement(files[i].fd,
                       &files[i].time,
                       &files[i].len,
                       files[i].text))
        {
            files[i].active = 1;
        }
        else
        {
            files[i].active = 0;
        }
    }

    while(1)
    {
        int min = -1;

        for(int i = 0; i < n; i++)
        {
            if(files[i].active)
            {
                if(min == -1 || files[i].time < files[min].time){
                    min = i;
                }
            }
        }

        if(min == -1){
            break;
        }

        write(1, files[min].role, strlen(files[min].role));
        write(1, ": ", 2);
        write(1, files[min].text, files[min].len);
        write(1, "\n", 1);

        if(readElement(files[min].fd,
                       &files[min].time,
                       &files[min].len,
                       files[min].text))
        {
            files[min].active = 1;
        }
        else
        {
            files[min].active = 0;
        }
    }

    for(int i = 0; i < n; i++)
    {
        close(files[i].fd);
    }

    exit(0);
}
///////////////////////////////////////////////////////////////////////////////////////

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
