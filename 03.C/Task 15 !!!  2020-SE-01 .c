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
