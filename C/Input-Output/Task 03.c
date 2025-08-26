//Реализирайте команда wc, с един аргумент подаден като входен параметър.

#include <stdio.h>
  2 #include <stdlib.h>
  3 #include <err.h>
  4 #include <unistd.h>
  5 #include <fcntl.h>
  6 #include <ctype.h>
  7
  8 #define BUFF_SIZE 1024
  9
 10 int main(int argc, char * argv[])
 11 {
 12     if(argc !=2 )
 13     {
 14     errx(1, "2 arguments needed");
 15     }
 16
 17     int fd=open(argv[1],O_RDONLY);
 18     if(fd == -1){
 19         errx(2,"error");
 20     }
 21
 22     char buffer[BUFF_SIZE];
 23     ssize_t bytesRead;
 24     long lines=0;
 25     long words=0;
 26     long chars=0;
 27     int in_word=0;
 28
 29     while((bytesRead=read(fd,buffer, BUFF_SIZE))>0)
 30     {
 31      for(ssize_t i=0; i < bytesRead ;i++)
 32      {
 33         chars++;
 34         if(buffer[i] =='\n')
 35         {
 36             lines++;
 37         }
 38
 39         if (!isspace(buffer[1])){
 40             if(!in_word)
 41             {
 42             words++;
 43             in_word = 1;
 44             }
 45          }
 46          else
 47          {
 48          in_word=0;
 49          }
 50
 51
 52     }
 53
 54     }
 55     if(bytesRead == -1)
 56     {
 57     errx(3,"error");
 58     }
 59
 60     dprintf(1, "lines: %ld\nWords: %ld\nChars: %ld\n",lines,words,chars);
 61     close(fd);
 62     return 0;
 63 }
