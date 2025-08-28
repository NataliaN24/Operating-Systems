//T3 - Напишете програма на C, която демонстрира употребата на dup/dup2 между parent и child процеси.
//Parent-ът трябва да изпраща стринга, получен като първи аргумент на командния ред към child-а,
//където той да може да се чете от stdin. Child процесът да изпълнява wc -c.

1 #include <stdio.h>
  2 #include <stdlib.h>
  3 #include <unistd.h>
  4 #include<string.h>
  5 #include <sys/wait.h>
  6 #include <err.h>
  7
  8 int main(int argc, char *argv[])
  9 {
 10     if(argc != 2)
 11     {
 12     errx(1,"error");
 13     }
 14
 15     int fd[2];
 16     if(pipe(fd) == -1)
 17     {
 18     errx(2,"error");}
 19
 20     pid_t pid=fork();
 21     if(pid ==-1)
 22     {
 23     errx(3,"error");
 24     }
 25
 26     if(pid ==0)
 27     {
 28
 29     close(fd[1]);
 30     dup2(fd[0],0);// direct stdin to pipe read end
 31
 32     if(execlp("wc","wc","-c",(char *)NULL) ==-1) //executes wc -c
 33     { close(fd[0]);
 34     errx(4,"error");
 35     }
 36     }
 37     else
 38     {
 39     close(fd[0]);
 40     const char*str=argv[1];
 41     if(write(fd[1],str,strlen(str)) != strlen(str))
 42     {
 43     errx(5,"error");
 44     }
 45     close(fd[1]);
 46     wait(NULL);

 47     }
 48 }

