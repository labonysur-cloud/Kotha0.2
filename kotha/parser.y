%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex();

char param_buf[1024];
int first_param;
int has_random = 0;

%}

%union {
    int ival;
    float fval;
    char *sval;
}

/* Keywords */
%token JODI OTHOBA NOYTO PALTAW HOLO SES CHOLBE THEKE PORJONTO JOTOKKHON FEROT
%token DHORO STHIR TALIKA NEW DEKHAW NAO RANDOM PORISHKAR WAIT SONGJUKTO KAJ VOID
%token MAIN

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

%type <sval> expression arguments arg_list

%%

program:
    { printf("#include <stdio.h>\n#include <string.h>\n#include <stdlib.h>\n#include <time.h>\n#ifdef _WIN32\n#include <windows.h>\n#define sleep(x) Sleep((x)*1000)\n#else\n#include <unistd.h>\n#endif\n\n"); }
    global_declarations
    functions
    ;

global_declarations:
    /* empty */
    | global_declarations global_declaration
    ;

global_declaration:
      STHIR ID ASSIGN expression SEMICOLON { printf("const int %s = %s;\n", $2, $4); free($2); free($4); }
    | STHIR ID SEMICOLON { printf("const int %s;\n", $2); free($2); }
    | DHORO ID ASSIGN expression SEMICOLON { printf("int %s = %s;\n", $2, $4); free($2); free($4); }
    | DHORO ID SEMICOLON { printf("int %s;\n", $2); free($2); }
    | TALIKA ID LBRACKET INT RBRACKET SEMICOLON { printf("int %s[%d];\n", $2, $4); free($2); }
    | TALIKA ID LBRACKET INT RBRACKET LBRACKET INT RBRACKET SEMICOLON { printf("int %s[%d][%d];\n", $2, $4, $7); free($2); }
    ;

functions:
    /* empty */
    | functions function
    ;

function:
    KAJ ID LPAREN { strcpy(param_buf, ""); first_param = 1; } parameters RPAREN LBRACE { printf("int %s(%s) {\n", $2, param_buf); free($2); } statements RBRACE { printf("}\n\n"); }
    | VOID KAJ ID LPAREN { strcpy(param_buf, ""); first_param = 1; } parameters RPAREN LBRACE { printf("void %s(%s) {\n", $3, param_buf); free($3); } statements RBRACE { printf("}\n\n"); }
    | MAIN LBRACE { printf("int main() {\n"); if (has_random) printf("    srand(time(NULL));\n"); } statements RBRACE { printf("    return 0;\n}\n"); }
    ;

parameters:
    /* empty */
    | parameter_list
    ;

parameter_list:
    ID { if (first_param) { sprintf(param_buf, "int %s", $1); first_param = 0; } else { sprintf(param_buf + strlen(param_buf), ", int %s", $1); } free($1); }
    | parameter_list COMMA ID { sprintf(param_buf + strlen(param_buf), ", int %s", $3); free($3); }
    ;

statements:
    /* empty */
    | statements statement
    ;

statement:
    declaration
    | string_declaration
    | assignment
    | compound_assignment
    | array_decl
    | array_2d_decl
    | array_assign
    | array_2d_assign
    | if_statement
    | while_statement
    | for_statement
    | switch_statement
    | print_statement
    | input_statement
    | clear_statement
    | wait_statement
    | return_statement
    | function_call_stmt
    | increment_stmt
    | decrement_stmt
    | block
    | SES SEMICOLON { printf("    break;\n"); }
    ;

block:
    LBRACE { printf("{\n"); } statements RBRACE { printf("}\n"); }
    ;

declaration:
    STHIR ID ASSIGN expression SEMICOLON { printf("    const int %s = %s;\n", $2, $4); free($2); free($4); }
    | STHIR ID SEMICOLON { printf("    const int %s;\n", $2); free($2); }
    | DHORO ID ASSIGN expression SEMICOLON { printf("    int %s = %s;\n", $2, $4); free($2); free($4); }
    | DHORO ID SEMICOLON { printf("    int %s;\n", $2); free($2); }
    ;

string_declaration:
      STHIR ID ASSIGN STR SEMICOLON { printf("    const char %s[256] = %s;\n", $2, $4); free($2); free($4); }
    | DHORO ID ASSIGN STR SEMICOLON { printf("    char %s[256] = %s;\n", $2, $4); free($2); free($4); }
    ;

array_decl:
    TALIKA ID LBRACKET INT RBRACKET SEMICOLON { printf("    int %s[%d];\n", $2, $4); free($2); }
    ;

array_2d_decl:
    TALIKA ID LBRACKET INT RBRACKET LBRACKET INT RBRACKET SEMICOLON { printf("    int %s[%d][%d];\n", $2, $4, $7); free($2); }
    ;

array_assign:
    ID LBRACKET expression RBRACKET ASSIGN expression SEMICOLON { printf("    %s[%s] = %s;\n", $1, $3, $6); free($1); free($3); free($6); }
    ;

array_2d_assign:
    ID LBRACKET expression RBRACKET LBRACKET expression RBRACKET ASSIGN expression SEMICOLON { printf("    %s[%s][%s] = %s;\n", $1, $3, $6, $9); free($1); free($3); free($6); free($9); }
    ;

assignment:
    ID ASSIGN expression SEMICOLON { printf("    %s = %s;\n", $1, $3); free($1); free($3); }
    | ID ASSIGN STR SEMICOLON { printf("    strcpy(%s, %s);\n", $1, $3); free($1); free($3); }
    ;

compound_assignment:
    ID PLUSEQ expression SEMICOLON { printf("    %s += %s;\n", $1, $3); free($1); free($3); }
    | ID MINUSEQ expression SEMICOLON { printf("    %s -= %s;\n", $1, $3); free($1); free($3); }
    | ID MULTEQ expression SEMICOLON { printf("    %s *= %s;\n", $1, $3); free($1); free($3); }
    | ID DIVEQ expression SEMICOLON { printf("    %s /= %s;\n", $1, $3); free($1); free($3); }
    | ID MODEQ expression SEMICOLON { printf("    %s %%= %s;\n", $1, $3); free($1); free($3); }
    ;

increment_stmt:
    ID INC SEMICOLON { printf("    %s++;\n", $1); free($1); }
    | INC ID SEMICOLON { printf("    ++%s;\n", $2); free($2); }
    ;

decrement_stmt:
    ID DEC SEMICOLON { printf("    %s--;\n", $1); free($1); }
    | DEC ID SEMICOLON { printf("    --%s;\n", $2); free($2); }
    ;

input_statement:
    NAO LPAREN ID RPAREN SEMICOLON { printf("    scanf(\"%%d\", &%s);\n", $3); free($3); }
    ;

clear_statement:
    PORISHKAR LPAREN RPAREN SEMICOLON { printf("    #ifdef _WIN32\n    system(\"cls\");\n    #else\n    system(\"clear\");\n    #endif\n"); }
    ;

wait_statement:
    WAIT LPAREN expression RPAREN SEMICOLON { printf("    sleep(%s);\n", $3); free($3); }
    ;

return_statement:
    FEROT expression SEMICOLON { printf("    return %s;\n", $2); free($2); }
    | FEROT SEMICOLON { printf("    return;\n"); }
    ;

function_call_stmt:
    ID LPAREN arguments RPAREN SEMICOLON { printf("    %s(%s);\n", $1, $3); free($1); free($3); }
    ;

if_head:
    JODI LPAREN expression RPAREN { printf("    if (%s) ", $3); free($3); }
    ;

if_statement:
    if_head block
    | if_head block else_if_list
    | if_head block else_if_list OTHOBA { printf("    else "); } block
    | if_head block OTHOBA { printf("    else "); } block
    ;

else_if_list:
    else_if_clause
    | else_if_list else_if_clause
    ;

else_if_clause:
    NOYTO LPAREN expression RPAREN { printf("    else if (%s) ", $3); free($3); } block
    ;

while_statement:
    JOTOKKHON LPAREN expression RPAREN { printf("    while (%s) ", $3); free($3); } block
    ;

for_statement:
    CHOLBE LPAREN ID THEKE expression PORJONTO expression RPAREN { 
        printf("    for (int %s = %s; %s <= %s; %s++) ", $3, $5, $3, $7, $3); 
        free($3); free($5); free($7); 
    } block
    ;

switch_statement:
    PALTAW LPAREN expression RPAREN { printf("    switch (%s) {\n", $3); free($3); } LBRACE cases RBRACE { printf("    }\n"); }
    ;

cases:
    /* empty */
    | cases case
    ;

case:
    HOLO expression COLON { printf("    case %s:\n", $2); free($2); } statements
    | OTHOBA COLON { printf("    default:\n"); } statements
    ;

print_statement:
    DEKHAW LPAREN expression RPAREN SEMICOLON { printf("    printf(\"%%d\\n\", %s);\n", $3); free($3); }
    | DEKHAW LPAREN STR RPAREN SEMICOLON { printf("    printf(\"%%s\\n\", %s);\n", $3); free($3); }
    ;

expression:
    INT { char buf[32]; sprintf(buf, "%d", $1); $$ = strdup(buf); }
    | FLOAT { char buf[32]; sprintf(buf, "%g", $1); $$ = strdup(buf); }
    | ID { $$ = strdup($1); }
    | ID LBRACKET expression RBRACKET { char *s = malloc(strlen($1)+strlen($3)+4); sprintf(s, "%s[%s]", $1, $3); $$ = s; free($1); free($3); }
    | ID LBRACKET expression RBRACKET LBRACKET expression RBRACKET { char *s = malloc(strlen($1)+strlen($3)+strlen($6)+6); sprintf(s, "%s[%s][%s]", $1, $3, $6); $$ = s; free($1); free($3); free($6); }
    | ID LPAREN arguments RPAREN { char *s = malloc(strlen($1)+strlen($3)+4); sprintf(s, "%s(%s)", $1, $3); $$ = s; free($1); free($3); }
    | RANDOM LPAREN expression RPAREN { has_random = 1; char *s = malloc(strlen($3)+12); sprintf(s, "rand() %% %s", $3); $$ = s; free($3); }
    | expression PLUS expression { char *s = malloc(strlen($1)+strlen($3)+4); sprintf(s, "%s + %s", $1, $3); $$ = s; free($1); free($3); }
    | expression MINUS expression { char *s = malloc(strlen($1)+strlen($3)+4); sprintf(s, "%s - %s", $1, $3); $$ = s; free($1); free($3); }
    | expression MULT expression { char *s = malloc(strlen($1)+strlen($3)+4); sprintf(s, "%s * %s", $1, $3); $$ = s; free($1); free($3); }
    | expression DIV expression { char *s = malloc(strlen($1)+strlen($3)+4); sprintf(s, "%s / %s", $1, $3); $$ = s; free($1); free($3); }
    | expression MOD expression { char *s = malloc(strlen($1)+strlen($3)+4); sprintf(s, "%s %% %s", $1, $3); $$ = s; free($1); free($3); }
    | expression GT expression { char *s = malloc(strlen($1)+strlen($3)+4); sprintf(s, "%s > %s", $1, $3); $$ = s; free($1); free($3); }
    | expression LT expression { char *s = malloc(strlen($1)+strlen($3)+4); sprintf(s, "%s < %s", $1, $3); $$ = s; free($1); free($3); }
    | expression GTE expression { char *s = malloc(strlen($1)+strlen($3)+5); sprintf(s, "%s >= %s", $1, $3); $$ = s; free($1); free($3); }
    | expression LTE expression { char *s = malloc(strlen($1)+strlen($3)+5); sprintf(s, "%s <= %s", $1, $3); $$ = s; free($1); free($3); }
    | expression EQ expression { char *s = malloc(strlen($1)+strlen($3)+5); sprintf(s, "%s == %s", $1, $3); $$ = s; free($1); free($3); }
    | expression NEQ expression { char *s = malloc(strlen($1)+strlen($3)+5); sprintf(s, "%s != %s", $1, $3); $$ = s; free($1); free($3); }
    | expression AND expression { char *s = malloc(strlen($1)+strlen($3)+5); sprintf(s, "%s && %s", $1, $3); $$ = s; free($1); free($3); }
    | expression OR expression { char *s = malloc(strlen($1)+strlen($3)+5); sprintf(s, "%s || %s", $1, $3); $$ = s; free($1); free($3); }
    | NOT expression { char *s = malloc(strlen($2)+2); sprintf(s, "!%s", $2); $$ = s; free($2); }
    | INC ID { char *s = malloc(strlen($2)+3); sprintf(s, "++%s", $2); $$ = s; free($2); }
    | DEC ID { char *s = malloc(strlen($2)+3); sprintf(s, "--%s", $2); $$ = s; free($2); }
    | ID INC { char *s = malloc(strlen($1)+3); sprintf(s, "%s++", $1); $$ = s; free($1); }
    | ID DEC { char *s = malloc(strlen($1)+3); sprintf(s, "%s--", $1); $$ = s; free($1); }
    | LPAREN expression RPAREN { char *s = malloc(strlen($2)+3); sprintf(s, "(%s)", $2); $$ = s; free($2); }
    ;

arguments:
    /* empty */ { $$ = strdup(""); }
    | arg_list { $$ = $1; }
    ;

arg_list:
    expression { $$ = $1; }
    | arg_list COMMA expression { char *s = malloc(strlen($1)+strlen($3)+3); sprintf(s, "%s, %s", $1, $3); $$ = s; free($1); free($3); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main() {
    yyparse();
    return 0;
}
