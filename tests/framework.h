#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <stdio.h>

#define COL_GREEN "\033[32m"
#define COL_RED "\033[31m"
#define COL_BLUE "\033[34m"
#define COL_RESET "\033[0m"

extern int g_passed;
extern int g_failed;

static inline void test_header(const char *name) {
    printf("\n" COL_BLUE "%s" COL_RESET "\n", name);
}

static inline void check(const char *desc, int cond) {
    if (cond) {
        printf(COL_GREEN "  PASS" COL_RESET "  %s\n", desc);
        g_passed++;
    } else {
        printf(COL_RED "  FAIL" COL_RESET "  %s\n", desc);
        g_failed++;
    }
}

void run_core_tests(void);
void run_statistical_tests(void);
void run_double_tests(void);

#endif