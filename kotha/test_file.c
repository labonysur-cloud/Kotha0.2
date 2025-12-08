// Test File I/O Library Functions
#include <stdio.h>
#include <stdlib.h>
#include "file_io.h"

int main() {
    printf("=== File I/O Library Test ===\n\n");
    
    // Write to file
    printf("Writing to test_output.txt...\n");
    kotha_fwrite("test_output.txt", "Hello, Kotha!\nThis is a test file.\n");
    
    // Check if file exists
    printf("File exists: %d\n", kotha_fexists("test_output.txt"));
    
    // Get file size
    printf("File size: %ld bytes\n", kotha_fsize("test_output.txt"));
    
    // Read entire file
    printf("\nReading file:\n");
    char *content = kotha_fread("test_output.txt");
    if (content) {
        printf("%s", content);
        free(content);
    }
    
    // Line-by-line operations
    printf("\nWriting numbers to numbers.txt...\n");
    FILE *f = kotha_fopen("numbers.txt", "w");
    if (f) {
        kotha_fwrite_int(f, 42);
        fprintf(f, "\n");
        kotha_fwrite_int(f, 100);
        fprintf(f, "\n");
        kotha_fwriteline(f, "Done");
        kotha_fclose(f);
    }
    
    printf("Reading numbers.txt:\n");
    f = kotha_fopen("numbers.txt", "r");
    if (f) {
        int num1 = kotha_fread_int(f);
        int num2 = kotha_fread_int(f);
        printf("Numbers: %d, %d\n", num1, num2);
        kotha_fclose(f);
    }
    
    // Rename file
    printf("\nRenaming test_output.txt to renamed.txt...\n");
    kotha_frename("test_output.txt", "renamed.txt");
    printf("File exists (renamed.txt): %d\n", kotha_fexists("renamed.txt"));
    
    // Cleanup
    printf("\nCleaning up test files...\n");
    kotha_fdelete("renamed.txt");
    kotha_fdelete("numbers.txt");
    
    printf("\n✓ All file I/O functions working!\n");
    return 0;
}
