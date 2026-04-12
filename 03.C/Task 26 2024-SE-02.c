#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stdlib.h>

void processPacket(int compress, uint32_t packetCount, int orig)
{
    uint8_t byte;

    for (uint32_t i = 0; i < packetCount; i++)
    {
        if (read(compress, &byte, sizeof(byte)) != sizeof(byte))
        {
            err(1, "read packet control byte");
        }

        uint8_t type = byte >> 7;
        uint8_t N = byte & 0x7F;
        uint8_t count = N + 1;

        uint8_t bytesWrite;

        if (type == 0)
        {
            for (uint8_t j = 0; j < count; j++)
            {
                if (read(compress, &bytesWrite, sizeof(bytesWrite)) != sizeof(bytesWrite))
                {
                    err(1, "read normal packet byte");
                }

                if (write(orig, &bytesWrite, sizeof(bytesWrite)) != sizeof(bytesWrite))
                {
                    err(1, "write normal packet byte");
                }
            }
        }
        else if (type == 1)
        {
            if (read(compress, &bytesWrite, sizeof(bytesWrite)) != sizeof(bytesWrite))
            {
                err(1, "read repeated packet byte");
            }

            for (uint8_t j = 0; j < count; j++)
            {
                if (write(orig, &bytesWrite, sizeof(bytesWrite)) != sizeof(bytesWrite))
                {
                    err(1, "write repeated packet byte");
                }
            }
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        errx(1, "usage: %s compressed_file output_file", argv[0]);
    }

    int compress = open(argv[1], O_RDONLY);
    if (compress < 0)
    {
        err(1, "open compressed file");
    }

    int orig = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (orig < 0)
    {
        err(1, "open output file");
    }

    uint32_t magic, packetCount;
    uint64_t origSize;

    if (read(compress, &magic, sizeof(magic)) != sizeof(magic) ||
        read(compress, &packetCount, sizeof(packetCount)) != sizeof(packetCount) ||
        read(compress, &origSize, sizeof(origSize)) != sizeof(origSize))
    {
        err(1, "read header");
    }

    if (magic != 0x21494D46)
    {
        errx(1, "invalid magic");
    }

    processPacket(compress, packetCount, orig);

    close(compress);
    close(orig);

    return 0;
}
