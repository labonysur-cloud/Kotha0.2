/*
 * Kotha String Library Header
 * Provides string manipulation functions
 */

#ifndef STRING_LIB_H
#define STRING_LIB_H

/* String length */
int kotha_strlen(const char *str);

/* String concatenation */
char* kotha_strcat(char *dest, const char *src);

/* String comparison */
int kotha_strcmp(const char *str1, const char *str2);

/* String copy */
char* kotha_strcpy(char *dest, const char *src);

/* Substring */
char* kotha_substr(const char *str, int start, int length);

/* String to uppercase */
char* kotha_toupper(char *str);

/* String to lowercase */
char* kotha_tolower(char *str);

/* String reverse */
char* kotha_reverse(char *str);

/* String contains */
int kotha_contains(const char *str, const char *substr);

/* String replace */
char* kotha_replace(const char *str, const char *old, const char *new);

/* String splitting */
char** kotha_split(const char *str, const char *delimiter, int *count);

/* String joining */
char* kotha_join(char **strings, int count, const char *separator);

/* String trimming */
char* kotha_trim(char *str);
char* kotha_ltrim(char *str);
char* kotha_rtrim(char *str);

/* String checking */
int kotha_startswith(const char *str, const char *prefix);
int kotha_endswith(const char *str, const char *suffix);

/* String searching */
int kotha_indexof(const char *str, const char *substr);
int kotha_lastindexof(const char *str, const char *substr);

/* String repetition */
char* kotha_repeat(const char *str, int count);

#endif /* STRING_LIB_H */
