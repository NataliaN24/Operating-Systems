#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

void computeMessage(int stream, int messages)
{
    uint8_t N;

    if(read(stream, &N, sizeof(N)) < 0) { err(101, "Read"); }

    if(N < 3) return;

    uint8_t checkSum = 0x55 ^ N;
    uint8_t checkSumFile;

    int bytesCount = N - 3;
    char* message = malloc(bytesCount + 1);
    message[bytesCount] = '\n';

    uint8_t byte;

    for(int i = 0; i < bytesCount; i++)
    {
        if(read(stream, &byte, sizeof(byte)) < 0) { err(102, "Read"); }
        checkSum ^= byte;
        message[i] = (char)byte;
    }

    if(read(stream, &checkSumFile, sizeof(checkSumFile)) < 0) { err(103, "Read"); }

    if(checkSum == checkSumFile)
    {
        if(write(messages, message, bytesCount + 1) < 0) { err(104, "Read"); }
    }

    free(message);
}


int main(int argc, const char* argv[])
{
    if(argc != 3){
        err(1, "Arguments count");
    }

    int stream = open(argv[1], O_RDONLY);
    if(stream < 0) { err(1, "Open"); }

    int messages = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(messages < 0) { err(2, "Open"); }

    uint8_t byte;
    int bytesRead;

    while((bytesRead = read(stream, &byte, sizeof(byte))) > 0)
    {
        if(byte == 0x55)
        {
            computeMessage(stream, messages);
        }
    }

    exit(0);
}

