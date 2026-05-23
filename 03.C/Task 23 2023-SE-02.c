#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[])
{
    if(argc != 4){
        errx(1, "usage: ./main word english.dic english.idx");
    }

    if(strlen(argv[1]) > 63){
        errx(2, "word too long");
    }

    int dic = open(argv[2], O_RDONLY);
    if(dic < 0){
        err(3, "open dictionary");
    }

    int idx = open(argv[3], O_RDONLY);
    if(idx < 0){
        err(4, "open index");
    }

    off_t idxSize = lseek(idx, 0, SEEK_END);
    if(idxSize < 0){
        err(5, "lseek");
    }

    if(idxSize % sizeof(uint32_t) != 0){
        errx(6, "invalid index file");
    }

    uint32_t count = idxSize / sizeof(uint32_t);

    uint32_t left = 0;
    uint32_t right = count;

    while(left < right)
    {
        uint32_t mid = left + (right - left) / 2;

        uint32_t pos;

        if(lseek(idx, mid * sizeof(uint32_t), SEEK_SET) < 0){
            err(7, "lseek idx");
        }

        if(read(idx, &pos, sizeof(pos)) != sizeof(pos)){
            err(8, "read idx");
        }

        if(lseek(dic, pos + 1, SEEK_SET) < 0){
            err(9, "lseek dic");
        }

        char word[64];
        uint8_t c;
        uint8_t len = 0;

        while(1)
        {
            int r = read(dic, &c, sizeof(c));
            if(r < 0){
                err(10, "read word");
            }
            if(r == 0){
                errx(11, "invalid dictionary");
            }

            if(c == '\n'){
                break;
            }

            if(c == 0){
                errx(12, "invalid word");
            }

            if(len == 63){
                errx(13, "word too long in dictionary");
            }

            word[len] = c;
            len++;
        }

        word[len] = '\0';

        int cmp = strcmp(argv[1], word);

        if(cmp == 0)
        {
            uint8_t buff[4096];

            while(1)
            {
                int r = read(dic, buff, sizeof(buff));
                if(r < 0){
                    err(14, "read definition");
                }

                if(r == 0){
                    break;
                }

                int i;
                for(i = 0; i < r; i++)
                {
                    if(buff[i] == 0){
                        break;
                    }
                }

                if(write(1, buff, i) != i){
                    err(15, "write stdout");
                }

                if(i < r){
                    break;
                }
            }

            close(dic);
            close(idx);
            exit(0);
        }

        if(cmp < 0){
            right = mid;
        } else {
            left = mid + 1;
        }
    }

    const char msg[] = "word not found\n";
    if(write(1, msg, sizeof(msg) - 1) != sizeof(msg) - 1){
        err(16, "write");
    }

    close(dic);
    close(idx);
    exit(0);
}
/////////////////////////////////////////////////////


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
