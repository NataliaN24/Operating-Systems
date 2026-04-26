#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <sys/wait.h>

#define N 4
#define SIZE 16

int main() {
    int driver_p[2];
    int wheel_p[N][2];

    // pipe за driver
    if (pipe(driver_p) < 0) err(1, "pipe");

    // pipes за колелата
    for (int i = 0; i < N; i++) {
        if (pipe(wheel_p[i]) < 0) err(1, "pipe");
    }

    // --- driver ---
    if (fork() == 0) {
        dup2(driver_p[1], 1); // stdout -> pipe
        close(driver_p[0]);
        close(driver_p[1]);

        execlp("./fake_driver", "fake_driver", NULL);
        err(1, "exec driver");
    }

    // --- wheels ---
    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            dup2(wheel_p[i][0], 0); // stdin
            dup2(wheel_p[i][1], 1); // stdout

            close(wheel_p[i][0]);
            close(wheel_p[i][1]);

            execlp("./fake_wheel", "fake_wheel", NULL);
            err(1, "exec wheel");
        }
    }

    // родителят затваря ненужните
    close(driver_p[1]);
    for (int i = 0; i < N; i++) {
        close(wheel_p[i][0]);
    }

    uint16_t I = 0;

    while (1) {
        uint8_t buf[SIZE];

        // --- четем газ ---
        if (read(driver_p[0], buf, SIZE) != SIZE) {
            err(1, "read driver");
        }

        uint16_t omega_req = (buf[8] << 8) | buf[9];

        // --- четем скорости ---
        uint16_t sum = 0;

        for (int i = 0; i < N; i++) {
            if (read(wheel_p[i][1], buf, SIZE) != SIZE) {
                err(1, "read wheel");
            }

            uint16_t w = (buf[2] << 8) | buf[3];
            sum += w;
        }

        uint16_t real_speed = sum / 4;

        // --- update I ---
        if (real_speed < omega_req) {
            I++;
        } else if (real_speed > omega_req) {
            I--;
        }

        // --- пращаме ток ---
        uint8_t out[SIZE];
        memset(out, 0, SIZE);

        out[2] = (I >> 8) & 0xFF;
        out[3] = I & 0xFF;

        for (int i = 0; i < N; i++) {
            if (write(wheel_p[i][1], out, SIZE) != SIZE) {
                err(1, "write wheel");
            }
        }
    }

    return 0;
}
