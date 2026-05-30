#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <err.h>

int main(void)
{
    int input = open("input.bin", O_RDONLY);
    if (input < 0) {
        err(1, "open input.bin");
    }

    uint16_t ram_size;
    uint16_t register_count;
    char filename[8];

    while (read(input, &ram_size, sizeof(ram_size)) == sizeof(ram_size)) {
        if (read(input, &register_count, sizeof(register_count)) != sizeof(register_count)) {
            err(2, "read register_count");
        }

        if (read(input, filename, sizeof(filename)) != sizeof(filename)) {
            err(3, "read filename");
        }

        pid_t pid = fork();
        if (pid < 0) {
            err(4, "fork");
        }

        if (pid == 0) {
            int fd = open(filename, O_RDWR);
            if (fd < 0) {
                err(5, "open processor file");
            }

            uint8_t registers[32];
            uint8_t ram[512];

            if (read(fd, registers, register_count) != register_count) {
                err(6, "read registers");
            }

            if (read(fd, ram, ram_size) != ram_size) {
                err(7, "read ram");
            }

            uint8_t instructions[4096];
            ssize_t instr_bytes = read(fd, instructions, sizeof(instructions));
            if (instr_bytes < 0) {
                err(8, "read instructions");
            }

            if (instr_bytes % 4 != 0) {
                errx(9, "bad instructions size");
            }

            int instr_count = instr_bytes / 4;
            int pc = 0;

            while (pc < instr_count) {
                uint8_t opcode = instructions[pc * 4];
                uint8_t op1 = instructions[pc * 4 + 1];
                uint8_t op2 = instructions[pc * 4 + 2];
                uint8_t op3 = instructions[pc * 4 + 3];

                if (opcode == 0) {
                    registers[op1] = registers[op2] & registers[op3];
                    pc++;
                } else if (opcode == 1) {
                    registers[op1] = registers[op2] | registers[op3];
                    pc++;
                } else if (opcode == 2) {
                    registers[op1] = registers[op2] + registers[op3];
                    pc++;
                } else if (opcode == 3) {
                    registers[op1] = registers[op2] * registers[op3];
                    pc++;
                } else if (opcode == 4) {
                    registers[op1] = registers[op2] ^ registers[op3];
                    pc++;
                } else if (opcode == 5) {
                    if (write(1, &registers[op1], 1) != 1) {
                        err(10, "print");
                    }
                    pc++;
                } else if (opcode == 6) {
                    sleep(registers[op1]);
                    pc++;
                } else if (opcode == 7) {
                    registers[op1] = ram[registers[op2]];
                    pc++;
                } else if (opcode == 8) {
                    ram[registers[op2]] = registers[op1];
                    pc++;
                } else if (opcode == 9) {
                    if (registers[op1] != registers[op2]) {
                        pc = op3;
                    } else {
                        pc++;
                    }
                } else if (opcode == 10) {
                    registers[op1] = op2;
                    pc++;
                } else if (opcode == 11) {
                    ram[registers[op1]] = op2;
                    pc++;
                } else {
                    errx(11, "bad opcode");
                }
            }

            if (lseek(fd, 0, SEEK_SET) < 0) {
                err(12, "lseek");
            }

            if (write(fd, registers, register_count) != register_count) {
                err(13, "write registers");
            }

            if (write(fd, ram, ram_size) != ram_size) {
                err(14, "write ram");
            }

            close(fd);
            _exit(0);
        }
    }

    close(input);

    while (wait(NULL) > 0) {
    }

    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////




#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <sys/stat.h>
#include <sys/wait.h>

typedef struct {
    uint8_t opcode;
    uint8_t op1;
    uint8_t op2;
    uint8_t op3;
} instr_t;
void runSimulation(uint16_t ramSize,uint16_t regCnt,const char *fname)
{
       int fd=open(fname,O_RDWR);
       if(fd<0)
       {
              err(1,"error");
       }
       struct stat st;
       if(stat(fd,&st)<0)
       {
              err(1,"error");
       }
       size_t startSize=regCnt+ramSize;
       if((size_t)st.st_size <startSize)
       {
              err(1,"error");
       }
       size_t instrBytes=st.st_size -startSize;
       if(instrBytes %4!=0)
       {
              err(1,"error");
       }
       size_t instrCnt=instrBytes/4;
       uint8_t*registers=malloc(regCnt);
       uint8_t*ram=malloc(ramSize);
       instr_t*instructions=malloc(instrBytes);

       if (registers == NULL || ram == NULL || instructions == NULL) {
        err(1, "malloc");
       }

       if(read(fd,registers,regCnt)!=regCnt)
       {
              err(1,"error");
       }
       if(read(fd,ram,ramSize)!=ramSize)
       {
               err(1,"error");
       }
       if(read(fd,instructions,instrBytes)!=(ssize_t)instrBytes)
       {
               err(1,"error");
       }

       size_t pc=0;
       while(pc<instrCnt)
       {
              instr_t in=instructions[pc];
              switch(in.opcode)
              {
                   case 0: // AND
                registers[in.op1] = registers[in.op2] & registers[in.op3];
                pc++;
                break;

            case 1: // OR
                registers[in.op1] = registers[in.op2] | registers[in.op3];
                pc++;
                break;

            case 2: // ADD
                registers[in.op1] = registers[in.op2] + registers[in.op3];
                pc++;
                break;

            case 3: // MULTIPLY
                registers[in.op1] = registers[in.op2] * registers[in.op3];
                pc++;
                break;

            case 4: // XOR
                registers[in.op1] = registers[in.op2] ^ registers[in.op3];
                pc++;
                break;

            case 5: // PRINT
                if (write(1, &registers[in.op1], 1) != 1) {
                    err(1, "write stdout");
                }
                pc++;
                break;

            case 6: // SLEEP
                sleep(registers[in.op1]);
                pc++;
                break;

            case 7: // LOAD
                registers[in.op1] = ram[registers[in.op2]];
                pc++;
                break;

            case 8: // STORE
                ram[registers[in.op2]] = registers[in.op1];
                pc++;
                break;

            case 9: // JNE
                if (registers[in.op1] != registers[in.op2]) {
                    pc = in.op3;
                } else {
                    pc++;
                }
                break;

            case 10: // LOADI
                registers[in.op1] = in.op2;
                pc++;
                break;

            case 11: // STOREI
                ram[registers[in.op1]] = in.op2;
                pc++;
                break;

            default:
                errx(1, "unknown opcode");
              }
         }
         if (lseek(fd, 0, SEEK_SET) < 0) {
        err(1, "lseek");
    }

    if (write(fd, registers, regCnt) != regCnt) {
        err(1, "write registers");
    }

    if (write(fd, ram, ramSize) != ramSize) {
        err(1, "write ram");
    }

    free(registers);
    free(ram);
    free(instructions);
    close(fd);
 }

int main(int argc, char* argv[])
{
    
   if (argc != 2) {
        errx(1, "usage: %s input.bin", argv[0]);
    }
    int inputFd=open(argv[1],O_RDONLY);
    if(inputFd <0)
    {
        err(1, "open input");
    }
    struct stat st;
    if(stat(inputFd,&st)<0)
    {
       err(1, "stat input");
    }
    if(st.st_size%12!=0)
    {
       err(1,"error");
    }
    int processors=st.st_size /12;

    for(int i=0;i<processors;i++)
    {
       uint16_t ramSize;
       uint16_t regCnt;
       char filename[8];
       if(read(inputFd,&ramSize,sizeof(ramSize))!=sizeof(ramSize))
       {
              err(1,"error");
       }
       if(read(inputFd,&regCnt,sizeof(regCnt))!=sizeof(regCnt))
       {
              err(1,"error");
       }
       if(read(inputFd,&filename,sizeof(filename))!=sizeof(filename))
       {
              err(1,"error");
       }
       if(ramSize>512)
       {
              err(1,"error");
       }
       if(regCnt>32)
       {
              err(1,"error");
       }
       if(filename[7]!='\0')
       {
              err(1,"error");
       }

       pid_t pid=fork();
       if(pid<0)
       {
              err(1,"error");
       }
       if(pid==0)
       {
              close(inputFd);
              runSimulation(ramSize,regCnt,filename);
              _exit(0);
       }
    }
    close(inputFd);
    fo(int i=0;i<processors;i++)
    {
       if(wait(NULL)<0)
       {
              err(1,"error");
       }
    }
    return 0;
    
   
}
