// AST - Full Implementation
#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Node creation functions */
ASTNode* create_node(NodeType type) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    memset(node, 0, sizeof(ASTNode));
    node->type = type;
    return node;
}

ASTNode* create_int_node(int val) {
    ASTNode *node = create_node(NODE_LITERAL_INT);
    node->ival = val;
    return node;
}

ASTNode* create_float_node(float val) {
    ASTNode *node = create_node(NODE_LITERAL_FLOAT);
    node->fval = val;
    return node;
}

ASTNode* create_string_node(const char *val) {
    ASTNode *node = create_node(NODE_LITERAL_STRING);
    node->sval = strdup(val);
    return node;
}

ASTNode* create_id_node(const char *name) {
    ASTNode *node = create_node(NODE_VAR_REF);
    node->sval = strdup(name);
    return node;
}

ASTNode* create_bin_op(int op, ASTNode *left, ASTNode *right) {
    ASTNode *node = create_node(NODE_BIN_OP);
    node->op = op;
    node->left = left;
    node->right = right;
    return node;
}

/* Utilities */
void free_ast(ASTNode *node) {
    if (!node) return;
    
    if (node->sval) free(node->sval);
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->cond);
    free_ast(node->body);
    free_ast(node->catch_body);
    free_ast(node->next);
    free_ast(node->params);
    
    free(node);
}

void print_ast(ASTNode *node, int level) {
    if (!node) return;
    
    for (int i = 0; i < level; i++) printf("  ");
    
    switch (node->type) {
        case NODE_LITERAL_INT:
            printf("INT: %d\n", node->ival);
            break;
        case NODE_LITERAL_FLOAT:
            printf("FLOAT: %f\n", node->fval);
            break;
        case NODE_LITERAL_STRING:
            printf("STRING: %s\n", node->sval);
            break;
        case NODE_VAR_REF:
            printf("VAR: %s\n", node->sval);
            break;
        case NODE_BIN_OP:
            printf("BIN_OP: %d\n", node->op);
            print_ast(node->left, level + 1);
            print_ast(node->right, level + 1);
            break;
        default:
            printf("NODE: %d\n", node->type);
            break;
    }
}
