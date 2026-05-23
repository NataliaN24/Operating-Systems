#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>

typedef struct {
    uint16_t magic;
    uint16_t type;
    uint32_t count;
} Header;

int main(int argc, char* argv[])
{
    if(argc != 4){
        errx(1, "invalid args");
    }

    int list = open(argv[1], O_RDONLY);
    if(list < 0){
        err(2, "open list");
    }

    int data = open(argv[2], O_RDONLY);
    if(data < 0){
        err(3, "open data");
    }

    int out = open(argv[3], O_CREAT | O_TRUNC | O_RDWR, 0644);
    if(out < 0){
        err(4, "open out");
    }

    Header hList;
    Header hData;

    if(read(list, &hList, sizeof(hList)) != sizeof(hList)){
        err(5, "read");
    }

    if(read(data, &hData, sizeof(hData)) != sizeof(hData)){
        err(6, "read");
    }

    if(hList.magic != 0x5A4D || hList.type != 1){
        errx(7, "invalid list");
    }

    if(hData.magic != 0x5A4D || hData.type != 2){
        errx(8, "invalid data");
    }

    if(hList.count > hData.count){
        errx(9, "invalid counts");
    }

    uint16_t val;
    uint32_t max = 0;

    for(uint32_t i = 0; i < hList.count; i++)
    {
        if(read(list, &val, sizeof(val)) != sizeof(val)){
            err(10, "read");
        }

        if(val > max){
            max = val;
        }
    }

    Header hOut;
    hOut.magic = 0x5A4D;
    hOut.type = 3;
    hOut.count = max + 1;

    if(lseek(out, 0, SEEK_SET) < 0){
        err(11, "lseek");
    }

    if(write(out, &hOut, sizeof(hOut)) != sizeof(hOut)){
        err(12, "write");
    }

    uint64_t zero = 0;

    for(uint32_t i = 0; i < hOut.count; i++)
    {
        if(write(out, &zero, sizeof(zero)) != sizeof(zero)){
            err(13, "write");
        }
    }

    if(lseek(list, sizeof(Header), SEEK_SET) < 0){
        err(14, "lseek");
    }

    for(uint32_t i = 0; i < hList.count; i++)
    {
        uint16_t outPos;

        if(read(list, &outPos, sizeof(outPos)) != sizeof(outPos)){
            err(15, "read");
        }

        if(lseek(data,
                 sizeof(Header) + i * sizeof(uint32_t),
                 SEEK_SET) < 0){
            err(16, "lseek");
        }

        uint32_t curr;

        if(read(data, &curr, sizeof(curr)) != sizeof(curr)){
            err(17, "read");
        }

        uint64_t curr64 = curr;

        if(lseek(out,
                 sizeof(Header) + outPos * sizeof(uint64_t),
                 SEEK_SET) < 0){
            err(18, "lseek");
        }

        if(write(out, &curr64, sizeof(curr64)) != sizeof(curr64)){
            err(19, "write");
        }
    }

    close(list);
    close(data);
    close(out);

    exit(0);
}
//////////////////////////////////////////////////////////////////////////////////////////////



#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

const uint32_t MAX_SIZE = 524288;

int main(int argc, const char* argv[])
{
    if(argc != 4){
        err(1, "Incorrect input");
    }

    int list = open(argv[1], O_RDONLY);
    if(list < 0){
        err(2, "Open");
    }
    int data = open(argv[2], O_RDONLY);
    if(data < 0){
        err(3, "Open");
    }
    int out = open(argv[3], O_RDWR);
    if(out < 0){
        err(4, "Error");
    }

    uint16_t magicList, magicData, magicOut;
    uint16_t ftypeList, ftypeData, ftypeOut;
    uint32_t countList, countData, countOut;

    if(read(list, &magicList, sizeof(magicList)) < 0 || read(list, &ftypeList, sizeof(ftypeList)) < 0 || read(list, &countList, sizeof(countList)) < 0){
        err(5, "Error");
    }

    if(read(data, &magicData, sizeof(magicData)) < 0 || read(data, &ftypeData, sizeof(ftypeData)) < 0 || read(data, &countData, sizeof(countData)) < 0){
        err(6, "Error");
    }

    if(read(out, &magicOut, sizeof(magicOut)) < 0 || read(out, &ftypeOut, sizeof(ftypeOut)) < 0 || read(out, &countOut, sizeof(countOut)) < 0){
        err(7, "Error");
    }

    if(magicList != 0x5A4D || ftypeList != 1){
        err(8, "Error");
    }

    if(magicData != 0x5A4D || ftypeData != 2){
        err(9, "Error");
    }

    if(magicOut != 0x5A4D || ftypeOut != 3){
        err(10 , "Error");
    }

    uint16_t pos, val;
    int readBytes;
    while((readBytes = read(list, &pos, sizeof(pos))) > 0 && (readBytes = read(list, &val, sizeof(val))) > 0)
    {
        if(lseek(data, (off_t)pos, SEEK_SET) < 0){
            err(12, "Lseek");
        }

        uint32_t currData;
        if(read(data, &currData, sizeof(currData)) < 0){
            err(13, "Read");
        }

        if(lseek(out, (off_t)val, SEEK_SET) < 0){
            err(14, "Lseek");
        }

        uint64_t currData64 = (uint64_t)currData;
        if(write(out, &currData64, sizeof(currData64)) < 0){
            err(15, "Error");
        }
    }

    if(readBytes < 0){
        err(11, "Error");
    }

    if(close(list) < 0 || close(data) < 0 || close(out) < 0){
        err(16, "Close");
    }

    exit(0);
}

