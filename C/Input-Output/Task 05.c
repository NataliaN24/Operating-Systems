//Реализирайте команда cp, работеща с два аргумента, подадени като входни параметри.

 1 #include <unistd.h>
  2 #include <fcntl.h>
  3 #include <stdlib.h>
  4 #include <err.h>
  5
  6 #define BUFFsize 1024
  7
  8 int main(int argc, char *argv[])
  9 {
 10     if(argc !=3 )
 11     {
 12     errx(1,"error");
 13     }
 14
 15     int fd1=open(argv[1],O_RDONLY);
 16     if(fd1 == -1)
 17     {
 18     errx(2,"error");
 19     }
 20
 21     int fd2=open(argv[2],O_WRONLY | O_CREAT | O_TRUNC, 0644);
 22     if(fd2 == -1)
 23     {
 24     close(fd1);
 25     errx(3,"error");
 26     }
 27
 28     char buff[BUFFsize];
 29     ssize_t bytes;
 30
 31     while((bytes = read(fd1,buff,BUFFsize))>0)
 32     {
 33     if(write(fd2,buff,bytes) != bytes)
 34     {
 35     close(fd1);
 36     close(fd2);
 37     errx(3,"error");
 38     }
 39     }
 40     if(bytes == -1)
 41     {
 42     close(fd1);
 43     close(fd2);
 44     errx(4,"error");
 45     }
 46     close(fd1);
 47     close(fd2);
 48     return 0;
 49
 50 }
