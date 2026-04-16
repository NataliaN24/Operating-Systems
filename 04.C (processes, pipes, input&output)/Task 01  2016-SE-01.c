#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <sys/stat.h>


//cat file.txt | sort

int main(int argc, char* argv[])
{
    if (argc !=2) {
        errx(1, "one file");
    }
    int fd[2];
    if(pipe(fd)<0)
    {
        err(2,"error");
    }
    pid_t pid1=fork(); //two processes parent and child,if pid==0 we are in the child
    if(pid1<0)
    {
        err(3,"error");
    }
    if(pid1==0)
    {
         // child 1 -> cat
         dup2(fd[1],1);//stdout → да сочи към pipe
         close(fd[0]);//close for read because this hild will only write
         close(fd[1]);//вече НЕ ни трябва оригиналният fd[1] имаме копие
         //fd[1] → pipe
         //   1      → pipe   (след dup2)
         execlp("cat","cat",argv[1],NULL);
         err(3, "exec cat");
    }
    if(fork()==0)
    {
        dup2(fd[0],0);
        close(fd[1]);
        close(fd[0]);
        execlp("sort","sort",NULL);
        err(4,"error");
    }
     close(fd[0]);
    close(fd[1]);

    //чака и двете деца да приключат
    wait(NULL);
    wait(NULL);

    return 0;

    
}
