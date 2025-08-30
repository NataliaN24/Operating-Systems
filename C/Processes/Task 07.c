//Да се напише програма на С, която получава като параметри три команди (без параметри), 
//изпълнява ги последователно,
//като изчаква края на всяка и извежда на стандартния изход номера на
//завършилия процес, както и неговия код на завършване.

  1 #include <stdio.h>
  2 #include<stdlib.h>
  3 #include<err.h>
  4 #include<sys/wait.h>
  5 #include<unistd.h>
  6
  7 int main(int argc, char*argv[])
  8 {
  9
 10     if(argc !=4 ){errx(1,"error");}
 11
 12     for(int i=1;i<=3;i++)
 13     {
 14     pid_t pid=fork();
 15
 16     if(pid <0)
 17     {
 18     errx(2,"error");
 19     }
 20     if(pid ==0)
 21     {
 22     execlp(argv[i],argv[i],(char*)NULL);
 23     errx(4,"error for '%s' ",argv[i]);
 24     }
 25     else
 26     {
 27     int status;
 28     if(wait(&status)==-1)
 29     {
 30     errx(5,"error");
 31     }
 32     if(WIFEXITED (status))
 33     {
 34     printf("process %d (command '%s') exited with status %d\n",pid, argv[i],WEXITSTATUS(status));
 35     }
 36    }
 37    }
 38
 39 }
