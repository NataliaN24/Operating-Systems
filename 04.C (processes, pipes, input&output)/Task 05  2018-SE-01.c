#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <sys/stat.h>

//// find ./C/Sbornik/Processes/ -printf "%t\n" | sort | head -n 1

int main(int argc,const char*argv[])
{
    if(argc!=2)
    {
        err(1,"error");
    }
    int findFd[2];
    int sortFd[2];
    if(pipe(findFd)<0 || pipe(sortFd)<0)
    {
         err(1,"error");
    }
    int find=fork();
    if(find<0) err(1,"error");
    if(find==0)
    {
        if(close(findFd[0])<0||close(sortFd[0])<0 ||close(sortFd[1])<0) err(1,"error");
        if(dup2(findFd[1],1)<0) err(1,"error");
        if(close(findFd[1])<0) err(1,"error");
          execlp("find", "find", argv[1], "-printf", "%t\n", (char*)NULL);
        err(7, "Couldn't execlp find");
    }
    if(close(findFd[1])<0) err(1,"error");

    int sort=fork();
    if(sort<0) err(1,"error");
    if(sort==0)
    {
         if(close(findFd[1])<0||close(sortFd[0])<0) err(1,"error");
        if(dup2(findFd[0],0)<0) err(1,"error");
        if(dup2(sortFd[1],1)<0) err(1,"error");
        if(close(sortFd[1])<0) err(1,"error");
        execlp("sort", "sort", (char*)NULL);
        err(14, "Couldn't execlp sort");
    }
     if(close(findFd[0])<0||close(sortFd[1])<0) err(1,"error");
    int head=fork();
    if(head < 0) { err(16, "Fork"); }
    if(head == 0)
    {
        if(close(findFd[0])<0||close(findFd[1])<0 |close(sortFd[1])<0) err(1,"error");
        if(dup2(sortFd[0],0)<0)err(1,"error");
        if(close(sortFd[0])<0)err(1,"error");
        execlp("head","head","-n1",(char*)NULL);
          err(20, "Couldn't execlp head");
    }
    if(close(sortFd[0]<0)<0)err(1,"error");
    wait(NULL);
    wait(NULL);
    wait(NULL);

}
