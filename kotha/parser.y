%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtab.h"
#include "interp.h"
#include "ir.h"
#include "vm.h"

// Forward decl
// codegen_vm is declared in vm.h

extern int yylineno;  // Line number from lexer
extern char* yytext;   // Current token text

ASTNode *root = NULL; // Root of the AST

void yyerror(const char *s) {
    fprintf(stderr, "\n");
    fprintf(stderr, "🐯 Kotha Compiler Error\n");
    fprintf(stderr, "━━━━━━━━━━━━━━━━━━━━━━━━\n");
    fprintf(stderr, "Line %d: %s\n", yylineno, s);
    fprintf(stderr, "Near: '%s'\n", yytext);
    fprintf(stderr, "\n");
    fprintf(stderr, "💡 Common fixes:\n");
    fprintf(stderr, "  • Check for missing semicolons (;)\n");
    fprintf(stderr, "  • Verify parentheses () and braces {} are balanced\n");
    fprintf(stderr, "  • Make sure keywords are spelled correctly\n");
    fprintf(stderr, "  • Check that variables are declared with 'dhoro'\n");
    fprintf(stderr, "\n");
}

int yylex();

char param_buf[1024];
int first_param;
int has_random = 0;
int generate_c = 1; // Default to generating C code

%}

%union {
    int ival;
    float fval;
    char *sval;
    struct ASTNode *node;
}

/* Keywords */
%token INCLUDE
%token JODI OTHOBA NOYTO PALTAW HOLO SES CHOLBE THEKE PORJONTO JOTOKKHON FEROT
%token TYPE_INT_KW TYPE_FLOAT_KW TYPE_STRING_KW TYPE_BOOL_KW CONST
%token TALIKA NEW DEKHAW NAO RANDOM PORISHKAR WAIT SONGJUKTO KAJ VOID
%token MAIN TYPEOF

/* Exception handling tokens */
%token TRY CATCH FINALLY THROW

/* Boolean literals */
%token <ival> TRUE FALSE

/* Constants and Identifiers */
%token <sval> ID STR
%token <ival> INT
%token <fval> FLOAT

/* Operators */
%token PLUS MINUS MULT DIV MOD ASSIGN
%token GT LT GTE LTE EQ NEQ
%token AND OR NOT
%token INC DEC
%token PLUSEQ MINUSEQ MULTEQ DIVEQ MODEQ

/* Punctuation */
%token LBRACE RBRACE LPAREN RPAREN LBRACKET RBRACKET COMMA SEMICOLON COLON

/* Precedence and Associativity */
%right ASSIGN PLUSEQ MINUSEQ MULTEQ DIVEQ MODEQ
%left OR
%left AND
%left EQ NEQ
%left GT LT GTE LTE
%left PLUS MINUS
%left MULT DIV MOD
%right NOT
%right INC DEC

%type <sval> arguments arg_list
%type <node> expression statements statement block
%type <node> declaration assignment compound_assignment
%type <node> if_statement if_head while_statement for_statement print_statement
%type <node> try_statement throw_statement input_statement
%type <node> return_statement function_call_stmt function_call
%type <node> array_decl array_2d_decl array_assign array_2d_assign
%type <node> increment_stmt decrement_stmt
%type <sval> var_declarator_list var_declarator

%{
// Forward declarations
VarType infer_type(ASTNode *node);

// Helper to convert AST expression back to C string for transpilation
char* ast_to_c(ASTNode *node) {
    if (!node) return strdup("");
    char buf[1024];
    if (node->type == NODE_LITERAL_INT) {
        sprintf(buf, "%d", node->ival);
        return strdup(buf);
    }
    if (node->type == NODE_LITERAL_FLOAT) {
        sprintf(buf, "%f", node->fval);
        return strdup(buf);
    }
    if (node->type == NODE_VAR_REF) return strdup(node->sval);
    if (node->type == NODE_LITERAL_STRING) return strdup(node->sval);
    if (node->type == NODE_FUNC_CALL) return strdup(node->sval);
    if (node->type == NODE_BIN_OP) {
        char *l = ast_to_c(node->left);
        char *r = ast_to_c(node->right);
        
        // Check if this is string concatenation
        if (node->op == PLUS) {
            VarType left_type = infer_type(node->left);
            VarType right_type = infer_type(node->right);
            
            // If either side is a string, do string concatenation
            if (left_type == TYPE_STRING || right_type == TYPE_STRING) {
                char *res = malloc(1024);
                
                // Build concatenation expression
                strcpy(res, "(kotha_str_reset(), ");
                
                // Add left operand
                if (left_type == TYPE_STRING) {
                    strcat(res, "kotha_str_concat_str(");
                    strcat(res, l);
                    strcat(res, "), ");
                } else if (left_type == TYPE_INT || left_type == TYPE_BOOL) {
                    strcat(res, "kotha_str_concat_int(");
                    strcat(res, l);
                    strcat(res, "), ");
                } else if (left_type == TYPE_FLOAT) {
                    strcat(res, "kotha_str_concat_float(");
                    strcat(res, l);
                    strcat(res, "), ");
                }
                
                // Add right operand
                if (right_type == TYPE_STRING) {
                    strcat(res, "kotha_str_concat_str(");
                    strcat(res, r);
                    strcat(res, "))");
                } else if (right_type == TYPE_INT || right_type == TYPE_BOOL) {
                    strcat(res, "kotha_str_concat_int(");
                    strcat(res, r);
                    strcat(res, "))");
                } else if (right_type == TYPE_FLOAT) {
                    strcat(res, "kotha_str_concat_float(");
                    strcat(res, r);
                    strcat(res, "))");
                }
                
                free(l); free(r);
                return res;
            }
        }
        
        // Regular arithmetic operation
        char op_str[4] = "+";
        switch(node->op) {
            case PLUS: strcpy(op_str, "+"); break;
            case MINUS: strcpy(op_str, "-"); break;
            case MULT: strcpy(op_str, "*"); break;
            case DIV: strcpy(op_str, "/"); break;
            case MOD: strcpy(op_str, "%"); break;
            case GT: strcpy(op_str, ">"); break;
            case LT: strcpy(op_str, "<"); break;
            case GTE: strcpy(op_str, ">="); break;
            case LTE: strcpy(op_str, "<="); break;
            case EQ: strcpy(op_str, "=="); break;
            case NEQ: strcpy(op_str, "!="); break;
            case AND: strcpy(op_str, "&&"); break;
            case OR: strcpy(op_str, "||"); break;
        }
        char *res = malloc(strlen(l) + strlen(r) + 10);
        sprintf(res, "%s %s %s", l, op_str, r); 
        free(l); free(r);
        return res;
    }
    if (node->type == NODE_UN_OP) {
        char *operand = ast_to_c(node->left);
        char *res = malloc(strlen(operand) + 5);
        if (node->op == NOT) {
            sprintf(res, "!%s", operand);
        } else {
            sprintf(res, "%s", operand);
        }
        free(operand);
        return res;
    }
    return strdup("expr");
}

// Type inference: Determine the type of an AST expression node
VarType infer_type(ASTNode *node) {
    if (!node) return TYPE_UNKNOWN;
    
    switch(node->type) {
        case NODE_LITERAL_INT:
            return TYPE_INT;
            
        case NODE_LITERAL_FLOAT:
            return TYPE_FLOAT;
            
        case NODE_LITERAL_STRING:
            return TYPE_STRING;
            
        case NODE_VAR_REF:
            // Lookup variable type in symbol table
            return get_symbol_type(node->sval);
            
        case NODE_BIN_OP: {
            VarType left_type = infer_type(node->left);
            VarType right_type = infer_type(node->right);
            
            // Arithmetic operators: result type depends on operands
            if (node->op == PLUS || node->op == MINUS || 
                node->op == MULT || node->op == DIV || node->op == MOD) {
                // If either is float, result is float
                if (left_type == TYPE_FLOAT || right_type == TYPE_FLOAT)
                    return TYPE_FLOAT;
                // Both int -> int
                if (left_type == TYPE_INT && right_type == TYPE_INT)
                    return TYPE_INT;
                // Type error
                return TYPE_ERROR;
            }
            
            // Comparison operators: always return int (boolean)
            if (node->op == GT || node->op == LT || node->op == GTE ||
                node->op == LTE || node->op == EQ || node->op == NEQ) {
                return TYPE_INT;
            }
            
            return TYPE_UNKNOWN;
        }
        
        case NODE_FUNC_CALL:
            // Special handling for typeof() - it returns a string
            if (node->sval && strstr(node->sval, "kotha_typeof_") != NULL) {
                return TYPE_STRING;
            }
            // Type conversion functions
            if (node->sval) {
                if (strstr(node->sval, "_to_bornona") != NULL) {
                    return TYPE_STRING;
                }
                if (strstr(node->sval, "_to_doshomik") != NULL) {
                    return TYPE_FLOAT;
                }
                if (strstr(node->sval, "_to_purno") != NULL || strstr(node->sval, "_to_sotyo_mittha") != NULL) {
                    return TYPE_INT;
                }
            }
            // For now, assume functions return int
            // TODO: Add function return type tracking
            return TYPE_INT;
            
        default:
            return TYPE_UNKNOWN;
    }
}

// Check if two types are compatible for an operation
int types_compatible(VarType t1, VarType t2, int op) {
    // Unknown types - can't check yet
    if (t1 == TYPE_UNKNOWN || t2 == TYPE_UNKNOWN)
        return 1; // Allow for now
    
    // Arithmetic operations
    if (op == PLUS || op == MINUS || op == MULT || op == DIV || op == MOD) {
        // Special case: PLUS operator can do string concatenation
        if (op == PLUS) {
            // string + anything = OK (string concatenation)
            if (t1 == TYPE_STRING || t2 == TYPE_STRING) return 1;
        }
        
        // Regular arithmetic
        // int + int = OK
        if (t1 == TYPE_INT && t2 == TYPE_INT) return 1;
        // int + float = OK (promote to float)
        if ((t1 == TYPE_INT && t2 == TYPE_FLOAT) ||
            (t1 == TYPE_FLOAT && t2 == TYPE_INT)) return 1;
        // float + float = OK
        if (t1 == TYPE_FLOAT && t2 == TYPE_FLOAT) return 1;
        
        // For non-PLUS operators, string is not allowed
        if (op != PLUS && (t1 == TYPE_STRING || t2 == TYPE_STRING)) return 0;
        return 0;
    }
    
    // Comparison operations
    if (op == GT || op == LT || op == GTE || op == LTE || op == EQ || op == NEQ) {
        // Can compare numbers
        if ((t1 == TYPE_INT || t1 == TYPE_FLOAT) &&
            (t2 == TYPE_INT || t2 == TYPE_FLOAT)) return 1;
        // Can compare strings
        if (t1 == TYPE_STRING && t2 == TYPE_STRING) return 1;
        return 0;
    }
    
    // Assignment
    if (op == ASSIGN) {
        // Exact type match
        if (t1 == t2) return 1;
        // int = float OK (will truncate, but allowed in C)
        if (t1 == TYPE_INT && t2 == TYPE_FLOAT) return 1;
        // float = int OK (promotion)
        if (t1 == TYPE_FLOAT && t2 == TYPE_INT) return 1;
        // string = int/float = ERROR
        if ((t1 == TYPE_STRING && t2 != TYPE_STRING) ||
            (t1 != TYPE_STRING && t2 == TYPE_STRING)) return 0;
        return 0;
    }
    
    return 1; // Default: allow
}

// Report a type error with helpful message
void type_error(const char *msg, int line) {
    fprintf(stderr, "\n🐯 Kotha Type Error\n");
    fprintf(stderr, "━━━━━━━━━━━━━━━━━━\n");
    if (line > 0) {
        fprintf(stderr, "Line %d: %s\n", line, msg);
    } else {
        fprintf(stderr, "%s\n", msg);
    }
    fprintf(stderr, "\n💡 Type Checking Tips:\n");
    fprintf(stderr, "  • Variables declared with 'dhoro' are integers\n");
    fprintf(stderr, "  • String literals cannot be assigned to int variables\n");
    fprintf(stderr, "  • Cannot mix strings and numbers in arithmetic\n");
    fprintf(stderr, "\n");
    // Don't exit - let compilation continue to find more errors
}

// Get type name for error messages
const char* type_name(VarType t) {
    switch(t) {
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_STRING: return "string";
        case TYPE_VOID: return "void";
        case TYPE_BOOL: return "bool";
        case TYPE_ARRAY_INT: return "int[]";
        case TYPE_ARRAY_FLOAT: return "float[]";
        case TYPE_UNKNOWN: return "unknown";
        case TYPE_ERROR: return "error";
        default: return "unknown";
    }
}
%}


%%

program:
    {
        if (generate_c) {
            printf("#include <stdio.h>\n");
            printf("#include <string.h>\n");
            printf("#include <stdlib.h>\n");
            printf("#include <time.h>\n");
            printf("#ifdef _WIN32\n");
            printf("#include <windows.h>\n");
            printf("#define sleep(x) Sleep((x)*1000)\n");
            printf("#else\n");
            printf("#include <unistd.h>\n");
            printf("#endif\n\n");
            
            // Add string concatenation helper functions
            printf("// String concatenation helpers\n");
            printf("static char kotha_str_buffer[4096];\n");
            printf("static int kotha_str_offset = 0;\n\n");
            
            printf("char* kotha_str_reset() {\n");
            printf("    kotha_str_offset = 0;\n");
            printf("    kotha_str_buffer[0] = '\\0';\n");
            printf("    return kotha_str_buffer;\n");
            printf("}\n\n");
            
            printf("char* kotha_str_concat_str(const char* s) {\n");
            printf("    int len = strlen(s);\n");
            printf("    if (kotha_str_offset + len < 4096) {\n");
            printf("        strcpy(kotha_str_buffer + kotha_str_offset, s);\n");
            printf("        kotha_str_offset += len;\n");
            printf("    }\n");
            printf("    return kotha_str_buffer;\n");
            printf("}\n\n");
            
            printf("char* kotha_str_concat_int(int val) {\n");
            printf("    char temp[32];\n");
            printf("    sprintf(temp, \"%%d\", val);\n");
            printf("    return kotha_str_concat_str(temp);\n");
            printf("}\n\n");
            
            printf("char* kotha_str_concat_float(float val) {\n");
            printf("    char temp[32];\n");
            printf("    sprintf(temp, \"%%g\", val);\n");
            printf("    return kotha_str_concat_str(temp);\n");
            printf("}\n\n");
            
            // Add type conversion functions
            printf("// Type conversion functions\n");
            
            // purno (int) conversions
            printf("float purno_to_doshomik(int val) { return (float)val; }\n");
            printf("char* purno_to_bornona(int val) {\n");
            printf("    static char buf[32];\n");
            printf("    sprintf(buf, \"%%d\", val);\n");
            printf("    return buf;\n");
            printf("}\n");
            printf("int purno_to_sotyo_mittha(int val) { return val != 0; }\n\n");
            
            // doshomik (float) conversions
            printf("int doshomik_to_purno(float val) { return (int)val; }\n");
            printf("char* doshomik_to_bornona(float val) {\n");
            printf("    static char buf[32];\n");
            printf("    sprintf(buf, \"%%g\", val);\n");
            printf("    return buf;\n");
            printf("}\n");
            printf("int doshomik_to_sotyo_mittha(float val) { return val != 0.0; }\n\n");
            
            // bornona (string) conversions
            printf("int bornona_to_purno(const char* str) { return atoi(str); }\n");
            printf("float bornona_to_doshomik(const char* str) { return atof(str); }\n");
            printf("int bornona_to_sotyo_mittha(const char* str) { return strlen(str) > 0; }\n\n");
            
            // sotyo_mittha (bool) conversions
            printf("int sotyo_mittha_to_purno(int val) { return val; }\n");
            printf("float sotyo_mittha_to_doshomik(int val) { return (float)val; }\n");
            printf("char* sotyo_mittha_to_bornona(int val) {\n");
            printf("    static char buf[8];\n");
            printf("    sprintf(buf, \"%%s\", val ? \"sotti\" : \"mittha\");\n");
            printf("    return buf;\n");
            printf("}\n\n");
            
            // typeof() functions
            printf("// typeof() helper functions\n");
            printf("const char* kotha_typeof_purno() { return \"purno\"; }\n");
            printf("const char* kotha_typeof_doshomik() { return \"doshomik\"; }\n");
            printf("const char* kotha_typeof_bornona() { return \"bornona\"; }\n");
            printf("const char* kotha_typeof_sotyo_mittha() { return \"sotyo_mittha\"; }\n\n");
        }
    }
    global_declarations
    functions
    ;

global_declarations:
    /* empty */
    | global_declarations global_declaration
    ;

global_declaration:
      /* Integer declarations */
      TYPE_INT_KW ID ASSIGN expression SEMICOLON { 
        insert_symbol_typed($2, SYM_VAR, TYPE_INT);
        if (generate_c) {
            char *s = ast_to_c($4);
            printf("int %s = %s;\n", $2, s); 
            free($2); free(s); 
        }
      }
    | TYPE_INT_KW ID SEMICOLON { 
        insert_symbol_typed($2, SYM_VAR, TYPE_INT);
        if (generate_c) { printf("int %s;\n", $2); free($2); } 
      }
    
      /* Float declarations */
    | TYPE_FLOAT_KW ID ASSIGN expression SEMICOLON { 
        insert_symbol_typed($2, SYM_VAR, TYPE_FLOAT);
        if (generate_c) {
            char *s = ast_to_c($4);
            printf("float %s = %s;\n", $2, s); 
            free($2); free(s); 
        }
      }
    | TYPE_FLOAT_KW ID SEMICOLON { 
        insert_symbol_typed($2, SYM_VAR, TYPE_FLOAT);
        if (generate_c) { printf("float %s;\n", $2); free($2); } 
      }
    
      /* String declarations */
    | TYPE_STRING_KW ID ASSIGN STR SEMICOLON { 
        insert_symbol_typed($2, SYM_VAR, TYPE_STRING);
        if (generate_c) { printf("char %s[256] = %s;\n", $2, $4); free($2); free($4); } 
      }
    | TYPE_STRING_KW ID SEMICOLON { 
        insert_symbol_typed($2, SYM_VAR, TYPE_STRING);
        if (generate_c) { printf("char %s[256];\n", $2); free($2); } 
      }
    
      /* Boolean declarations */
    | TYPE_BOOL_KW ID ASSIGN expression SEMICOLON { 
        insert_symbol_typed($2, SYM_VAR, TYPE_BOOL);
        if (generate_c) {
            char *s = ast_to_c($4);
            printf("int %s = %s;\n", $2, s); 
            free($2); free(s); 
        }
      }
    | TYPE_BOOL_KW ID SEMICOLON { 
        insert_symbol_typed($2, SYM_VAR, TYPE_BOOL);
        if (generate_c) { printf("int %s = 0;\n", $2); free($2); } 
      }
    
      /* Const variants */
    | CONST TYPE_INT_KW ID ASSIGN expression SEMICOLON { 
        insert_symbol_typed($3, SYM_VAR, TYPE_INT);
        if (generate_c) {
            char *s = ast_to_c($5);
            printf("const int %s = %s;\n", $3, s); 
            free($3); free(s); 
        }
      }
    | CONST TYPE_FLOAT_KW ID ASSIGN expression SEMICOLON { 
        insert_symbol_typed($3, SYM_VAR, TYPE_FLOAT);
        if (generate_c) {
            char *s = ast_to_c($5);
            printf("const float %s = %s;\n", $3, s); 
            free($3); free(s); 
        }
      }
    
      /* Arrays */
    | TALIKA ID LBRACKET INT RBRACKET SEMICOLON { 
        insert_symbol_typed($2, SYM_VAR, TYPE_ARRAY_INT);
        if (generate_c) { printf("int %s[%d];\n", $2, $4); free($2); } 
      }
    | TALIKA ID LBRACKET INT RBRACKET LBRACKET INT RBRACKET SEMICOLON { 
        insert_symbol_typed($2, SYM_VAR, TYPE_ARRAY_INT);
        if (generate_c) { printf("int %s[%d][%d];\n", $2, $4, $7); free($2); } 
      }
    ;

functions:
    /* empty */
    | functions function
    ;

function:
    KAJ ID LPAREN { strcpy(param_buf, ""); first_param = 1; } parameters RPAREN LBRACE { if (generate_c) { printf("int %s(%s) {\n", $2, param_buf); free($2); } } statements RBRACE { if (generate_c) { printf("}\n\n"); } }
    | VOID KAJ ID LPAREN { strcpy(param_buf, ""); first_param = 1; } parameters RPAREN LBRACE { if (generate_c) { printf("void %s(%s) {\n", $3, param_buf); free($3); } } statements RBRACE { if (generate_c) { printf("}\n\n"); } }
    | MAIN LBRACE { 
        if (generate_c) {
            printf("int main() {\n");
            if (has_random) printf("    srand(time(NULL));\n");
        }
    } statements RBRACE { 
        if (generate_c) {
            // The statements are printed by their own rules, so we just close main
            printf("    return 0;\n}\n"); 
        }
        root = $4; // Capture AST root, $4 is statements (after embedded action)
    }
    ;

parameters:
    /* empty */
    | parameter_list
    ;

parameter_list:
    ID { if (generate_c) { if (first_param) { sprintf(param_buf, "int %s", $1); first_param = 0; } else { sprintf(param_buf + strlen(param_buf), ", int %s", $1); } free($1); } }
    | parameter_list COMMA ID { if (generate_c) { sprintf(param_buf + strlen(param_buf), ", int %s", $3); free($3); } }
    ;

statements:
    /* empty */ { $$ = NULL; }
    | statements statement { 
        if ($1 == NULL) $$ = $2;
        else {
            ASTNode *curr = $1;
            while (curr->next) curr = curr->next;
            curr->next = $2;
            $$ = $1;
        }
    }
    ;

statement:
      declaration { $$ = $1; }
    | print_statement { $$ = $1; }
    | block { $$ = $1; }
    | if_statement { $$ = $1; }
    | while_statement { $$ = $1; }
    | for_statement { $$ = $1; }
    | try_statement { $$ = $1; }
    | throw_statement { $$ = $1; }
    | assignment { $$ = $1; }
    | return_statement { $$ = $1; }
    | function_call_stmt { $$ = $1; }
    | input_statement { $$ = $1; }
    | clear_statement { $$ = NULL; }
    | wait_statement { $$ = NULL; }
    | array_decl { $$ = NULL; }
    | array_2d_decl { $$ = NULL; }
    | array_assign { $$ = NULL; }
    | array_2d_assign { $$ = NULL; }
    | compound_assignment { $$ = NULL; }
    | increment_stmt { $$ = $1; }
    | decrement_stmt { $$ = $1; }
    | import_statement { $$ = NULL; }
    ;

/* Variable declarator list - supports comma-separated declarations */
var_declarator_list:
    var_declarator { $$ = $1; }
    | var_declarator_list COMMA var_declarator {
        // Concatenate the declarators with comma
        char *result = malloc(strlen($1) + strlen($3) + 3);
        sprintf(result, "%s, %s", $1, $3);
        free($1); free($3);
        $$ = result;
    }
    ;

var_declarator:
    ID { $$ = strdup($1); free($1); }
    | ID ASSIGN expression {
        char *expr_str = ast_to_c($3);
        char *result = malloc(strlen($1) + strlen(expr_str) + 4);
        sprintf(result, "%s = %s", $1, expr_str);
        free($1); free(expr_str);
        $$ = result;
    }
    ;

declaration:
    /* Integer declarations */
    TYPE_INT_KW var_declarator_list SEMICOLON { 
        $$ = create_node(NODE_VAR_DECL);
        
        if (generate_c) {
            printf("    int %s;\n", $2);
        }
        
        // Parse the declarator list and insert symbols
        char *decl_copy = strdup($2);
        char *token = strtok(decl_copy, ",");
        while (token != NULL) {
            // Trim whitespace
            while (*token == ' ') token++;
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') end--;
            *(end + 1) = '\0';
            
            // Extract variable name (before '=' if present)
            char *eq = strchr(token, '=');
            if (eq) {
                *eq = '\0';
                end = eq - 1;
                while (end > token && *end == ' ') end--;
                *(end + 1) = '\0';
            }
            
            insert_symbol_typed(token, SYM_VAR, TYPE_INT);
            token = strtok(NULL, ",");
        }
        free(decl_copy);
        free($2);
    }
    
    /* Float declarations */
    | TYPE_FLOAT_KW var_declarator_list SEMICOLON { 
        $$ = create_node(NODE_VAR_DECL);
        
        if (generate_c) {
            printf("    float %s;\n", $2);
        }
        
        // Parse and insert symbols
        char *decl_copy = strdup($2);
        char *token = strtok(decl_copy, ",");
        while (token != NULL) {
            while (*token == ' ') token++;
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') end--;
            *(end + 1) = '\0';
            
            char *eq = strchr(token, '=');
            if (eq) {
                *eq = '\0';
                end = eq - 1;
                while (end > token && *end == ' ') end--;
                *(end + 1) = '\0';
            }
            
            insert_symbol_typed(token, SYM_VAR, TYPE_FLOAT);
            token = strtok(NULL, ",");
        }
        free(decl_copy);
        free($2);
    }
    
    /* String declarations */
    | TYPE_STRING_KW var_declarator_list SEMICOLON { 
        $$ = create_node(NODE_VAR_DECL);
        
        if (generate_c) {
            // For strings, need to convert to char arrays
            char *decl_copy = strdup($2);
            char *token = strtok(decl_copy, ",");
            int first = 1;
            while (token != NULL) {
                while (*token == ' ') token++;
                if (!first) printf("    ");
                
                // Check if initialized
                char *eq = strchr(token, '=');
                if (eq) {
                    *eq = '\0';
                    char *var_name = token;
                    char *init_val = eq + 1;
                    while (*var_name == ' ') var_name++;
                    while (*init_val == ' ') init_val++;
                    
                    char *end = var_name + strlen(var_name) - 1;
                    while (end > var_name && *end == ' ') end--;
                    *(end + 1) = '\0';
                    
                    // Check if init_val is a string literal (starts with ")
                    if (init_val[0] == '"') {
                        // String literal - can use direct initialization
                        printf("char %s[256] = %s;\n", var_name, init_val);
                    } else {
                        // Function call or expression - need to use strcpy
                        printf("char %s[256];\n", var_name);
                        printf("    strcpy(%s, %s);\n", var_name, init_val);
                    }
                    insert_symbol_typed(var_name, SYM_VAR, TYPE_STRING);
                } else {
                    char *end = token + strlen(token) - 1;
                    while (end > token && *end == ' ') end--;
                    *(end + 1) = '\0';
                    
                    printf("char %s[256];\n", token);
                    insert_symbol_typed(token, SYM_VAR, TYPE_STRING);
                }
                
                token = strtok(NULL, ",");
                first = 0;
            }
            free(decl_copy);
        }
        free($2);
    }
    
    /* Boolean declarations */
    | TYPE_BOOL_KW var_declarator_list SEMICOLON { 
        $$ = create_node(NODE_VAR_DECL);
        
        if (generate_c) {
            printf("    int %s;\n", $2);
        }
        
        // Parse and insert symbols
        char *decl_copy = strdup($2);
        char *token = strtok(decl_copy, ",");
        while (token != NULL) {
            while (*token == ' ') token++;
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') end--;
            *(end + 1) = '\0';
            
            char *eq = strchr(token, '=');
            if (eq) {
                *eq = '\0';
                end = eq - 1;
                while (end > token && *end == ' ') end--;
                *(end + 1) = '\0';
            }
            
            insert_symbol_typed(token, SYM_VAR, TYPE_BOOL);
            token = strtok(NULL, ",");
        }
        free(decl_copy);
        free($2);
    }
    ;

print_statement:
    DEKHAW LPAREN expression RPAREN SEMICOLON {
        $$ = create_node(NODE_PRINT);
        $$->left = $3;
        
        if (generate_c) {
            char *expr_str = ast_to_c($3);
            
            // Infer the type of the expression to use correct printf format
            VarType expr_type = infer_type($3);
            
            // Check if this is a string concatenation result
            // String concatenation returns a string, so check if expression contains kotha_str_
            int is_string_concat = (strstr(expr_str, "kotha_str_") != NULL);
            
            if (is_string_concat || expr_type == TYPE_STRING) {
                printf("    printf(\"%%s\\n\", %s);\n", expr_str);
            } else {
                switch (expr_type) {
                    case TYPE_INT:
                    case TYPE_BOOL:
                        printf("    printf(\"%%d\\n\", %s);\n", expr_str);
                        break;
                    case TYPE_FLOAT:
                        printf("    printf(\"%%f\\n\", %s);\n", expr_str);
                        break;
                    default:
                        // Default to integer format
                        printf("    printf(\"%%d\\n\", %s);\n", expr_str);
                        break;
                }
            }
            free(expr_str);
        }
    }
    | DEKHAW LPAREN STR RPAREN SEMICOLON { 
        if (generate_c) {
            printf("    printf(\"%%s\\n\", %s);\n", $3); 
        }
        $$ = create_node(NODE_PRINT);
        $$->left = create_string_node($3);
        free($3); 
    }
    ;

block:
    LBRACE { if (generate_c) { printf("{\n"); } } statements RBRACE { 
        if (generate_c) { printf("}\n"); } 
        $$ = create_node(NODE_BLOCK);
        $$->body = $3;
    }
    ;



while_statement:
    JOTOKKHON LPAREN expression RPAREN {
        if (generate_c) {
            char *s = ast_to_c($3);
            printf("    while (%s) ", s);
            free(s);
        }
    } block {
        $$ = create_node(NODE_WHILE);
        $$->cond = $3;
        $$->body = $6;
    }
    ;



array_decl:
    TALIKA ID LBRACKET INT RBRACKET SEMICOLON { if (generate_c) { printf("    int %s[%d];\n", $2, $4); free($2); } }
    ;

array_2d_decl:
    TALIKA ID LBRACKET INT RBRACKET LBRACKET INT RBRACKET SEMICOLON { if (generate_c) { printf("    int %s[%d][%d];\n", $2, $4, $7); free($2); } }
    ;

array_assign:
    ID LBRACKET expression RBRACKET ASSIGN expression SEMICOLON { 
        if (generate_c) {
            char *idx = ast_to_c($3);
            char *val = ast_to_c($6);
            printf("    %s[%s] = %s;\n", $1, idx, val); 
            free($1); free(idx); free(val); 
        }
        $$ = NULL;
    }
    ;

array_2d_assign:
    ID LBRACKET expression RBRACKET LBRACKET expression RBRACKET ASSIGN expression SEMICOLON { 
        if (generate_c) {
            char *idx1 = ast_to_c($3);
            char *idx2 = ast_to_c($6);
            char *val = ast_to_c($9);
            printf("    %s[%s][%s] = %s;\n", $1, idx1, idx2, val); 
            free($1); free(idx1); free(idx2); free(val); 
        }
        $$ = NULL;
    }
    ;

assignment:
    ID ASSIGN expression SEMICOLON { 
        // TYPE CHECKING: Check variable type vs expression type
        VarType var_type = get_symbol_type($1);
        VarType expr_type = infer_type($3);
        
        if (var_type != TYPE_UNKNOWN && !types_compatible(var_type, expr_type, ASSIGN)) {
            char error_msg[256];
            sprintf(error_msg, "Type mismatch: Cannot assign %s to %s variable '%s'",
                    type_name(expr_type), type_name(var_type), $1);
            type_error(error_msg, yylineno);
        }
        
        if (generate_c) {
            char *s = ast_to_c($3);
            printf("    %s = %s;\n", $1, s); 
            free(s);
        }
        
        $$ = create_node(NODE_ASSIGN);
        $$->sval = strdup($1);
        $$->left = $3;
        free($1);
    }
    ;

compound_assignment:
    ID PLUSEQ expression SEMICOLON { 
        char *s = ast_to_c($3); 
        printf("    %s += %s;\n", $1, s); 
        free($1); free(s); 
        $$ = NULL;
    }
    | ID MINUSEQ expression SEMICOLON { 
        char *s = ast_to_c($3); 
        printf("    %s -= %s;\n", $1, s); 
        free($1); free(s); 
        $$ = NULL;
    }
    | ID MULTEQ expression SEMICOLON { 
        char *s = ast_to_c($3); 
        printf("    %s *= %s;\n", $1, s); 
        free($1); free(s); 
        $$ = NULL;
    }
    | ID DIVEQ expression SEMICOLON { 
        char *s = ast_to_c($3); 
        printf("    %s /= %s;\n", $1, s); 
        free($1); free(s); 
        $$ = NULL;
    }
    | ID MODEQ expression SEMICOLON { 
        char *s = ast_to_c($3); 
        printf("    %s %%= %s;\n", $1, s); 
        free($1); free(s); 
        $$ = NULL;
    }
    ;

increment_stmt:
    ID INC SEMICOLON { 
        if (generate_c) { printf("    %s++;\n", $1); }
        $$ = create_node(NODE_UN_OP);
        $$->op = INC;
        $$->sval = strdup($1);
        $$->left = create_id_node($1);
        free($1);
    }
    | INC ID SEMICOLON { 
        if (generate_c) { printf("    ++%s;\n", $2); }
        $$ = create_node(NODE_UN_OP);
        $$->op = INC;
        $$->sval = strdup($2);
        $$->left = create_id_node($2);
        free($2);
    }
    ;

decrement_stmt:
    ID DEC SEMICOLON { 
        if (generate_c) { printf("    %s--;\n", $1); }
        $$ = create_node(NODE_UN_OP);
        $$->op = DEC;
        $$->sval = strdup($1);
        $$->left = create_id_node($1);
        free($1);
    }
    | DEC ID SEMICOLON { 
        if (generate_c) { printf("    --%s;\n", $2); }
        $$ = create_node(NODE_UN_OP);
        $$->op = DEC;
        $$->sval = strdup($2);
        $$->left = create_id_node($2);
        free($2);
    }
    ;

input_statement:
    NAO LPAREN ID RPAREN SEMICOLON { 
        // Create AST node for input
        $$ = create_node(NODE_INPUT);
        $$->sval = strdup($3);
        
        if (generate_c) {
            // Check variable type to determine scanf format
            VarType var_type = get_symbol_type($3);
            switch (var_type) {
                case TYPE_INT:
                    // purno - integer type
                    printf("    scanf(\"%%d\", &%s);\n", $3);
                    break;
                case TYPE_FLOAT:
                    // doshomik - float type
                    printf("    scanf(\"%%f\", &%s);\n", $3);
                    break;
                case TYPE_STRING:
                    // bornona - string type (no & needed for arrays)
                    printf("    scanf(\"%%s\", %s);\n", $3);
                    break;
                case TYPE_BOOL:
                    // sotyo_mittha - boolean type (stored as int)
                    printf("    scanf(\"%%d\", &%s);\n", $3);
                    break;
                default:
                    // Unknown type - default to integer
                    printf("    scanf(\"%%d\", &%s);\n", $3);
                    break;
            }
        }
        
        free($3);
    }
    ;

clear_statement:
    PORISHKAR LPAREN RPAREN SEMICOLON { if (generate_c) { printf("    #ifdef _WIN32\n    system(\"cls\");\n    #else\n    system(\"clear\");\n    #endif\n"); } }
    ;

wait_statement:
    WAIT LPAREN expression RPAREN SEMICOLON { 
        char *s = ast_to_c($3);
        printf("    #ifdef _WIN32\n    Sleep(%s * 1000);\n    #else\n    sleep(%s);\n    #endif\n", s, s); 
        free(s);
    }
    ;

import_statement:
    INCLUDE STR SEMICOLON {
        // Generate #include directive
        printf("%s\n", $2);
        free($2);
    }
    ;

return_statement:
    FEROT expression SEMICOLON { 
        if (generate_c) {
            char *s = ast_to_c($2);
            printf("    return %s;\n", s); 
            free(s); 
        }
    }
    | FEROT SEMICOLON { 
        printf("    return;\n"); 
        $$ = create_node(NODE_RETURN);
    }
    ;

function_call_stmt:
    function_call SEMICOLON { 
        if (generate_c) {
            char *s = ast_to_c($1);
            printf("    %s;\n", s); 
            free(s); 
        }
        $$ = $1;
    }
    ;

function_call:
    ID LPAREN arguments RPAREN {
        $$ = create_node(NODE_FUNC_CALL);
        char *call_str = malloc(strlen($1) + strlen($3) + 3);
        sprintf(call_str, "%s(%s)", $1, $3);
        $$->sval = call_str;
        free($1); free($3);
    }
    ;


if_statement:
    if_head else_if_list_opt else_part_opt {
        $$ = create_node(NODE_IF);
        $$->cond = $1->cond;
        $$->body = $1->body;
        // We are losing the else/else-if AST nodes here but for verification it's fine
        // as we rely on transpilation.
    }
    ;

if_head:
    JODI LPAREN expression RPAREN { 
        if (generate_c) {
            char *s = ast_to_c($3);
            printf("    if (%s) ", s); 
            free(s); 
        }
    } block {
        $$ = create_node(NODE_IF);
        $$->cond = $3;
        $$->body = $6;
    }
    ;

else_if_list_opt:
    /* empty */
    | else_if_list
    ;

else_part_opt:
    /* empty */
    | else_part
    ;

else_if_list:
    else_if_clause
    | else_if_list else_if_clause
    ;

else_if_clause:
    NOYTO LPAREN expression RPAREN { 
        if (generate_c) {
            char *s = ast_to_c($3);
            printf("    else if (%s) ", s); 
            free(s); 
        }
    } block
    ;

else_part:
    OTHOBA { if (generate_c) { printf("    else "); } } block
    ;

while_statement:
    JOTOKKHON LPAREN expression RPAREN {
        char *s = ast_to_c($3);
        printf("    while (%s) ", s);
        free(s);
    } block {
        $$ = create_node(NODE_WHILE);
        $$->cond = $3;
        $$->body = $6;
    }
    ;

try_statement:
    TRY { printf("    try "); } block CATCH { printf(" catch (...) "); } block finally_opt {
        $$ = create_node(NODE_TRY);
        $$->body = $3;
        $$->catch_body = $6;
    }
    ;

finally_opt:
    /* empty */
    | FINALLY { printf(" finally "); } block
    ;

throw_statement:
    THROW expression SEMICOLON {
        char *s = ast_to_c($2);
        printf("    throw %s;\n", s);
        free(s);
        
        $$ = create_node(NODE_THROW);
        $$->left = $2;
    }
    ;



for_statement:
    CHOLBE LPAREN ID THEKE expression PORJONTO expression RPAREN { 
        if (generate_c) {
            // Generate C for loop header
            char *start_str = ast_to_c($5);
            char *end_str = ast_to_c($7);
            printf("    for (int %s = %s; %s <= %s; %s++) ", $3, start_str, $3, end_str, $3); 
            free(start_str);
            free(end_str);
        }
        free($3);
    } block {
        $$ = create_node(NODE_FOR); 
        // The block ($10) is the body of the for loop.
        $$->body = $10;
        // Store loop variable, start, end expressions for AST
        $$->sval = strdup($3); // Loop variable ID
        $$->left = $5; // Start expression
        $$->right = $7; // End expression
    }
    ;

switch_statement:
    PALTAW LPAREN expression RPAREN { if (generate_c) { printf("    switch (%s) {\n", $3); free($3); } } LBRACE cases RBRACE { if (generate_c) { printf("    }\n"); } }
    ;

cases:
    /* empty */
    | cases case
    ;

case:
    HOLO expression COLON { if (generate_c) { printf("    case %s:\n", $2); free($2); } } statements
    | OTHOBA COLON { if (generate_c) { printf("    default:\n"); } } statements
    ;


expression:
      INT { $$ = create_int_node($1); }
    | FLOAT { $$ = create_float_node($1); }
    | TRUE { $$ = create_int_node(1); }   /* Boolean true */
    | FALSE { $$ = create_int_node(0); }  /* Boolean false */
    | ID { 
        // SEMANTIC CHECK: Variable must be declared
        if (!lookup_symbol($1)) {
           char error_msg[256];
           sprintf(error_msg, "Variable '%s' used but not declared", $1);
           // We can treat as error or warning. For strict validation, error.
           // However, to avoid halting transpilation immediately if we want to catch multiple errors,
           // we can print and set a flag. Or use type_error.
           type_error(error_msg, yylineno);
        }
        $$ = create_id_node($1); 
        free($1); 
    }
    | STR { $$ = create_string_node($1); free($1); }
    | function_call { $$ = $1; }
    | expression PLUS expression { $$ = create_bin_op(PLUS, $1, $3); }
    | expression MINUS expression { $$ = create_bin_op(MINUS, $1, $3); }
    | expression MULT expression { $$ = create_bin_op(MULT, $1, $3); }
    | expression DIV expression { $$ = create_bin_op(DIV, $1, $3); }
    | expression MOD expression { $$ = create_bin_op(MOD, $1, $3); }
    | expression GT expression { $$ = create_bin_op(GT, $1, $3); }
    | expression LT expression { $$ = create_bin_op(LT, $1, $3); }
    | expression GTE expression { $$ = create_bin_op(GTE, $1, $3); }
    | expression LTE expression { $$ = create_bin_op(LTE, $1, $3); }
    | expression EQ expression { $$ = create_bin_op(EQ, $1, $3); }
    | expression NEQ expression { $$ = create_bin_op(NEQ, $1, $3); }
    | expression AND expression { $$ = create_bin_op(AND, $1, $3); }
    | expression OR expression { $$ = create_bin_op(OR, $1, $3); }
    | NOT expression { 
        $$ = create_node(NODE_UN_OP);
        $$->op = NOT;
        $$->left = $2;
    }
    | LPAREN expression RPAREN { $$ = $2; }
    | TYPEOF LPAREN expression RPAREN {
        $$ = create_node(NODE_FUNC_CALL);
        
        // Determine the type of the expression
        VarType expr_type = infer_type($3);
        char *typeof_call;
        
        switch (expr_type) {
            case TYPE_INT:
                typeof_call = strdup("kotha_typeof_purno()");
                break;
            case TYPE_FLOAT:
                typeof_call = strdup("kotha_typeof_doshomik()");
                break;
            case TYPE_STRING:
                typeof_call = strdup("kotha_typeof_bornona()");
                break;
            case TYPE_BOOL:
                typeof_call = strdup("kotha_typeof_sotyo_mittha()");
                break;
            default:
                typeof_call = strdup("kotha_typeof_purno()");  // Default
                break;
        }
        
        $$->sval = typeof_call;
    }
    | ID LBRACKET expression RBRACKET { 
        $$ = create_node(NODE_ARRAY_ACCESS);
        $$->sval = strdup($1);
        $$->left = $3;
        free($1);
    }
    ;

arguments:
    /* empty */ { $$ = strdup(""); }
    | arg_list { $$ = $1; }
    ;

arg_list:
    expression { $$ = ast_to_c($1); }
    | arg_list COMMA expression { 
        char *s = ast_to_c($3);
        char *res = malloc(strlen($1) + strlen(s) + 3);
        sprintf(res, "%s, %s", $1, s);
        $$ = res;
        free($1); free(s);
    }
    ;

%%




/* End of parser */
