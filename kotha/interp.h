/*
 * Kotha Interpreter Header
 * Executes AST directly
 */

#ifndef INTERP_H
#define INTERP_H

#include "ast.h"


/* Execute the AST */
void interpret(ASTNode *root);

/* Helper functions */
void interp_init();
void interp_cleanup();
void interp_print_vars();

#endif /* INTERP_H */
