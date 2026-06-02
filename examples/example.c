#include "../include/prng30.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void die(const char *msg, prng30_err err) {
    fprintf(stderr, "error: %s (code %d)\n", msg, err);
    exit(1);
}

static void example_basic(void) {
    printf("1. Basic 32-bit Generation\n");
    prng30_state st;
    if (prng30_init(&st, 12345, 64) != PRNG30_OK)
        die("prng30_init", -1);
    for (int i = 0; i < 10; i++) {
        uint64_t v = prng30_generate(&st, 32);
        printf("  [%2d]  %10llu  (0x%08llX)\n", i + 1, (unsigned long long)v, (unsigned long long)v);
    }
    prng30_free(&st);
    printf("\n");
}

static void example_bitwidths(void) {
    printf("2. Different Bit Widths\n");
    prng30_state st;
    prng30_init(&st, 99999, 64);
    uint64_t r8  = prng30_generate(&st, 8);
    uint64_t r16 = prng30_generate(&st, 16);
    uint64_t r32 = prng30_generate(&st, 32);
    uint64_t r64 = prng30_generate(&st, 64);
    printf("   8-bit: %3llu  (0x%02llX)\n", (unsigned long long)r8, (unsigned long long)r8);
    printf("  16-bit: %5llu  (0x%04llX)\n", (unsigned long long)r16, (unsigned long long)r16);
    printf("  32-bit: %10llu  (0x%08llX)\n", (unsigned long long)r32, (unsigned long long)r32);
    printf("  64-bit: %20llu  (0x%016llX)\n", (unsigned long long)r64, (unsigned long long)r64);
    prng30_free(&st);
    printf("\n");
}

static void example_time_seed(void) {
    printf("3. Time-Based Seed\n");
    uint64_t seed = (uint64_t)time(NULL);
    printf("  seed = %llu\n", (unsigned long long)seed);
    prng30_state st;
    prng30_init(&st, seed, 64);
    for (int i = 0; i < 5; i++)
        printf("  %llu\n", (unsigned long long)prng30_generate(&st, 32));
    prng30_free(&st);
    printf("\n");
}

static void example_dice(void) {
    printf("4. Six-Sided Dice Rolls (20 rolls)\n");
    prng30_state st;
    prng30_init(&st, (uint64_t)time(NULL), 64);
    for (int i = 0; i < 20; i++) {
        printf("  %d", (int)(prng30_generate(&st, 8) % 6) + 1);
        if ((i + 1) % 10 == 0)
            printf("\n");
    }
    prng30_free(&st);
    printf("\n");
}

static void example_doubles(void) {
    printf("5. Uniform Doubles in [0, 1)\n");
    prng30_state st;
    prng30_init(&st, 0xDEADBEEF, 64);
    double sum = 0.0;
    for (int i = 0; i < 10; i++) {
        double d = prng30_generate_double(&st);
        printf("  %.8f\n", d);
        sum += d;
    }
    printf("  mean = %.4f\n", sum / 10);
    prng30_free(&st);
    printf("\n");
}

static void example_grid_sizes(void) {
    printf("6. Grid Size Comparison (seed=777)\n");
    int sizes[] = {32, 64, 128, 256};
    for (int i = 0; i < 4; i++) {
        prng30_state st;
        prng30_init(&st, 777, sizes[i]);
        printf("  width=%3d: ", sizes[i]);
        for (int j = 0; j < 5; j++)
            printf("%10llu ", (unsigned long long)prng30_generate(&st, 32));
        printf("\n");
        prng30_free(&st);
    }
    printf("\n");
}

static void example_histogram(void) {
    printf("7. Distribution Histogram (10 bins, 10000 samples)\n");
    prng30_state st;
    prng30_init(&st, 54321, 64);
    int bins[10] = {0};
    for (int i = 0; i < 10000; i++)
        bins[prng30_generate(&st, 32) % 10]++;
    for (int i = 0; i < 10; i++) {
        printf("  [%d] %4d ", i, bins[i]);
        for (int b = 0; b < bins[i] / 50; b++)
            printf("|");
        printf("\n");
    }
    prng30_free(&st);
    printf("\n");
}

int main(void) {
    printf("prng30 examples\n\n");

    example_basic();
    example_bitwidths();
    example_time_seed();
    example_dice();
    example_doubles();
    example_grid_sizes();
    example_histogram();

    printf("Run ./prng30_visualizer for the animated CA display.\n");
    return 0;
}