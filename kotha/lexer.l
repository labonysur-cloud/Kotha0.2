%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

void yyerror(const char *s);
%}

%option yylineno

%%

"jodi"          { return JODI; }
"othoba"        { return OTHOBA; }
"noyto"         { return NOYTO; }
"paltaw"        { return PALTAW; }
"holo"          { return HOLO; }
"ses"           { return SES; }
"cholbe"        { return CHOLBE; }
"theke"         { return THEKE; }
"porjonto"      { return PORJONTO; }
"jotokkhon"     { return JOTOKKHON; }
"ferot"         { return FEROT; }
"dhoro"         { return DHORO; }
"sthir"         { return STHIR; }
"talika"        { return TALIKA; }
"new"           { return NEW; }
"dekhaw"        { return DEKHAW; }
"nao"           { return NAO; }
"random"        { return RANDOM; }
"porishkar"     { return PORISHKAR; }
"wait"          { return WAIT; }
"songjukto"     { return SONGJUKTO; }
"kaj"           { return KAJ; }
"void"        { return VOID; }
"main function" { return MAIN; }

    /* Exception Handling Keywords */
"try"           { return TRY; }
"catch"         { return CATCH; }
"finally"       { return FINALLY; }
"throw"         { return THROW; }

"include"       { return INCLUDE; }


[a-zA-Z_][a-zA-Z0-9_]*  { yylval.sval = strdup(yytext); return ID; }
[0-9]+                  { yylval.ival = atoi(yytext); return INT; }
[0-9]+\.[0-9]+          { yylval.fval = atof(yytext); return FLOAT; }
\"[^"]*\"               { yylval.sval = strdup(yytext); return STR; }

"+="            { return PLUSEQ; }
"-="            { return MINUSEQ; }
"*="            { return MULTEQ; }
"/="            { return DIVEQ; }
"%="            { return MODEQ; }

"++"            { return INC; }
"--"            { return DEC; }

"&&"            { return AND; }
"||"            { return OR; }

">="            { return GTE; }
"<="            { return LTE; }
"=="            { return EQ; }
"!="            { return NEQ; }
">"             { return GT; }
"<"             { return LT; }
"!"             { return NOT; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULT; }
"/"             { return DIV; }
"%"             { return MOD; }
"="             { return ASSIGN; }

"{"             { return LBRACE; }
"}"             { return RBRACE; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }
","             { return COMMA; }
";"             { return SEMICOLON; }
":"             { return COLON; }

[ \t\n]+        { /* ignore whitespace */ }
"//".*          { /* ignore comments */ }
.               { printf("Unknown character: %s\n", yytext); }

%%

int yywrap() {
    return 1;
}
