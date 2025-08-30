//T3 - Да се напише програма на C, която изпълнява команда sleep (за 60 секунди).

  1 #include<err.h>
  2 #include <unistd.h>
  3 #include<stdlib.h>
  4
  5 int main(void)
  6 {
  7
  8 if(execlp("sleep","sleep","60",(char*)NULL)==-1)
  9 {
 10 errx(1,"error");
 11 }
 12 return 0;
 13 }
~
