#include <unistd.h>
#include <stdint.h>
#include <err.h>

#define WHEELS 4
#define PACKET_SIZE 16

int main(void)
{
    int driverPipe[2];

    if (pipe(driverPipe) < 0) {
        err(1, "pipe driver");
    }

    pid_t pid = fork();

    if (pid < 0) {
        err(1, "fork driver");
    }

    if (pid == 0) {
        close(driverPipe[0]);

        if (dup2(driverPipe[1], 1) < 0) {
            err(1, "dup2 driver");
        }

        close(driverPipe[1]);

        execlp("./fake_driver", "fake_driver", (char*)NULL);
        err(1, "exec driver");
    }

    close(driverPipe[1]);

    int wheelInput[WHEELS][2];   // parent -> wheel
    int wheelOutput[WHEELS][2];  // wheel -> parent

    for (int i = 0; i < WHEELS; i++) {
        if (pipe(wheelInput[i]) < 0) {
            err(1, "pipe wheel input");
        }

        if (pipe(wheelOutput[i]) < 0) {
            err(1, "pipe wheel output");
        }

        pid = fork();

        if (pid < 0) {
            err(1, "fork wheel");
        }

        if (pid == 0) {
            close(wheelInput[i][1]);
            close(wheelOutput[i][0]);

            if (dup2(wheelInput[i][0], 0) < 0) {
                err(1, "dup2 wheel stdin");
            }

            if (dup2(wheelOutput[i][1], 1) < 0) {
                err(1, "dup2 wheel stdout");
            }

            close(wheelInput[i][0]);
            close(wheelOutput[i][1]);

            execlp("./fake_wheel", "fake_wheel", (char*)NULL);
            err(1, "exec wheel");
        }

        close(wheelInput[i][0]);
        close(wheelOutput[i][1]);
    }

    uint16_t current = 0;

    while (1) {
        uint8_t packet[PACKET_SIZE];

        if (read(driverPipe[0], packet, PACKET_SIZE) != PACKET_SIZE) {
            err(1, "read driver");
        }

        uint16_t wantedSpeed = packet[8] | (packet[9] << 8);

        uint32_t sum = 0;

        for (int i = 0; i < WHEELS; i++) {
            if (read(wheelOutput[i][0], packet, PACKET_SIZE) != PACKET_SIZE) {
                err(1, "read wheel");
            }

            uint16_t wheelSpeed = packet[2] | (packet[3] << 8);

            sum += wheelSpeed;
        }

        uint32_t realSpeed = sum / WHEELS;

        if (realSpeed < wantedSpeed) {
            current++;
        } else if (realSpeed > wantedSpeed) {
            current--;
        }

        uint8_t currentPacket[PACKET_SIZE] = {0};

        currentPacket[2] = current & 0xFF;
        currentPacket[3] = current >> 8;

        for (int i = 0; i < WHEELS; i++) {
            if (write(wheelInput[i][1], currentPacket, PACKET_SIZE) != PACKET_SIZE) {
                err(1, "write wheel");
            }
        }
    }

    return 0;
}
//////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#define N 4
#define SIZE 16

int main(void)
{
    int driver_p[2];

    int to_wheel[N][2];     // parent -> wheel
    int from_wheel[N][2];   // wheel -> parent

    if (pipe(driver_p) < 0) {
        err(1, "pipe driver");
    }

    for (int i = 0; i < N; i++) {
        if (pipe(to_wheel[i]) < 0) {
            err(1, "pipe to wheel");
        }

        if (pipe(from_wheel[i]) < 0) {
            err(1, "pipe from wheel");
        }
    }

    // driver
    pid_t pid = fork();
    if (pid < 0) {
        err(1, "fork driver");
    }

    if (pid == 0) {
        dup2(driver_p[1], 1);

        close(driver_p[0]);
        close(driver_p[1]);

        for (int i = 0; i < N; i++) {
            close(to_wheel[i][0]);
            close(to_wheel[i][1]);
            close(from_wheel[i][0]);
            close(from_wheel[i][1]);
        }

        execlp("./fake_driver", "fake_driver", (char*)NULL);
        err(1, "exec driver");
    }

    // wheels
    for (int i = 0; i < N; i++) {
        pid = fork();
        if (pid < 0) {
            err(1, "fork wheel");
        }

        if (pid == 0) {
            dup2(to_wheel[i][0], 0);
            dup2(from_wheel[i][1], 1);

            close(driver_p[0]);
            close(driver_p[1]);

            for (int j = 0; j < N; j++) {
                close(to_wheel[j][0]);
                close(to_wheel[j][1]);
                close(from_wheel[j][0]);
                close(from_wheel[j][1]);
            }

            execlp("./fake_wheel", "fake_wheel", (char*)NULL);
            err(1, "exec wheel");
        }
    }

    // parent
    close(driver_p[1]);

    for (int i = 0; i < N; i++) {
        close(to_wheel[i][0]);
        close(from_wheel[i][1]);
    }

    uint16_t I = 0;

    while (1) {
        uint8_t buf[SIZE];

        if (read(driver_p[0], buf, SIZE) != SIZE) {
            err(1, "read driver");
        }

        uint16_t omega_req = ((uint16_t)buf[8] << 8) | buf[9];

        uint32_t sum = 0;

        for (int i = 0; i < N; i++) {
            if (read(from_wheel[i][0], buf, SIZE) != SIZE) {
                err(1, "read wheel");
            }

            uint16_t w = ((uint16_t)buf[2] << 8) | buf[3];
            sum += w;
        }

        uint16_t real_speed = sum / N;

        if (real_speed < omega_req) {
            I++;
        } else if (real_speed > omega_req) {
            I--;
        }

        uint8_t out[SIZE];
        memset(out, 0, SIZE);

        out[2] = (I >> 8) & 0xFF;
        out[3] = I & 0xFF;

        for (int i = 0; i < N; i++) {
            if (write(to_wheel[i][1], out, SIZE) != SIZE) {
                err(1, "write wheel");
            }
        }
    }

    return 0;
}
