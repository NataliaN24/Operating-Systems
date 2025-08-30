//Да се напише програма на С, която получава като параметър име на файл.
Създава процес син, който записва стринга
//foobar във файла (ако не съществува, го създава, в
противен случай го занулява), след което процеса родител прочита
//записаното във файла съдържание и го извежда на 
стандартния изход, добавяйки по един интервал между всеки два символа.
 1 #include<stdio.h>
  2 #include<stdlib.h>
  3 #include<unistd.h>
  4 #include<sys/wait.h>
  5 #include<fcntl.h>
  6 #include<err.h>
  7 #include<string.h>
  8 int main(int argc,char*argv[])
  9 {
 10     if(argc != 2)
 11     {
 12     errx(1,"error");
 13     }
 14
 15     char *filename=argv[1];
 16     pid_t pid=fork();
 17
 18     if(pid ==-1)
 19     {
 20     errx(2,"error");
 21     }
 22
 23     if(pid ==0)
 24     {
 25     int fd=open(filename,O_WRONLY|O_CREAT|O_TRUNC,0644);
 26
 27     if(fd ==-1)
 28     {
 29     errx(3,"error");
 30     }
 31     const char*str="foobar";
 32     if(write(fd,str,strlen(str))!=strlen(str))
 33     {
 34     errx(4,"error");
 35     }
 36     close(fd);
 37     exit(0); //child completes execution
 38     }
 39     else{
 40         int status;
 41         if(wait(&status) == -1)
 42         {
 43         errx(5,"error");
 44         }
 45
 46         if(WIFEXITED(status) && WEXITSTATUS(status) ==0)
    47         {
 48         int fd=open(filename,O_RDONLY);
 49         if(fd ==-1)
 50         {
 51         errx(6,"error");
 52         }
 53         char buff[2];
 54         ssize_t bytes;
 55         while((bytes= read(fd,buff,2))>0)
 56         {
 57         write(1,buff,2);
 58         write(1," " ,1);
 59         }
 60         if(bytes ==-1)
 61         {
 62         errx(7,"error");
 63         }
 64         close(fd);
 65         }
 66         }
 67 }

