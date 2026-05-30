#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/wait.h>
#include <err.h>

int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        errx(26, "Invalid arguments");
    }

    char* program = argv[1];

    long N = strtol(argv[2], NULL, 10);

    if(N < 0 || N >= 256)
    {
        errx(26, "Invalid N");
    }

    int result_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if(result_fd < 0)
    {
        err(26, "open result");
    }

    int urandom = open("/dev/urandom", O_RDONLY);

    if(urandom < 0)
    {
        err(26, "open urandom");
    }

    for(long i = 0; i < N; i++)
    {
        uint16_t S;

        if(read(urandom, &S, sizeof(S)) != sizeof(S))
        {
            err(26, "read S");
        }

        char input[S];

        if(S > 0)
        {
            if(read(urandom, input, S) < 0)
            {
                err(26, "read input");
            }
        }

        int p[2];

        if(pipe(p) < 0)
        {
            err(26, "pipe");
        }

        pid_t pid = fork();

        if(pid < 0)
        {
            err(26, "fork");
        }

        if(pid == 0)
        {
            close(p[1]);

            if(dup2(p[0], 0) < 0)
            {
                _exit(26);
            }

            close(p[0]);

            int devnull = open("/dev/null", O_WRONLY);

            if(devnull < 0)
            {
                _exit(26);
            }

            if(dup2(devnull, 1) < 0)
            {
                _exit(26);
            }

            if(dup2(devnull, 2) < 0)
            {
                _exit(26);
            }

            close(devnull);

            execl(program, program, (char*)NULL);

            _exit(26);
        }

        close(p[0]);

        if(S > 0)
        {
            if(write(p[1], input, S) < 0)
            {
                err(26, "write input");
            }
        }

        close(p[1]);

        int status;

        if(waitpid(pid, &status, 0) < 0)
        {
            err(26, "waitpid");
        }

        if(WIFSIGNALED(status))
        {
            if(S > 0)
            {
                if(write(result_fd, input, S) < 0)
                {
                    err(26, "write result");
                }
            }

            close(result_fd);
            close(urandom);

            return 42;
        }
    }

    close(result_fd);
    close(urandom);

    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////



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
