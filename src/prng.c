#include "../include/prng30.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static inline uint8_t rule30(uint8_t left, uint8_t mid, uint8_t right) {
    return left ^ (mid | right);
}

void prng30_init(prng30_state *st, uint64_t seed, int width) {
    st->size = width;
    st->current_row = 0;
    st->rows = calloc(width * width, sizeof(uint8_t)); // calloc zeros memory
    
    if (!st->rows) {
        exit(1);
    }
    
    // Edge case: if seed is 0 or MAX, use a default non-zero seed
    if (seed == 0 || seed == UINT64_MAX) {
        seed = 0x123456789ABCDEFULL;
    }
    
    // Initialize first row from seed with better entropy distribution
    // Use all 64 bits of seed more effectively
    for (int i = 0; i < width; i++) {
        if (i < 64) {
            // Use seed bits directly
            st->rows[i] = (seed >> i) & 1;
        } else {
            // For wider grids, use a simple hash function
            // Overflow here leads to a free modulus operation
            uint64_t hash = seed * 6364136223846793005ULL + (uint64_t)i;
            st->rows[i] = (hash >> 32) & 1;
        }
    }
    
    // For very large grids, ensure we have enough non-zero cells
    // Count set bits and add more if needed
    int set_bits = 0;
    for (int i = 0; i < width; i++) {
        if (st->rows[i]) set_bits++;
    }
    
    // If less than 20% of cells are set, add more entropy
    if (set_bits < width / 5) {
        for (int i = 0; i < width; i += 7) {
            st->rows[i] ^= 1;
        }
    }
    
    // Warmup iterations to ensure good mixing
    int warmup_rounds = (width > 128) ? 50 : 30;
    for (int warmup = 0; warmup < warmup_rounds; warmup++) {
        prng30_step(st);
    }
}

void prng30_free(prng30_state *st) {
    if (st) {
        free(st->rows);
        st->rows = NULL;
        st->size = 0;
        st->current_row = 0;
    }
}

void prng30_step(prng30_state *st) {
    int n = st->size;
    int r = st->current_row;
    int next = (r + 1) % n; // Wrap around circularly
    
    for (int j = 0; j < n; j++) {
        uint8_t left  = st->rows[r * n + ((j - 1 + n) % n)];
        uint8_t mid   = st->rows[r * n + j];
        uint8_t right = st->rows[r * n + ((j + 1) % n)];
        st->rows[next * n + j] = rule30(left, mid, right);
    }
    
    st->current_row = next;
}

uint64_t prng30_generate(prng30_state *st, int nbits) {
    if (nbits > 64) nbits = 64;
    if (nbits <= 0) return 0;
    
    uint64_t out = 0; // The output (obviously)
    int mid = st->size / 2;
    
    for (int i = 0; i < nbits; i++) {
        prng30_step(st);
        
        // Extract bit from center column
        uint8_t center_bit = st->rows[st->current_row * st->size + mid];
        
        // Also XOR with adjacent cells for better mixing
        uint8_t left_bit = st->rows[st->current_row * st->size + (mid - 1 + st->size) % st->size];
        uint8_t right_bit = st->rows[st->current_row * st->size + (mid + 1) % st->size];
        
        // Combine bits for better randomness
        uint8_t bit = center_bit ^ (left_bit & right_bit);
        
        out = (out << 1) | (bit & 1);
    }
    
    return out;
}