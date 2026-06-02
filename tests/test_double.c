#include "../include/prng30.h"
#include "framework.h"

#include <stdio.h>

void run_double_tests(void) {
    test_header("prng30_generate_double: range [0, 1)");

    prng30_state st;
    prng30_init(&st, 0xCAFEBABE, 64);

    int       in_range = 1;
    double    sum      = 0.0;
    const int n        = 10000;
    for (int i = 0; i < n; i++) {
        double d = prng30_generate_double(&st);
        if (d < 0.0 || d >= 1.0) {
            in_range = 0;
            break;
        }
        sum += d;
    }

    double mean = sum / n;
    printf("  mean over %d samples=%.4f  (expected ≈ 0.5)\n", n, mean);
    check("All values in [0, 1)", in_range);
    check("Mean within 0.49-0.51", mean > 0.49 && mean < 0.51);
    prng30_free(&st);
}
