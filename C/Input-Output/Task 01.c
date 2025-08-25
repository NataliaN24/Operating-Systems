//Копирайте съдържанието на файл1 във файл2.

 #include <stdio.h>
  2 #include <stdlib.h>
  3 #include <unistd.h>
  4 #include <fcntl.h>
  5 #include <err.h>
  6 #include <stdio.h>
  7
  8 #define BUF_SIZE 4096   // 4 kb
  9
 10 int main(int argc, char *argv[])
 11 {
 12     if(argc !=3){
 13         errx(1,"expected 3 arguments");
 14     }
 15
 16     int fd_input=open(argv[1], O_RDONLY);
 17
 18     if(fd_input == -1)
 19     {
 20         errx(2,"couldn't open the file");
 21     }
 22
 23     int fd_out=open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);  // 644 ~owner can read and write the others only read
 24
 25     if(fd_out == -1)
 26     {
 27     errx(3,"error file could not be opened");
 28     close(fd_input);
 29     }
 30
 31     char buffer[BUF_SIZE];
 32     size_t bytes_read;  //how many bytes will be read after each call of read()
 33
 34     while ((bytes_read = read(fd_input, buffer, BUF_SIZE)) > 0){
 35         if ( write(fd_out, buffer, bytes_read) != bytes_read){
 36             errx(4,"error while writing to file");   //no need to close the files after errx, bc the program is automatically exited
 37             }
 38         }
 39     if(bytes_read == -1)
 40     {
 41     errx(5,"error reading");
 42     }
 43
 44     close(fd_input);
 45     close(fd_out);
 46
 47     printf("copy completed");

