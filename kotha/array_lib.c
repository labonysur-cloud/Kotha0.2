/*
 * Kotha Array Library Implementation
 */

#include "array_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_CAPACITY 10

/* Helper: Resize array */
static void resize_array(IntArray *arr, int new_capacity) {
    int *new_data = (int*)realloc(arr->data, new_capacity * sizeof(int));
    if (new_data) {
        arr->data = new_data;
        arr->capacity = new_capacity;
    }
}

/* Creation and destruction */
IntArray* kotha_array_create(int capacity) {
    if (capacity <= 0) capacity = INITIAL_CAPACITY;
    
    IntArray *arr = (IntArray*)malloc(sizeof(IntArray));
    if (!arr) return NULL;
    
    arr->data = (int*)malloc(capacity * sizeof(int));
    if (!arr->data) {
        free(arr);
        return NULL;
    }
    
    arr->length = 0;
    arr->capacity = capacity;
    return arr;
}

void kotha_array_free(IntArray *arr) {
    if (!arr) return;
    if (arr->data) free(arr->data);
    free(arr);
}

/* Basic operations */
int kotha_array_length(IntArray *arr) {
    return arr ? arr->length : 0;
}

int kotha_array_get(IntArray *arr, int index) {
    if (!arr || index < 0 || index >= arr->length) return 0;
    return arr->data[index];
}

void kotha_array_set(IntArray *arr, int index, int value) {
    if (!arr || index < 0 || index >= arr->length) return;
    arr->data[index] = value;
}

void kotha_array_clear(IntArray *arr) {
    if (!arr) return;
    arr->length = 0;
}

/* Stack operations */
void kotha_array_push(IntArray *arr, int value) {
    if (!arr) return;
    
    if (arr->length >= arr->capacity) {
        resize_array(arr, arr->capacity * 2);
    }
    
    arr->data[arr->length++] = value;
}

int kotha_array_pop(IntArray *arr) {
    if (!arr || arr->length == 0) return 0;
    return arr->data[--arr->length];
}

int kotha_array_shift(IntArray *arr) {
    if (!arr || arr->length == 0) return 0;
    
    int value = arr->data[0];
    for (int i = 0; i < arr->length - 1; i++) {
        arr->data[i] = arr->data[i + 1];
    }
    arr->length--;
    return value;
}

void kotha_array_unshift(IntArray *arr, int value) {
    if (!arr) return;
    
    if (arr->length >= arr->capacity) {
        resize_array(arr, arr->capacity * 2);
    }
    
    for (int i = arr->length; i > 0; i--) {
        arr->data[i] = arr->data[i - 1];
    }
    arr->data[0] = value;
    arr->length++;
}

/* Search */
int kotha_array_find(IntArray *arr, int value) {
    return kotha_array_findindex(arr, value);
}

int kotha_array_findindex(IntArray *arr, int value) {
    if (!arr) return -1;
    
    for (int i = 0; i < arr->length; i++) {
        if (arr->data[i] == value) return i;
    }
    return -1;
}

int kotha_array_includes(IntArray *arr, int value) {
    return kotha_array_findindex(arr, value) != -1;
}

int kotha_array_count(IntArray *arr, int value) {
    if (!arr) return 0;
    
    int count = 0;
    for (int i = 0; i < arr->length; i++) {
        if (arr->data[i] == value) count++;
    }
    return count;
}

/* Transform */
IntArray* kotha_array_slice(IntArray *arr, int start, int end) {
    if (!arr) return NULL;
    
    if (start < 0) start = 0;
    if (end > arr->length) end = arr->length;
    if (start >= end) return kotha_array_create(INITIAL_CAPACITY);
    
    int length = end - start;
    IntArray *result = kotha_array_create(length);
    if (!result) return NULL;
    
    for (int i = 0; i < length; i++) {
        kotha_array_push(result, arr->data[start + i]);
    }
    
    return result;
}

IntArray* kotha_array_concat(IntArray *arr1, IntArray *arr2) {
    if (!arr1 && !arr2) return NULL;
    if (!arr1) return kotha_array_copy(arr2);
    if (!arr2) return kotha_array_copy(arr1);
    
    IntArray *result = kotha_array_create(arr1->length + arr2->length);
    if (!result) return NULL;
    
    for (int i = 0; i < arr1->length; i++) {
        kotha_array_push(result, arr1->data[i]);
    }
    for (int i = 0; i < arr2->length; i++) {
        kotha_array_push(result, arr2->data[i]);
    }
    
    return result;
}

IntArray* kotha_array_reverse(IntArray *arr) {
    if (!arr) return NULL;
    
    IntArray *result = kotha_array_copy(arr);
    if (!result) return NULL;
    
    for (int i = 0; i < result->length / 2; i++) {
        int temp = result->data[i];
        result->data[i] = result->data[result->length - 1 - i];
        result->data[result->length - 1 - i] = temp;
    }
    
    return result;
}

IntArray* kotha_array_copy(IntArray *arr) {
    if (!arr) return NULL;
    
    IntArray *copy = kotha_array_create(arr->capacity);
    if (!copy) return NULL;
    
    memcpy(copy->data, arr->data, arr->length * sizeof(int));
    copy->length = arr->length;
    
    return copy;
}

/* Sort */
static int compare_asc(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

static int compare_desc(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

void kotha_array_sort(IntArray *arr) {
    if (!arr || arr->length == 0) return;
    qsort(arr->data, arr->length, sizeof(int), compare_asc);
}

void kotha_array_sort_desc(IntArray *arr) {
    if (!arr || arr->length == 0) return;
    qsort(arr->data, arr->length, sizeof(int), compare_desc);
}

/* Utility */
void kotha_array_print(IntArray *arr) {
    if (!arr) {
        printf("[]\n");
        return;
    }
    
    printf("[");
    for (int i = 0; i < arr->length; i++) {
        printf("%d", arr->data[i]);
        if (i < arr->length - 1) printf(", ");
    }
    printf("]\n");
}

char* kotha_array_join(IntArray *arr, const char *separator) {
    if (!arr || !separator) return NULL;
    
    // Estimate size (assuming max 11 chars per int + separator)
    int est_size = arr->length * (11 + strlen(separator)) + 1;
    char *result = (char*)malloc(est_size);
    if (!result) return NULL;
    
    result[0] = '\0';
    for (int i = 0; i < arr->length; i++) {
        char num[12];
        sprintf(num, "%d", arr->data[i]);
        strcat(result, num);
        if (i < arr->length - 1) strcat(result, separator);
    }
    
    return result;
}

int kotha_array_sum(IntArray *arr) {
    if (!arr) return 0;
    
    int sum = 0;
    for (int i = 0; i < arr->length; i++) {
        sum += arr->data[i];
    }
    return sum;
}

double kotha_array_average(IntArray *arr) {
    if (!arr || arr->length == 0) return 0.0;
    return (double)kotha_array_sum(arr) / arr->length;
}

int kotha_array_min(IntArray *arr) {
    if (!arr || arr->length == 0) return 0;
    
    int min = arr->data[0];
    for (int i = 1; i < arr->length; i++) {
        if (arr->data[i] < min) min = arr->data[i];
    }
    return min;
}

int kotha_array_max(IntArray *arr) {
    if (!arr || arr->length == 0) return 0;
    
    int max = arr->data[0];
    for (int i = 1; i < arr->length; i++) {
        if (arr->data[i] > max) max = arr->data[i];
    }
    return max;
}

/* Advanced */
void kotha_array_fill(IntArray *arr, int value) {
    if (!arr) return;
    for (int i = 0; i < arr->length; i++) {
        arr->data[i] = value;
    }
}

IntArray* kotha_array_filter_greater(IntArray *arr, int threshold) {
    if (!arr) return NULL;
    
    IntArray *result = kotha_array_create(arr->length);
    if (!result) return NULL;
    
    for (int i = 0; i < arr->length; i++) {
        if (arr->data[i] > threshold) {
            kotha_array_push(result, arr->data[i]);
        }
    }
    
    return result;
}

IntArray* kotha_array_filter_less(IntArray *arr, int threshold) {
    if (!arr) return NULL;
    
    IntArray *result = kotha_array_create(arr->length);
    if (!result) return NULL;
    
    for (int i = 0; i < arr->length; i++) {
        if (arr->data[i] < threshold) {
            kotha_array_push(result, arr->data[i]);
        }
    }
    
    return result;
}

void kotha_array_remove_at(IntArray *arr, int index) {
    if (!arr || index < 0 || index >= arr->length) return;
    
    for (int i = index; i < arr->length - 1; i++) {
        arr->data[i] = arr->data[i + 1];
    }
    arr->length--;
}

void kotha_array_insert_at(IntArray *arr, int index, int value) {
    if (!arr || index < 0 || index > arr->length) return;
    
    if (arr->length >= arr->capacity) {
        resize_array(arr, arr->capacity * 2);
    }
    
    for (int i = arr->length; i > index; i--) {
        arr->data[i] = arr->data[i - 1];
    }
    arr->data[index] = value;
    arr->length++;
}
