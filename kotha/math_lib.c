/*
 * Kotha Math Library Implementation
 */

#include "math_lib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

/* Basic operations */
int kotha_abs(int x) {
    return abs(x);
}

double kotha_fabs(double x) {
    return fabs(x);
}

int kotha_min(int a, int b) {
    return (a < b) ? a : b;
}

int kotha_max(int a, int b) {
    return (a > b) ? a : b;
}

double kotha_fmin(double a, double b) {
    return fmin(a, b);
}

double kotha_fmax(double a, double b) {
    return fmax(a, b);
}

double kotha_pow(double base, double exp) {
    return pow(base, exp);
}

double kotha_sqrt(double x) {
    if (x < 0) return 0.0;  // Return 0 for negative numbers
    return sqrt(x);
}

/* Trigonometry */
double kotha_sin(double x) {
    return sin(x);
}

double kotha_cos(double x) {
    return cos(x);
}

double kotha_tan(double x) {
    return tan(x);
}

double kotha_asin(double x) {
    return asin(x);
}

double kotha_acos(double x) {
    return acos(x);
}

double kotha_atan(double x) {
    return atan(x);
}

double kotha_atan2(double y, double x) {
    return atan2(y, x);
}

/* Rounding */
double kotha_floor(double x) {
    return floor(x);
}

double kotha_ceil(double x) {
    return ceil(x);
}

double kotha_round(double x) {
    return round(x);
}

double kotha_trunc(double x) {
    return trunc(x);
}

/* Logarithms and exponentials */
double kotha_log(double x) {
    if (x <= 0) return 0.0;  // Return 0 for invalid input
    return log(x);
}

double kotha_log10(double x) {
    if (x <= 0) return 0.0;
    return log10(x);
}

double kotha_log2(double x) {
    if (x <= 0) return 0.0;
    return log2(x);
}

double kotha_exp(double x) {
    return exp(x);
}

/* Random number generation */
static int random_initialized = 0;

void kotha_srand(unsigned int seed) {
    srand(seed);
    random_initialized = 1;
}

int kotha_rand(void) {
    if (!random_initialized) {
        srand(time(NULL));
        random_initialized = 1;
    }
    return rand();
}

int kotha_random_range(int min, int max) {
    if (!random_initialized) {
        srand(time(NULL));
        random_initialized = 1;
    }
    
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;
    }
    
    return min + (rand() % (max - min + 1));
}

double kotha_random_double(void) {
    if (!random_initialized) {
        srand(time(NULL));
        random_initialized = 1;
    }
    return (double)rand() / (double)RAND_MAX;
}

/* Utility */
int kotha_sign(int x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}

double kotha_fsign(double x) {
    if (x > 0.0) return 1.0;
    if (x < 0.0) return -1.0;
    return 0.0;
}

int kotha_is_even(int x) {
    return (x % 2) == 0;
}

int kotha_is_odd(int x) {
    return (x % 2) != 0;
}
