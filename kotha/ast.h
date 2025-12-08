/*
 * Kotha Abstract Syntax Tree Header
 */

#ifndef AST_H
#define AST_H

typedef enum {
    NODE_VAR_DECL,
    NODE_LITERAL_INT,
    NODE_LITERAL_FLOAT,
    NODE_LITERAL_STRING,
    NODE_VAR_REF,
    NODE_BIN_OP,
    NODE_UN_OP,          // NEW: Unary operations (!, ++, --)
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_BLOCK,
    NODE_FUNC_DECL,
    NODE_FUNC_CALL,
    NODE_RETURN,
    NODE_TRY,
    NODE_CATCH,
    NODE_THROW,
    NODE_ARRAY_ACCESS,   // NEW: Array element access
    NODE_ARRAY_DECL      // NEW: Array declaration
} NodeType;

typedef struct ASTNode {
    NodeType type;
    
    // For literals/identifiers
    int ival;
    float fval;
    char *sval;
    int op; // Operator type for BIN_OP
    
    // Children
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *cond;
    struct ASTNode *body;
    struct ASTNode *catch_body; // For try-catch
    struct ASTNode *next; // For lists of statements
    
    // For functions
    struct ASTNode *params;
    
} ASTNode;

/* Node creation functions */
ASTNode* create_node(NodeType type);
ASTNode* create_int_node(int val);
ASTNode* create_float_node(float val);
ASTNode* create_string_node(const char *val);
ASTNode* create_id_node(const char *name);
ASTNode* create_bin_op(int op, ASTNode *left, ASTNode *right);

/* Utilities */
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int level);

#endif /* AST_H */
