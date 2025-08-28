//T2 - Напишете програма на C, която демонстрира комуникация през pipe между parent и child процеси.
//Parent-ът трябва да изпраща стринга, получен като първи аргумент на командния ред към child-а, който да го 
//отпечатва на стандартния изход.

 1 #include <stdio.h>
  2 #include<stdlib.h>
  3 #include <unistd.h>
  4 #include <string.h>
  5 #include <err.h>
  6 #include <sys/wait.h>
  7
  8
  9 int main(int argc, char *argv[])
 10 {
 11
 12     if(argc !=2)
 13     {
 14     errx(1,"error");
 15     }
 16
 17     //create an array of 2 elements
 18     //it will contain pipefd[0] the ennd for reading in the pipe
 19     //pipefd[1] write end
 20
 21     int pipefd[2];
 22     if(pipe(pipefd) ==  -1)
 23     {
 24     perror("pipe");
 25     exit(2);
 26     }
 27
 28     pid_t pid=fork();
 29     if(pid == -1)
 30     {
 31     perror("fork");
 32     exit (3);
 33     }
 34
 35     if(pid == 0){
 36     //child process reads
 37     close(pipefd[1]); //close write end bc the child won't write
 38     char buff[1024];
 39     ssize_t bytes;
 40     while((bytes= read( pipefd[0], buff, sizeof(buff))) >0)
 41     {
 42     if(write(1,buff,bytes)!=bytes)
 43     {
 44     errx(4,"error");
 45     }
 46     }
 47     if(bytes ==-1){
 48     errx(5,"error");
 49     }
 50     }
 51         else
 52         {
 53         //parent process writes
 54         close (pipefd[0]); //close read end bc parent won't read
 55         const char *msg=argv[1]; //this is the string the parent needs to send
 56         size_t len= strlen(msg);
 57         if(write (pipefd[1],msg,len) !=len)
 58         {
 59         errx(6,"error");
 60         }
 61         close (pipefd[1]);
 62         wait(NULL);// wait the child to finish
 63         }
 64 }

