#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <sys/stat.h>



int main(void)
{
    char promt[]="enter command";
    char command[1024];
    char binpath[1032];

    while(true)
    {
        if(write(1,promt,strlen(promt))<0)
        {
            err(1,"error");
        }
        int bytesRead=read(0,command,sizeof(command));
        if(bytesRead<0)
        {
            err(2,"Error");
        }
        command[bytesRead-1]='\0';

        if(strcmp(command,"exit")==0)
        {
            break;

        }
        snprintf(binpath,sizeof(binpath),"/bin/%s",command);

        int pid=fork();
        if(pid<0)
        {
            err(3,"Error");
        }
        if(pid==0)
        {
            execlp(binpath,command,NULL);
            err(3,"Error");
        }
        int stat;//как е приключил child процесът
        wait(&stat);
        if(!WIFEXITED(stat))//дали процесът е приключил нормално (с exit)
        {
            err(3,"Error");
        }
        
    }
    
    
}
