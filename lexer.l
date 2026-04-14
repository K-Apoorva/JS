%{
#include "parser.tab.h"
#include <string.h>
#include <stdio.h>

void yyerror(const char *s);
void trace_shift(const char *token);
%}

%option noyywrap yylineno

%%

"async"         { trace_shift("async");     return ASYNC; }
"function"      { trace_shift("function");  return FUNCTION; }
"const"         { trace_shift("const");     return CONST; }
"let"           { trace_shift("let");       return LET; }
"if"            { trace_shift("if");        return IF; }
"return"        { trace_shift("return");    return RETURN; }
"throw"         { trace_shift("throw");     return THROW; }
"new"           { trace_shift("new");       return NEW; }
"true"          { trace_shift("true");      return TRUE_LIT; }
"false"         { trace_shift("false");     return FALSE_LIT; }
"=>"            { trace_shift("=>");        return ARROW; }
"==="           { trace_shift("===");       return EQ; }
"||"            { trace_shift("||");        return OR; }
"-="            { trace_shift("-=");        return MINUS_ASSIGN; }
"."             { trace_shift(".");         return DOT; }
","             { trace_shift(",");         return COMMA; }
";"             { trace_shift(";");         return SEMICOLON; }
":"             { trace_shift(":");         return COLON; }
"("             { trace_shift("(");         return LPAREN; }
")"             { trace_shift(")");         return RPAREN; }
"{"             { trace_shift("{");         return LBRACE; }
"}"             { trace_shift("}");         return RBRACE; }
"["             { trace_shift("[");         return LBRACKET; }
"]"             { trace_shift("]");         return RBRACKET; }
"!"             { trace_shift("!");         return NOT; }
"="             { trace_shift("=");         return ASSIGN; }
"*"             { trace_shift("*");         return STAR; }
"<"             { trace_shift("<");         return LT; }
">"             { trace_shift(">");         return GT; }

\"[^\"]*\"      { trace_shift("STRING");    yylval.str = strdup(yytext); return STRING_LIT; }
[0-9]+          { trace_shift("NUMBER");    yylval.str = strdup(yytext); return NUMBER_LIT; }
[a-zA-Z_][a-zA-Z0-9_]* { trace_shift(yytext); yylval.str = strdup(yytext); return IDENTIFIER; }

[ \t\r\n]+      { /* skip whitespace */ }
"//"[^\n]*      { /* skip line comments */ }
.               { /* skip unknown chars */ }

%%
