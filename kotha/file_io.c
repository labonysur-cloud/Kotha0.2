/*
 * Kotha File I/O Library Implementation
 */

#include "file_io.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* File open */
FILE* kotha_fopen(const char *filename, const char *mode) {
    if (!filename || !mode) return NULL;
    return fopen(filename, mode);
}

/* File close */
int kotha_fclose(FILE *file) {
    if (!file) return -1;
    return fclose(file);
}

/* Read entire file */
char* kotha_fread(const char *filename) {
    if (!filename) return NULL;
    
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate buffer
    char *content = (char*)malloc(size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }
    
    // Read file
    fread(content, 1, size, file);
    content[size] = '\0';
    
    fclose(file);
    return content;
}

/* Read line from file */
char* kotha_freadline(FILE *file) {
    if (!file) return NULL;
    
    char *line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, file);
    
    if (read == -1) {
        free(line);
        return NULL;
    }
    
    return line;
}

/* Read integer from file */
int kotha_fread_int(FILE *file) {
    if (!file) return 0;
    
    int value;
    if (fscanf(file, "%d", &value) == 1) {
        return value;
    }
    return 0;
}

/* Write to file */
int kotha_fwrite(const char *filename, const char *content) {
    if (!filename || !content) return -1;
    
    FILE *file = fopen(filename, "w");
    if (!file) return -1;
    
    int result = fprintf(file, "%s", content);
    fclose(file);
    
    return result;
}

/* Write line to file */
int kotha_fwriteline(FILE *file, const char *line) {
    if (!file || !line) return -1;
    return fprintf(file, "%s\n", line);
}

/* Write integer to file */
int kotha_fwrite_int(FILE *file, int value) {
    if (!file) return -1;
    return fprintf(file, "%d", value);
}

/* Check if file exists */
int kotha_fexists(const char *filename) {
    if (!filename) return 0;
    
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

/* Get file size */
long kotha_fsize(const char *filename) {
    if (!filename) return -1;
    
    struct stat buffer;
    if (stat(filename, &buffer) != 0) return -1;
    
    return buffer.st_size;
}

/* Delete file */
int kotha_fdelete(const char *filename) {
    if (!filename) return -1;
    return remove(filename);
}

/* Rename file */
int kotha_frename(const char *oldname, const char *newname) {
    if (!oldname || !newname) return -1;
    return rename(oldname, newname);
}
