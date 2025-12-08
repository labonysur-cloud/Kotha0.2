/*
 * Kotha Optimizer
 * Performs optimization passes on IR/AST
 */

#include "optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Optimization statistics */
static int const_folds = 0;
static int algebraic_simplifications = 0;
static int strength_reductions = 0;
static int dead_code_removals = 0;

/* Helper: Check if string is a number */
static int is_number(const char *str) {
    if (!str || !*str) return 0;
    
    int has_digit = 0;
    int i = 0;
    
    // Skip leading whitespace
    while (isspace(str[i])) i++;
    
    // Check for sign
    if (str[i] == '+' || str[i] == '-') i++;
    
    // Check digits
    while (str[i]) {
        if (isdigit(str[i])) {
            has_digit = 1;
        } else if (str[i] == '.' && has_digit) {
            // Allow decimal point
        } else if (!isspace(str[i])) {
            return 0;
        }
        i++;
    }
    
    return has_digit;
}

/* Helper: Parse integer from string */
static int parse_int(const char *str) {
    return atoi(str);
}

/* Helper: Extract operator and operands from expression */
static int parse_binary_expr(const char *expr, char *left, char *op, char *right) {
    if (!expr || !left || !op || !right) return 0;
    
    // Simple parser for "a OP b" format
    const char *p = expr;
    int i = 0;
    
    // Skip whitespace
    while (*p && isspace(*p)) p++;
    
    // Extract left operand
    while (*p && !isspace(*p) && strchr("+-*/%", *p) == NULL) {
        left[i++] = *p++;
    }
    left[i] = '\0';
    
    // Skip whitespace
    while (*p && isspace(*p)) p++;
    
    // Extract operator
    if (*p && strchr("+-*/%", *p)) {
        *op = *p++;
    } else {
        return 0;
    }
    
    // Skip whitespace
    while (*p && isspace(*p)) p++;
    
    // Extract right operand
    i = 0;
    while (*p && !isspace(*p)) {
        right[i++] = *p++;
    }
    right[i] = '\0';
    
    return (left[0] && *op && right[0]);
}

/* Constant folding: Evaluate constant expressions */
int try_fold_constants(const char* expr, char* result, int max_len) {
    if (!expr || !result || max_len <= 0) return 0;
    
    char left[256], right[256];
    char op;
    
    if (!parse_binary_expr(expr, left, &op, right)) {
        return 0;
    }
    
    // Check if both operands are numbers
    if (!is_number(left) || !is_number(right)) {
        return 0;
    }
    
    int a = parse_int(left);
    int b = parse_int(right);
    int res = 0;
    
    switch (op) {
        case '+': res = a + b; break;
        case '-': res = a - b; break;
        case '*': res = a * b; break;
        case '/': 
            if (b == 0) return 0;
            res = a / b;
            break;
        case '%':
            if (b == 0) return 0;
            res = a % b;
            break;
        default:
            return 0;
    }
    
    snprintf(result, max_len, "%d", res);
    const_folds++;
    return 1;
}

/* Algebraic simplification: x + 0 = x, x * 1 = x, etc. */
int try_algebraic_simplify(const char* expr, char* result, int max_len) {
    if (!expr || !result || max_len <= 0) return 0;
    
    char left[256], right[256];
    char op;
    
    if (!parse_binary_expr(expr, left, &op, right)) {
        return 0;
    }
    
    // x + 0 = x
    if (op == '+' && strcmp(right, "0") == 0) {
        snprintf(result, max_len, "%s", left);
        algebraic_simplifications++;
        return 1;
    }
    
    // 0 + x = x
    if (op == '+' && strcmp(left, "0") == 0) {
        snprintf(result, max_len, "%s", right);
        algebraic_simplifications++;
        return 1;
    }
    
    // x - 0 = x
    if (op == '-' && strcmp(right, "0") == 0) {
        snprintf(result, max_len, "%s", left);
        algebraic_simplifications++;
        return 1;
    }
    
    // x * 1 = x
    if (op == '*' && strcmp(right, "1") == 0) {
        snprintf(result, max_len, "%s", left);
        algebraic_simplifications++;
        return 1;
    }
    
    // 1 * x = x
    if (op == '*' && strcmp(left, "1") == 0) {
        snprintf(result, max_len, "%s", right);
        algebraic_simplifications++;
        return 1;
    }
    
    // x * 0 = 0
    if (op == '*' && (strcmp(right, "0") == 0 || strcmp(left, "0") == 0)) {
        snprintf(result, max_len, "0");
        algebraic_simplifications++;
        return 1;
    }
    
    // x / 1 = x
    if (op == '/' && strcmp(right, "1") == 0) {
        snprintf(result, max_len, "%s", left);
        algebraic_simplifications++;
        return 1;
    }
    
    // x - x = 0 (if both are identical identifiers)
    if (op == '-' && strcmp(left, right) == 0 && !is_number(left)) {
        snprintf(result, max_len, "0");
        algebraic_simplifications++;
        return 1;
    }
    
    return 0;
}

/* Strength reduction: x * 2 = x + x, x / 2 = x >> 1 */
int try_strength_reduction(const char* expr, char* result, int max_len) {
    if (!expr || !result || max_len <= 0) return 0;
    
    char left[256], right[256];
    char op;
    
    if (!parse_binary_expr(expr, left, &op, right)) {
        return 0;
    }
    
    // x * 2 = x + x
    if (op == '*' && strcmp(right, "2") == 0) {
        snprintf(result, max_len, "%s + %s", left, left);
        strength_reductions++;
        return 1;
    }
    
    // 2 * x = x + x
    if (op == '*' && strcmp(left, "2") == 0) {
        snprintf(result, max_len, "%s + %s", right, right);
        strength_reductions++;
        return 1;
    }
    
    // x / 2 = x >> 1 (for power of 2)
    if (op == '/' && strcmp(right, "2") == 0) {
        snprintf(result, max_len, "%s >> 1", left);
        strength_reductions++;
        return 1;
    }
    
    // x * 4 = x << 2
    if (op == '*' && strcmp(right, "4") == 0) {
        snprintf(result, max_len, "%s << 2", left);
        strength_reductions++;
        return 1;
    }
    
    // x * 8 = x << 3
    if (op == '*' && strcmp(right, "8") == 0) {
        snprintf(result, max_len, "%s << 3", left);
        strength_reductions++;
        return 1;
    }
    
    return 0;
}

/* Dead code detection: Check if code appears after return */
int is_dead_code_after_return(const char* line) {
    if (!line) return 0;
    
    // Simple heuristic: check if line contains "return"
    // This is a simplified version - real implementation would track control flow
    static int seen_return = 0;
    
    if (strstr(line, "return") || strstr(line, "ferot")) {
        seen_return = 1;
        return 0; // The return statement itself is not dead
    }
    
    if (seen_return && line[0] != '}' && strlen(line) > 0) {
        dead_code_removals++;
        return 1; // Code after return is dead
    }
    
    if (line[0] == '}') {
        seen_return = 0; // Reset at block end
    }
    
    return 0;
}

/* Print optimization statistics */
void print_opt_stats() {
    printf("=== Optimization Statistics ===\n");
    printf("Constant folds:           %d\n", const_folds);
    printf("Algebraic simplifications: %d\n", algebraic_simplifications);
    printf("Strength reductions:       %d\n", strength_reductions);
    printf("Dead code removals:        %d\n", dead_code_removals);
    printf("Total optimizations:       %d\n", 
           const_folds + algebraic_simplifications + 
           strength_reductions + dead_code_removals);
    printf("===============================\n");
}

/* Reset optimization statistics */
void reset_opt_stats() {
    const_folds = 0;
    algebraic_simplifications = 0;
    strength_reductions = 0;
    dead_code_removals = 0;
}

/* Main optimization entry point */
void optimize() {
    // This is a placeholder for the main optimization pass
    // In a full implementation, this would:
    // 1. Traverse the IR or AST
    // 2. Apply optimization passes
    // 3. Return optimized IR/AST
    
    printf("Optimizer: Running optimization passes...\n");
    
    // Example usage of optimization functions:
    char result[256];
    
    // Test constant folding
    if (try_fold_constants("2 + 3", result, 256)) {
        printf("Optimized: 2 + 3 => %s\n", result);
    }
    
    // Test algebraic simplification
    if (try_algebraic_simplify("x + 0", result, 256)) {
        printf("Optimized: x + 0 => %s\n", result);
    }
    
    // Test strength reduction
    if (try_strength_reduction("x * 2", result, 256)) {
        printf("Optimized: x * 2 => %s\n", result);
    }
}

/* Optimize a single expression */
char* optimize_expr(const char* expr) {
    if (!expr) return NULL;
    
    char result[1024];
    
    // Try optimizations in order
    if (try_fold_constants(expr, result, 1024)) {
        return strdup(result);
    }
    
    if (try_algebraic_simplify(expr, result, 1024)) {
        return strdup(result);
    }
    
    if (try_strength_reduction(expr, result, 1024)) {
        return strdup(result);
    }
    
    // No optimization applied
    return strdup(expr);
}
