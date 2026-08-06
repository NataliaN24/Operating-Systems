#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        exit(26);
    }

    int fd[2];

    if(pipe(fd)<0)
    {
        exit(26);
    }

    pid_t pid=fork();

    if(pid<0)
    {
        exit(26);
    }

    if(pid==0)
    {
        close(fd[0]);

        if(dup2(fd[1],1)<0)
        {
            exit(26);
        }

        close(fd[1]);

        execlp("tar","tar","-cf","-",argv[1],(char*)NULL);

        exit(26);
    }


    close(fd[1]);

    uint8_t byte;
    uint8_t res=0;

    ssize_t r;

    while((r=read(fd[0],&byte,1))==1)
    {
        res ^= byte;
    }

    if(r<0)
    {
        exit(26);
    }

    close(fd[0]);

    waitpid(pid,NULL,0);


    char buff[10];

    int len=snprintf(buff,sizeof(buff),"%02X\n",res);

    write(1,buff,len);

    return 0;
}
