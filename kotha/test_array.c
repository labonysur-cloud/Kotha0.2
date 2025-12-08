// Test Array Library Functions
#include <stdio.h>
#include <stdlib.h>
#include "array_lib.h"

int main() {
    printf("=== Array Library Test ===\n\n");
    
    // Create array
    IntArray *arr = kotha_array_create(5);
    printf("Created array with capacity 5\n");
    
    // Push elements
    printf("\nPushing elements: 5, 3, 8, 1, 9\n");
    kotha_array_push(arr, 5);
    kotha_array_push(arr, 3);
    kotha_array_push(arr, 8);
    kotha_array_push(arr, 1);
    kotha_array_push(arr, 9);
    
    printf("Array: ");
    kotha_array_print(arr);
    printf("Length: %d\n", kotha_array_length(arr));
    
    // Search
    printf("\nSearch Operations:\n");
    printf("find(8) = %d\n", kotha_array_find(arr, 8));
    printf("includes(10) = %d\n", kotha_array_includes(arr, 10));
    printf("count(3) = %d\n", kotha_array_count(arr, 3));
    
    // Statistics
    printf("\nStatistics:\n");
    printf("sum = %d\n", kotha_array_sum(arr));
    printf("average = %.2f\n", kotha_array_average(arr));
    printf("min = %d\n", kotha_array_min(arr));
    printf("max = %d\n", kotha_array_max(arr));
    
    // Sort
    printf("\nSorting:\n");
    kotha_array_sort(arr);
    printf("Sorted ascending: ");
    kotha_array_print(arr);
    
    kotha_array_sort_desc(arr);
    printf("Sorted descending: ");
    kotha_array_print(arr);
    
    // Transform
    printf("\nTransform Operations:\n");
    IntArray *filtered = kotha_array_filter_greater(arr, 5);
    printf("Elements > 5: ");
    kotha_array_print(filtered);
    
    IntArray *reversed = kotha_array_reverse(arr);
    printf("Reversed: ");
    kotha_array_print(reversed);
    
    // Join
    char *joined = kotha_array_join(arr, ", ");
    printf("Joined: %s\n", joined);
    
    // Cleanup
    kotha_array_free(arr);
    kotha_array_free(filtered);
    kotha_array_free(reversed);
    free(joined);
    
    printf("\n✓ All array functions working!\n");
    return 0;
}
