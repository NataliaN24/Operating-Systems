1 Инженерите от съседната лабораторя ползват специализиран хардуер и софтуер за прехвърляне на данни по радио, но за съжаление имат два проблема:
• в радио частта: дълги поредици битове само 0 или само 1 чупят преноса;
• в софтуерната част: софтуерът, който ползват, може да прехвърля само файлове с четен брой
байтове дължина.
Помогнете на колегите си, като напишете програма на C, която решава тези проблеми, като подготвя
файлове за прехвърляне. Програмата трябва да приема два задължителни позиционни аргумента –
имена на файлове. Примерно извикване:
$ ./main input.bin output.bin
Програмата чете данни от input.bin и записва резултат след обработка в output.bin. Програмата
трябва да работи като encoder, който имплементира вариант на Manchester code, т.е.:
• за всеки входен бит 1 извежда битовете 10, и
• за всеки входен бит 0 извежда битовете 01
Например, следните 8 бита вход
  1011 0110 == 0xB6
по описаният алгоритъм дават следните 16 бита изход
1001 1010 0110 1001 == 0x9A69

  #include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.bin output.bin\n", argv[0]);
        return 1;
    }

    FILE *fin = fopen(argv[1], "rb");
    if (!fin) {
        perror("fopen input");
        return 1;
    }

    FILE *fout = fopen(argv[2], "wb");
    if (!fout) {
        perror("fopen output");
        fclose(fin);
        return 1;
    }

    int byte;
    uint8_t out_byte = 0;
    int out_bits = 0; // брой битове, записани в текущия out_byte

    while ((byte = fgetc(fin)) != EOF) {
        for (int i = 7; i >= 0; i--) { // четем бита MSB -> LSB
            int bit = (byte >> i) & 1;

            // Manchester encoding
            int first_bit = bit ? 1 : 0;
            int second_bit = bit ? 0 : 1;

            // записваме първия бит
            out_byte = (out_byte << 1) | first_bit;
            out_bits++;
            if (out_bits == 8) {
                fputc(out_byte, fout);
                out_byte = 0;
                out_bits = 0;
            }

            // записваме втория бит
            out_byte = (out_byte << 1) | second_bit;
            out_bits++;
            if (out_bits == 8) {
                fputc(out_byte, fout);
                out_byte = 0;
                out_bits = 0;
            }
        }
    }

    // ако има останали битове в out_byte, допълваме с 0 отдясно
    if (out_bits > 0) {
        out_byte <<= (8 - out_bits);
        fputc(out_byte, fout);
    }

    fclose(fin);
    fclose(fout);

    return 0;
}
