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
#include <sys/time.h>
#include <pwd.h>
#include <errno.h>

const char rndmFile[] = "/dev/urandom";

void getProgramName(const char* inp, char program[])
{
    int len = strlen(inp);
    int lastSepIndx = 0, commInd = 0;

    for(int i = 0; i < len; i++)
    {
        if(inp[i] == '/' && i != len - 1) { lastSepIndx = i; }
    }

    for(int i = lastSepIndx + 1; i < len; i++)
    {
        program[commInd] = inp[i];
        commInd++;
    }
    program[commInd] = '\0';
}


int main(int argc, const char* argv[])
{
    if(argc != 4) { err(1, "Not enough argumenets"); }

    int N = atoi(argv[2]);
    if(!(N >= 0 && N < 256)) { err(2, "Wrong value for N, N >= 0 && N < 256"); }

    int exitCode = 0;

    char program[128];
    getProgramName(argv[1], program);

    int res = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(res < 0) { err(3, "Couldn't open res file %s", argv[3]); }

    int wire[2];
    if(pipe(wire) < 0) { err(4, "Couldn't pipe"); }

    int rndm = open(rndmFile, O_RDONLY);
    if(rndm < 0) { err(5, "Couldn't open /dev/urandom"); }

    if(dup2(wire[1], 1) < 0) { err(6, "Dup2 stdoud"); }
    if(close(wire[1]) < 0) { err(7, "Close pipe[1]"); }

    for(int i = 0; i < N; i++)
    {
        uint16_t currRndm; //let's say that the read num is always in the true interval
        if(read(rndm, &currRndm, sizeof(currRndm)) < 0) { err(8, "Error reading num from /dev/urandom"); }

        char bytes[currRndm];
        if(read(rndm, bytes, sizeof(bytes)) < 0) { err(9, "Rad from /dev/urandom"); }
        if(write(1, bytes, sizeof(bytes)) < 0) { err(10, "Write to pipe"); }

        int pid = fork();
        if(pid < 0) { err(11, "Fork"); }
        if(pid == 0)
        {
            if(dup2(wire[0], 0) < 0) { err(12, "Dup2 pipe[0]"); }
            if(close(wire[0]) < 0) { err(13, "Close pipe[0]"); }

            execl(argv[1], program, (char*)NULL);
            err(14, "Couldn't exec the program");
        }

        int status;
        wait(&status);
        if(!WIFEXITED(status))
        {
            if(write(res, bytes, sizeof(bytes)) < 0) { err(15, "Couldn't write to result file"); }
            exitCode = 42;
            break;
        }
    }

    if(close(wire[0]) < 0) { err(16, "Close"); }

    if(close(res) < 0 || close(rndm) < 0) { err(17, "Close"); }

    exit(exitCode);
}
