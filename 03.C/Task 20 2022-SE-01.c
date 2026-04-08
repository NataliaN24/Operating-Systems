#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

const uint32_t MAX_SIZE = 524288;

int main(int argc, const char* argv[])
{
    if(argc != 3){
        err(1, "Incorrect input");
    }

    int data = open(argv[1], O_RDONLY);
    if(data < 0){
        err(2, "Open");
    }

    int compare = open(argv[2], O_RDONLY);
    if(compare < 0){
        err(3, "Open");
    }

    uint32_t magic, countData;

    if(read(data, &magic, sizeof(magic)) < 0 || read(data, &countData, sizeof(countData)) < 0){
        err(4, "Read");
    }

    if(magic != 0x21796F4A){
        err(5, "Logical error");
    }

    uint32_t magic1;
    uint16_t magic2, reserved;
    uint64_t countComp;

    if(read(compare, &magic1, sizeof(magic1)) < 0 || read(compare, &magic2, sizeof(magic2)) < 0 || read(compare, &reserved, sizeof(reserved)) < 0 || read(compare, &countComp, sizeof(countComp)) < 0){
        err(6, "Read");
    }

    uint16_t type, reserved1, reserved2, reserved3;
    uint32_t offset1, offset2;

    for(uint64_t i = 0; i < countComp; i++)
    {
        if(read(compare, &type, sizeof(type)) < 0 || read(compare, &reserved1, sizeof(reserved1)) < 0 || read(compare, &reserved2, sizeof(reserved2)) ||
           read(compare, &reserved3, sizeof(reserved3)) < 0 || read(compare, &offset1, sizeof(offset1)) < 0 || read(compare, &offset2, sizeof(offset2)) < 0){
            err(7, "Read");
        }

        if(reserved1 != 0 || reserved2 != 0 || reserved2 != 0){
            err(12, "Reserved != 0");
        }

        uint64_t num1, num2;
        if(lseek(data, (off_t)offset1, SEEK_SET) < 0){
            err(8, "Lseek");
        }

        if(read(data, &num1, sizeof(num1)) < 0){
            err(9, "Read");
        }

        if(lseek(data, (off_t)offset2, SEEK_SET) < 0){
            err(10, "Lseek");
        }

        if(read(data, &num2, sizeof(num2)) < 0){
            err(11, "Read");
        }

        bool fulfilled = 1;
        if(type == 1){
            if(!(num1 > num2)){
                fulfilled = 0;
            }
        } else if(type == 0){
            if(!(num1 < num2)){
                fulfilled = 0;
            }
        } else {
            err(18, "Error");
        }

        if(!fulfilled){
            if(lseek(data, (off_t)offset1, SEEK_SET) < 0){
                err(13, "Lseek");
            }

            if(write(data, &num2, sizeof(num2)) < 0){
                err(14, "Write");
            }

            if(lseek(data, (off_t)offset2, SEEK_SET) < 0){
                err(15, "Lseek");
            }

            if(write(data, &num1, sizeof(num1)) < 0){
                err(16, "Write");
            }
        }
    }

    if(close(data) < 0 || close(compare) < 0){
        err(17, "Close");
    }

    exit(0);
}
