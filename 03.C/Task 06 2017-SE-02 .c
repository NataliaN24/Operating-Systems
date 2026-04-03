
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

//akо няма файлове → чети от STDIN
//ако има файлове → печатай ги един след друг
//ако някой аргумент е - → вместо файл, чети от STDIN
//ако първият аргумент е -n → номерирай редовете глобално, тоест броячът не се занулява между файловете
void printNum(int num)
{
    char buff[16];
    snprintf(buff,sizeof(buff),"%d",num);
    if(write(1,buff,strlen(buff))<0)
    {
        err(1,"error");
    }
}
void processFile(int fd,bool printNumber)
{
    int num=1;
    char c;
    char prev='\n';
    int flag;

    while((flag=read(fd,&c,sizeof(c)))>0)
    {
        if(printNum && prev='\n')
        {
            printNum(num);
        }
        if(write(1,&c,sizeof(c))<0)
        {
            err(1,"error");
        }
        if(c=='\n')
        {
            num++;
        }
        prev=c;
    }
    if(flag<0)
    {
        err(1,"error");
    }

}
void readFromSTDIN(bool isNumbered)
{

    char c;
    char prev='\n';
    int num=1;
    int flag;

    while((flag=read(0,&c,sizeof(c)))>0) //reads from stdin
    {
        if(prev == '\n')
        {
            if(c=='\n')
            {
                break;
            }
            else if(isNumbered)
            {
                printNum(num);
            }
        }

        if(write(1,&c,sizeof(c))<0)
        {
            err(1,"error");
        }
        if(c=='\n')
        {
            num++;
        }
        prev=c;

    }
    if(flag<0)
    {
        err(1,"error");
    }

}


int main(int argc, char** argv)
{
    if (argc == 1) { //without any parameter cat a.txt
        readFromSTDIN(0);
    }
    bool isNumbered=0;
    if(strcmp(argv[1],"-n")==0)
    {
        isNumbered=1;
    }

    for(int i=1;i<argc;i++)
    {
        if(isNumbered && i==1)
        {
            continue;
        }
        if(strcmp(argv[i],"-")==0)
        {
            readFromSTDIN(isNumbered);
        }
        else
        {
            int fd=open(argv[i],O_RDONLY);
            if(fd<0)
            {
                err(1,"error");
            }
            processFile(fd,isNumbered);
            if(close(fd)<0)
            {
                err(1,"error");
            }
        }
    }
        exit (0);
    

   
}
