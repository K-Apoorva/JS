%{
#include "parser.tab.h"
#include <string.h>
#include <stdlib.h>
%}

%option noyywrap yylineno

%%

"async"         { return ASYNC; }
"function"      { return FUNCTION; }
"const"         { return CONST; }
"let"           { return LET; }
"if"            { return IF; }
"else"          { return ELSE; }
"return"        { return RETURN; }
"throw"         { return THROW; }
"new"           { return NEW; }
"true"          { yylval.str = strdup("true");  return TRUE_LIT; }
"false"         { yylval.str = strdup("false"); return FALSE_LIT; }
"null"          { return NULL_LIT; }

"==="           { yylval.str = strdup("==="); return EQ; }
"!=="           { yylval.str = strdup("!=="); return NEQ; }
"<="            { yylval.str = strdup("<=");  return LE; }
">="            { yylval.str = strdup(">=");  return GE; }
"<"             { yylval.str = strdup("<");   return LT; }
">"             { yylval.str = strdup(">");   return GT; }
"||"            { return OR; }
"&&"            { return AND; }
"=>"            { return ARROW; }
"-="            { yylval.str = strdup("-="); return ASSIGN_OP; }
"+="            { yylval.str = strdup("+="); return ASSIGN_OP; }
"*="            { yylval.str = strdup("*="); return ASSIGN_OP; }
"="             { return ASSIGN; }
"!"             { return NOT; }
"+"             { yylval.str = strdup("+"); return ADDOP; }
"-"             { yylval.str = strdup("-"); return ADDOP; }
"*"             { yylval.str = strdup("*"); return MULOP; }
"/"             { yylval.str = strdup("/"); return MULOP; }
"%"             { yylval.str = strdup("%"); return MULOP; }
"."             { return DOT; }
","             { return COMMA; }
";"             { return SEMICOLON; }
":"             { return COLON; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }

\"([^\"\\]|\\.)*\"  { yylval.str = strdup(yytext); return STRING_LIT; }
[0-9]+(\.[0-9]+)?   { yylval.str = strdup(yytext); return NUMBER_LIT; }
[a-zA-Z_$][a-zA-Z0-9_$]* { yylval.str = strdup(yytext); return IDENTIFIER; }

[ \t\r\n]+          { /* skip whitespace */ }
"//"[^\n]*          { /* skip line comments */ }
"/*"([^*]|\*[^/])*"*/" { /* skip block comments */ }
.                   { /* skip unknown */ }

%%
