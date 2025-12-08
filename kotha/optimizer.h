/*
 * Kotha Optimizer Header
 */

#ifndef OPTIMIZER_H
#define OPTIMIZER_H


/* Optimization functions */
int try_fold_constants(const char* expr, char* result, int max_len);
int try_algebraic_simplify(const char* expr, char* result, int max_len);
int try_strength_reduction(const char* expr, char* result, int max_len);
int is_dead_code_after_return(const char* line);

/* Main optimization */
void optimize();
char* optimize_expr(const char* expr);

/* Statistics */
void print_opt_stats();
void reset_opt_stats();

/* Codegen cleanup */
void codegen_vm_cleanup();

#endif /* OPTIMIZER_H */
