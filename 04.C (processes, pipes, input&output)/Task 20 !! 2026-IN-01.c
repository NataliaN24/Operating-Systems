#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        errx(1, "usage");
    }

    int toChild[3][2];
    int fromChild[3][2];
    pid_t pids[3];

    for(int i = 0; i < 3; i++)
    {
        if(pipe(toChild[i]) < 0)
        {
            err(1, "pipe");
        }

        if(pipe(fromChild[i]) < 0)
        {
            err(1, "pipe");
        }

        pids[i] = fork();

        if(pids[i] < 0)
        {
            err(1, "fork");
        }

        if(pids[i] == 0)
        {
            close(toChild[i][1]);
            close(fromChild[i][0]);

            if(dup2(toChild[i][0], 0) < 0)
            {
                err(1, "dup2");
            }

            if(dup2(fromChild[i][1], 1) < 0)
            {
                err(1, "dup2");
            }

            for(int j = 0; j < 3; j++)
            {
                close(toChild[j][0]);
                close(toChild[j][1]);
                close(fromChild[j][0]);
                close(fromChild[j][1]);
            }

            execl(argv[i + 1], argv[i + 1], (char*)NULL);
            err(1, "exec");
        }

        close(toChild[i][0]);
        close(fromChild[i][1]);
    }

    uint8_t current;
    uint8_t previous = 0;
    int first = 1;

    while(read(0, &current, sizeof(current)) == sizeof(current))
    {
        /*
         * Ако имаме повторение на байт
         * и той не е специален
         * пращаме допълнително 0x55
         */
        if(!first &&
           current == previous &&
           current != 0x00 &&
           current != 0x55 &&
           current != 0x7D &&
           current != 0xFF)
        {
            uint8_t byte = 0x55;

            for(int i = 0; i < 3; i++)
            {
                if(write(toChild[i][1], &byte, 1) != 1)
                {
                    err(1, "write");
                }
            }
        }

        /*
         * Специален байт
         */
        if(current == 0x00 ||
           current == 0x55 ||
           current == 0x7D ||
           current == 0xFF)
        {
            uint8_t escape = 0x7D;
            uint8_t encoded = current ^ 0x20;

            for(int i = 0; i < 3; i++)
            {
                if(write(toChild[i][1], &escape, 1) != 1)
                {
                    err(1, "write");
                }

                if(write(toChild[i][1], &encoded, 1) != 1)
                {
                    err(1, "write");
                }
            }
        }
        else
        {
            for(int i = 0; i < 3; i++)
            {
                if(write(toChild[i][1], &current, 1) != 1)
                {
                    err(1, "write");
                }
            }
        }

        /*
         * Чакаме потвърждение от всички програми
         */
        for(int i = 0; i < 3; i++)
        {
            uint8_t ack;

            if(read(fromChild[i][0], &ack, 1) != 1)
            {
                err(1, "read");
            }

            if(ack != 0x01)
            {
                errx(1, "bad ack");
            }
        }

        previous = current;
        first = 0;
    }

    for(int i = 0; i < 3; i++)
    {
        close(toChild[i][1]);
    }

    for(int i = 0; i < 3; i++)
    {
        wait(NULL);
    }

    return 0;
}
