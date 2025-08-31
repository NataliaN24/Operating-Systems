//Инженерите от съседната лабораторя работят с комплекти SCL/SDL файлове, напр.
//input.scl/input.sdl.
//В SCL файла са записани нива на сигнали (ниско 0 или високо 1), т.е., файлът се третира като състоящ се от битове.
//В SDL файла са записани данни от тип uint16_t, като всеки елемент съответства
//позиционно на даден бит от SCL файла.
//Помогнете на колегите си, като напишете програма на C, която да бъде удобен инструмент за
//        изваждане в нов файл само на тези SDL елементи, които са имали високо ниво в SCL файла,
//запазвайки наредбата им.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char **argv) {
    if (argc != 4) errx(1, "Usage: ./main input.scl input.sdl output.bin");

    const char *scl_file = argv[1];
    const char *sdl_file = argv[2];
    const char *out_file = argv[3];

    int fd_scl = open(scl_file, O_RDONLY);
    int fd_sdl = open(sdl_file, O_RDONLY);
    int fd_out = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (fd_scl < 0 || fd_sdl < 0 || fd_out < 0) err(2, "cannot open files");

    uint8_t scl_byte;
    ssize_t scl_read;
    uint64_t sdl_index = 0;

    while ((scl_read = read(fd_scl, &scl_byte, 1)) > 0) {
        for (int bit = 0; bit < 8; bit++) {
            int level = (scl_byte >> bit) & 1;

            uint16_t sdl_value;
            if (read(fd_sdl, &sdl_value, sizeof(uint16_t)) != sizeof(uint16_t))
                errx(3, "SDL file too short for SCL bits");

            if (level) {
                if (write(fd_out, &sdl_value, sizeof(uint16_t)) != sizeof(uint16_t))
                    err(4, "write failed");
            }
            sdl_index++;
        }
    }

    if (scl_read < 0) err(5, "read SCL failed");

    close(fd_scl);
    close(fd_sdl);
    close(fd_out);

    return 0;
}
