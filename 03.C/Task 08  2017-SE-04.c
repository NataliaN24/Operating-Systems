
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <err.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//$ ./main f - g

void copyToStdout(int fd)
{
    char buff[4096];
    ssize_t bytesRead;
    while((bytesRead=read(fd,buf,sizeof(buf)))>0)
    {
        ssize_t totalWritten=0;
        while(totalWritten<bytesRead)
        {
            ssize_t bytesWritten=write(1,buf+totalWritten,bytesRead-totalWritten);
            if(bytesWritten<0)
            {
                err(1,"error");
            }
            totalWritten+=bytesWritten;
        }
    }
    if(bytesRead<0)
    {
        err(1,"error");
    }
}

int main(int argc, char** argv)
{ 
       char c;
    if(argc ==1 )//one argument only read from stdin
    {
     
      copyToStdout(0);
    }
    else 
    {
        for(int i=1;i<argc;i++)
        {
            if(strcmp(argv[i],"-")==0)
            {
                copyToStdout(0);
            }
            else //only files
            {
                int fd=open(argv[i],O_RDONLY);
                if(fd<0)
                {
                    err(1,"error");
                }
                copyToStdout(fd);

                if (close(fd) < 0) {
                    err(1, "close");
                }
            }
        }

    }


}

