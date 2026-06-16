#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <err.h>
#include <stdlib.h>

void run_cmd(char* cmd, char* arg1, char* arg2, int cnt)
{
    pid_t pid = fork();

    if(pid == -1)
    {
        err(1, "fork");
    }

    if(pid == 0)
    {
        if(cnt == 1)
        {
            execlp(cmd, cmd, arg1, (char*)NULL);
        }
        else
        {
            execlp(cmd, cmd, arg1, arg2, (char*)NULL);
        }

        err(1, "exec");
    }

    if(wait(NULL) == -1)
    {
        err(1, "wait");
    }
}

int main(int argc, char* argv[])
{
    char* cmd = "echo";

    if(argc > 2)
    {
        errx(1, "too many arguments");
    }

    if(argc == 2)
    {
        if(strlen(argv[1]) > 4)
        {
            errx(1, "command too long");
        }

        cmd = argv[1];
    }

    char arg1[5];
    char arg2[5];
    char word[5];

    int wordLen = 0;
    int cnt = 0;

    char c;
    ssize_t bytesRead;

    while((bytesRead = read(0, &c, 1)) > 0)
    {
        if(c != ' ' && c != '\n')
        {
            if(wordLen == 4)
            {
                errx(1, "word too long");
            }

            word[wordLen] = c;
            wordLen++;
        }
        else
        {
            if(wordLen == 0)
            {
                continue;
            }

            word[wordLen] = '\0';

            if(cnt == 0)
            {
                strcpy(arg1, word);
                cnt = 1;
            }
            else
            {
                strcpy(arg2, word);
                cnt = 2;

                run_cmd(cmd, arg1, arg2, cnt);

                cnt = 0;
            }

            wordLen = 0;
        }
    }

    if(bytesRead == -1)
    {
        err(1, "read");
    }

    if(wordLen > 0) //word="f3" read() връща:0 EOF
    {
        word[wordLen] = '\0';

        if(cnt == 0)
        {
            strcpy(arg1, word);
            cnt = 1;
        }
        else
        {
            strcpy(arg2, word);
            cnt = 2;
        }
    }

    if(cnt > 0)
    {
        run_cmd(cmd, arg1, arg2, cnt);
    }

    return 0;
}
