/*
 * Kotha Math Library Header
 * Comprehensive mathematical functions
 */

#ifndef MATH_LIB_H
#define MATH_LIB_H

/* Constants */
#define KOTHA_PI 3.14159265358979323846
#define KOTHA_E  2.71828182845904523536

/* Basic operations */
int kotha_abs(int x);
double kotha_fabs(double x);
int kotha_min(int a, int b);
int kotha_max(int a, int b);
double kotha_fmin(double a, double b);
double kotha_fmax(double a, double b);
double kotha_pow(double base, double exp);
double kotha_sqrt(double x);

/* Trigonometry (angles in radians) */
double kotha_sin(double x);
double kotha_cos(double x);
double kotha_tan(double x);
double kotha_asin(double x);
double kotha_acos(double x);
double kotha_atan(double x);
double kotha_atan2(double y, double x);

/* Rounding */
double kotha_floor(double x);
double kotha_ceil(double x);
double kotha_round(double x);
double kotha_trunc(double x);

/* Logarithms and exponentials */
double kotha_log(double x);
double kotha_log10(double x);
double kotha_log2(double x);
double kotha_exp(double x);

/* Random number generation */
void kotha_srand(unsigned int seed);
int kotha_rand(void);
int kotha_random_range(int min, int max);
double kotha_random_double(void);

/* Utility */
int kotha_sign(int x);
double kotha_fsign(double x);
int kotha_is_even(int x);
int kotha_is_odd(int x);

#endif /* MATH_LIB_H */
