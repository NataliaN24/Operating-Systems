#include <unistd.h>
#include <err.h>
#include <string.h>
#include <stdbool.h>

int findChar(const char* set1, char c)
{
    for (int i = 0; set1[i] != '\0'; i++) {
        if (set1[i] == c) {
            return i;
        }
    }
    return -1;
}

void deleteChars(const char* set1)
{
    char cRead;
    ssize_t bytesRead;

    while ((bytesRead = read(0, &cRead, 1)) > 0) {
        if (findChar(set1, cRead) == -1) {
            if (write(1, &cRead, 1) != 1) {
                err(1, "write");
            }
        }
    }

    if (bytesRead < 0) {
        err(1, "read");
    }
}

void squeeze(const char* set1)
{
    char cRead;
    char lastWritten = '\0';
    bool hasLastWritten = false;
    ssize_t bytesRead;

    while ((bytesRead = read(0, &cRead, 1)) > 0) {
        int inSet = (findChar(set1, cRead) != -1);

        if (inSet && hasLastWritten && cRead == lastWritten) {
            continue;
        }

        if (write(1, &cRead, 1) != 1) {
            err(1, "write");
        }

        lastWritten = cRead;
        hasLastWritten = true;
    }

    if (bytesRead < 0) {
        err(1, "read");
    }
}

void replace(const char* set1, const char* set2)
{
    char cRead;
    ssize_t bytesRead;

    while ((bytesRead = read(0, &cRead, 1)) > 0) {
        int index = findChar(set1, cRead);

        if (index != -1) {
            char toReplaceWith = set2[index];
            if (write(1, &toReplaceWith, 1) != 1) {
                err(1, "write");
            }
        } else {
            if (write(1, &cRead, 1) != 1) {
                err(1, "write");
            }
        }
    }

    if (bytesRead < 0) {
        err(1, "read");
    }
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        errx(1, "Usage: ./main -d SET1 | ./main -s SET1 | ./main SET1 SET2");
    }

    if (strcmp(argv[1], "-d") == 0) {
        char* set1 = argv[2];
        deleteChars(set1);
    }
    else if (strcmp(argv[1], "-s") == 0) {
        char* set1 = argv[2];
        squeeze(set1);
    }
    else {
        char* set1 = argv[1];
        char* set2 = argv[2];

        if (strlen(set1) != strlen(set2)) {
            errx(1, "SET1 and SET2 must be same length");
        }

        replace(set1, set2);
    }

    return 0;
}
