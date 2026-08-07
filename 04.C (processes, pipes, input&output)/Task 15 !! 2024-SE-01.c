#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

#define MAX_SIZE 65535

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        errx(1, "usage: %s program N result_file", argv[0]);
    }

    int N = atoi(argv[2]);

    if (N < 0 || N >= (1 << 28))
    {
        errx(1, "invalid N");
    }


    // /dev/urandom
    int rnd = open("/dev/urandom", O_RDONLY);

    if (rnd < 0)
    {
        err(1, "open urandom");
    }


    // резултатен файл - празен в началото
    int result = open(argv[3],
                      O_WRONLY | O_CREAT | O_TRUNC,
                      0644);

    if (result < 0)
    {
        err(1, "open result");
    }

    close(result);



    uint8_t input[MAX_SIZE];


    for (int test = 0; test < N; test++)
    {

        // -------- генериране на размер S --------

        uint16_t S;

        if (read(rnd, &S, sizeof(S)) != sizeof(S))
        {
            err(1, "read size");
        }



        // -------- генериране на входа --------

        if (read(rnd, input, S) != S)
        {
            err(1, "read input");
        }



        // -------- pipe към stdin --------

        int pipefd[2];

        if (pipe(pipefd) < 0)
        {
            err(1, "pipe");
        }



        pid_t pid = fork();


        if (pid < 0)
        {
            err(1, "fork");
        }



        // ------------ CHILD ------------

        if (pid == 0)
        {

            // stdin от pipe
            close(pipefd[1]);

            if (dup2(pipefd[0], STDIN_FILENO) < 0)
            {
                err(1, "dup2 stdin");
            }


            close(pipefd[0]);



            // stdout и stderr към /dev/null

            int null = open("/dev/null", O_WRONLY);

            if (null < 0)
            {
                err(1, "open null");
            }


            if (dup2(null, STDOUT_FILENO) < 0)
            {
                err(1, "dup2 stdout");
            }


            if (dup2(null, STDERR_FILENO) < 0)
            {
                err(1, "dup2 stderr");
            }


            close(null);



            execl(argv[1], argv[1], (char *)NULL);

            err(1, "exec");
        }



        // ------------ PARENT ------------

        close(pipefd[0]);


        // подаваме входа
        if (write(pipefd[1], input, S) != S)
        {
            err(1, "write input");
        }


        close(pipefd[1]);



        int status;


        if (waitpid(pid, &status, 0) < 0)
        {
            err(1, "waitpid");
        }



        // проверка за crash

        if (WIFSIGNALED(status))
        {

            int out = open(argv[3],
                           O_WRONLY | O_TRUNC);

            if (out < 0)
            {
                err(1, "open result");
            }


            if (write(out, input, S) != S)
            {
                err(1, "write result");
            }


            close(out);


            return 42;
        }
    }



    close(rnd);


    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////
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


    if(close(wire[0]) < 0) { err(16, "Close"); }

    if(close(res) < 0 || close(rndm) < 0) { err(17, "Close"); }

    exit(exitCode);
}
