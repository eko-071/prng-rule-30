#include "framework.h"
#include <stdio.h>

int g_passed = 0;
int g_failed = 0;

int main(void) {
    printf("\nprng30 test suite\n");

    run_core_tests();
    run_statistical_tests();
    run_double_tests();

    printf("\n");
    printf("passed: %d\n", g_passed);
    printf("failed: %d\n", g_failed);
    printf("total:  %d\n", g_passed + g_failed);

    if (g_failed == 0)
        printf(COL_GREEN "\nall tests passed\n" COL_RESET);
    else
        printf(COL_RED "\n%d test(s) failed\n" COL_RESET, g_failed);

    printf("\n");
    return (g_failed == 0) ? 0 : 1;
}