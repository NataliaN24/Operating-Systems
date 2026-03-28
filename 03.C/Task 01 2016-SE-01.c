#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        err(1, "error");
    }
    int bytes[256];
    for (int i = 0; i < 256; i++)
    {
        bytes[i] = 0;
    }
    int fd = open(argv[1], O_RDWR);
    if (fd < 0)
    {
        err(1, "error");
    }
    uint8_t c; //var for 1 byte only
    int byteRead;  //what read has returned
    while ((byteRead = read(fd, &c, sizeof(c)) > 0)
    {
        bytes[c]++;
    }
    if(byteRead < 0)
    {
        err(1,"error");
    }

    if(lseek(fd,0,SEEK_SET)<0)
    {
        err(1,"error");
    }

    for(int i=0;i<256;i++)
    {
        for(int j=0;;j<bytes[i];j++)
        {
            if(write(fd,&c,sizeof(c))!=sizeof(c)) //checks if only one bytes is being written
            {
                err(1,"error");
            }

        }
        if(close(fd)<0)
        {
            err(1,"error");
        }
        exit(0);
    }
