//T1 - Да се напише програма на C, която изпълнява команда date.

1 #include <unistd.h>
  2 #include<stdlib.h>
  3 #include<err.h>
  4 #include<stdio.h>
  5
  6 int main(void)
  7 {
  8     if(execl("/bin/date","date",(char*)NULL)==-1)
  9     {
 10     errx(1,"error");
 11     }
 12     return 0;
 13 }
~
