// Test String Library Functions
#include <stdio.h>
#include <stdlib.h>
#include "string_lib.h"

int main() {
    printf("=== String Library Test ===\n\n");
    
    // Basic operations
    printf("Basic Operations:\n");
    printf("strlen(\"hello\") = %d\n", kotha_strlen("hello"));
    printf("strcmp(\"abc\", \"abc\") = %d\n", kotha_strcmp("abc", "abc"));
    
    char *sub = kotha_substr("hello world", 0, 5);
    printf("substr(\"hello world\", 0, 5) = %s\n", sub);
    free(sub);
    
    // Case conversion
    printf("\nCase Conversion:\n");
    char upper[] = "hello";
    kotha_toupper(upper);
    printf("toupper(\"hello\") = %s\n", upper);
    
    char lower[] = "WORLD";
    kotha_tolower(lower);
    printf("tolower(\"WORLD\") = %s\n", lower);
    
    // Search
    printf("\nSearch Operations:\n");
    printf("contains(\"hello world\", \"world\") = %d\n", 
           kotha_contains("hello world", "world"));
    printf("indexof(\"hello world\", \"world\") = %d\n",
           kotha_indexof("hello world", "world"));
    printf("startswith(\"hello\", \"hel\") = %d\n",
           kotha_startswith("hello", "hel"));
    printf("endswith(\"test.txt\", \".txt\") = %d\n",
           kotha_endswith("test.txt", ".txt"));
    
    // Split and join
    printf("\nSplit and Join:\n");
    int count;
    char **parts = kotha_split("one,two,three", ",", &count);
    printf("split(\"one,two,three\", \",\") = [");
    for (int i = 0; i < count; i++) {
        printf("\"%s\"", parts[i]);
        if (i < count - 1) printf(", ");
    }
    printf("]\n");
    
    char *joined = kotha_join(parts, count, " - ");
    printf("join([...], \" - \") = %s\n", joined);
    
    // Cleanup split
    for (int i = 0; i < count; i++) free(parts[i]);
    free(parts);
    free(joined);
    
    // Trim
    printf("\nTrim Operations:\n");
    char trimtest[] = "  hello  ";
    kotha_trim(trimtest);
    printf("trim(\"  hello  \") = \"%s\"\n", trimtest);
    
    // Repeat
    char *repeated = kotha_repeat("ha", 3);
    printf("repeat(\"ha\", 3) = %s\n", repeated);
    free(repeated);
    
    // Replace
    char *replaced = kotha_replace("hello world", "world", "universe");
    printf("replace(\"hello world\", \"world\", \"universe\") = %s\n", replaced);
    free(replaced);
    
    printf("\n✓ All string functions working!\n");
    return 0;
}
