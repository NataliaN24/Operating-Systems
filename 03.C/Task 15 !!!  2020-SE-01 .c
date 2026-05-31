#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>

#define H 16

int main(int argc, char* argv[])
{
    if (argc != 7) {
        errx(1, "Usage: %s affix postfix prefix infix suffix crucifixus", argv[0]);
    }

    int affix = open(argv[1], O_RDONLY);
    int postfix = open(argv[2], O_RDONLY);
    int prefix = open(argv[3], O_RDONLY);
    int infix = open(argv[4], O_RDONLY);
    int suffix = open(argv[5], O_RDONLY);
    int out = open(argv[6], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (affix < 0 || postfix < 0 || prefix < 0 || infix < 0 || suffix < 0 || out < 0) {
        err(2, "open");
    }

    uint16_t affixCount, postCount, preCount, inCount, sufCount;

    lseek(affix, 4, SEEK_SET);
    read(affix, &affixCount, sizeof(affixCount));

    lseek(postfix, 4, SEEK_SET);
    read(postfix, &postCount, sizeof(postCount));

    lseek(prefix, 4, SEEK_SET);
    read(prefix, &preCount, sizeof(preCount));

    lseek(infix, 4, SEEK_SET);
    read(infix, &inCount, sizeof(inCount));

    lseek(suffix, 4, SEEK_SET);
    read(suffix, &sufCount, sizeof(sufCount));

    if (affixCount % 8 != 0) {
        errx(3, "bad affix count");
    }

    uint8_t zero = 0;

    for (int i = 0; i < H; i++) {
        write(out, &zero, 1);
    }

    lseek(affix, H, SEEK_SET);

    uint16_t outCount = 0;

    for (int i = 0; i < affixCount / 8; i++)
    {
        uint16_t postStart, postLen;
        uint16_t preStart, preLen;
        uint16_t inStart, inLen;
        uint16_t sufStart, sufLen;

        read(affix, &postStart, 2);
        read(affix, &postLen, 2);

        read(affix, &preStart, 2);
        read(affix, &preLen, 2);

        read(affix, &inStart, 2);
        read(affix, &inLen, 2);

        read(affix, &sufStart, 2);
        read(affix, &sufLen, 2);

        if (postStart + postLen > postCount) {
            errx(4, "bad postfix interval");
        }

        if (preStart + preLen > preCount) {
            errx(5, "bad prefix interval");
        }

        if (inStart + inLen > inCount) {
            errx(6, "bad infix interval");
        }

        if (sufStart + sufLen > sufCount) {
            errx(7, "bad suffix interval");
        }

        uint32_t p;
        lseek(postfix, H + postStart * 4, SEEK_SET);
        for (int j = 0; j < postLen; j++) {
            read(postfix, &p, 4);
            write(out, &p, 4);
            outCount += 4;
        }

        uint8_t pr;
        lseek(prefix, H + preStart, SEEK_SET);
        for (int j = 0; j < preLen; j++) {
            read(prefix, &pr, 1);
            write(out, &pr, 1);
            outCount += 1;
        }

        uint16_t in;
        lseek(infix, H + inStart * 2, SEEK_SET);
        for (int j = 0; j < inLen; j++) {
            read(infix, &in, 2);
            write(out, &in, 2);
            outCount += 2;
        }

        uint64_t s;
        lseek(suffix, H + sufStart * 8, SEEK_SET);
        for (int j = 0; j < sufLen; j++) {
            read(suffix, &s, 8);
            write(out, &s, 8);
            outCount += 8;
        }
    }

    lseek(out, 4, SEEK_SET);
    write(out, &outCount, 2);

    close(affix);
    close(postfix);
    close(prefix);
    close(infix);
    close(suffix);
    close(out);

    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <stdlib.h>

#define HEADER_SIZE 16

static void read_exact(int fd, void* buf, size_t count, const char* msg) {
    size_t done = 0;
    while (done < count) {
        ssize_t r = read(fd, (char*)buf + done, count - done);
        if (r < 0) {
            err(1, "%s", msg);
        }
        if (r == 0) {
            errx(1, "%s: unexpected EOF", msg);
        }
        done += (size_t)r;
    }
}

static void write_exact(int fd, const void* buf, size_t count, const char* msg) {
    size_t done = 0;
    while (done < count) {
        ssize_t w = write(fd, (const char*)buf + done, count - done);
        if (w < 0) {
            err(1, "%s", msg);
        }
        if (w == 0) {
            errx(1, "%s: could not write", msg);
        }
        done += (size_t)w;
    }
}

static uint16_t read_count(int fd, const char* name) {
    uint16_t count;
    if (lseek(fd, 4, SEEK_SET) < 0) {
        err(1, "lseek %s", name);
    }
    read_exact(fd, &count, sizeof(count), name);
    return count;
}

static void copy_header_from_affix(int aff, int out) {
    uint8_t header[HEADER_SIZE];

    if (lseek(aff, 0, SEEK_SET) < 0) {
        err(1, "lseek affix");
    }
    read_exact(aff, header, sizeof(header), "read affix header");
    write_exact(out, header, sizeof(header), "write output header");
}

static void copy_interval(int src_fd, uint16_t start, uint16_t cnt, size_t elem_size, int out_fd, const char* name) {
    char buffer[8]; /* enough for uint64_t */

    off_t pos = HEADER_SIZE + (off_t)start * (off_t)elem_size;
    if (lseek(src_fd, pos, SEEK_SET) < 0) {
        err(1, "lseek %s", name);
    }

    for (uint16_t i = 0; i < cnt; i++) {
        read_exact(src_fd, buffer, elem_size, name);
        write_exact(out_fd, buffer, elem_size, "write output");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        errx(1, "Usage: %s affix postfix prefix infix suffix crucifixus", argv[0]);
    }

    int aff  = open(argv[1], O_RDONLY);
    int post = open(argv[2], O_RDONLY);
    int pref = open(argv[3], O_RDONLY);
    int inf  = open(argv[4], O_RDONLY);
    int suff = open(argv[5], O_RDONLY);
    int cruc = open(argv[6], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (aff < 0)  err(1, "open %s", argv[1]);
    if (post < 0) err(1, "open %s", argv[2]);
    if (pref < 0) err(1, "open %s", argv[3]);
    if (inf < 0)  err(1, "open %s", argv[4]);
    if (suff < 0) err(1, "open %s", argv[5]);
    if (cruc < 0) err(1, "open %s", argv[6]);

    uint16_t aff_count  = read_count(aff,  argv[1]);
    uint16_t post_count = read_count(post, argv[2]);
    uint16_t pref_count = read_count(pref, argv[3]);
    uint16_t inf_count  = read_count(inf,  argv[4]);
    uint16_t suff_count = read_count(suff, argv[5]);

    if (aff_count % 8 != 0) {
        errx(1, "affix count is invalid");
    }

    copy_header_from_affix(aff, cruc);

    if (lseek(aff, HEADER_SIZE, SEEK_SET) < 0) {
        err(1, "lseek affix data");
    }

    uint16_t output_count = 0;
    int kits = aff_count / 8;

    for (int i = 0; i < kits; i++) {
        uint16_t x[8];
        read_exact(aff, x, sizeof(x), "read affix kit");

        uint16_t post_start = x[0], post_cnt = x[1];
        uint16_t pref_start = x[2], pref_cnt = x[3];
        uint16_t inf_start  = x[4], inf_cnt  = x[5];
        uint16_t suff_start = x[6], suff_cnt = x[7];

        if ((uint32_t)post_start + post_cnt > post_count) {
            errx(1, "invalid postfix interval");
        }
        if ((uint32_t)pref_start + pref_cnt > pref_count) {
            errx(1, "invalid prefix interval");
        }
        if ((uint32_t)inf_start + inf_cnt > inf_count) {
            errx(1, "invalid infix interval");
        }
        if ((uint32_t)suff_start + suff_cnt > suff_count) {
            errx(1, "invalid suffix interval");
        }

        copy_interval(post, post_start, post_cnt, sizeof(uint32_t), cruc, "postfix");
        copy_interval(pref, pref_start, pref_cnt, sizeof(uint8_t),  cruc, "prefix");
        copy_interval(inf,  inf_start,  inf_cnt,  sizeof(uint16_t), cruc, "infix");
        copy_interval(suff, suff_start, suff_cnt, sizeof(uint64_t), cruc, "suffix");

        output_count += post_cnt * sizeof(uint32_t);
        output_count += pref_cnt * sizeof(uint8_t);
        output_count += inf_cnt  * sizeof(uint16_t);
        output_count += suff_cnt * sizeof(uint64_t);
    }

    if (lseek(cruc, 4, SEEK_SET) < 0) {
        err(1, "lseek output count");
    }
    write_exact(cruc, &output_count, sizeof(output_count), "write output count");

    if (close(aff) < 0)  err(1, "close affix");
    if (close(post) < 0) err(1, "close postfix");
    if (close(pref) < 0) err(1, "close prefix");
    if (close(inf) < 0)  err(1, "close infix");
    if (close(suff) < 0) err(1, "close suffix");
    if (close(cruc) < 0) err(1, "close crucifixus");

    return 0;
}
