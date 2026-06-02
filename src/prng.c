#include "../include/prng30.h"

#include <stdlib.h>
#include <string.h>

//  Rule 30:  neighbours 111 110 101 100 011 010 001 000
//            next cell   0   0   0   1   1   1   1   0
// Binary 00011110 = 30. Closed form: left XOR (mid OR right).

static inline uint8_t rule30(uint8_t left, uint8_t mid, uint8_t right) {
    return left ^ (mid | right);
}

// splitmix64 — spreads seed bits across cells beyond index 63.
static uint64_t splitmix64(uint64_t *state) {
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z          = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z          = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

prng30_err prng30_init(prng30_state *st, uint64_t seed, int width) {
    if (!st)
        return PRNG30_ERR_NULL;

    memset(st, 0, sizeof(*st));

    if (width < PRNG30_MIN_WIDTH || width > PRNG30_MAX_WIDTH)
        return PRNG30_ERR_BADWIDTH;

    st->row      = calloc((size_t)width, sizeof(uint8_t));
    st->next_row = calloc((size_t)width, sizeof(uint8_t));

    if (!st->row || !st->next_row) {
        prng30_free(st);
        return PRNG30_ERR_ALLOC;
    }

    st->width = width;

    uint64_t sm_state = seed;
    for (int i = 0; i < width; i++) {
        st->row[i] = (i < 64) ? (uint8_t)((seed >> i) & 1) : (uint8_t)(splitmix64(&sm_state) & 1);
    }

    // All-zero and all-one initial states reach a fixed point under Rule 30.
    int all_zero = 1, all_one = 1;
    for (int i = 0; i < width; i++) {
        if (st->row[i])
            all_zero = 0;
        if (!st->row[i])
            all_one = 0;
    }
    if (all_zero || all_one)
        st->row[width / 2] = 1;

    // width/2 warmup steps guarantee full diffusion under periodic boundaries.
    int warmup = width / 2;
    for (int i = 0; i < warmup; i++)
        prng30_step(st);

    // Some symmetric seeds collapse to all-zeros during warmup; recover.
    int live = 0;
    for (int i = 0; i < width; i++)
        if (st->row[i]) {
            live = 1;
            break;
        }
    if (!live) {
        st->row[width / 2] = 1;
        for (int i = 0; i < warmup; i++)
            prng30_step(st);
    }

    return PRNG30_OK;
}

void prng30_free(prng30_state *st) {
    if (!st)
        return;
    free(st->row);
    free(st->next_row);
    memset(st, 0, sizeof(*st));
}

void prng30_step(prng30_state *st) {
    int n = st->width;
    for (int j = 0; j < n; j++) {
        uint8_t left    = st->row[(j - 1 + n) % n];
        uint8_t mid     = st->row[j];
        uint8_t right   = st->row[(j + 1) % n];
        st->next_row[j] = rule30(left, mid, right);
    }
    uint8_t *tmp = st->row;
    st->row      = st->next_row;
    st->next_row = tmp;
}

uint64_t prng30_generate(prng30_state *st, int nbits) {
    if (nbits <= 0)
        return 0;
    if (nbits > 64)
        nbits = 64;

    int      mid = st->width / 2;
    int      tap = st->width / 8;
    int      n   = st->width;
    uint64_t out = 0;

    for (int i = 0; i < nbits; i++) {
        prng30_step(st);
        uint8_t bit = st->row[mid] ^ st->row[(mid - tap + n) % n] ^ st->row[(mid + tap) % n];
        out         = (out << 1) | (bit & 1);
    }

    return out;
}

double prng30_generate_double(prng30_state *st) {
    return (double)prng30_generate(st, 53) / (double)(1ULL << 53);
}
