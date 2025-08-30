//Да се напише програма на С, която получава като параметър 
//команда (без параметри) и при успешното ѝ изпълнение,
//извежда на стандартния изход името на командата.
 1 #include<unistd.h>
  2 #include<err.h>
  3 #include<stdlib.h>
  4 #include<sys/wait.h>
  5 #include<stdio.h>
  6
  7 int main(int argc,char*argv[])
  8 {
  9
 10     if(argc !=2)
 11     {
 12     errx(1,"error");
 13     }
 14
 15     pid_t pid=fork();
 16
 17     if(pid <0)
 18     {
 19     errx(2,"error");
 20     }
 21
 22     if(pid==0)
 23     {
 24         //child process executes the program
 25
 26         execlp(argv[1],argv[1],(char*)NULL);
 27         errx(3,"error ");
 28     }
 29     else
 30     {
 31         //parent waits for child
 32         int status;
 33         if(wait(&status) == -1)
 34         {
 35         err(4,"error");
 36         }
 37
 38         if(WIFEXITED(status) &&WEXITSTATUS(status)==0)
 39         {
 40         printf("command '%s' is succesfully executed\n",argv[1]);
 41         }
 42         else
 43         {
 44         printf("not successfully exceuted\n",argv[1]);
 45         }
 46     }
47     return 0;
 48     }

