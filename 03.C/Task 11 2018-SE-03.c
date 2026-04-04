
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/param.h>

void processC(int beg,int end) //abcdef 2-4  ->bcd
{
    char c;
    int readBytes;
    int count=1;
    while((readBytes=read(0,&c,sizeof(c)))>0)
    {
        if(count>=beg && count<=end)
        {
            if(write(1,&c,sizeof(c))<0)
            {
                err(1,"error");
            }
            if(count==end)
            {
                char s='\n';
                if(write(1,&s,sizeof(s))<0)
                {
                    err(1,"error");
                }
            }
            count++;
        }
        if(c=='\n')
        {
            count=1;
        }
    }
    if(readBytes<0)
    {
        err(1,"error");
    }

}
void processD(char sep,int beg,int end) //aaa:bbb:ccc:dddd:eee 2-4
{
    char c;
    int bytesRead;
    int count=1;
    while((bytesRead=read(0,&c,sizeof(c)))>0)
    {
        if(count>=beg &&count <=end)
        {
            if(!(c==sep && count==beg &&count==end))
            {
                if(write(1,&c,sizeof(c))<0)
                {
                    err(1,"error");
                }
            }
        }
        if(c==sep)
        {
            count++;
        }
        else if(c=='\n')
        {
            if(write(1,&c,sizeof(c))<0)
            {
                err(1,"error");
            }
            count=1;
        }
    }



}
int main(int argc, char** argv) {
    if(strcmp(argv[1],"-c")==0)
    {
        if(strlen(argv[2])==1) //cut -c 2
        {
            processC(argv[2][0]-'0',argv[2][0]-'0');
        }
        else //cut -c 2-5
        {
            processC(argv[2][0]-'0',argv[2][2]-'0');
        }
    }
    if(strcmp(argv[1],"-d")==0) //./main -d : -f 2-3
    {
        if(strlen(argv[4])==1)
        {
            processD(argv[2][0],argv[4][0]-'0',argv[4][0]-'0');
        }
        else{
            processD(argv[2][0],argv[4][0]-'0',argv[4][2]-'0');
        }

    }
        
}



argv[2]     → "3-5"
argv[2][0]  → '3'
argv[2][1]  → '-'
argv[2][2]  → '5'
