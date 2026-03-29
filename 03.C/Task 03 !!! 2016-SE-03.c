//100 000 000 числа uint32_t са около 400 MB.
//Ограничението е 256 MB RAM, значи не можем да заредим целия файл в паметта.
//Затова четем файла на части, сортираме всяка част отделно, записваме я във временен файл, после правим k-way merge на временните файлове.
//Условието за еднакъв endianness означава, че можем да четем uint32_t директно като бинарни числа, без конвертиране .


#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <err.h>
#include <stdio.h>

//100 000 000 числа × 4 байта = около 400 MB, а условието дава само 256 MB RAM
int fds[3];

void close_all(void)
{
    int errno_=errno;
    for(int i=0;i<3;i++)
    {
        if(fd[i]>0)
        {
            close(fd[i]);
        }
    }
    errno=errno_;
}


int openReadSafe(const char*filename)
{
    int fd;
    if((fd=open(filename,O_RDONLY))==-1)
    {
        close_all();
        err(10,"error while opening the file %s",filename);

    }
    return fd;
}
int compareUint32_t(const void *a,const void *b)
{
    const uint32_t*num1=(const uint32_t*)a;
    const uint32_t*num2=(const uint32_t*)b;
    if(*num1 < *num2)
    {
        return -1;
    }
    else if(*num1>*num2)
    {
        return 1;
    }
    else{
        return 0;
    }
}

void writeSafe(int fd,uint32_t number,const char*filename)
{
    if(write(fd,&number,sizeof(uint32_t))==-1)
    {
        close_all();
        err(11,"error writing in the file");
    }
    return number;
}

void split(int halfCount,int fd,const char*tempFile,const char *sourceFile)
{
    uint32_t number;
    uint32_t*half=malloc(halfCount *sizeof(uint32_t));
    if(half==NULL)
    {
        close(fd);
        errx(7,"error");
    }
    int index=0;
    int bytesCount;
    while(index<halfCount &&(bytesCount=read(fd,&number,sizeof(uint32_t)))==sizeof(uint32_t))
    {
        half[index]=number;
        index++;
    }
    if(bytesCount==-1)
    {
        close(fd);
        errx(8,"error");
    }
    qsort(half,halfCount,sizeof(uint32_t),compareUint32_t);
    int fd1;
    if(fd1=open(tempFile,O_RDWR|O_TRUNC|O_CREAT,S_IRWXU)==-1)
    {
        free(half);
        errx(9,"error");
    }
    for(int i=0;i<halfCount;i++)
    {
        writeSafe(fd1,half[i].tempFile);
    }
    close(fd1);
    free(half);
}
uint32_t readNumber(int fd,const char *filename)
{
    uint32_t number;
    if(read(fd,&number,sizeof(uint32_t))==-1)
    {
        close_all();
        err(12,"error");
    }
    return number;
}
void merge(const char*temp1,const char*temp2,int temp1Size,int temp2Size,const char*sourceFile)
{
    fds[1]=openReadSafe(temp1);
    fds[2]=openReadSafe(temp2);
    if((fds[0]==open(sourceFile,O_RDWR|O _TRUNC))==-1)
    {
        err(11,"error reading from file");
    }
    uint32_t num1=readNumber(fds[1],temp1);
    uint32_t num2=readNumber(fds[2],temp2);
    int counter=0;
    int minSize=MIN(temp1Size,temp2Size);
    while (counter<minSize)
    {
        if(num1<num2)
        {
            writeSafe(fds[0],num2,sourceFile)
            dprintf(1,"%d\n",num1);//to print it in the screen
            num1=readNumber(fds[1],temp1);
        }
        else{
             writeSafe(fds[0], num2, sourceFile);
            dprintf(1, "%d\n", num2);
            num2 = readNumber(fds[2], temp2);
        }
        counter++;
    }
    while (counter < temp1Size) {
        writeSafe(fds[0], num1, sourceFile);
        dprintf(1, "%d\n", num1);
        num1 = readNumber(fds[1], temp1);
        counter++;
    }

    while (counter < temp2Size) {
        writeSafe(fds[0], num2, sourceFile);
        dprintf(1, "%d\n", num2);
        num2 = readNumber(fds[2], temp2);
        counter++;
    }
}

void sortFile(const char *filename)
{
    struct stat st;
    if(stat(filename,&st)== -1)
    {
        err(2,"error");
    }
    //get the size of file in bytes by using stat
    if((st.st_size % sizeof(uint32_t))!=0)
    {
         errx(3, "File %s does not contain only unit32_t numbers", file_name);
        exit(0);
    }
    ssize_t fileSize=st.st_size;
    if((fileSize > 100000000))
    {
        errx(4."error,file contains too many numbers");
    }
    int fd=openReadSafe(filename);
    char temp1[11]="tempXXXXXX";
    char temp2[11]="tempXXXXXX";
    if(mktemp(temp1)==-1 ||  mktemp(temp2)==-1)
    {
        err(5,"couldn't create the temp files");
    }
    int numbersCount=fileSize/sizeof(uint32_t);
    int halfCount=numbersCount/2;
    int otherHalfCount=numbersCount-halfCount; //in case of odd size

    split(halfCount,fd,temp1,filename);
    split(otherHalfCount,fd,temp2,filename);
    close(fd);
    merge(temp1,temp2,halfCount,otherHalfCount,filename);
    close_all();

     if (unlink(temp1) == -1 || unlink(temp2) == -1) {
        err(6, "Could not unlink");
    }

}


int main(int argc, const char* argv[])
{
    if (argc != 2) { //cmd f1 
        err(1, "error");
    }
   const char *filename=argv[1];
   sortFile(filename);

    
}

 
