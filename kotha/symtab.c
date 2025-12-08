// Symbol Table - Full Implementation
#include "symtab.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define HASH_SIZE 211

static Symbol *hash_table[HASH_SIZE];
static int current_scope = 0;

/* Hash function */
static unsigned int hash(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

/* Initialize symbol table */
void init_symtab() {
    for (int i = 0; i < HASH_SIZE; i++) {
        hash_table[i] = NULL;
    }
    current_scope = 0;
}

/* Enter a new scope */
void enter_scope() {
    current_scope++;
}

/* Exit current scope */
void exit_scope() {
    // Remove symbols from current scope
    for (int i = 0; i < HASH_SIZE; i++) {
        Symbol **ptr = &hash_table[i];
        while (*ptr) {
            if ((*ptr)->scope_level == current_scope) {
                Symbol *temp = *ptr;
                *ptr = (*ptr)->next;
                free(temp->name);
                free(temp);
            } else {
                ptr = &(*ptr)->next;
            }
        }
    }
    if (current_scope > 0) current_scope--;
}

/* Insert a symbol into the current scope */
int insert_symbol(const char *name, SymbolType type) {
    return insert_symbol_typed(name, type, TYPE_UNKNOWN);
}

/* Insert a symbol with known data type */
int insert_symbol_typed(const char *name, SymbolType type, VarType value_type) {
    unsigned int idx = hash(name);
    
    // Check if already exists in current scope
    Symbol *s = hash_table[idx];
    while (s) {
        if (strcmp(s->name, name) == 0 && s->scope_level == current_scope) {
            return 0; // Already exists
        }
        s = s->next;
    }
    
    // Create new symbol
    Symbol *new_sym = (Symbol*)malloc(sizeof(Symbol));
    new_sym->name = strdup(name);
    new_sym->type = type;
    new_sym->value_type = value_type;
    new_sym->scope_level = current_scope;
    new_sym->line_declared = 0;
    new_sym->array_size = 0;
    new_sym->array_cols = 0;
    new_sym->next = hash_table[idx];
    hash_table[idx] = new_sym;
    
    return 1;
}

/* Get the data type of a symbol */
VarType get_symbol_type(const char *name) {
    Symbol *s = lookup_symbol(name);
    if (s) return s->value_type;
    return TYPE_UNKNOWN;
}

/* Set/update the data type of an existing symbol */
int set_symbol_type(const char *name, VarType value_type) {
    Symbol *s = lookup_symbol(name);
    if (s) {
        s->value_type = value_type;
        return 1;
    }
    return 0;
}

/* Lookup a symbol in all active scopes */
Symbol* lookup_symbol(const char *name) {
    unsigned int idx = hash(name);
    Symbol *s = hash_table[idx];
    Symbol *best = NULL;
    
    // Find symbol with highest scope level (most recent)
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (!best || s->scope_level > best->scope_level) {
                best = s;
            }
        }
        s = s->next;
    }
    
    return best;
}

/* Cleanup */
void free_symtab() {
    for (int i = 0; i < HASH_SIZE; i++) {
        Symbol *s = hash_table[i];
        while (s) {
            Symbol *temp = s;
            s = s->next;
            free(temp->name);
            free(temp);
        }
        hash_table[i] = NULL;
    }
}
