#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stdlib.h>
void readWord(int dict,char*buff,uint32_t buffSize)
{
    char c;
    int readBytes;
    uint8_t buffIndex=0;
    while((readBytes=read(dict,&c,sizeof(c))))>0&& buffIndex<buffSize-1)
    {
        if(c=='\n')
        {
            break;
        }
        buff[buffIndex]=c;
        buffIndex++;
    }
    buff[buffIndex]='\0';
    if(readBytes<0)
    {
     err(2,"error");   
    }
}
int findWordIndex(int dict,int positions,const char*word,uint32_t wordsCount)
{
    int left=0;
    int right=wordsCount-1;
    uint32_t seeked=-1;
    while(left<right)
    {
        int mid=left+(right-left)/2;
        if(lseek(positions,mid*sizeof(uint32_t),SEEK_SET)<0)
        {
            err(4,"error");
        }
        uint32_t currPos;
        if(read(positions,&currPos,sizeof(currPos))<0)
        {
            err(4,"error");
        }
        if(lseek(dict,currPos+1,SEEK_SET)<0)
        {
            err(4,"error");
        }
        char currentWord[64];
        readWord(dict,currentWord,64);
        int res=strcmp(currentWord,word);
        if(res==0)
        {
            seeked=currPos;
            break;
        }
        else if(res<0)
        {
            right=mid-1;
        }
        else{
            left=mid+1;
        }
    }
    return seeked;
}
void getDefinition(int dict,int32_t wordIndex,const char* word)
{
    uint32_t len=strlen(word);
    if(lseek(dict,wordIndex+len+2,SEEK_SET)<0)
    {
        err(1,"error");
    }
    char byte;
    int readBytes;
    while(readBytes=read(dict,&byte,sizeof(byte))>0)
    {
        if(byte==0x00)
        {
            break;
        }
        if(write(1,&byte,sizeof(byte))<0)
        {
            err(1,"error");
        }
    }
    char newLine='\n';
    if(write(1,&newLine,sizeof(newLine))<0)
    {
        err(1,"Error");
    }
    if(readBytes<0)
    {
        err(1,"Error");
    }
}

int main(int argc, char* argv[])
{
    // \0word\ndefinition
    //\0apple\nfruit\0banana\nyellow fruit\0cat\nanimal
    //./main respect english.dic english.idx

    if(argc != 4){
        err(1, "Arguments count");
    }

    int dict=open(argv[2],O_RDONLY);
    if(dict < 0) { err(1, "error"); }


    int wordsPositions=open(argv[3],O_RDONLY);
    if(wordsPositions<0) { err(1, "error"); }
    char*word=argv[1];
    if(strlen(word)==0) { err(1, "error"); }

    struct stat st;
    if(fstat(wordsPositions,&st)<0){err(1,"error"); }
    uint32_t wordsCount=st.st_size /sizeof(uint32_t);
    int32_t  wordIndex=findWordIndex(dict,wordsPositions,word,wordsCount);

    if(wordIndex<0)
    {
        err(1,"error");
    }
    getDefinition(dict,wordIndex,word);

}



    

   

   
}
