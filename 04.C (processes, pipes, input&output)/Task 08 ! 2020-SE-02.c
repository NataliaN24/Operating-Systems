#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char* argv[])
{
    if(argc != 3) { err(1, "Not enough argumenets"); }

    int data[2];
    if(pipe(data) < 0) { err(2, "Couldn't pipe"); }

    int pid = fork();
    if(pid < 0) { err(3, "Fork"); }
    if(pid == 0)
    {
        if(close(data[0]) < 0) { err(4, "Close"); }
        if(dup2(data[1], 1) < 0) { err(5, "Dup2"); }
        if(close(data[1]) < 0) { err(6, "Close"); }

        execlp("cat", "cat", argv[1], (char*)NULL);
        err(7, "Couldn't exec cat on %s", argv[1]);
    }

    int outp = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(outp < 0) { err(8, "Open"); }

    int readBytes;
    uint8_t byte;
    uint8_t mask = 0x20;
    uint8_t prev = 0;

    while((readBytes = read(data[0], &byte, sizeof(byte))) > 0)
    {
        if(prev == 0x7D)
        {
            byte = byte ^ mask;
        }
        else
        {
            if(byte == 0x55)
            {
                continue;
            }
            else if(byte == 0x7D)
            {
                prev = byte;
                continue;
            }
        }

        if(write(outp, &byte, sizeof(byte)) < 0) { err(9, "Write"); }
        prev = byte;
    }

    if(close(outp) < 0) { err(10, "Close"); }

    return 0;
}
