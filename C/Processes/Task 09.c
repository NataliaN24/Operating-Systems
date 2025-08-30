//Да се напише програма на C, която която създава файл в текущата директория
//и генерира два процесa,
//които записват низовете foo и bar в създадения файл.
//Програмата не гарантира последователното записване на низове.
//Променете програмата така, че да записва низовете последователно, като първия е foo.
 1 #include<stdio.h>
  2 #include<stdlib.h>
  3 #include<unistd.h>
  4 #include <sys/wait.h>
  5 #include<fcntl.h>
  6 #include<err.h>
  7 #include<string.h>
  8
  9 int main(void)
 10 {
 11     char*filename="file2.txt";
 12
 13
 14     pid_t pid1=fork();
 15
 16     if(pid1==-1)
 17     {
 18     errx(1,"error");
 19     }
 20
 21     if(pid1==0)
 22     {
 23     int fd=open(filename,O_WRONLY|O_CREAT|O_TRUNC,0644);
 24     if(fd ==-1)
 25     {
 26     errx(2,"error");
 27     }
 28     const char*str1="foo";
 29
 30     if(write(fd,str1,(ssize_t)strlen(str1))!=(ssize_t)strlen(str1))
 31     {
 32     errx(3,"error");
 33     }
 34
 35     close(fd);
 36     exit(0);
 37     }
 38
 39     int status;
 40     if(wait(&status)==-1)
 41     {
 42     errx(4,"Error");
 43     }
 44
 45      pid_t pid2=fork();
 46     if(pid2 ==-1)
   47     {
 48     errx(5,"error");
 49     }
 50     if(pid2 ==0)
 51     {
 52   int  fd=open(filename, O_WRONLY|O_APPEND,0644);
 53     if(fd ==-1)
 54     {
 55     errx(6,"error");
 56     }
 57     const char*str2="bar";
 58     if(write(fd,str2,strlen(str2) )!=strlen(str2))
 59     {
 60     errx(7,"error");
 61     }
 62     close(fd);
 63     exit(0);
 64     }
 65     if(wait(&status) ==-1){
 66     errx(8,"error");
 67     }
 68     return 0;
 69
 70 }

