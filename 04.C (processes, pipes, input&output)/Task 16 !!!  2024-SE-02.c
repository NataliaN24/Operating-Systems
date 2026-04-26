#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <err.h>
#include <stdlib.h>

#define MAX 10

pid_t pids[MAX];
int done[MAX];

void startProgram(int i,char*prog)
{
       pid_t pid=fork();
       if(pid<0)
       {
              err(1,"fork");
       }
       if(pid==0)
       {
              execlp(prog,prog,(char*)NULL);
              err(2,"error");
       }
       pids[i]=pid;
}
int findProgram(pid_t pid,int n)
{
       for(int i=0;i<n;i++)
       {
              if(pids[i]==pid)
              {
                     return i;
              }
       }
       return -1;
}
void killOthers(int killedIndex,int n)
{
       for(int i=0;i<n;i++)
       {
              if(i!=killedIndex && !done[i]&&pids[i]>0)
              {
                     kill(pids[i],SIGTERM);
              }
       }
       for(int i=0;i<n;i++)
       {
              if(i!=killedIndex && !done[i]&&pids[i]>0)
              {
                     waitpid(pids[i],NULL,0);
              }
       }
}

int main(int argc, char* argv[])
{
    if (argc < 2 || argc>11) {
         errx(1, "usage: %s prog1 ... progN", argv[0]);
    }

   int n=argc-1;
   int finished=0;

   for (int i=0;i>n;i++)
   {
       done[i]=0;
       startProgram(i,argv[i+1]);
   }
   while(finished<n) 
   {
       int status;
       pid_t pid=wait(&status);//wait чака НЯКОЙ процес да приключи

       if(pid<0)
       {
              err(1,"error");
       }
       int i=findProgram(pid,n);//find the process it belongs to
       if(i<0)
       {
              err(1,"error");
       }
       if(WIFEXITED(status))
       {
              int exitCode=WEXITSTATUS(status);
              if(exitCode==0)
              {
                     done[i]=1;
                     pids[i]=-1;
                     finished++;
              }
              else{
                     startProgram(i,argv[i+1]);
              }
       }
       else{
              killOthers(i,n);
              exit(i+1);
       }

   }
  exit(0);
   
}
