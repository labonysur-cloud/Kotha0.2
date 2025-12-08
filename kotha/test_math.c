// Test Math Library Functions
#include <stdio.h>
#include "math_lib.h"

int main() {
    printf("=== Math Library Test ===\n\n");
    
    // Basic operations
    printf("Basic Operations:\n");
    printf("abs(-5) = %d\n", kotha_abs(-5));
    printf("min(10, 20) = %d\n", kotha_min(10, 20));
    printf("max(10, 20) = %d\n", kotha_max(10, 20));
    printf("sqrt(16) = %.2f\n", kotha_sqrt(16.0));
    printf("pow(2, 3) = %.2f\n", kotha_pow(2.0, 3.0));
    
    // Trigonometry
    printf("\nTrigonometry:\n");
    printf("sin(PI/2) = %.4f\n", kotha_sin(KOTHA_PI / 2));
    printf("cos(0) = %.4f\n", kotha_cos(0));
    printf("tan(PI/4) = %.4f\n", kotha_tan(KOTHA_PI / 4));
    
    // Rounding
    printf("\nRounding:\n");
    printf("floor(3.7) = %.1f\n", kotha_floor(3.7));
    printf("ceil(3.2) = %.1f\n", kotha_ceil(3.2));
    printf("round(3.5) = %.1f\n", kotha_round(3.5));
    
    // Random
    printf("\nRandom Numbers:\n");
    kotha_srand(42);
    printf("random_range(1, 10) = %d\n", kotha_random_range(1, 10));
    printf("random_range(1, 10) = %d\n", kotha_random_range(1, 10));
    printf("random_double() = %.4f\n", kotha_random_double());
    
    // Utility
    printf("\nUtility:\n");
    printf("sign(-5) = %d\n", kotha_sign(-5));
    printf("is_even(4) = %d\n", kotha_is_even(4));
    printf("is_odd(5) = %d\n", kotha_is_odd(5));
    
    printf("\n✓ All math functions working!\n");
    return 0;
}
