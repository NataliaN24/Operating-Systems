1 Напишете програма на C, която приема четири параметъра – имена на двоични файлове.
Примерно извикване:
$ ./main f1.dat f1.idx f2.dat f2.idx
Първите два (f1.dat и f1.idx) и вторите два (f2.dat и f2.idx) файла са входен и изходен комплект
със следния смисъл:
• DAT-файловете (f1.dat и f2.dat) представляват двоични файлове, състоящи се от байтове (uint8_t);
• IDX-файловете представляват двоични файлове, състоящи се от наредени тройки от следните
елементи (и техните типове), които дефинират поредици от байтове (низове) от съответния DAT
файл:
– отместване uint16_t – показва позицията на първия байт от даден низ спрямо началото
на файла;
– дължина uint8_t – показва дължината на низа;
– запазен uint8_t – не се използва.
Първата двойка файлове (f1.dat и f1.idx) съществува, а втората трябва да бъде създадена от програмата по следния начин:
• трябва да се копират само низовете (поредици от байтове) от входния комплект, които започват
с главна латинска буква (A - 0x41, Z - 0x5A).
• ако файловете са неконсистентни по някакъв начин, програмата да прекратява изпълнението
си по подходящ начин.
Забележка: За удобство приемаме, че DAT файлът съдържа текстови данни на латински с ASCII кодова
таблица (един байт за буква).
Примерен вход и изход:
  $ xxd f1.dat
00000000: 4c6f 7265 6d20 6970 7375 6d20 646f 6c6f Lorem ipsum dolo
00000010: 7220 7369 7420 616d 6574 2c20 636f 6e73 r sit amet, cons
00000020: 6563 7465 7475 7220 6164 6970 6973 6369 ectetur adipisci
00000030: 6e67 2065 6c69 742c 2073 6564 2064 6f20 ng elit, sed do
00000040: 6569 7573 6d6f 6420 7465 6d70 6f72 2069 eiusmod tempor i
00000050: 6e63 6964 6964 756e 7420 7574 206c 6162 ncididunt ut lab
00000060: 6f72 6520 6574 2064 6f6c 6f72 6520 6d61 ore et dolore ma
00000070: 676e 6120 616c 6971 7561 2e20 5574 2065 gna aliqua. Ut e
00000080: 6e69 6d20 6164 206d 696e 696d 2076 656e nim ad minim ven
00000090: 6961 6d2c 2071 7569 7320 6e6f 7374 7275 iam, quis nostru
000000a0: 6420 6578 6572 6369 7461 7469 6f6e 2075 d exercitation u
000000b0: 6c6c 616d 636f 206c 6162 6f72 6973 206e llamco laboris n
000000c0: 6973 6920 7574 2061 6c69 7175 6970 2065 isi ut aliquip e
000000d0: 7820 6561 2063 6f6d 6d6f 646f 2063 6f6e x ea commodo con
000000e0: 7365 7175 6174 2e20 4475 6973 2061 7574 sequat. Duis aut
000000f0: 6520 6972 7572 6520 646f 6c6f 7220 696e e irure dolor in
00000100: 2072 6570 7265 6865 6e64 6572 6974 2069 reprehenderit i
00000110: 6e20 766f 6c75 7074 6174 6520 7665 6c69 n voluptate veli
00000120: 7420 6573 7365 2063 696c 6c75 6d20 646f t esse cillum do
00000130: 6c6f 7265 2065 7520 6675 6769 6174 206e lore eu fugiat n
00000140: 756c 6c61 2070 6172 6961 7475 722e 2045 ulla pariatur. E
00000150: 7863 6570 7465 7572 2073 696e 7420 6f63 xcepteur sint oc
00000160: 6361 6563 6174 2063 7570 6964 6174 6174 caecat cupidatat
00000170: 206e 6f6e 2070 726f 6964 656e 742c 2073 non proident, s
00000180: 756e 7420 696e 2063 756c 7061 2071 7569 unt in culpa qui
00000190: 206f 6666 6963 6961 2064 6573 6572 756e officia deserun
000001a0: 7420 6d6f 6c6c 6974 2061 6e69 6d20 6964 t mollit anim id
000001b0: 2065 7374 206c 6162 6f72 756d 2e0a est laborum..
$ xxd f1.idx
00000000: 0000 0500 4f01 0200 4e01 0300 ....O...N...
$ xxd f2.dat
00000000: 4c6f 7265 6d45 78 LoremEx
$ xxd f2.idx
00000000: 0000 0500 0500 0200 ........

  #include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdlib.h>

typedef struct {
    uint16_t offset;
    uint8_t length;
    uint8_t reserved;
} __attribute__((packed)) idx_entry;

int main(int argc, char **argv) {
    if (argc != 5) {
        errx(1, "Usage: %s f1.dat f1.idx f2.dat f2.idx", argv[0]);
    }

    const char *f1_dat = argv[1];
    const char *f1_idx = argv[2];
    const char *f2_dat = argv[3];
    const char *f2_idx = argv[4];

    int fd_dat_in = open(f1_dat, O_RDONLY);
    if (fd_dat_in < 0) err(2, "Cannot open %s", f1_dat);

    int fd_idx_in = open(f1_idx, O_RDONLY);
    if (fd_idx_in < 0) err(3, "Cannot open %s", f1_idx);

    int fd_dat_out = open(f2_dat, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dat_out < 0) err(4, "Cannot create %s", f2_dat);

    int fd_idx_out = open(f2_idx, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_idx_out < 0) err(5, "Cannot create %s", f2_idx);

    idx_entry entry;
    uint32_t out_offset = 0;

    while (read(fd_idx_in, &entry, sizeof(entry)) == sizeof(entry)) {
        if (entry.offset + entry.length > 0xFFFF) {
            // прост пример за консистентност: offset + length не трябва да излиза извън файл
            warnx("Skipping inconsistent entry at offset %u", entry.offset);
            continue;
        }

        uint8_t first_byte;
        if (lseek(fd_dat_in, entry.offset, SEEK_SET) < 0) err(6, "lseek failed");
        if (read(fd_dat_in, &first_byte, 1) != 1) err(7, "read failed");

        if (first_byte >= 'A' && first_byte <= 'Z') {
            // копираме целия низ
            uint8_t *buffer = malloc(entry.length);
            if (!buffer) errx(8, "malloc failed");

            if (lseek(fd_dat_in, entry.offset, SEEK_SET) < 0) err(9, "lseek failed");
            if (read(fd_dat_in, buffer, entry.length) != entry.length) err(10, "read failed");

            if (write(fd_dat_out, buffer, entry.length) != entry.length) err(11, "write failed");

            // записваме нов индекс
            idx_entry new_entry;
            new_entry.offset = out_offset;
            new_entry.length = entry.length;
            new_entry.reserved = entry.reserved;

            if (write(fd_idx_out, &new_entry, sizeof(new_entry)) != sizeof(new_entry))
                err(12, "write failed");

            out_offset += entry.length;
            free(buffer);
        }
    }

    close(fd_dat_in);
    close(fd_idx_in);
    close(fd_dat_out);
    close(fd_idx_out);

    return 0;
}
