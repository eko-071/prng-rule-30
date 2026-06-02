#include "../include/prng30.h"
#include "framework.h"

#include <stdint.h>
#include <stdio.h>

void run_core_tests(void) {
    /* --- Initialisation and Memory Management --- */
    test_header("Initialisation and Memory Management");
    {
        prng30_state st;
        prng30_err   err = prng30_init(&st, 12345, 64);
        check("prng30_init returns PRNG30_OK", err == PRNG30_OK);
        check("width set correctly", st.width == 64);
        check("row buffer allocated", st.row != NULL);
        check("next_row buffer allocated", st.next_row != NULL);
        prng30_free(&st);
        check("row pointer zeroed after free", st.row == NULL);
        check("next_row pointer zeroed", st.next_row == NULL);
        check("width zeroed after free", st.width == 0);
    }

    /* --- Determinism --- */
    test_header("Determinism (same seed → same sequence)");
    {
        prng30_state a, b;
        prng30_init(&a, 99999, 64);
        prng30_init(&b, 99999, 64);
        int match = 1;
        for (int i = 0; i < 200; i++)
            if (prng30_generate(&a, 32) != prng30_generate(&b, 32)) {
                match = 0;
                break;
            }
        check("200 consecutive 32-bit values identical", match);
        prng30_free(&a);
        prng30_free(&b);
    }

    /* --- Seed Variation --- */
    test_header("Seed Variation (different seeds → different sequences)");
    {
        prng30_state a, b;
        prng30_init(&a, 12345, 64);
        prng30_init(&b, 54321, 64);
        int diff = 0;
        for (int i = 0; i < 100; i++)
            if (prng30_generate(&a, 32) != prng30_generate(&b, 32))
                diff++;
        check("≥ 95/100 values differ between seeds", diff >= 95);
        prng30_free(&a);
        prng30_free(&b);
    }

    /* --- Variable Bit Widths --- */
    test_header("Variable Bit Width Generation");
    {
        prng30_state st;
        prng30_init(&st, 555, 64);
        uint64_t r8  = prng30_generate(&st, 8);
        uint64_t r16 = prng30_generate(&st, 16);
        uint64_t r32 = prng30_generate(&st, 32);
        uint64_t r64 = prng30_generate(&st, 64);
        check("8-bit  value ≤ 0xFF", r8 <= 0xFF);
        check("16-bit value ≤ 0xFFFF", r16 <= 0xFFFF);
        check("32-bit value ≤ 0xFFFFFFFF", r32 <= 0xFFFFFFFF);
        check("64-bit value generated", r64 != r32);
        prng30_free(&st);
    }

    /* --- Grid Size Variations --- */
    test_header("Grid Size Variations");
    {
        int sizes[] = {32, 64, 128, 256, 512};
        for (int i = 0; i < 5; i++) {
            prng30_state st;
            char         msg[64];
            prng30_err   err = prng30_init(&st, 9999, sizes[i]);
            snprintf(msg, sizeof(msg), "size=%d initialises OK", sizes[i]);
            check(msg, err == PRNG30_OK);
            if (err == PRNG30_OK) {
                snprintf(msg, sizeof(msg), "size=%d produces non-zero output", sizes[i]);
                check(msg, prng30_generate(&st, 32) != 0);
                prng30_free(&st);
            }
        }
    }

    /* --- Edge Cases --- */
    test_header("Edge Cases");
    {
        prng30_state st;

        prng30_init(&st, 0, 64);
        check("seed=0: non-zero output", prng30_generate(&st, 32) != 0);
        prng30_free(&st);

        prng30_init(&st, UINT64_MAX, 64);
        check("seed=UINT64_MAX: non-zero output", prng30_generate(&st, 32) != 0);
        prng30_free(&st);

        prng30_init(&st, 123, 64);
        check("nbits=0 returns 0", prng30_generate(&st, 0) == 0);
        prng30_free(&st);

        prng30_init(&st, 123, 64);
        (void)prng30_generate(&st, 200);
        check("nbits=200 does not crash", 1);
        prng30_free(&st);
    }

    /* --- Error Return Codes --- */
    test_header("Error Return Codes");
    {
        prng30_state st;
        check("NULL → PRNG30_ERR_NULL", prng30_init(NULL, 1, 64) == PRNG30_ERR_NULL);
        check("width too small → PRNG30_ERR_BADWIDTH", prng30_init(&st, 1, PRNG30_MIN_WIDTH - 1) == PRNG30_ERR_BADWIDTH);
        check("width too large → PRNG30_ERR_BADWIDTH", prng30_init(&st, 1, PRNG30_MAX_WIDTH + 1) == PRNG30_ERR_BADWIDTH);
        prng30_free(&st);
        check("prng30_free safe after failed init", 1);
    }
}
