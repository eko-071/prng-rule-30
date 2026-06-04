#include "../include/prng30.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Usage:
 *   ./practrand_dump [width] | RNG_test stdin
 *   ./practrand_dump [width] | RNG_test stdin32
 *
 * width defaults to 64. Writes 64-bit values as raw bytes to stdout.
 * Pipe directly into PractRand; it will stop reading when it decides.
 *
 * Example:
 *   ./practrand_dump 64  | RNG_test stdin -tlmax 1TB
 *   ./practrand_dump 128 | RNG_test stdin -tlmax 1TB
 */

int main(int argc, char *argv[]) {
    int width = 64;
    if (argc >= 2)
        width = atoi(argv[1]);

    if (width < PRNG30_MIN_WIDTH || width > PRNG30_MAX_WIDTH) {
        fprintf(stderr, "width must be in [%d, %d]\n", PRNG30_MIN_WIDTH, PRNG30_MAX_WIDTH);
        return 1;
    }

    uint64_t seed = (uint64_t)time(NULL);

    prng30_state st;
    prng30_err   err = prng30_init(&st, seed, width);
    if (err != PRNG30_OK) {
        fprintf(stderr, "prng30_init failed: %d\n", err);
        return 1;
    }

    uint64_t buf[256];
    while (1) {
        for (int i = 0; i < 256; i++)
            buf[i] = prng30_generate(&st, 64);
        if (fwrite(buf, sizeof(uint64_t), 256, stdout) != 256)
            break;
    }

    prng30_free(&st);
    return 0;
}