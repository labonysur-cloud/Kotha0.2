/*
 * Kotha Symbol Table Header
 * Handles scope management and symbol tracking
 */

#ifndef SYMTAB_H
#define SYMTAB_H

/* Category of symbol (variable, constant, function, etc.) */
typedef enum {
    SYM_VAR,
    SYM_CONST,
    SYM_FUNC,
    SYM_ARRAY
} SymbolType;

/* Data type of the value stored in the symbol */
typedef enum {
    TYPE_UNKNOWN,   // Type not yet determined
    TYPE_INT,       // Integer type
    TYPE_FLOAT,     // Floating-point type  
    TYPE_STRING,    // String type (char array)
    TYPE_VOID,      // Void (for functions)
    TYPE_BOOL,      // Boolean (future)
    TYPE_ARRAY_INT, // Array of integers
    TYPE_ARRAY_FLOAT, // Array of floats
    TYPE_ERROR      // Type error detected
} VarType;

typedef struct Symbol {
    char *name;
    SymbolType type;        // Symbol category (var, const, func, array)
    VarType value_type;     // Data type of the value (int, float, string, etc.)
    int scope_level;
    int line_declared;
    // For arrays
    int array_size;
    int array_cols; // For 2D
    struct Symbol *next; // Hash collision chain
} Symbol;

/* Initialize symbol table */
void init_symtab();

/* Enter a new scope (e.g., function, block) */
void enter_scope();

/* Exit current scope */
void exit_scope();

/* Insert a symbol into the current scope */
/* Returns 1 on success, 0 if already exists in current scope */
int insert_symbol(const char *name, SymbolType type);

/* Insert a symbol with known data type */
int insert_symbol_typed(const char *name, SymbolType type, VarType value_type);

/* Get the data type of a symbol */
VarType get_symbol_type(const char *name);

/* Set/update the data type of an existing symbol */
int set_symbol_type(const char *name, VarType value_type);

/* Lookup a symbol in all active scopes (stack) */
Symbol* lookup_symbol(const char *name);

/* Cleanup */
void free_symtab();

#endif /* SYMTAB_H */
