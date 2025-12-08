/*
 * Kotha String Library Implementation
 */

#include "string_lib.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* String length */
int kotha_strlen(const char *str) {
    if (!str) return 0;
    return strlen(str);
}

/* String concatenation */
char* kotha_strcat(char *dest, const char *src) {
    if (!dest || !src) return dest;
    return strcat(dest, src);
}

/* String comparison */
int kotha_strcmp(const char *str1, const char *str2) {
    if (!str1 || !str2) return -1;
    return strcmp(str1, str2);
}

/* String copy */
char* kotha_strcpy(char *dest, const char *src) {
    if (!dest || !src) return dest;
    return strcpy(dest, src);
}

/* Substring */
char* kotha_substr(const char *str, int start, int length) {
    if (!str) return NULL;
    
    int len = strlen(str);
    if (start < 0 || start >= len) return NULL;
    
    if (start + length > len) {
        length = len - start;
    }
    
    char *result = (char*)malloc(length + 1);
    if (!result) return NULL;
    
    strncpy(result, str + start, length);
    result[length] = '\0';
    
    return result;
}

/* String to uppercase */
char* kotha_toupper(char *str) {
    if (!str) return NULL;
    
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
    
    return str;
}

/* String to lowercase */
char* kotha_tolower(char *str) {
    if (!str) return NULL;
    
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
    
    return str;
}

/* String reverse */
char* kotha_reverse(char *str) {
    if (!str) return NULL;
    
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
    
    return str;
}

/* String contains */
int kotha_contains(const char *str, const char *substr) {
    if (!str || !substr) return 0;
    return strstr(str, substr) != NULL;
}

/* String replace */
char* kotha_replace(const char *str, const char *old, const char *new) {
    if (!str || !old || !new) return NULL;
    
    char *result;
    int i, count = 0;
    int newlen = strlen(new);
    int oldlen = strlen(old);
    
    // Count occurrences
    for (i = 0; str[i] != '\0'; i++) {
        if (strstr(&str[i], old) == &str[i]) {
            count++;
            i += oldlen - 1;
        }
    }
    
    // Allocate result
    result = (char*)malloc(i + count * (newlen - oldlen) + 1);
    if (!result) return NULL;
    
    i = 0;
    while (*str) {
        if (strstr(str, old) == str) {
            strcpy(&result[i], new);
            i += newlen;
            str += oldlen;
        } else {
            result[i++] = *str++;
        }
    }
    
    result[i] = '\0';
    return result;
}

/* String splitting */
char** kotha_split(const char *str, const char *delimiter, int *count) {
    if (!str || !delimiter || !count) return NULL;
    
    *count = 0;
    int capacity = 10;
    char **result = (char**)malloc(capacity * sizeof(char*));
    if (!result) return NULL;
    
    char *str_copy = strdup(str);
    char *token = strtok(str_copy, delimiter);
    
    while (token) {
        if (*count >= capacity) {
            capacity *= 2;
            char **new_result = (char**)realloc(result, capacity * sizeof(char*));
            if (!new_result) {
                for (int i = 0; i < *count; i++) free(result[i]);
                free(result);
                free(str_copy);
                return NULL;
            }
            result = new_result;
        }
        
        result[*count] = strdup(token);
        (*count)++;
        token = strtok(NULL, delimiter);
    }
    
    free(str_copy);
    return result;
}

/* String joining */
char* kotha_join(char **strings, int count, const char *separator) {
    if (!strings || count <= 0 || !separator) return NULL;
    
    int total_len = 0;
    for (int i = 0; i < count; i++) {
        if (strings[i]) total_len += strlen(strings[i]);
    }
    total_len += (count - 1) * strlen(separator) + 1;
    
    char *result = (char*)malloc(total_len);
    if (!result) return NULL;
    
    result[0] = '\0';
    for (int i = 0; i < count; i++) {
        if (strings[i]) {
            strcat(result, strings[i]);
            if (i < count - 1) strcat(result, separator);
        }
    }
    
    return result;
}

/* String trimming */
char* kotha_trim(char *str) {
    if (!str) return NULL;
    kotha_ltrim(str);
    kotha_rtrim(str);
    return str;
}

char* kotha_ltrim(char *str) {
    if (!str) return NULL;
    
    int i = 0;
    while (str[i] && isspace(str[i])) i++;
    
    if (i > 0) {
        int j = 0;
        while (str[i]) str[j++] = str[i++];
        str[j] = '\0';
    }
    
    return str;
}

char* kotha_rtrim(char *str) {
    if (!str) return NULL;
    
    int len = strlen(str);
    while (len > 0 && isspace(str[len - 1])) {
        str[--len] = '\0';
    }
    
    return str;
}

/* String checking */
int kotha_startswith(const char *str, const char *prefix) {
    if (!str || !prefix) return 0;
    
    int str_len = strlen(str);
    int prefix_len = strlen(prefix);
    
    if (prefix_len > str_len) return 0;
    
    return strncmp(str, prefix, prefix_len) == 0;
}

int kotha_endswith(const char *str, const char *suffix) {
    if (!str || !suffix) return 0;
    
    int str_len = strlen(str);
    int suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) return 0;
    
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

/* String searching */
int kotha_indexof(const char *str, const char *substr) {
    if (!str || !substr) return -1;
    
    char *pos = strstr(str, substr);
    if (!pos) return -1;
    
    return pos - str;
}

int kotha_lastindexof(const char *str, const char *substr) {
    if (!str || !substr) return -1;
    
    char *last = NULL;
    char *pos = strstr(str, substr);
    
    while (pos) {
        last = pos;
        pos = strstr(pos + 1, substr);
    }
    
    if (!last) return -1;
    return last - str;
}

/* String repetition */
char* kotha_repeat(const char *str, int count) {
    if (!str || count <= 0) return NULL;
    
    int len = strlen(str);
    char *result = (char*)malloc(len * count + 1);
    if (!result) return NULL;
    
    result[0] = '\0';
    for (int i = 0; i < count; i++) {
        strcat(result, str);
    }
    
    return result;
}
