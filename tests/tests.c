#include "../include/prng30.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

// Test results tracking
static int tests_passed = 0;
static int tests_failed = 0;

// Color codes for output
#define COLOR_GREEN "\033[32m"
#define COLOR_RED "\033[31m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_RESET "\033[0m"

void print_test_header(const char *test_name) {
    printf("\n" COLOR_BLUE "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST: %s\n", test_name);
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" COLOR_RESET "\n");
}

void assert_test(const char *description, int condition) {
    if (condition) {
        printf(COLOR_GREEN "✓ PASS" COLOR_RESET ": %s\n", description);
        tests_passed++;
    } else {
        printf(COLOR_RED "✗ FAIL" COLOR_RESET ": %s\n", description);
        tests_failed++;
    }
}

// Test 1: Basic initialization and cleanup
void test_initialization() {
    print_test_header("Initialization and Memory Management");
    
    prng30_state prng;
    prng30_init(&prng, 12345, 64);
    
    assert_test("State size is correct", prng.size == 64);
    assert_test("Rows buffer is allocated", prng.rows != NULL);
    assert_test("Current row is initialized", prng.current_row >= 0);
    
    prng30_free(&prng);
    assert_test("Rows buffer freed correctly", prng.rows == NULL);
}

// Test 2: Deterministic behavior (same seed = same output)
void test_determinism() {
    print_test_header("Deterministic Behavior");
    
    prng30_state prng1, prng2;
    uint64_t seed = 99999;
    
    prng30_init(&prng1, seed, 64);
    prng30_init(&prng2, seed, 64);
    
    int matches = 0;
    for (int i = 0; i < 100; i++) {
        uint64_t r1 = prng30_generate(&prng1, 32);
        uint64_t r2 = prng30_generate(&prng2, 32);
        if (r1 == r2) matches++;
    }
    
    assert_test("Same seed produces identical sequences", matches == 100);
    
    prng30_free(&prng1);
    prng30_free(&prng2);
}

// Test 3: Different seeds produce different outputs
void test_seed_variation() {
    print_test_header("Seed Variation");
    
    prng30_state prng1, prng2;
    prng30_init(&prng1, 12345, 64);
    prng30_init(&prng2, 54321, 64);
    
    uint64_t r1 = prng30_generate(&prng1, 64);
    uint64_t r2 = prng30_generate(&prng2, 64);
    
    assert_test("Different seeds produce different outputs", r1 != r2);
    
    int different_count = 0;
    for (int i = 0; i < 100; i++) {
        r1 = prng30_generate(&prng1, 32);
        r2 = prng30_generate(&prng2, 32);
        if (r1 != r2) different_count++;
    }
    
    assert_test("Different seeds maintain divergence", different_count > 95);
    
    prng30_free(&prng1);
    prng30_free(&prng2);
}

// Test 4: Chi-squared uniformity test
void test_chi_squared() {
    print_test_header("Chi-Squared Uniformity Test");
    
    prng30_state prng;
    prng30_init(&prng, 777, 64);
    
    int bins = 10;
    int samples = 10000;
    int counts[10] = {0};
    double expected = samples / (double)bins;
    
    // Generate samples
    for (int i = 0; i < samples; i++) {
        uint64_t r = prng30_generate(&prng, 32);
        counts[r % bins]++;
    }
    
    // Calculate chi-squared statistic
    double chi_squared = 0.0;
    for (int i = 0; i < bins; i++) {
        double diff = counts[i] - expected;
        chi_squared += (diff * diff) / expected;
    }
    
    // Critical value for 9 degrees of freedom at 95% confidence: 16.92
    double critical_value = 16.92;
    
    printf("Chi-squared statistic: %.2f (critical value: %.2f)\n", 
           chi_squared, critical_value);
    printf("Distribution across %d bins:\n", bins);
    for (int i = 0; i < bins; i++) {
        printf("  Bin %d: %4d (%.1f%%)\n", i, counts[i], 
               100.0 * counts[i] / samples);
    }
    
    assert_test("Passes chi-squared test", chi_squared < critical_value);
    assert_test("No bin is empty", counts[0] > 0 && counts[bins-1] > 0);
    
    prng30_free(&prng);
}

// Test 5: Bit distribution test
void test_bit_distribution() {
    print_test_header("Bit Distribution Test");
    
    prng30_state prng;
    prng30_init(&prng, 42, 64);
    
    int bit_counts[64] = {0};
    int samples = 1000;
    
    for (int i = 0; i < samples; i++) {
        uint64_t r = prng30_generate(&prng, 64);
        for (int bit = 0; bit < 64; bit++) {
            if (r & (1ULL << bit)) {
                bit_counts[bit]++;
            }
        }
    }
    
    // Each bit should be set approximately 50% of the time
    int good_bits = 0;
    for (int i = 0; i < 64; i++) {
        double percentage = 100.0 * bit_counts[i] / samples;
        if (percentage >= 40.0 && percentage <= 60.0) {
            good_bits++;
        }
    }
    
    printf("Bits within 40-60%% range: %d/64\n", good_bits);
    assert_test("Most bits have ~50% distribution", good_bits >= 60);
    
    prng30_free(&prng);
}

// Test 6: Runs test (sequences test)
void test_runs() {
    print_test_header("Runs Test (Sequence Independence)");
    
    prng30_state prng;
    prng30_init(&prng, 888, 64);
    
    int samples = 1000;
    uint64_t prev = prng30_generate(&prng, 32);
    int runs = 1;
    
    for (int i = 1; i < samples; i++) {
        uint64_t curr = prng30_generate(&prng, 32);
        if ((curr > prev) != ((i > 1) ? (prev > 0) : 0)) {
            runs++;
        }
        prev = curr;
    }
    
    // Expected runs: approximately n/2
    double expected_runs = samples / 2.0;
    double z_score = fabs(runs - expected_runs) / sqrt(samples / 4.0);
    
    printf("Observed runs: %d (expected: %.0f)\n", runs, expected_runs);
    printf("Z-score: %.2f\n", z_score);
    
    assert_test("Passes runs test (Z < 2.0)", z_score < 2.0);
    
    prng30_free(&prng);
}

// Test 7: Autocorrelation test
void test_autocorrelation() {
    print_test_header("Autocorrelation Test");
    
    prng30_state prng;
    prng30_init(&prng, 1234, 64);
    
    int samples = 500;
    uint64_t values[500];
    
    for (int i = 0; i < samples; i++) {
        values[i] = prng30_generate(&prng, 32);
    }
    
    // Test lag-1 autocorrelation
    double sum_xy = 0, sum_x = 0, sum_y = 0, sum_x2 = 0, sum_y2 = 0;
    for (int i = 0; i < samples - 1; i++) {
        double x = (double)values[i];
        double y = (double)values[i + 1];
        sum_xy += x * y;
        sum_x += x;
        sum_y += y;
        sum_x2 += x * x;
        sum_y2 += y * y;
    }
    
    int n = samples - 1;
    double correlation = (n * sum_xy - sum_x * sum_y) / 
                        sqrt((n * sum_x2 - sum_x * sum_x) * 
                             (n * sum_y2 - sum_y * sum_y));
    
    printf("Lag-1 autocorrelation: %.4f\n", correlation);
    
    assert_test("Low autocorrelation", fabs(correlation) < 0.1);
    
    prng30_free(&prng);
}

// Test 8: Different bit widths
void test_bit_widths() {
    print_test_header("Variable Bit Width Generation");
    
    prng30_state prng;
    prng30_init(&prng, 555, 64);
    
    uint64_t r8 = prng30_generate(&prng, 8);
    uint64_t r16 = prng30_generate(&prng, 16);
    uint64_t r32 = prng30_generate(&prng, 32);
    uint64_t r64 = prng30_generate(&prng, 64);
    
    assert_test("8-bit value in range", r8 <= 0xFF);
    assert_test("16-bit value in range", r16 <= 0xFFFF);
    assert_test("32-bit value in range", r32 <= 0xFFFFFFFF);
    assert_test("64-bit value generated", r64 > 0);
    
    // Test that different widths produce different values
    assert_test("Different widths produce different sequences", 
                r8 != (r16 & 0xFF) && r16 != (r32 & 0xFFFF));
    
    prng30_free(&prng);
}

// Test 9: Grid size variations
void test_grid_sizes() {
    print_test_header("Different Grid Sizes");
    
    int sizes[] = {32, 64, 128, 256};
    uint64_t seed = 9999;
    
    for (int i = 0; i < 4; i++) {
        prng30_state prng;
        prng30_init(&prng, seed, sizes[i]);
        
        uint64_t r = prng30_generate(&prng, 32);
        
        char msg[100];
        sprintf(msg, "Size %d produces valid output", sizes[i]);
        assert_test(msg, r > 0);
        
        prng30_free(&prng);
    }
}

// Test 10: Non-zero output test
void test_non_zero() {
    print_test_header("Non-Zero Output Test");
    
    prng30_state prng;
    prng30_init(&prng, 111, 64);
    
    int zero_count = 0;
    int samples = 1000;
    
    for (int i = 0; i < samples; i++) {
        uint64_t r = prng30_generate(&prng, 32);
        if (r == 0) zero_count++;
    }
    
    double zero_percentage = 100.0 * zero_count / samples;
    printf("Zero values: %d/%d (%.2f%%)\n", zero_count, samples, zero_percentage);
    
    assert_test("Generates non-zero values", zero_count < samples);
    assert_test("Very few zero values", zero_percentage < 1.0);
    
    prng30_free(&prng);
}

// Test 11: Birthday spacing test (simplified)
void test_birthday_spacing() {
    print_test_header("Birthday Spacing Test");
    
    prng30_state prng;
    prng30_init(&prng, 2468, 64);
    
    int samples = 500;
    uint32_t values[500];
    
    for (int i = 0; i < samples; i++) {
        values[i] = (uint32_t)prng30_generate(&prng, 32);
    }
    
    // Count collisions
    int collisions = 0;
    for (int i = 0; i < samples; i++) {
        for (int j = i + 1; j < samples; j++) {
            if (values[i] == values[j]) {
                collisions++;
            }
        }
    }
    
    printf("Collisions in %d samples: %d\n", samples, collisions);
    
    // For 32-bit values, expect very few collisions
    assert_test("Very few collisions in 32-bit space", collisions < 5);
    
    prng30_free(&prng);
}

// Test 12: Edge cases
void test_edge_cases() {
    print_test_header("Edge Cases");
    
    prng30_state prng;
    
    // Test with seed = 0
    prng30_init(&prng, 0, 64);
    uint64_t r1 = prng30_generate(&prng, 32);
    assert_test("Works with seed = 0", r1 != 0);
    prng30_free(&prng);
    
    // Test with seed = MAX
    prng30_init(&prng, UINT64_MAX, 64);
    uint64_t r2 = prng30_generate(&prng, 32);
    assert_test("Works with seed = UINT64_MAX", r2 > 0);
    prng30_free(&prng);
    
    // Test generating 0 bits
    prng30_init(&prng, 123, 64);
    uint64_t r3 = prng30_generate(&prng, 0);
    assert_test("0 bits returns 0", r3 == 0);
    prng30_free(&prng);
}

void print_summary() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════╗\n");
    printf("║              TEST SUITE SUMMARY                    ║\n");
    printf("╠════════════════════════════════════════════════════╣\n");
    printf("║  " COLOR_GREEN "Passed: %3d" COLOR_RESET "                                       ║\n", tests_passed);
    printf("║  " COLOR_RED "Failed: %3d" COLOR_RESET "                                       ║\n", tests_failed);
    printf("║  Total:  %3d                                       ║\n", tests_passed + tests_failed);
    printf("╠════════════════════════════════════════════════════╣\n");
    
    if (tests_failed == 0) {
        printf("║  " COLOR_GREEN "STATUS: ALL TESTS PASSED ✓" COLOR_RESET "                        ║\n");
    } else {
        printf("║  " COLOR_RED "STATUS: SOME TESTS FAILED ✗" COLOR_RESET "                    ║\n");
    }
    
    printf("╚════════════════════════════════════════════════════╝\n");
}

int main() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════╗\n");
    printf("║        PRNG30 Comprehensive Test Suite             ║\n");
    printf("║    Rule 30 Cellular Automaton PRNG Testing         ║\n");
    printf("╚════════════════════════════════════════════════════╝\n");
    
    // Run all tests
    test_initialization();
    test_determinism();
    test_seed_variation();
    test_chi_squared();
    test_bit_distribution();
    test_runs();
    test_autocorrelation();
    test_bit_widths();
    test_grid_sizes();
    test_non_zero();
    test_birthday_spacing();
    test_edge_cases();
    
    // Print summary
    print_summary();
    
    return (tests_failed == 0) ? 0 : 1;
}