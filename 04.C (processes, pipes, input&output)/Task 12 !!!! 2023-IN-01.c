
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        errx(26, "Invalid arguments");
    }

    int NC = strtol(argv[1], NULL, 10);
    int WC = strtol(argv[2], NULL, 10);

    if(NC < 1 || NC > 7 || WC < 1 || WC > 35)
    {
        errx(26, "Invalid arguments");
    }

    int total = NC + 1;

    int pipes[total][2];
    pid_t pids[NC];

    for(int i = 0; i < total; i++)
    {
        if(pipe(pipes[i]) < 0)
        {
            err(26, "pipe");
        }
    }

    int id = 0;

    for(int i = 1; i < total; i++)
    {
        pid_t pid = fork();

        if(pid < 0)
        {
            err(26, "fork");
        }

        if(pid == 0)
        {
            id = i;
            break;
        }

        pids[i - 1] = pid;
    }

    char* words[3] =
    {
        "tic ",
        "tac ",
        "toe\n"
    };

    while(1)
    {
        int num;

        if(read(pipes[id][0], &num, sizeof(num)) < 0)
        {
            err(26, "read");
        }

        if(num >= WC)
        {
            int next = (id + 1) % total;

            write(pipes[next][1], &num, sizeof(num));

            break;
        }

        if(write(1, words[num % 3], 4) < 0)
        {
            err(26, "write");
        }

        num++;

        int next = (id + 1) % total;

        if(write(pipes[next][1], &num, sizeof(num)) < 0)
        {
            err(26, "write");
        }
    }

    if(id == 0)
    {
        int start = 0;

        if(write(pipes[0][1], &start, sizeof(start)) < 0)
        {
            err(26, "write");
        }

        for(int i = 0; i < NC; i++)
        {
            waitpid(pids[i], NULL, 0);
        }
    }

    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/wait.h>
#include <err.h>
#include <errno.h>

const char *words[]={ "tic ", "tac ", "toe\n"};

int main(int argc, char* argv[])
{
   if(argc!=3)
   {
     err(1,"error");
   }
       char *endptr;


       long NC=strtol(argv[1],&endptr,10); ///10 значи десетична бройна система.
       if ( *end!='\0' || NC <1 ||NC>7)
       {
              errx(1, "invalid NC");
       }
       long WC=strtol(argv[2],&end,10);
        if ( *end!='\0' || WC <1 ||WC>35)
       {
              errx(1, "invalid WC");
       }
       int total=NC+1; //child+parent
       int p[8][2];//max NC=7 -> 7 childs+1 parent 
       for(int i=0;i<total;i++)
       {
              if(pipe(p[1])<0)
              {
                     err(1,"error");
              }
       }
       for(int i=1;i<total;i++)//bc i=0 is parent
       {
              pid_t pid=fork();
              if(pid<0)
              {
                      err(1,"error");
              }
              if(pid==0)
              {
                     int id=i;
                     int next=(id +1)%total;
                     int k;
              
                     while(1)
                     {
                            if(read(p[id][0],&k,sizeof(k))!=sizeof(k))
                            {
                                   err(1,"error");
                            }
                            if(k>=WC)
                            {
                                   write(p[next][1],&k,sizeof(k));
                                   exit(0);
                            }
                            write(1,words[k%3],4);
                            k++;
                            write(p[next][1],&k,sizeof(k));

                            }
              }
       }
       int k=0;
       write(p[0][1],&k,sizeof(k));
       while(1)
       {
              if(read(p[0][0],&k,sizeof(k))!=sizeof(k))
              {
                     err(1,"error");
              }
              if(k>=WC )
              {
                     write(p[1][1],&k,sizeof(k));
                     break;
              }
              write (1,words[k%3],4);
              k++;
              write(p[1][1],&k,sizeof(k));

       }
       for(int i=0;i<NC;i++)
       {
              wait(NULL);
       }

}
