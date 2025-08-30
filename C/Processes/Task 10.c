//Да се напише програма на C, която получава като параметри от 
команден ред две команди (без параметри).
//Изпълнява първата. Ако тя е завършила успешно изпълнява втората.
//Ако не, завършва с код 42.

   1 #include<stdio.h>
  2 #include<stdlib.h>
  3 #include<unistd.h>
  4 #include <sys/wait.h>
  5 #include<fcntl.h>
  6 #include<err.h>
  7 #include<string.h>
  8
  9 int main(int argc,char *argv[])
 10 {
 11    if(argc != 3)
 12    {
 13    errx(1,"Error");
 14    }
 15
 16    int status;
 17    pid_t pid=fork();
 18    if(pid ==-1)
 19    {
 20    errx(1,"Error");
 21    }
 22    if(pid==0)
 23    {
 24     execlp(argv[1],argv[1],(char*)NULL);
 25     errx(2,"Error");
 26
 27    }
 28    else
 29    {
 30    if(wait(&status)==-1)
 31    {
 32    errx(3,"Error");
 33    }
 34    if(WIFEXITED(status) && WEXITSTATUS(status)==0)
 35    {
 36    pid=fork();
 37    if(pid ==-1)
 38    {
 39    errx(4,"error");
 40    }
 41    if(pid==0)
 42    {
 43    execlp(argv[2],argv[2],(char*)NULL);
 44    errx(5,"Error");
 45    }
 46    if(wait(&status)==-1)
 47    {
 48    errx(6,"Error");
 49    }
 50    return WEXITSTATUS(status);
 51    }
 52     return 42;
 53    }
 54 }

 46    if(wait(&status)==-1)
