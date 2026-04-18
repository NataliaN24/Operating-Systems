
#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <pwd.h>

const char DING[] = "DING ";
const char DONG[] = "DONG ";
char c = 'a';

int main(int argc, char* argv[])
{
   if(argc!=3)
   {
     err(1,"error");
   }
   int n=*argv[1]-'0';
   int d=*argv[2]-'0';
   
   int parentToChild[2];
   if(pipe(pipe1)<0)
   {
      err(1,"error");
   }
   int childToParent[2];
   if(pipe(pip2)<0)
   {
      err(1,"error");
   }
   int child=fork();
   if(child<0){err(1,"error");}
   if(child==0)
   {
     close(parentToChild[1]);
     close(childToParent[0]);
     char x;
     for(long i=0;i<n;i++)
     {
          if(read(parentToChild[0],&x,1)!=1)
          {
                 err(1, "child read");
          }
          if(write(1,DONG,5)!=5)
          {
                 err(1, "child read");
          }
          if(write(childToParent[1],"x",1)!=1)
          {
                 err(1, "child read");
          }
     }
     close(parentToChild[0]);
     close(childToParent[1]);
     _exit(0);
   }
   //parent

   close(parentToChild[0]);
   close(childToParent[1]);
   char x;
   for(long i=0;i<n;i++)
   {
     if(write(1,DING,5)!=5)
     {
            err(1, "child read");
     }
     if(write(parentToChild[1],"x",1)!=1)
     {
            err(1, "child read");
     }
     if(read(childToParent[0],&x,1)!=1)
     {
            err(1, "child read");
     }
     sleep((unsigned int)D);
   }
     close(p2c[1]);
    close(c2p[0]);

    int status;
    if (wait(&status) < 0) {
        err(1, "wait");
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        errx(1, "child did not exit successfully");
    }

    return 0;

}
