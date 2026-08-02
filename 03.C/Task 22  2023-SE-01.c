#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        errx(1, "args");
    }

    int stream = open(argv[1], O_RDONLY);
    if (stream < 0) {
        err(1, "open input");
    }

    int msg = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (msg < 0) {
        err(1, "open output");
    }

    uint8_t start;
    uint8_t N;

    while (read(stream, &start, 1) == 1)
    {
        if (start != 0x55) {
            continue;
        }

        if (read(stream, &N, 1) != 1) {
            break;
        }

        if (N < 3) {
            continue;
        }

        uint8_t message[N];

        message[0] = start;
        message[1] = N;

        for (int i = 2; i < N; i++) {
            if (read(stream, &message[i], 1) != 1) {
                errx(1, "bad file");
            }
        }

        uint8_t checksum = 0;

        for (int i = 0; i < N-1; i++) {
            checksum ^= message[i];
        }

        if (checksum == message[N-1]) {
            if (write(msg, message, N) != N) {
                err(1, "write");
            }
        }
    }

    close(stream);
    close(msg);

    return 0;
}
///////////////////////////////////////////////
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[])
{
    if(argc != 3){
        errx(1, "Usage: ./main stream.bin messages.bin");
    }

    int in = open(argv[1], O_RDONLY);
    if(in < 0){
        err(2, "open input");
    }

    int out = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if(out < 0){
        err(3, "open output");
    }

    uint8_t b;

    while(read(in, &b, 1) == 1)
    {
        if(b != 0x55){
            continue;
        }

        uint8_t len;

        if(read(in, &len, 1) != 1){
            break;
        }

        if(len < 3){
            continue;
        }

        uint8_t msg[256];

        msg[0] = 0x55;
        msg[1] = len;

        uint8_t checksum = msg[0] ^ msg[1];

        int ok = 1;

        for(uint16_t i = 2; i < len; i++)
        {
            if(read(in, &msg[i], 1) != 1){
                ok = 0;
                break;
            }

            if(i < len - 1){
                checksum ^= msg[i];
            }
        }

        if(!ok){
            break;
        }

        if(checksum == msg[len - 1]){
            if(write(out, msg, len) != len){
                err(4, "write");
            }
        }
    }

    close(in);
    close(out);

    exit(0);
}
///////////////////////////////////////////////

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

