/*
 * Kotha Intermediate Representation (IR) Header
 * Defines 3-Address Code structures
 */

#ifndef IR_H
#define IR_H

#include "ast.h"

typedef enum {
    IR_NOP,
    IR_ASSIGN,      // result = arg1
    IR_ADD,         // result = arg1 + arg2
    IR_SUB,         // result = arg1 - arg2
    IR_MUL,         // result = arg1 * arg2
    IR_DIV,         // result = arg1 / arg2
    IR_MOD,         // result = arg1 % arg2
    IR_LABEL,       // label:
    IR_GOTO,        // goto result
    IR_IF_FALSE,    // if false arg1 goto result
    IR_PARAM,       // param arg1
    IR_CALL,        // result = call arg1, arg2 (num_args)
    IR_RETURN,      // return arg1
    IR_PRINT,       // print arg1
    IR_EQ,          // result = arg1 == arg2
    IR_NEQ,         // result = arg1 != arg2
    IR_LT,          // result = arg1 < arg2
    IR_GT,          // result = arg1 > arg2
    IR_LTE,         // result = arg1 <= arg2
    IR_GTE,         // result = arg1 >= arg2
    IR_TRY_START,   // try start (arg1 = catch label)
    IR_TRY_END,     // try end
    IR_CATCH,       // catch label
    IR_THROW        // throw arg1
} IROp;

typedef struct IRInstr {
    IROp op;
    char *arg1;
    char *arg2;
    char *result;
    struct IRInstr *next;
} IRInstr;

/* Global list of instructions */
extern IRInstr *ir_head;
extern IRInstr *ir_tail;

/* Functions */
void ir_init();
void ir_add(IROp op, const char *a1, const char *a2, const char *res);
char* ir_new_temp();
char* ir_new_label();
void ir_print();
void ir_generate(ASTNode *node); // Generate IR from AST

#endif /* IR_H */
