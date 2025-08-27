//Реализирайте команда cp, работеща с произволен брой подадени входни параметри.
//explanation
The standard UNIX cp works like this:

If you give two arguments, it copies file1 → file2.
If you give more than two arguments, it copies all source files into the last argument,
which must be a directory.
If you give only one argument, it’s invalid.

 1 #include <unistd.h>
  2 #include <fcntl.h>
  3 #include <stdlib.h>
  4 #include <err.h>
  5 #include <string.h>
  6 #include <sys/stat.h>
  7
  8 #define size 4096
  9 #define maxlen_path 1024
 10
11 void copyFile(const char* src, const char*dest)
 12 {
 13
 14     int fd1=open(src,O_RDONLY);
 15     if(fd1 == -1)
 16     {
 17     errx(1,"error");
 18     }
 19     int fd2=open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
 20     if(fd2 == -1){
 21     close(fd1);
 22     errx(2,"error");
 23     }
 24     char buff[size];
 25     ssize_t bytes;
 26     while((bytes=read(fd1,buff,sizeof(buff)))>0){
 27         if(write(fd2,buff,bytes)!= bytes){
 28         close(fd1);
 29         close(fd2);
 30         errx(3,"error");
 31         }
 32     }
 33     if(bytes == -1)
 34     {
 35     close(fd1);
 36     close(fd2);
 37     errx(4,"error");
 38     }
 39     close(fd1);
 40     close(fd2);
 41 }
 42
 int main(int argc, char *argv[])
 44 {
 45     if(argc <3)
 46     {
 47     errx(6,"error");
 48     }
 49     if(argc == 3) {
 50     //two arguments
 51     copyFile(argv[1],argv[2]);
 52     }
 53     else
 54     {
 55     const char *destDir=argv[argc - 1 ];
 56
 57     for(int i=1;i<argc -1;i++)
 58     {
 59     const char *src=argv[i];
 60     const char *srcname=strrchr(src,'/');
 61     srcname=srcname ? srcname +1 :src;
 62
 63     //build dest path manually
 64
 65     char destpath[maxlen_path];
 66     strcpy(destpath,destDir);
 67     strcat(destpath,"/");
 68     strcat(destpath,srcname);
 69
 70     copyFile(src,destpath);
 71
 72     }
 73     }
 74     }

 73 }
