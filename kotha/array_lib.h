/*
 * Kotha Array Library Header
 * Dynamic array utilities
 */

#ifndef ARRAY_LIB_H
#define ARRAY_LIB_H

/* Array structure */
typedef struct {
    int *data;
    int length;
    int capacity;
} IntArray;

/* Creation and destruction */
IntArray* kotha_array_create(int capacity);
void kotha_array_free(IntArray *arr);

/* Basic operations */
int kotha_array_length(IntArray *arr);
int kotha_array_get(IntArray *arr, int index);
void kotha_array_set(IntArray *arr, int index, int value);
void kotha_array_clear(IntArray *arr);

/* Stack operations */
void kotha_array_push(IntArray *arr, int value);
int kotha_array_pop(IntArray *arr);
int kotha_array_shift(IntArray *arr);
void kotha_array_unshift(IntArray *arr, int value);

/* Search */
int kotha_array_find(IntArray *arr, int value);
int kotha_array_findindex(IntArray *arr, int value);
int kotha_array_includes(IntArray *arr, int value);
int kotha_array_count(IntArray *arr, int value);

/* Transform */
IntArray* kotha_array_slice(IntArray *arr, int start, int end);
IntArray* kotha_array_concat(IntArray *arr1, IntArray *arr2);
IntArray* kotha_array_reverse(IntArray *arr);
IntArray* kotha_array_copy(IntArray *arr);

/* Sort */
void kotha_array_sort(IntArray *arr);
void kotha_array_sort_desc(IntArray *arr);

/* Utility */
void kotha_array_print(IntArray *arr);
char* kotha_array_join(IntArray *arr, const char *separator);
int kotha_array_sum(IntArray *arr);
double kotha_array_average(IntArray *arr);
int kotha_array_min(IntArray *arr);
int kotha_array_max(IntArray *arr);

/* Advanced */
void kotha_array_fill(IntArray *arr, int value);
IntArray* kotha_array_filter_greater(IntArray *arr, int threshold);
IntArray* kotha_array_filter_less(IntArray *arr, int threshold);
void kotha_array_remove_at(IntArray *arr, int index);
void kotha_array_insert_at(IntArray *arr, int index, int value);

#endif /* ARRAY_LIB_H */
