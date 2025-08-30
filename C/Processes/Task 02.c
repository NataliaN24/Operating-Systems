//T2 - Да се напише програма на C, която изпълнява команда ls с точно един аргумент.

 1 #include<unistd.h>
  2 #include<err.h>
  3 #include<stdio.h>
  4
  5
  6 int main(int argc, char*argv[])
  7 {
  8     if(argc <2)
  9     {
 10     errx(1,"error");
 11     }
 12
 13     if(execlp("ls","ls",argv[1],(char*)NULL)==-1)
 14     {
 15     errx(2,"error");
 16     }
 17     return 0;
 18 }
