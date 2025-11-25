#ifndef PRNG30_H
#define PRNG30_H

#include <stdint.h>

typedef struct {
    int size;              // width of automaton
    uint8_t *rows;         // a size * size matrix stored as linear memory
    int current_row;
} prng30_state;

// This initialises the cellular automata
void prng30_init(prng30_state *st, uint64_t seed, int width);

// This frees memory
void prng30_free(prng30_state *st);

// This creates the next iteration
void prng30_step(prng30_state *st);

// This extracts the random number by extracting it from the automaton
uint64_t prng30_generate(prng30_state *st, int nbits);

// This is the visualizer 
void prng30_visualize_animated(prng30_state *st);

#endif