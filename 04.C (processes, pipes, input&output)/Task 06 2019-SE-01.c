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
