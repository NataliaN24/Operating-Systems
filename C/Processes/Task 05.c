//T5 - Да се напише програма на C, която е аналогична на горния пример, 
//но принуждава бащата да изчака сина си да завърши.
  1 #include<stdio.h>
  2 #include<unistd.h>
  3 #include<err.h>
  4 #include<sys/wait.h>
  5
  6 int main(void)
  7 {
  8
  9 pid_t pid=fork();
 10 if(pid <0)  //error in fork
 11 {
 12 errx(1,"error");
 13 }
 14
 15 if(pid == 0)//in child
 16 {
 17     for(int i=0;i<5;i++)
 18     {
 19     write(1,"child\n",5);
 20     }
 21 }
 22 else
 23 {
 24     wait(NULL);
 25 //parent process
 26
 27     for(int i=0;i<5;i++)
 28     {
 29     write(1,"parent",6);
 30     }
 31
 32     }
 33 }
~
