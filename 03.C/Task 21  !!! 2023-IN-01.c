#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

uint32_t computedBytes = 0;

void computeData(int inp, int outp, uint8_t key[], int keySize, uint32_t orgBytes)
{
    uint8_t data[16];
    if(read(inp, data, sizeof(data)) < 0){
        err(201, "Read");
    }

    for(int i = 0; i < keySize; i++)
    {
        data[i] ^= key[i];
    }

    for(int i = 0; i < keySize; i++)
    {
        if(computedBytes < orgBytes)
        {
            if(write(outp, &data[i], sizeof(data[i])) < 0){
                err(202, "Wrirte");
            }
        }
        computedBytes++;
    }
}

void computeSection(int inp, int outp, uint8_t sectionKey[], int sKeySize, uint32_t offset, uint32_t orgBytes)
{
    if(lseek(inp, offset * 16, SEEK_SET) < 0){
        err(101, "Lseek");
    }

    uint8_t lenANDOfft[16];
    if(read(inp, lenANDOfft, sizeof(lenANDOfft)) < 0){
        err(102,  "Read");
    }

    for(int i = 0; i < sKeySize; i++)
    {
        lenANDOfft[i] ^= sectionKey[i];
    }

    int64_t relOfft = 0;
    uint64_t len = 0;

    for(int i = 0; i < 8; i++)
    {
        relOfft |= ((int64_t)lenANDOfft[i] << (8 * i));
    }

    for(int i = 0; i < 8; i++)
    {
        len |= ((uint64_t)lenANDOfft[i + 8] << (8 * i));
    }

    uint8_t key[16];
    if(read(inp, key, sizeof(key)) < 0){
        err(103, "Read");
    }

    for(int i = 0; i < sKeySize; i++)
    {
        key[i] ^= sectionKey[i];
    }

    if(lseek(inp, (relOfft + offset) * 16, SEEK_SET) < 0){
        err(104, "Read");
    }

    for(uint32_t i = 0; i < len; i++)
    {
        computeData(inp, outp, key, 16, orgBytes);
    }
}


int main(int argc, const char* argv[])
{
    if(argc != 3){
        err(1, "Arguments count");
    }

    int encrypted = open(argv[1], O_RDONLY);
    if(encrypted < 0){
        err(2, "Open");
    }
    int decrypted = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if(decrypted < 0){
        err(3, "Open");
    }


    uint64_t magic;
    if(read(encrypted, &magic, sizeof(magic)) < 0){
        err(4, "Read");
    }

    if(magic != 0x0000534f44614c47){
        err(5, "Invalid magic");
    }

    uint32_t bytesCount, unitsCount, orgBytesCount, orgUnitsCount, unused, checkSum;
    uint8_t sectionKey[16];
    uint32_t slots[4];

    if(read(encrypted, &bytesCount, sizeof(bytesCount)) < 0 || read(encrypted, &unitsCount, sizeof(unitsCount)) < 0 || read(encrypted, &orgBytesCount, sizeof(orgBytesCount)) < 0
        || read(encrypted, &orgUnitsCount, sizeof(orgUnitsCount)) < 0 || read(encrypted, &unused, sizeof(unused)) < 0 || read(encrypted, &checkSum, sizeof(checkSum)) < 0
        || read(encrypted, sectionKey, sizeof(sectionKey)) < 0 || read(encrypted, slots, sizeof(slots)) < 0)
    {
        err(6, "Read");
    }

    struct stat info_encrypted;
    if(fstat(encrypted, &info_encrypted) < 0){
        err(7, "Error");
    }

    if(info_encrypted.st_size != bytesCount){
        err(8, "Logical error");
    }

    for(int i = 0; i < 4; i++)
    {
        if(slots[i] != 0)
        {
            computeSection(encrypted, decrypted, sectionKey, 16, slots[i], orgBytesCount);
        }
    }

    struct stat info_decr;
    if(fstat(decrypted, &info_decr) < 0){
        err(9, "Fstat");
    }

    if(info_decr.st_size != orgBytesCount){
        err(10, "LOgical error");
    }

    if(close(encrypted) < 0 || close(decrypted) < 0){
        err(11, "Close");
    }

    exit(0);
}
