#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <err.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc < 3) {
        errx(1, "usage: %s limit program [args...]", argv[0]);
    }

    if (argv[1][0] < '1' || argv[1][0] > '9' || argv[1][1] != '\0') {
        errx(1, "limit must be digit 1-9");
    }

    int limit = argv[1][0] - '0';

    int log = open("run.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log < 0) {
        err(1, "open");
    }

    int badCount = 0;

    while (1) {
        time_t start = time(NULL);

        pid_t pid = fork();
        if (pid < 0) {
            err(1, "fork");
        }

        if (pid == 0) {
            execvp(argv[2], &argv[2]);
            err(1, "execvp");
        }

        int status;
        if (wait(&status) < 0) {
            err(1, "wait");
        }

        time_t end = time(NULL);

        int exitCode = 129;

        if (WIFEXITED(status)) {
            exitCode = WEXITSTATUS(status);
        }

        char buff[1024];

        int s = snprintf(
            buff,
            sizeof(buff),
            "%ld %ld %d\n",
            (long)start,
            (long)end,
            exitCode
        );

        if (s < 0 || s >= (int)sizeof(buff)) {
            errx(1, "snprintf");
        }

        if (write(log, buff, s) != s) {
            err(1, "write");
        }

        if (exitCode != 0 && end - start < limit) {
            badCount++;
        } else {
            badCount = 0;
        }

        if (badCount == 2) {
            break;
        }
    }

    close(log);
    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <sys/stat.h>

//// ./P 3 ./Q arg1 arg2
const char*filename="run.log";

int main(int argc,const char*argv[])
{
    if(argc<2)
    {
        err(1,"error");
    }
    int max=*argv[1]-'0';
    if(!(max>=1 &&max<=9))
    {
        err(1,"error");
    }
    char *commArgs[1024];
    int j=0;
    for(int i=2;i<=argc;i++)
    {
        commArgs[j++]=argv[i];
    }
    commArgs[j]=NULL;
    int prevStatus=-1;
    int diff=max;
    int runLog=open(filename,O_WRONLY|O_CREAT|O_TRUNC,0644);
    if(runLog<0)
    {
        err(1,"error");
    }
    while(true)
    {
        int currDiff;
        long long currStart=(long long)time(NULL);
        int pid=fork();
        if(pid<0){err(1,"error");}
        if(pid==0)
        {
            execvp(argv[2],commArgs);
            err(1,"error");
        }
        long long currEnd=(long long)time(NULL);
        currDiff=(int)currEnd-currStart;

        int status;
        wait(&status);
        if(WIFEXIT(status)==0)
        {
            err(129,"error");
        }
        int currCode=WEXITSTATUS(status);
        if(write(runLog,&currStart,sizeof(currStart))<0 || write(runLog,&currEnd,sizeof(currEnd))<0)
        {
            err(1,"error");
        }
        if(prevStatus!=0 &&diff<max && currCode!=0&&currDiff<max)
        {
            break;
        }
        prevStatus=currCode;
        diff=currDiff;
    }
        return 0;
   

}
