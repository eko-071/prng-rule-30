#include "../include/prng30.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>

// Example 1: Basic random number generation
void example_basic_generation() {
    printf("=== Example 1: Basic Random Number Generation ===\n");
    
    prng30_state prng;
    prng30_init(&prng, 12345, 64);
    
    printf("Generating 10 random 32-bit numbers:\n");
    for (int i = 0; i < 10; i++) {
        uint64_t random = prng30_generate(&prng, 32);
        printf("%2d: %10lu (0x%08lX)\n", i + 1, random, random);
    }
    
    prng30_free(&prng);
    printf("\n");
}

// Example 2: Different bit widths
void example_different_bitwidths() {
    printf("=== Example 2: Generating Different Bit Widths ===\n");
    
    prng30_state prng;
    prng30_init(&prng, 99999, 64);
    
    printf("8-bit random:  %3lu (0x%02lX)\n", 
           prng30_generate(&prng, 8), prng30_generate(&prng, 8));
    
    printf("16-bit random: %5lu (0x%04lX)\n", 
           prng30_generate(&prng, 16), prng30_generate(&prng, 16));
    
    printf("32-bit random: %10lu (0x%08lX)\n", 
           prng30_generate(&prng, 32), prng30_generate(&prng, 32));
    
    printf("64-bit random: %20lu (0x%016lX)\n", 
           prng30_generate(&prng, 64), prng30_generate(&prng, 64));
    
    prng30_free(&prng);
    printf("\n");
}

// Example 3: Using time-based seed
void example_time_seed() {
    printf("=== Example 3: Time-Based Seed ===\n");
    
    uint64_t seed = (uint64_t)time(NULL);
    printf("Using seed from current time: %lu\n", seed);
    
    prng30_state prng;
    prng30_init(&prng, seed, 64);
    
    printf("Random numbers with time seed:\n");
    for (int i = 0; i < 5; i++) {
        uint64_t random = prng30_generate(&prng, 32);
        printf("  %lu\n", random);
    }
    
    prng30_free(&prng);
    printf("\n");
}

// Example 4: Simulating dice rolls
void example_dice_rolls() {
    printf("=== Example 4: Simulating 6-Sided Dice Rolls ===\n");
    
    prng30_state prng;
    uint64_t seed = (uint64_t)time(NULL);  // Use current time as seed
    prng30_init(&prng, seed, 64);
    
    printf("Rolling 20 dice (seed: %lu):\n", seed);
    for (int i = 0; i < 20; i++) {
        uint64_t random = prng30_generate(&prng, 8);
        int dice = (random % 6) + 1;  // 1-6
        printf("%d ", dice);
        if ((i + 1) % 10 == 0) printf("\n");
    }
    
    prng30_free(&prng);
    printf("\n");
}

// Example 5: Different automaton sizes
void example_different_sizes() {
    printf("=== Example 5: Different Automaton Sizes ===\n");
    
    uint64_t seed = 777;
    int sizes[] = {32, 64, 128};
    
    for (int i = 0; i < 3; i++) {
        prng30_state prng;
        prng30_init(&prng, seed, sizes[i]);
        
        printf("Size %3d: ", sizes[i]);
        for (int j = 0; j < 5; j++) {
            printf("%10lu ", prng30_generate(&prng, 32));
        }
        printf("\n");
        
        prng30_free(&prng);
    }
    printf("\n");
}

// Example 6: Visualization demonstration
void example_visualization() {
    printf("=== Example 6: Cellular Automaton Visualization ===\n");
    printf("Press Enter to start animated visualization...\n");
    getchar();
    
    prng30_state prng;
    prng30_init(&prng, 12345, 40);  // Smaller size for better visualization
    
    prng30_visualize_animated(&prng);
    
    prng30_free(&prng);
}

// Example 7: Statistical distribution check
void example_statistics() {
    printf("=== Example 7: Basic Statistical Check ===\n");
    
    prng30_state prng;
    prng30_init(&prng, 54321, 64);
    
    int bins[10] = {0};
    int samples = 10000;
    
    printf("Generating %d samples and checking distribution...\n", samples);
    for (int i = 0; i < samples; i++) {
        uint64_t random = prng30_generate(&prng, 32);
        int bin = (random % 10);
        bins[bin]++;
    }
    
    printf("Distribution across 10 bins (expected ~%d per bin):\n", samples / 10);
    for (int i = 0; i < 10; i++) {
        printf("Bin %d: %4d ", i, bins[i]);
        // Simple bar chart
        for (int j = 0; j < bins[i] / 50; j++) printf("█");
        printf("\n");
    }
    
    prng30_free(&prng);
    printf("\n");
}

int main() {
    printf("╔════════════════════════════════════════════════════╗\n");
    printf("║     Rule 30 Cellular Automaton PRNG Examples       ║\n");
    printf("╚════════════════════════════════════════════════════╝\n\n");
    
    example_basic_generation();
    example_different_bitwidths();
    example_time_seed();
    example_dice_rolls();
    example_different_sizes();
    example_statistics();
    
    printf("Would you like to see the visualization? (y/n): ");
    char response;
    scanf(" %c", &response);
    getchar();  // consume newline
    
    if (response == 'y' || response == 'Y') {
        example_visualization();
    }
    
    printf("\n╔════════════════════════════════════════════════════╗\n");
    printf("║              All examples completed!               ║\n");
    printf("╚════════════════════════════════════════════════════╝\n");
    
    return 0;
}