/*
 * Kotha Intermediate Representation (IR) Generator
 * Translates AST to 3-Address Code
 */

#include "ir.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

IRInstr *ir_head = NULL;
IRInstr *ir_tail = NULL;

static int temp_count = 0;
static int label_count = 0;

/* Helper: Generate IR for expressions and return result temp/var */
static char* ir_gen_expr(ASTNode *node);

void ir_init() {
    ir_head = NULL;
    ir_tail = NULL;
    temp_count = 0;
    label_count = 0;
}

void ir_add(IROp op, const char *a1, const char *a2, const char *res) {
    IRInstr *instr = (IRInstr*)malloc(sizeof(IRInstr));
    instr->op = op;
    instr->arg1 = a1 ? strdup(a1) : NULL;
    instr->arg2 = a2 ? strdup(a2) : NULL;
    instr->result = res ? strdup(res) : NULL;
    instr->next = NULL;
    
    if (!ir_head) {
        ir_head = ir_tail = instr;
    } else {
        ir_tail->next = instr;
        ir_tail = instr;
    }
}

char* ir_new_temp() {
    char *temp = (char*)malloc(16);
    sprintf(temp, "t%d", temp_count++);
    return temp;
}

char* ir_new_label() {
    char *label = (char*)malloc(16);
    sprintf(label, "L%d", label_count++);
    return label;
}

/* Generate IR for expressions */
static char* ir_gen_expr(ASTNode *node) {
    if (!node) return NULL;
    
    switch (node->type) {
        case NODE_LITERAL_INT: {
            char val[32];
            sprintf(val, "%d", node->ival);
            char *temp = ir_new_temp();
            ir_add(IR_ASSIGN, val, NULL, temp);
            return temp;
        }
        
        case NODE_LITERAL_FLOAT: {
            char val[32];
            sprintf(val, "%f", node->fval);
            char *temp = ir_new_temp();
            ir_add(IR_ASSIGN, val, NULL, temp);
            return temp;
        }
        
        case NODE_LITERAL_STRING: {
            // Store string in temp directly? 
            // For now, simpler to just return the string (without quotes ideally)
            // But strict 3AC uses temps.
            // Let's create a temp that points to string literal
            char *temp = ir_new_temp();
            // Note: In real compiler, would add to string table here
            ir_add(IR_ASSIGN, node->sval, NULL, temp);
            return temp;
        }
        
        case NODE_VAR_REF:
            return strdup(node->sval);
            
        case NODE_BIN_OP: {
            char *t1 = ir_gen_expr(node->left);
            char *t2 = ir_gen_expr(node->right);
            char *res = ir_new_temp();
            
            IROp op = IR_NOP;
            
            /* Operator mapping using correct token values from parser.tab.h:
             * PLUS=291, MINUS=292, MULT=293, DIV=294, MOD=295
             * GT=297, LT=298, GTE=299, LTE=300, EQ=301, NEQ=302
             * Also support ASCII values for basic operators
             */
            if (node->op == 291 || node->op == '+') op = IR_ADD;
            else if (node->op == 292 || node->op == '-') op = IR_SUB;
            else if (node->op == 293 || node->op == '*') op = IR_MUL;
            else if (node->op == 294 || node->op == '/') op = IR_DIV;
            else if (node->op == 295 || node->op == '%') op = IR_MOD;
            else if (node->op == 297 || node->op == '>') op = IR_GT;
            else if (node->op == 298 || node->op == '<') op = IR_LT;
            else if (node->op == 299) op = IR_GTE; // >=
            else if (node->op == 300) op = IR_LTE; // <=
            else if (node->op == 301) op = IR_EQ;  // ==
            else if (node->op == 302) op = IR_NEQ; // !=
            else {
                // Default fallback - should not reach here
                fprintf(stderr, "Warning: Unknown operator %d, defaulting to ADD\n", node->op);
                op = IR_ADD;
            }
            
            ir_add(op, t1, t2, res);
            free(t1); free(t2);
            return res;
        }
        
        case NODE_FUNC_CALL: {
            // Generate IR_PARAM for each argument
            ASTNode *arg = node->params;  // Function arguments
            int arg_count = 0;
            while (arg) {
                char *arg_val = ir_gen_expr(arg);
                if (arg_val) {
                    ir_add(IR_PARAM, arg_val, NULL, NULL);
                    free(arg_val);
                    arg_count++;
                }
                arg = arg->next;
            }
            
            // Generate IR_CALL
            char *result = ir_new_temp();
            char arg_count_str[16];
            sprintf(arg_count_str, "%d", arg_count);
            ir_add(IR_CALL, node->sval, arg_count_str, result);
            return result;
        }
        
        default:
            return NULL;
    }
}

/* Generate IR for statements */
void ir_generate(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_BLOCK:
            if (node->body) ir_generate(node->body);
            if (node->next) ir_generate(node->next);
            break;
            
        case NODE_VAR_DECL:
        case NODE_ASSIGN: {
            char *val = ir_gen_expr(node->left);
            ir_add(IR_ASSIGN, val, NULL, node->sval);
            if (val) free(val);
            
            if (node->next) ir_generate(node->next);
            break;
        }
        
        case NODE_PRINT: {
            char *val = ir_gen_expr(node->left); // Expression to print
            ir_add(IR_PRINT, val, NULL, NULL);
            if (val) free(val);
            
            if (node->next) ir_generate(node->next);
            break;
        }
        
        case NODE_IF: {
            char *cond = ir_gen_expr(node->cond);
            char *L_else = ir_new_label();
            char *L_end = ir_new_label();
            
            ir_add(IR_IF_FALSE, cond, NULL, L_else);
            free(cond);
            
            ir_generate(node->body);
            ir_add(IR_GOTO, NULL, NULL, L_end);
            
            ir_add(IR_LABEL, NULL, NULL, L_else);
            ir_add(IR_LABEL, NULL, NULL, L_else);
            if (node->catch_body) { // handle else/catch logic
                ir_generate(node->catch_body);
            }
            
            ir_add(IR_LABEL, NULL, NULL, L_end);
            
            if (node->next) ir_generate(node->next);
            break;
        }
        
        case NODE_WHILE: {
            char *L_start = ir_new_label();
            char *L_end = ir_new_label();
            
            ir_add(IR_LABEL, NULL, NULL, L_start);
            
            char *cond = ir_gen_expr(node->cond);
            ir_add(IR_IF_FALSE, cond, NULL, L_end);
            free(cond);
            
            ir_generate(node->body);
            ir_add(IR_GOTO, NULL, NULL, L_start);
            
            ir_add(IR_LABEL, NULL, NULL, L_end);
            
            free(L_start); free(L_end);
            if (node->next) ir_generate(node->next);
            break;
        }
        
        case NODE_RETURN: {
            char *val = ir_gen_expr(node->left);
            ir_add(IR_RETURN, val, NULL, NULL);
            if (val) free(val);
            break;
        }
        
        case NODE_UN_OP: {
            // Handle increment (++) and decrement (--)
            if (node->op == 306) { // INC token value
                // i++ becomes: i = i + 1
                char *temp = ir_new_temp();
                ir_add(IR_ASSIGN, "1", NULL, temp);
                char *result = ir_new_temp();
                ir_add(IR_ADD, node->sval, temp, result);
                ir_add(IR_ASSIGN, result, NULL, node->sval);
                free(temp);
                free(result);
            } else if (node->op == 307) { // DEC token value
                // i-- becomes: i = i - 1
                char *temp = ir_new_temp();
                ir_add(IR_ASSIGN, "1", NULL, temp);
                char *result = ir_new_temp();
                ir_add(IR_SUB, node->sval, temp, result);
                ir_add(IR_ASSIGN, result, NULL, node->sval);
                free(temp);
                free(result);
            }
            
            if (node->next) ir_generate(node->next);
            break;
        }
        
        case NODE_FUNC_DECL: {
            // Generate function label
            char func_label[256];
            snprintf(func_label, sizeof(func_label), "func_%s", node->sval);
            ir_add(IR_LABEL, NULL, NULL, func_label);
            
            // Generate function body
            if (node->body) {
                ir_generate(node->body);
            }
            
            // Add implicit return if not present
            // (In a more complete implementation, check if last statement is return)
            ir_add(IR_RETURN, NULL, NULL, NULL);
            
            // Process next function/statement
            if (node->next) ir_generate(node->next);
            break;
        }
        
        case NODE_FUNC_CALL: {
            // Function call as statement (not expression)
            char *result = ir_gen_expr(node);
            if (result) free(result);
            
            if (node->next) ir_generate(node->next);
            break;
        }
        
        default:
            // Just traverse children
             if (node->left) ir_generate(node->left);
             if (node->right) ir_generate(node->right);
             if (node->body) ir_generate(node->body);
             if (node->next) ir_generate(node->next);
            break;
    }
}

void ir_print() {
    IRInstr *instr = ir_head;
    int i = 0;
    while (instr) {
        printf("%03d: ", i++);
        switch (instr->op) {
            case IR_ASSIGN:
                printf("%s = %s\n", instr->result, instr->arg1);
                break;
            case IR_ADD:
                printf("%s = %s + %s\n", instr->result, instr->arg1, instr->arg2);
                break;
            case IR_SUB:
                printf("%s = %s - %s\n", instr->result, instr->arg1, instr->arg2);
                break;
            case IR_MUL:
                printf("%s = %s * %s\n", instr->result, instr->arg1, instr->arg2);
                break;
            case IR_DIV:
                printf("%s = %s / %s\n", instr->result, instr->arg1, instr->arg2);
                break;
            case IR_PRINT:
                printf("PRINT %s\n", instr->arg1);
                break;
            case IR_LABEL:
                printf("%s:\n", instr->result);
                break;
            case IR_GOTO:
                printf("GOTO %s\n", instr->result);
                break;
            case IR_IF_FALSE:
                printf("IF_FALSE %s GOTO %s\n", instr->arg1, instr->result);
                break;
            case IR_EQ:
                printf("%s = %s == %s\n", instr->result, instr->arg1, instr->arg2);
                break;
            case IR_LT:
                printf("%s = %s < %s\n", instr->result, instr->arg1, instr->arg2);
                break;
            case IR_GT:
                printf("%s = %s > %s\n", instr->result, instr->arg1, instr->arg2);
                break;
            case IR_RETURN:
                printf("RETURN %s\n", instr->arg1);
                break;
            default:
                printf("OP_%d %s, %s, %s\n", instr->op, instr->arg1 ? instr->arg1 : "_", 
                       instr->arg2 ? instr->arg2 : "_", instr->result ? instr->result : "_");
                break;
        }
        instr = instr->next;
    }
}
