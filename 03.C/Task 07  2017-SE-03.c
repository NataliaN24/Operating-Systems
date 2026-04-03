#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <sys/stat.h>

typedef struct {
    uint16_t offset;
    uint8_t oldByte;
    uint8_t newByte;
} Patch;

void readSafe(int fd, void* buf, size_t count, const char* msg) {
    size_t total = 0;
    while (total < count) {
        ssize_t r = read(fd, (char*)buf + total, count - total);
        if (r < 0) {
            err(1, "%s", msg);
        }
        if (r == 0) {
            errx(1, "%s: unexpected EOF", msg);
        }
        total += (size_t)r;
    }
}

void writeSafe(int fd, const void* buf, size_t count, const char* msg) {
    size_t total = 0;
    while (total < count) {
        ssize_t w = write(fd, (const char*)buf + total, count - total);
        if (w < 0) {
            err(1, "%s", msg);
        }
        total += (size_t)w;
    }
}

off_t getFileSize(int fd) {
    struct stat st;
    if (fstat(fd, &st) < 0) {
        err(1, "fstat");
    }
    return st.st_size;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        errx(1, "Usage: %s patch.bin f1.bin f2.bin", argv[0]);
    }

    int patchFd = open(argv[1], O_RDONLY);
    if (patchFd < 0) {
        err(1, "open %s", argv[1]);
    }

    int f1Fd = open(argv[2], O_RDONLY);
    if (f1Fd < 0) {
        err(1, "open %s", argv[2]);
    }

    int f2Fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (f2Fd < 0) {
        err(1, "open %s", argv[3]);
    }

    off_t patchSize = getFileSize(patchFd);
    if (patchSize % sizeof(Patch) != 0) {
        errx(1, "invalid patch file");
    }

    char buffer[4096];
    ssize_t bytesRead;

    while ((bytesRead = read(f1Fd, buffer, sizeof(buffer))) > 0) {
        writeSafe(f2Fd, buffer, (size_t)bytesRead, "write f2");
    }
    if (bytesRead < 0) {
        err(1, "read f1");
    }

    if (lseek(patchFd, 0, SEEK_SET) < 0) {
        err(1, "lseek patch");
    }

    Patch p;
    off_t patchCount = patchSize / sizeof(Patch);

    for (off_t i = 0; i < patchCount; i++) {
        readSafe(patchFd, &p, sizeof(p), "read patch entry");

        if (lseek(f2Fd, p.offset, SEEK_SET) < 0) {
            err(1, "lseek f2");
        }

        uint8_t current;
        ssize_t r = read(f2Fd, &current, sizeof(current));
        if (r < 0) {
            err(1, "read f2 at offset");
        }
        if (r == 0) {
            errx(1, "invalid offset %u", p.offset);
        }

        if (current != p.oldByte) {
            errx(1, "byte mismatch at offset %u", p.offset);
        }

        if (lseek(f2Fd, p.offset, SEEK_SET) < 0) {
            err(1, "lseek f2");
        }

        writeSafe(f2Fd, &p.newByte, sizeof(p.newByte), "write patched byte");
    }

    if (close(patchFd) < 0) {
        err(1, "close patch");
    }
    if (close(f1Fd) < 0) {
        err(1, "close f1");
    }
    if (close(f2Fd) < 0) {
        err(1, "close f2");
    }

    return 0;
}
