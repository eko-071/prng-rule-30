#include "../include/prng30.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
static void sleep_screen() { Sleep(80); }
static void clear_screen() { system("cls"); }
#else
#include <unistd.h>
static void sleep_screen() { usleep(80000); }
static void clear_screen() { system("clear"); }
#endif

static int check_range(int start, int size, int row) {
    if (size <= 0) return 0;
    
    int half = size / 2;
    int end = (start + half) % size;
    
    // Non-wrapping case
    if (start < end) {
        return (row >= start && row < end);
    }
    // Wrapping case
    else {
        return (row >= start || row < end);
    }
}

void prng30_visualize_animated(prng30_state *st) {
    if (!st || !st->rows || st->size <= 0) {
        fprintf(stderr, "Invalid automaton state\n");
        return;
    }
    
    int n = st->size;
    time_t t = time(NULL);
    int start = (t != -1) ? (t % n) : 0;
    int mid = n / 2;
    
    // Save the starting row (after warmup)
    int start_row = st->current_row;
    
    // Animate evolution
    for (int generation = 0; generation < n - 1; generation++) {
        clear_screen();
        printf("Generation %d/%d\n", generation + 1, n - 1);
        
        // Display rows relative to where we started
        int rows_to_show = generation + 1;
        for (int i = 0; i < rows_to_show && i < n; i++) {
            int actual_row = (start_row + i) % n;
            for (int j = 0; j < n; j++) {
                uint8_t bit = st->rows[actual_row * n + j];
                int highlight = (j == mid && check_range(start, n, actual_row));
                
                if (highlight) 
                    printf("\033[31m%d \033[0m", bit);      // Red for highlight
                else if (bit == 1) 
                    printf("\033[34m%d \033[0m", bit);      // Blue for 1
                else 
                    printf("\033[90m%d \033[0m", bit);      // Gray for 0
            }
            printf("\n");
        }
        
        prng30_step(st);
        sleep_screen();
    }
    
    // Final display
    clear_screen();
    printf("Generation %d/%d (final)\n", n - 1, n - 1);
    
    for (int i = 0; i < n - 1; i++) {
        int actual_row = (start_row + i) % n;
        for (int j = 0; j < n; j++) {
            uint8_t bit = st->rows[actual_row * n + j];
            int highlight = (j == mid && check_range(start, n, actual_row));
            
            if (highlight) 
                printf("\033[31m%d \033[0m", bit);
            else if (bit == 1) 
                printf("\033[34m%d \033[0m", bit);
            else 
                printf("\033[90m%d \033[0m", bit);
        }
        printf("\n");
    }
    
    printf("\nPress Enter to continue...");
    getchar();
}