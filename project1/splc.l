%{
    #include "splc.tab.h"
%}
%option yylineno

    /* keywords */
TYPE    "int"|"float"|"char"
STRUCT  "struct"
IF      "if"
ELSE    "else"
WHILE   "while"
RETURN  "return"
DOT     "."
SEMI    ";"
COMMA   ","
ASSIGN  "="
LT      "<"
LE      "<="
GT      ">"
GE      ">="
NE      "!="
EQ      "=="
PLUS    "+"
MINUS   "-"
MUL     "*"
DIV     "/"
ADD     "&&"
OR      "||"
NOT     "!"
LP      "("
RP      ")"
LB      "["
RB      "]"
LC      "{"
RC      "}"

    /* integer in 32-bits (decimal or hexadecimal) */
INT                 [+-]?((0x[0-9a-fA-F]+)|([0-9]+))
WRONG_HEX_INT       [+-]?(0x[0-9a-zA-Z]+)

    /* floating point number (only dot-form) */
FLOAT               [-+]?(([0-9]+\.?[0-9]*)|([0-9]*\.?[0-9]+))

    /* single character (printable or hex-form) */
CHAR                '(([ -~])|(\\x[0-9a-fA-F]{2}))'
WRONG_HEX_CHAR      '\\x[0-9a-zA-Z]*'
ID                  [_a-zA-Z][_a-zA-Zs0-9]*

%%
    /* keywords */
{TYPE}  return TYPE;
{STRUCT}  return STRUCT;
{IF}      return IF;
{ELSE}    return ELSE;
{WHILE}   return WHILE;
{RETURN}  return RETURN;
{INT} { return INT; }
{WRONG_HEX_INT} { printf("Error type A at Line %d: Mysterious lexeme %s\n", yylineno, yytext); }
{FLOAT}  { return FLOAT; }
    /* single character (printable or hex-form) */
{CHAR} { return CHAR; }
{WRONG_HEX_CHAR} { printf("Error type A at Line %d: Mysterious lexeme %s\n", yylineno, yytext); }
{ID} { return ID; }
{DOT} return DOT;
{SEMI} return SEMI;
{COMMA} return COMMA;
{ASSIGN} return ASSIGN;
{LT} return LT;
{LE} return LE;
{GT} return GT;
{GE} return GE;
{NE} return NE;
{EQ} return EQ;
{PLUS}  return PLUS;
{MINUS}     return MINUS;
{MUL}     return MUL;
{DIV}     return DIV;
{AND}    return AND;
{OR}    return OR;
{NOT}     return NOT;
{LP}     return LP;
{RP}     return RP;
{LB}     return LB;
{RB}     return RB;
{LC}     return LC;
{RC}     return RC;
    /* whitespace */
\n      ;
[\t ]+ ;
.       { printf("Error type A at Line %d: Unknown characters '%s'\n", yylineno, yytext); }
%%
