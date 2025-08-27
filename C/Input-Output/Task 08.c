//Koпирайте файл /etc/passwd в текущата ви работна директория и 
//променете разделителят на копирания файл от ":", на "?"

 1 #include <unistd.h>
  2 #include <fcntl.h>
  3 #include <stdlib.h>
  4 #include <err.h>
  5
  6 #define size 4096
  7 int main()
  8 {
  9     const char *src="/etc/passwd";
 10     const char *dest="passwdCopy";
 11
 12     int fd1=open(src,O_RDONLY);
 13     if(fd1==-1)
 14     {
 15     errx(1,"error");
 16     }
 17     int fd2=open(dest,O_WRONLY | O_TRUNC| O_CREAT ,0644);
 18     if(fd2 ==-1)
 19     {
 20     errx(2,"error");
 21     }
 22
 23     char buff[size];
 24     ssize_t bytes;
 25     while((bytes=read(fd1,buff,sizeof(buff)))>0){
 26     for(ssize_t i=0;i<bytes;i++)
 27     {
 28         if(buff[i]==':'){
 29         buff[i]='?';
 30         }
 31     }
 32     if(write(fd2,buff,bytes)!=bytes)
 33     {
 34     close(fd1);
 35     errx(3,"error");
 36     }
 37     }
 38     if(bytes==-1){
 39     close(fd1);
 40     close(fd2);
 41     errx(4,"Error");
 42     }
 43     close(fd1);
 44     close(fd2);
 45     return 0;
 46 }
