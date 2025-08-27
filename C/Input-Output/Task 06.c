//Реализирайте команда cat, работеща с произволен брой подадени входни параметри.

  1 #include <unistd.h>
  2 #include <fcntl.h>
  3 #include <stdlib.h>
  4 #include <err.h>
  5
  6
  7 void catFiles(const char *filename)
  8 {
  9     int fd=open(filename,O_RDONLY);
 10     if(fd == -1)
 11     {
 12     errx(2,"error");
 13     }
 14     char c;
 15     ssize_t bytes;
 16     while((bytes=read(fd,&c , sizeof(c)))>0) //read char after char
 17     {
 18     if(write(1, &c,bytes)!=bytes)
 19     {
 20     close(fd);
 21     errx(3,"error");
 22     }
 23     }
 24     if(bytes==-1)
 25     {
 26     close(fd);
 27     errx(4,"error");
 28     }
 29     close(fd);
 30
 31 }
 32 int main(int argc, char * argv[])
 33 {
 34     if(argc < 2)
 35     {
 36         errx(1,"one file ");
 37     }
 38
 39     for (int i=1 ; i<argc; i++) //bc argv[0] is the program name
 40     {
 41         catFiles(argv[i]);
 42     }
 43     return 0;
 44 }
