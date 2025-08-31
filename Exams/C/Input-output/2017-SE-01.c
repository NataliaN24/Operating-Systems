 Напишете програма на C, която приема три параметъра – имена на двоични файлове.
Примерно извикване:
$ ./main f1.bin f2.bin patch.bin
Файловете f1.bin и f2.bin се третират като двоични файлове, състоящи се от байтове (uint8_t). Файлът f1.bin e “оригиналният” файл, а f2.bin е негово копие, което е било модифицирано по някакъв
начин (извън обхвата на тази задача). Файлът patch.bin е двоичен файл, състоящ се от наредени
тройки от следните елементи (и техните типове):
• отместване (uint16_t) – спрямо началото на f1.bin/f2.bin
• оригинален байт (uint8_t) – на тази позиция в f1.bin
• нов байт (uint8_t) – на тази позиция в f2.bin
Вашата програма да създава файла patch.bin, на базата на съществуващите файлове f1.bin и f2.bin,
като описва вътре само разликите между двата файла. Ако дадено отместване съществува само в единия от файловете f1.bin/f2.bin, програмата да прекратява изпълнението си по подходящ начин.
Примерен f1.bin:
00000000: f5c4 b159 cc80 e2ef c1c7 c99a 2fb0 0d8c ...Y......../...
00000010: 3c83 6fed 6b46 09d2 90df cf1e 9a3c 1f05 <.o.kF.......<..
00000020: 05f9 4c29 fd58 a5f1 cb7b c9d0 b234 2398 ..L).X...{...4#.
00000030: 35af 6be6 5a71 b23a 0e8d 08de def2 214c 5.k.Zq.:......!L
Примерен f2.bin:
00000000: f5c4 5959 cc80 e2ef c1c7 c99a 2fb0 0d8c ..YY......../...
00000010: 3c83 6fed 6b46 09d2 90df cf1e 9a3c 1f05 <.o.kF.......<..
00000020: 05f9 4c29 fd58 a5f1 cb7b c9d0 b234 2398 ..L).X...{...4#.
00000030: afaf 6be6 5a71 b23a 0e8d 08de def2 214c ..k.Zq.:......!L
Примерен patch.bin:
00000000: 0200 b159 3000 35af ...Y0.5.

  #include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        errx(1, "Usage: %s f1.bin f2.bin patch.bin", argv[0]);
    }

    const char *f1_name = argv[1];
    const char *f2_name = argv[2];
    const char *patch_name = argv[3];

    int fd1 = open(f1_name, O_RDONLY);
    if (fd1 < 0) err(2, "Cannot open %s", f1_name);

    int fd2 = open(f2_name, O_RDONLY);
    if (fd2 < 0) err(3, "Cannot open %s", f2_name);

    int fd_patch = open(patch_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_patch < 0) err(4, "Cannot create %s", patch_name);

    uint8_t byte1, byte2;
    uint16_t offset = 0;
    ssize_t r1, r2;

    while (1) {
        r1 = read(fd1, &byte1, 1);
        r2 = read(fd2, &byte2, 1);

        if (r1 != r2) {
            errx(5, "Files have different lengths");
        }

        if (r1 == 0) break; // край на файловете

        if (byte1 != byte2) {
            // записваме offset (2 байта) и двата байта различия
            uint8_t patch[4];
            patch[0] = offset & 0xFF;       // нисък байт на offset
            patch[1] = (offset >> 8) & 0xFF; // висок байт на offset
            patch[2] = byte1; // оригинален байт
            patch[3] = byte2; // нов байт

            if (write(fd_patch, patch, 4) != 4) {
                err(6, "Write to patch file failed");
            }
        }

        offset++;
        if (offset == 0) {
            errx(7, "File too long for uint16_t offset");
        }
    }

    close(fd1);
    close(fd2);
    close(fd_patch);

    return 0;
}
