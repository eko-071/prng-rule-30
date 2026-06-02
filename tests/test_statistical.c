#include "../include/prng30.h"
#include "framework.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * NIST SP 800-22 Test 1. Statistic: |ones - zeros| / sqrt(n).
 * Threshold < 1.96 at 95% confidence.
 */
static void test_monobit(void) {
    test_header("Monobit Frequency Test (NIST SP 800-22 Test 1)");

    prng30_state st;
    prng30_init(&st, 0xDEADBEEF, 128);

    const int n    = 20000;
    int       ones = 0;
    for (int i = 0; i < n / 64; i++) {
        uint64_t v = prng30_generate(&st, 64);
        while (v) {
            ones++;
            v &= v - 1;
        } /* Kernighan popcount */
    }

    double s_obs = fabs((double)(ones - (n - ones))) / sqrt((double)n);
    printf("  ones=%d  zeros=%d  |S|/√n=%.4f  (threshold 1.96)\n", ones, n - ones, s_obs);

    check("Passes monobit test (|S|/√n < 1.96)", s_obs < 1.96);
    prng30_free(&st);
}

static void test_chi_squared(void) {
    test_header("Chi-Squared Uniformity (10 bins, 10 000 samples)");

    prng30_state st;
    prng30_init(&st, 777, 64);

    const int bins = 10, samples = 10000;
    int       counts[10] = {0};
    for (int i = 0; i < samples; i++)
        counts[prng30_generate(&st, 32) % (uint64_t)bins]++;

    double expected = (double)samples / bins;
    double chi2     = 0.0;
    for (int i = 0; i < bins; i++) {
        double d = counts[i] - expected;
        chi2 += (d * d) / expected;
    }

    printf("  χ²=%.2f  (χ²(9df) critical value at α=0.05: 16.92)\n", chi2);
    check("χ² < 16.92", chi2 < 16.92);
    prng30_free(&st);
}

static void test_bit_distribution(void) {
    test_header("Bit Distribution (each bit position ≈ 50%)");

    prng30_state st;
    prng30_init(&st, 42, 64);

    const int samples        = 1000;
    int       bit_counts[64] = {0};
    for (int i = 0; i < samples; i++) {
        uint64_t v = prng30_generate(&st, 64);
        for (int b = 0; b < 64; b++)
            if (v & (1ULL << b))
                bit_counts[b]++;
    }

    int good = 0;
    for (int b = 0; b < 64; b++) {
        double pct = 100.0 * bit_counts[b] / samples;
        if (pct >= 40.0 && pct <= 60.0)
            good++;
    }

    printf("  Bits within 40-60%%: %d/64\n", good);
    check("≥ 60/64 bit positions within 40-60%", good >= 60);
    prng30_free(&st);
}

/*
 * Knuth TAOCP Vol. 2 §3.3.2. A run ends when direction (up/down) changes.
 * E[R] = (2n-1)/3, Var[R] = (16n-29)/90. Threshold |Z| < 1.96.
 */
static void test_runs(void) {
    test_header("Runs Test (Knuth TAOCP §3.3.2)");

    prng30_state st;
    prng30_init(&st, 888, 64);

    const int n    = 2000;
    double   *vals = malloc((size_t)n * sizeof(double));
    if (!vals) {
        printf("  (skipped: malloc failed)\n");
        return;
    }

    for (int i = 0; i < n; i++)
        vals[i] = prng30_generate_double(&st);

    int runs = 1;
    for (int i = 1; i < n - 1; i++) {
        int up_before = (vals[i] > vals[i - 1]);
        int up_after  = (vals[i + 1] > vals[i]);
        if (up_before != up_after)
            runs++;
    }

    double mean = (2.0 * n - 1.0) / 3.0;
    double z    = fabs((runs - mean) / sqrt((16.0 * n - 29.0) / 90.0));
    printf("  runs=%d  E[R]=%.1f  Z=%.4f  (threshold 1.96)\n", runs, mean, z);

    check("Passes runs test |Z| < 1.96", z < 1.96);
    free(vals);
    prng30_free(&st);
}

static void test_autocorrelation(void) {
    test_header("Lag-1 Autocorrelation");

    prng30_state st;
    prng30_init(&st, 1234, 64);

    const int n = 1000;
    double   *v = malloc((size_t)n * sizeof(double));
    if (!v) {
        printf("  (skipped: malloc failed)\n");
        return;
    }

    for (int i = 0; i < n; i++)
        v[i] = prng30_generate_double(&st);

    double sx = 0, sy = 0, sxy = 0, sx2 = 0, sy2 = 0;
    for (int i = 0; i < n - 1; i++) {
        sx += v[i];
        sy += v[i + 1];
        sxy += v[i] * v[i + 1];
        sx2 += v[i] * v[i];
        sy2 += v[i + 1] * v[i + 1];
    }
    int    m    = n - 1;
    double num  = m * sxy - sx * sy;
    double den  = sqrt((m * sx2 - sx * sx) * (m * sy2 - sy * sy));
    double corr = (den == 0.0) ? 0.0 : num / den;

    printf("  lag-1 r=%.4f  (threshold |r| < 0.1)\n", corr);
    check("Low lag-1 autocorrelation |r| < 0.1", fabs(corr) < 0.1);
    free(v);
    prng30_free(&st);
}

/*
 * For n=500 values in [0, 2^32), expected collisions = n²/(2×2^32) ≈ 0.029.
 * Seeing ≥ 5 is a strong sign of non-uniformity.
 */
static void test_birthday_spacing(void) {
    test_header("Birthday Spacing (32-bit, 500 samples)");

    prng30_state st;
    prng30_init(&st, 2468, 128);

    const int n = 500;
    uint32_t  vals[500];
    for (int i = 0; i < n; i++)
        vals[i] = (uint32_t)prng30_generate(&st, 32);

    int collisions = 0;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (vals[i] == vals[j])
                collisions++;

    printf("  collisions in %d samples: %d  (expected ≈ 0)\n", n, collisions);
    check("< 5 collisions", collisions < 5);
    prng30_free(&st);
}

void run_statistical_tests(void) {
    test_monobit();
    test_chi_squared();
    test_bit_distribution();
    test_runs();
    test_autocorrelation();
    test_birthday_spacing();
}
