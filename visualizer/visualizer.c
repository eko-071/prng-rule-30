#include "../include/prng30.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
static void ms_sleep(int ms) {
    Sleep(ms);
}
static void clear_screen(void) {
    system("cls");
}
#else
#include <unistd.h>
static void ms_sleep(int ms) {
    usleep((unsigned int)(ms * 1000));
}
static void clear_screen(void) {
    system("clear");
}
#endif

#define COL_RED "\033[31m"
#define COL_BLUE "\033[34m"
#define COL_GRAY "\033[90m"
#define COL_RESET "\033[0m"

#define MAX_DISPLAY_ROWS 40

static void visualize(prng30_state *st, int steps, int delay_ms) {
    int width = st->width;
    int mid   = width / 2;

    uint8_t *history = calloc((size_t)(steps * width), sizeof(uint8_t));
    if (!history) {
        fprintf(stderr, "visualizer: out of memory\n");
        return;
    }

    for (int g = 0; g < steps; g++) {
        prng30_step(st);
        memcpy(history + g * width, st->row, (size_t)width);

        clear_screen();
        printf("Rule 30  width=%d  step %d/%d\n\n", width, g + 1, steps);

        int first = (g < MAX_DISPLAY_ROWS) ? 0 : (g + 1 - MAX_DISPLAY_ROWS);
        for (int row = first; row <= g; row++) {
            uint8_t *r = history + row * width;
            for (int col = 0; col < width; col++) {
                uint8_t bit = r[col];
                if (col == mid)
                    printf(COL_RED "%c " COL_RESET, bit ? '#' : '.');
                else if (bit)
                    printf(COL_BLUE "#" COL_RESET " ");
                else
                    printf(COL_GRAY "." COL_RESET " ");
            }
            printf("\n");
        }

        printf("\n" COL_RED "# = extraction column" COL_RESET "\n");
        ms_sleep(delay_ms);
    }

    free(history);

    uint64_t result = prng30_generate(st, 64);
    printf("\ngenerated: %llu (0x%016llX)\n", (unsigned long long)result, (unsigned long long)result);

    printf("\nPress Enter to exit...");
    fflush(stdout);
    getchar();
}

int main(int argc, char *argv[]) {
    uint64_t seed  = (uint64_t)time(NULL);
    int      width = 64;

    if (argc >= 2)
        seed = (uint64_t)strtoull(argv[1], NULL, 0);
    if (argc >= 3)
        width = (int)strtol(argv[2], NULL, 10);

    if (width < PRNG30_MIN_WIDTH || width > PRNG30_MAX_WIDTH) {
        fprintf(stderr, "width must be in [%d, %d]\n", PRNG30_MIN_WIDTH, PRNG30_MAX_WIDTH);
        return 1;
    }

    printf("seed=%llu  width=%d\n", (unsigned long long)seed, width);

    prng30_state st;
    prng30_err   err = prng30_init(&st, seed, width);
    if (err != PRNG30_OK) {
        fprintf(stderr, "prng30_init failed: error %d\n", err);
        return 1;
    }

    visualize(&st, width, 80);

    prng30_free(&st);
    return 0;
}