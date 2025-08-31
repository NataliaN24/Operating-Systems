//Инженерите от съседната лабораторя ползват специализиран хардуер и софтуер за
//прехвърляне на данни по радио, но за съжаление имат два проблема:
//• в радио частта: дълги поредици битове само 0 или само 1 чупят преноса;
//• в софтуерната част: софтуерът, който ползват, може да прехвърля само файлове с четен брой
//        байтове дължина.
//Помогнете на колегите си, като напишете програма на C, която решава тези проблеми, като подготвя
//файлове за прехвърляне. Програмата трябва да приема два задължителни позиционни аргумента
//– имена на файлове. Примерно извикване:
//$ ./main input.bin output.bin
//        Програмата чете данни от input.bin и записва резултат след обработка в output.bin. Програмата
//        трябва да работи като encoder, който имплементира вариант на Manchester code, т.е.:
//• за всеки входен бит 1 извежда битовете 10, и
//• за всеки входен бит 0 извежда битовете 01
//Например, следните 8 бита вход
//1011 0110 == 0xB6
//по описаният алгоритъм дават следните 16 бита изход
//1001 1010 0110 1001 == 0x9A69

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

int main(int argc, char **argv) {
    if (argc != 3)
        errx(1, "Usage: ./main input.bin output.bin");

    const char *in_file = argv[1];
    const char *out_file = argv[2];

    int fd_in = open(in_file, O_RDONLY);
    if (fd_in < 0) err(2, "Cannot open input file");

    int fd_out = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_out < 0) err(3, "Cannot open output file");

    uint8_t in_byte;
    uint8_t out_byte = 0;
    int out_bit_count = 0;

    while (read(fd_in, &in_byte, 1) == 1) {
        for (int i = 7; i >= 0; i--) {
            int bit = (in_byte >> i) & 1;
            // Manchester код: 0 -> 01, 1 -> 10
            int man_bits = bit ? 0b10 : 0b01;

            for (int j = 1; j >= 0; j--) {
                out_byte <<= 1;
                out_byte |= (man_bits >> j) & 1;
                out_bit_count++;

                if (out_bit_count == 8) {
                    if (write(fd_out, &out_byte, 1) != 1)
                        err(4, "write failed");
                    out_byte = 0;
                    out_bit_count = 0;
                }
            }
        }
    }

    // Ако има непълен байт, го допълваме с 0 и записваме
    if (out_bit_count > 0) {
        out_byte <<= (8 - out_bit_count);
        if (write(fd_out, &out_byte, 1) != 1)
            err(5, "write failed");
        out_bit_count = 0;
    }

    // Проверка: ако файлът има нечетен брой байтове, добавяме 0-байт
    off_t out_size = lseek(fd_out, 0, SEEK_END);
    if (out_size % 2 != 0) {
        uint8_t zero = 0;
        if (write(fd_out, &zero, 1) != 1)
            err(6, "write failed");
    }

    close(fd_in);
    close(fd_out);

    return 0;
}
