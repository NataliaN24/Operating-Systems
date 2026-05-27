#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <err.h>

void print_num(int n)
{
    char buf[16];
    int i = 0;

    while (n > 0) {
        buf[i++] = n % 10 + '0';
        n /= 10;
    }

    while (i > 0) {
        i--;
        if (write(1, &buf[i], 1) != 1) {
            err(1, "write");
        }
    }
}

int main(int argc, char *argv[])
{
    int use_numbers = 0;
    int start = 1;

    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        use_numbers = 1;
        start = 2;
    }

    int line_num = 1;
    int at_beginning = 1;

    if (start == argc) {
        char ch;
        ssize_t bytesread;

        while ((bytesread = read(0, &ch, 1)) > 0) {
            if (use_numbers && at_beginning) {
                print_num(line_num);
                write(1, " ", 1);
                line_num++;
                at_beginning = 0;
            }

            write(1, &ch, 1);

            if (ch == '\n') {
                at_beginning = 1;
            }
        }

        if (bytesread < 0) {
            err(1, "read");
        }

        return 0;
    }

    for (int i = start; i < argc; i++) {
        int fd;

        if (strcmp(argv[i], "-") == 0) {
            fd = 0;
        } else {
            fd = open(argv[i], O_RDONLY);
            if (fd < 0) {
                err(1, "open");
            }
        }

        char ch;
        ssize_t bytesread;

        while ((bytesread = read(fd, &ch, 1)) > 0) {
            if (use_numbers && at_beginning) {
                print_num(line_num);
                write(1, " ", 1);
                line_num++;
                at_beginning = 0;
            }

            write(1, &ch, 1);

            if (ch == '\n') {
                at_beginning = 1;
            }
        }

        if (bytesread < 0) {
            err(1, "read");
        }

        if (fd != 0) {
            close(fd);
        }
    }

    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////
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
