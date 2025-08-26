//Реализирайте команда swap, разменяща съдържанието на два файла, подадени като входни параметри.

 #include <fcntl.h>
  2 #include <unistd.h>
  3 #include <stdlib.h>
  4 #include <err.h>
  5
  6 #define BUFF_SIZE 4096
  7
  8 int main(int argc, char * argv[])
  9 {
 10     if(argc != 3)
 11     {
 12     errx(1,"error");
 13     }
 14
 15     char *f1=argv[1];
 16     char *f2=argv[2];
 17
 18     int fd1=open(f1,O_RDONLY);
 19     if(fd1 == -1)
 20     {
 21     errx(2,"error");
 22     }
 23     int fd2=open(f2,O_RDONLY);
 24     if(fd2 == -1)
 25     {
 26     close(fd1);
 27     errx(3,"error");
 28     }
 29
 30     char tempFile[] = "tempFile_XXXXXX";  //create a temp file in memory
 31     int fd3 = mkstemp(tempFile);
 32     if( fd3 == -1)
 33     {
 34         close(fd1);
 35         close(fd2);
 36         errx(4,"error");
 37     }
 38     //copy file 1 -> temp
 39
 40     char buffer[BUFF_SIZE];
 41     ssize_t bytes;
 42     while((bytes = read (fd1, buffer, BUFF_SIZE))>0)
 43     {
 44         if(write(fd3, buffer, bytes) != bytes)
 45         {
 46         close(fd1); close(fd2); close(fd3);
 47         errx(5, "error");
 48         }
 49
 50     }
 51     if(bytes == -1)
 52     {
 53     close(fd1);
 54     close(fd2);
 55     close(fd3);
 56     errx(6,"error");
 57     }
 58
 59     //copy File 2 to file 1
 60     close(fd1);
 61     fd1=open(f1, O_WRONLY | O_TRUNC);
 62     lseek(fd2,0,SEEK_SET); //reset file 2 to beginning
 63     while((bytes = read(fd2,buffer,BUFF_SIZE)) > 0)
 64     {
 65     if(write(fd1, buffer, bytes) != bytes)
 66     {
 67         close(fd1); close(fd2); close(fd3);
 68         errx(7,"error");
 69         }
 70     }
 71     if(bytes == -1) {
 72     close(fd1);
 73     close(fd2);
 74     close(fd3);
 75     }
 76         //copy temp -> file2
 77     fd3=open(tempFile, O_RDONLY);
 78     if(fd3 == -1){
 79     errx(8, "error");
 80     }
 81     fd2=open(f2, O_WRONLY | O_TRUNC);
 82     if(fd2 == -1)
 83     {
 84     close(fd1);
 85     close(fd2);
 86     close(fd3);
 87     errx(9,"error");
 88     }
 89
 90     while((bytes = read (fd3,buffer, BUFF_SIZE))>0){
 91
 92     if(write(fd2,buffer,bytes) != bytes)
    93     {
 94     close(fd2);
 95     close(fd3);
 96     errx(10,"error");
 97     }
 98
 99     }
100     if(bytes == -1)
101     {
102     close(fd2);
103     close(fd3);
104     errx(11,"error");
105     }
106
107     unlink(tempFile); //delete temp File
108     return 0;
109 }


