/*
 * Kotha Interpreter
 * Direct AST execution without compilation
 */

#include "interp.h"
#include "ast.h"
#include "symtab.h"
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INTERP_VARS 256

/* Runtime variable storage */
typedef struct {
    char *name;
    int int_value;
    float float_value;
    char *string_value;
    VarType type;
} InterpVar;

static InterpVar interp_vars[MAX_INTERP_VARS];
static int interp_var_count = 0;

/* Return value for functions */
static int return_value = 0;
static int has_returned = 0;

/* Initialize interpreter */
void interp_init() {
    interp_var_count = 0;
    has_returned = 0;
    return_value = 0;
    memset(interp_vars, 0, sizeof(interp_vars));
}

/* Get or create variable */
static InterpVar* get_or_create_var(const char *name) {
    if (!name) return NULL;
    
    // Check if variable exists
    for (int i = 0; i < interp_var_count; i++) {
        if (interp_vars[i].name && strcmp(interp_vars[i].name, name) == 0) {
            return &interp_vars[i];
        }
    }
    
    // Create new variable
    if (interp_var_count >= MAX_INTERP_VARS) {
        fprintf(stderr, "Interpreter Error: Too many variables\n");
        return NULL;
    }
    
    InterpVar *var = &interp_vars[interp_var_count++];
    var->name = strdup(name);
    var->type = TYPE_INT;
    var->int_value = 0;
    var->float_value = 0.0f;
    var->string_value = NULL;
    
    return var;
}

/* Evaluate expression and return integer value */
static int eval_expr(ASTNode *node) {
    if (!node) return 0;
    
    switch (node->type) {
        case NODE_LITERAL_INT:
            return node->ival;
        
        case NODE_LITERAL_FLOAT:
            return (int)node->fval;
        
        case NODE_VAR_REF: {
            InterpVar *var = get_or_create_var(node->sval);
            if (var) {
                return var->int_value;
            }
            return 0;
        }
        
        case NODE_BIN_OP: {
            int left = eval_expr(node->left);
            int right = eval_expr(node->right);
            
            switch (node->op) {
                case PLUS:  return left + right;
                case MINUS: return left - right;
                case MULT:  return left * right;
                case DIV:   
                    if (right == 0) {
                        fprintf(stderr, "Interpreter Error: Division by zero\n");
                        return 0;
                    }
                    return left / right;
                case MOD:   
                    if (right == 0) {
                        fprintf(stderr, "Interpreter Error: Modulo by zero\n");
                        return 0;
                    }
                    return left % right;
                case GT:    return left > right ? 1 : 0;
                case LT:    return left < right ? 1 : 0;
                case GTE:   return left >= right ? 1 : 0;
                case LTE:   return left <= right ? 1 : 0;
                case EQ:    return left == right ? 1 : 0;
                case NEQ:   return left != right ? 1 : 0;
                case AND:   return left && right ? 1 : 0;
                case OR:    return left || right ? 1 : 0;
                default:
                    fprintf(stderr, "Interpreter Error: Unknown binary operator %d\n", node->op);
                    return 0;
            }
        }
        
        case NODE_UN_OP: {
            int operand = eval_expr(node->left);
            switch (node->op) {
                case NOT:   return !operand ? 1 : 0;
                case MINUS: return -operand;
                default:
                    fprintf(stderr, "Interpreter Error: Unknown unary operator %d\n", node->op);
                    return 0;
            }
        }
        
        case NODE_FUNC_CALL:
            // Simplified: function calls not fully supported
            fprintf(stderr, "Interpreter Warning: Function calls not yet supported\n");
            return 0;
        
        default:
            fprintf(stderr, "Interpreter Error: Cannot evaluate node type %d\n", node->type);
            return 0;
    }
}

/* Execute statement */
static void exec_stmt(ASTNode *node) {
    if (!node || has_returned) return;
    
    switch (node->type) {
        case NODE_VAR_DECL: {
            // dhoro x = expr;
            InterpVar *var = get_or_create_var(node->sval);
            if (var && node->left) {
                var->int_value = eval_expr(node->left);
                var->type = TYPE_INT;
            }
            break;
        }
        
        case NODE_ASSIGN: {
            // x = expr;
            InterpVar *var = get_or_create_var(node->sval);
            if (var && node->left) {
                var->int_value = eval_expr(node->left);
            }
            break;
        }
        
        case NODE_PRINT: {
            // dekhaw(expr);
            if (node->left) {
                if (node->left->type == NODE_LITERAL_STRING) {
                    // Print string (remove quotes)
                    char *str = node->left->sval;
                    if (str && strlen(str) >= 2) {
                        // Remove surrounding quotes
                        char *unquoted = strndup(str + 1, strlen(str) - 2);
                        printf("%s\n", unquoted);
                        free(unquoted);
                    }
                } else {
                    // Print integer
                    int value = eval_expr(node->left);
                    printf("%d\n", value);
                }
            }
            break;
        }
        
        case NODE_IF: {
            // jodi (cond) { body }
            if (node->cond) {
                int condition = eval_expr(node->cond);
                if (condition) {
                    exec_stmt(node->body);
                } else if (node->catch_body) {
                    exec_stmt(node->catch_body);
                }
            }
            break;
        }
        
        case NODE_WHILE: {
            // jotokkhon (cond) { body }
            while (node->cond && eval_expr(node->cond) && !has_returned) {
                exec_stmt(node->body);
            }
            break;
        }
        
        case NODE_FOR: {
            // cholbe (i theke start porjonto end) { body }
            if (node->sval && node->left && node->right) {
                InterpVar *loop_var = get_or_create_var(node->sval);
                if (loop_var) {
                    int start = eval_expr(node->left);
                    int end = eval_expr(node->right);
                    
                    for (loop_var->int_value = start; 
                         loop_var->int_value <= end && !has_returned; 
                         loop_var->int_value++) {
                        exec_stmt(node->body);
                    }
                }
            }
            break;
        }
        
        case NODE_BLOCK: {
            // { statements }
            ASTNode *curr = node->body;
            while (curr && !has_returned) {
                exec_stmt(curr);
                curr = curr->next;
            }
            break;
        }
        
        case NODE_RETURN: {
            // ferot expr;
            if (node->left) {
                return_value = eval_expr(node->left);
            }
            has_returned = 1;
            break;
        }
        
        case NODE_TRY: {
            // try { body } catch { catch_body }
            // Simplified: just execute try block
            exec_stmt(node->body);
            break;
        }
        
        case NODE_THROW: {
            // throw expr;
            fprintf(stderr, "Interpreter: Exception thrown\n");
            // Simplified: just print message
            break;
        }
        
        default:
            // Execute next statement in sequence
            if (node->next) {
                exec_stmt(node->next);
            }
            break;
    }
}

/* Main interpreter entry point */
void interpret(ASTNode *root) {
    if (!root) {
        fprintf(stderr, "Interpreter Error: NULL AST root\n");
        return;
    }
    
    // Initialize interpreter state
    interp_init();
    
    // Execute the AST
    exec_stmt(root);
}

/* Cleanup interpreter */
void interp_cleanup() {
    for (int i = 0; i < interp_var_count; i++) {
        if (interp_vars[i].name) {
            free(interp_vars[i].name);
            interp_vars[i].name = NULL;
        }
        if (interp_vars[i].string_value) {
            free(interp_vars[i].string_value);
            interp_vars[i].string_value = NULL;
        }
    }
    interp_var_count = 0;
}

/* Debug: Print interpreter state */
void interp_print_vars() {
    printf("=== Interpreter Variables ===\n");
    for (int i = 0; i < interp_var_count; i++) {
        printf("%s = %d\n", interp_vars[i].name, interp_vars[i].int_value);
    }
    printf("=============================\n");
}
