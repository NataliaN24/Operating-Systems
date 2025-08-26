//Реализирайте команда head без опции (т.е. винаги да извежда на стандартния изход само първите 10 реда от
//съдържанието на файл подаден като първи параматър).


  1 #include <stdio.h>
  2 #include <stdlib.h>
  3 #include <err.h>
  4 #include <unistd.h>
  5 #include <fcntl.h>
  6
  7 #define BUFF_SIZE 1024
  8
  9 int main(int argc, char* argv[])
 10 {
 11
 12     if(argc != 2)
 13     {
 14         errx(1,"two arguments needed");
 15      }
 16
 17     int fd=open(argv[1],O_RDONLY);
 18     if(fd == -1)
 19     {
 20     errx(2,"file could not be opened");
 21     }
 22
 23
 24
 25     char buffer[BUFF_SIZE];
 26
 27     int count=0;
 28
 29     ssize_t bytesRead; //ssize_t is like size_t but is signed
 30
 31     while (( bytesRead = read(fd,buffer,BUFF_SIZE)) > 0)
 32     {
 33         for (ssize_t i=0; i < bytesRead; i++) {
 34             write(1, &buffer[i],1); // print the symbol
 35             if (buffer[i] == '\n')
 36             {
 37             count++;
 38             if(count == 10){
 39             close(fd);
 40             return 0;
 41             }
 42           }
 43         }
 44     }
 45     if (bytesRead == -1)
 46     {
 47     err(3,"error reading the file");
 48     }
 49     close(fd);
 50     return 0;
 51
 52 }
