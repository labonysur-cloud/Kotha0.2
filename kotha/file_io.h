/*
 * Kotha File I/O Library Header
 */

#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdio.h>

/* File operations */
FILE* kotha_fopen(const char *filename, const char *mode);
int kotha_fclose(FILE *file);

/* Read operations */
char* kotha_fread(const char *filename);
char* kotha_freadline(FILE *file);
int kotha_fread_int(FILE *file);

/* Write operations */
int kotha_fwrite(const char *filename, const char *content);
int kotha_fwriteline(FILE *file, const char *line);
int kotha_fwrite_int(FILE *file, int value);

/* File checks */
int kotha_fexists(const char *filename);
long kotha_fsize(const char *filename);

/* File utilities */
int kotha_fdelete(const char *filename);
int kotha_frename(const char *oldname, const char *newname);

#endif /* FILE_IO_H */
