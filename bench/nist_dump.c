#include "../include/prng30.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//  Writes 1 million bits (125000 bytes) of PRNG output to a binary file.
//  NIST STS default sequence length is 1,000,000 bits.

//  Usage:
//    ./nist_dump [width] [output_file]

//  Example:
//    ./nist_dump 64  nist_w64.bin
//    ./nist_dump 128 nist_w128.bin

#define NIST_BYTES (131072 * 100)

int main(int argc, char *argv[]) {
    int         width   = 64;
    const char *outfile = "nist_output.bin";

    if (argc >= 2)
        width = atoi(argv[1]);
    if (argc >= 3)
        outfile = argv[2];

    if (width < PRNG30_MIN_WIDTH || width > PRNG30_MAX_WIDTH) {
        fprintf(stderr, "width must be in [%d, %d]\n", PRNG30_MIN_WIDTH, PRNG30_MAX_WIDTH);
        return 1;
    }

    FILE *f = fopen(outfile, "wb");
    if (!f) {
        fprintf(stderr, "cannot open %s\n", outfile);
        return 1;
    }

    prng30_state st;
    prng30_err   err = prng30_init(&st, (uint64_t)time(NULL), width);
    if (err != PRNG30_OK) {
        fprintf(stderr, "prng30_init failed: %d\n", err);
        fclose(f);
        return 1;
    }

    uint8_t buf[4096];
    size_t  total = 0;
    while (total < NIST_BYTES) {
        size_t chunk = sizeof(buf);
        if (total + chunk > NIST_BYTES)
            chunk = NIST_BYTES - total;
        for (size_t i = 0; i < chunk; i++)
            buf[i] = (uint8_t)prng30_generate(&st, 8);
        fwrite(buf, 1, chunk, f);
        total += chunk;
    }

    fclose(f);
    prng30_free(&st);

    fprintf(stderr, "wrote %zu bytes to %s\n", total, outfile);
    return 0;
}