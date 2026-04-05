
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/param.h>

int main(int argc, char** argv) {
  //$ ./main patch.bin f1.bin f2.bin

   if(argc !=4)
   {
    err(1,"error");
   }   
   int fd1=open(argv[1],O_RDONLY);
   if(fd1<0)
   {
        err(1,"error");
   }
   int fd2=open(argv[2],O_RDONLY);
   if(fd2<0)
   {
        err(1,"error");
   }
   int fd3=open(argv[3],O_WRONLY|O_CREAT|O_TRUNC ,0644);
   if(fd3<0)
   {
        err(1,"error");
   }
   char buff[4096];
   while(read(fd2,buff,sizeof(buff))>0)
   {
      if(write(fd3,buff,sizeof(buff))<0)
      {
         err(1,"error");
      }
   }
   int fd2Size=lseek(fd2,0,SEEK_END);//move pointer to the end and get size
   if(fd2Size<0)
   {
      err(1,"error");
   }
   if(lseek(fd2,0,SEEK_SET)<0) //move pointer back to beginning
   {
      err(1,"Error");
   }
   uint32_t magic;//4 b
   uint8_t headerVer; // 1 b
   uint8_t dataVer;
   uint16_t count; //2
   uint32_t res1;
   uint32_t res2;
   //DE AD BE EF  01  00  02 00  00 00 00 00 00 00 00 00
   //01 00 42 78
   //03 00 44 79
   if(read(fd1,&magic,sizeof(magic))<0 || read(fd1,&headerVer,sizeof(headerVer))<0 || read(fd1,&dataVer,sizeof(dataVer))<0 || read(fd1,&count,sizeof(count))<0||read (fd1,&res1,sizeof(res1))<0||read (fd1,&res2,sizeof(res2))<0)
   {
      err(1,"error");
   }
   if(magic != "0xEFBEADDE " || headerVer != "0x01")
   {
      err(1,"error");
   }
   for(int i=0;i<count;i++)
   {
      if(dataVer=="0")
      {
         uint16_t offset;
         uint8_t originalByte;
         uint8_t oldByte;
         uint8_t newByte;

         if(read(fd1,&offset,sizeof(offset))<0 || read(fd1,&originalByte,sizeof(originalByte))<0 || read(fd1,&newByte,sizeof(newByte))<0)
         {
            err(1,"error");
         }
         if((int)offset>=fd2Size)
         {
            err(1,"error");
         }
         if(lseek(fd2,offset,SEEK_SET)<0)
         {
            err(1,"error");
         }
         if(read(fd2,&oldByte,sizeof(oldByte))<0)
         {
            err(1,"error");
         }
         if(oldByte !=originalByte)
         { 
            err(1,"error");
         }
         if(lseek(fd3,offset,SEEK_SET)<0)
         {
            err(1,"error");
         }
         if(write(fd3,&newByte,sizeof(newByte))<0)
         {
            err(1,"error");
         }

      }
      else if (dataVer=="1")
      {
            //same as above but different types for offset and words
      }
   }


  
   
}

