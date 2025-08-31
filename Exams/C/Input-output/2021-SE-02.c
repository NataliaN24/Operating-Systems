//Програмата работи като encoder, който имплементира вариант на Manchester code, т.е.:
//• за всеки входен бит 1 извежда битовете 10, и
//• за всеки входен бит 0 извежда битовете 01
//Например следните 8 бита вход
//1011 0110 == 0xB6
//по описания алгоритъм дават следните 16 бита изход
//1001 1010 0110 1001 == 0x9A69
//Напишете програма на C, която извършва обратния процес, т.е., декодира файлове, създадени от
//        горната програма.
//Примерно извикване:
//$ ./main input.bin output.bin

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
    int bit_buffer_count = 0;
    uint8_t bit_buffer = 0;

    while (read(fd_in, &in_byte, 1) == 1) {
        for (int i = 7; i >= 0; i--) {
            int bit = (in_byte >> i) & 1;

            // Добавяме бита към битов буфер
            bit_buffer = (bit_buffer << 1) | bit;
            bit_buffer_count++;

            if (bit_buffer_count == 2) {
                // Проверяваме Manchester код
                if (bit_buffer == 0b10)
                    out_byte = (out_byte << 1) | 1; // 1
                else if (bit_buffer == 0b01)
                    out_byte = (out_byte << 1) | 0; // 0
                else
                    errx(4, "Invalid Manchester encoding detected");

                out_bit_count++;
                bit_buffer = 0;
                bit_buffer_count = 0;

                if (out_bit_count == 8) {
                    if (write(fd_out, &out_byte, 1) != 1)
                        err(5, "write failed");
                    out_byte = 0;
                    out_bit_count = 0;
                }
            }
        }
    }

    // Ако накрая има непълен байт
    if (out_bit_count > 0) {
        out_byte <<= (8 - out_bit_count);
        if (write(fd_out, &out_byte, 1) != 1)
            err(6, "write failed");
    }

    close(fd_in);
    close(fd_out);

    return 0;
}
