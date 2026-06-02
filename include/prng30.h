#ifndef PRNG30_H
#define PRNG30_H

/*
 * prng30 — Rule 30 Cellular Automaton PRNG
 * NOT cryptographically secure.
 */

#include <stdint.h>

typedef enum {
    PRNG30_OK           = 0,
    PRNG30_ERR_ALLOC    = -1,
    PRNG30_ERR_BADWIDTH = -2,
    PRNG30_ERR_NULL     = -3,
} prng30_err;

#define PRNG30_MIN_WIDTH 32
#define PRNG30_MAX_WIDTH 4096

typedef struct {
    int      width;
    uint8_t *row;
    uint8_t *next_row;
} prng30_state;

/*
 * prng30_init — initialise the automaton.
 *   seed  : any 64-bit value including 0 and UINT64_MAX
 *   width : cell count [PRNG30_MIN_WIDTH .. PRNG30_MAX_WIDTH]
 * Returns PRNG30_OK or a negative error code.
 * On failure *st is zeroed and prng30_free is safe to call.
 */
prng30_err prng30_init(prng30_state *st, uint64_t seed, int width);

/* Release resources. Safe on a zeroed or already-freed state. */
void prng30_free(prng30_state *st);

/* Advance by one generation. Exposed for direct CA experiments. */
void prng30_step(prng30_state *st);

/*
 * prng30_generate — return a random integer.
 *   nbits : bits to generate [1 .. 64]; silently clamped if outside range
 */
uint64_t prng30_generate(prng30_state *st, int nbits);

/* Uniform double in [0, 1) using 53 bits of entropy. */
double prng30_generate_double(prng30_state *st);

#endif /* PRNG30_H */
