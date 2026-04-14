
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <sys/stat.h>

static void read_exact(int fd, void* buf, size_t size, const char* what)
{
    size_t total = 0;
    while (total < size) {
        ssize_t bytes_read = read(fd, (char*)buf + total, size - total);
        if (bytes_read < 0) {
            err(1, "read %s", what);
        }
        if (bytes_read == 0) {
            errx(1, "unexpected EOF while reading %s", what);
        }
        total += (size_t)bytes_read;
    }
}

static off_t get_file_size(int fd)
{
    off_t current = lseek(fd, 0, SEEK_CUR);
    if (current < 0) {
        err(1, "lseek");
    }

    off_t size = lseek(fd, 0, SEEK_END);
    if (size < 0) {
        err(1, "lseek");
    }

    if (lseek(fd, current, SEEK_SET) < 0) {
        err(1, "lseek");
    }

    return size;
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        errx(1, "usage: %s index values name", argv[0]);
    }

    const char* index_path = argv[1];
    const char* values_path = argv[2];
    const char* wanted_name = argv[3];

    int index_fd = open(index_path, O_RDONLY);
    if (index_fd < 0) {
        err(1, "open %s", index_path);
    }

    int values_fd = open(values_path, O_RDONLY);
    if (values_fd < 0) {
        err(1, "open %s", values_path);
    }

    off_t values_size = get_file_size(values_fd);
    if (values_size % sizeof(uint32_t) != 0) {
        errx(1, "invalid values file size");
    }

    uint32_t record_index = 0;
    int found = 0;
    uint8_t found_type = 0;

    while (1) {
        uint8_t header;
        ssize_t bytes_read = read(index_fd, &header, sizeof(header));

        if (bytes_read < 0) {
            err(1, "read index header");
        }

        if (bytes_read == 0) {
            break; /* край на индексния файл */
        }

        uint8_t name_len = header & 0x7F;         /* битове 0..6 */
        uint8_t value_type = (header >> 7) & 0x1; /* бит 7 */

        char* name = malloc((size_t)name_len + 1);
        if (name == NULL) {
            err(1, "malloc");
        }

        read_exact(index_fd, name, name_len, "index name");
        name[name_len] = '\0';

        if (strcmp(name, wanted_name) == 0) {
            found = 1;
            found_type = value_type;
            free(name);
            break;
        }

        free(name);
        record_index++;
    }

    if (!found) {
        errx(1, "record not found");
    }

    off_t value_offset = (off_t)record_index * (off_t)sizeof(uint32_t);
    if (value_offset + (off_t)sizeof(uint32_t) > values_size) {
        errx(1, "no corresponding value in values file");
    }

    if (lseek(values_fd, value_offset, SEEK_SET) < 0) {
        err(1, "lseek values");
    }

    uint32_t raw_value;
    read_exact(values_fd, &raw_value, sizeof(raw_value), "value");

    if (found_type == 0) {
        int32_t int_value = (int32_t)raw_value;
        printf("%d\n", int_value);
    } else {
        float float_value;
        memcpy(&float_value, &raw_value, sizeof(float_value));
        printf("%.3f\n", float_value);
    }

    close(index_fd);
    close(values_fd);
    return 0;
}
