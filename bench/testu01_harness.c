#include "../include/prng30.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <bbattery.h>
#include <unif01.h>

//  TestU01 requires a generator wrapped in a unif01_Gen struct.
//  The generator function must return a double in [0, 1).

//  Usage:
//    ./testu01_harness [width] [battery]

//    width   : automaton width (def: 64)
//    battery : 0 = SmallCrush (fast, ~10 min)
//              1 = Crush       (medium, ~2 hrs)
//              2 = BigCrush    (full, ~6 hrs)

//  Example:
//    ./testu01_harness 64 0    # SmallCrush, width 64
//    ./testu01_harness 128 2   # BigCrush, width 128

static prng30_state g_st;

static double prng30_testu01(void) {
    return prng30_generate_double(&g_st);
}

static unsigned long prng30_testu01_bits(void) {
    return (unsigned long)prng30_generate(&g_st, 32);
}

int main(int argc, char *argv[]) {
    int width   = 64;
    int battery = 0;

    if (argc >= 2)
        width = atoi(argv[1]);
    if (argc >= 3)
        battery = atoi(argv[2]);

    if (width < PRNG30_MIN_WIDTH || width > PRNG30_MAX_WIDTH) {
        fprintf(stderr, "width must be in [%d, %d]\n", PRNG30_MIN_WIDTH, PRNG30_MAX_WIDTH);
        return 1;
    }

    if (battery < 0 || battery > 2) {
        fprintf(stderr, "battery must be 0 (SmallCrush), 1 (Crush), or 2 (BigCrush)\n");
        return 1;
    }

    prng30_err err = prng30_init(&g_st, (uint64_t)time(NULL), width);
    if (err != PRNG30_OK) {
        fprintf(stderr, "prng30_init failed: %d\n", err);
        return 1;
    }

    char name[64];
    snprintf(name, sizeof(name), "prng30_w%d", width);

    unif01_Gen *gen = unif01_CreateExternGen01(name, prng30_testu01);

    printf("prng30  width=%d  battery=%s\n\n", width, battery == 0 ? "SmallCrush" : battery == 1 ? "Crush" : "BigCrush");

    switch (battery) {
    case 0:
        bbattery_SmallCrush(gen);
        break;
    case 1:
        bbattery_Crush(gen);
        break;
    case 2:
        bbattery_BigCrush(gen);
        break;
    }

    unif01_DeleteExternGen01(gen);
    prng30_free(&g_st);
    return 0;
}