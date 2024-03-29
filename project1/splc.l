%{
    #include "APT.h"
    #include "splc.tab.h"

    #define __ASSIGN_LEAF_NODE(NAME)    { \
        yylval.AptNode = newAnnotatedParseLeafNode(#NAME, yylineno); \
        return NAME; \
    }
    
    extern FILE* fout;
%}
%option yylineno

%x IN_COMMENT
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
AND     "&&"
OR      "||"
NOT     "!"
LP      "("
RP      ")"
LB      "["
RB      "]"
LC      "{"
RC      "}"

    /* integer in 32-bits (decimal or hexadecimal) */
INT                 [+-]?[0-9]+
HEX_INT             [+-]?0x[0-9a-fA-F]+
WRONG_HEX_INT       [+-]?0x[0-9a-zA-Z]+

    /* floating point number (only dot-form) */
FLOAT               [-+]?(([0-9]+\.?[0-9]*)|([0-9]*\.?[0-9]+))

    /* single character (printable or hex-form) */
CHAR                '(([ -~])|(\\x[0-9a-fA-F]{2}))'
WRONG_HEX_CHAR      '\\x[0-9a-zA-Z]*'
ID                  [_a-zA-Z][_a-zA-Zs0-9]*
WRONG_ID            [_a-zA-Zs0-9]*

    /* comments */
SINGLE_LINE_COMMENT \/\/[^\n]*
MULTI_LINE_COMMENT  \/\*.*\*\/
%%
    /* keywords */
{TYPE}    __ASSIGN_LEAF_NODE(TYPE);
{STRUCT}  __ASSIGN_LEAF_NODE(STRUCT);
{IF}      __ASSIGN_LEAF_NODE(IF);
{ELSE}    __ASSIGN_LEAF_NODE(ELSE);
{WHILE}   __ASSIGN_LEAF_NODE(WHILE);
{RETURN}  __ASSIGN_LEAF_NODE(RETURN);
    /* INT */
{INT}       __ASSIGN_LEAF_NODE(INT);
{HEX_INT}   __ASSIGN_LEAF_NODE(HEX_INT);
{FLOAT}     __ASSIGN_LEAF_NODE(FLOAT);
{WRONG_HEX_INT} { 
    fprintf(fout, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    __ASSIGN_LEAF_NODE(ERROR);
}
    /* single character (printable or hex-form) */
{CHAR}  __ASSIGN_LEAF_NODE(CHAR);
{WRONG_HEX_CHAR} {
    fprintf(fout, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    __ASSIGN_LEAF_NODE(ERROR);
}
{ID}  __ASSIGN_LEAF_NODE(ID);
{WRONG_ID} {
    fprintf(fout, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    __ASSIGN_LEAF_NODE(ERROR);
}
{DOT}  __ASSIGN_LEAF_NODE(DOT);
{SEMI}  __ASSIGN_LEAF_NODE(SEMI);
{COMMA}  __ASSIGN_LEAF_NODE(COMMA);
{ASSIGN}  __ASSIGN_LEAF_NODE(ASSIGN);
{LT}  __ASSIGN_LEAF_NODE(LT);
{LE}  __ASSIGN_LEAF_NODE(LE);
{GT}  __ASSIGN_LEAF_NODE(GT);
{GE}  __ASSIGN_LEAF_NODE(GE);
{NE}  __ASSIGN_LEAF_NODE(NE);
{EQ}  __ASSIGN_LEAF_NODE(EQ);
{PLUS}   __ASSIGN_LEAF_NODE(PLUS);
{MINUS}  __ASSIGN_LEAF_NODE(MINUS);
{MUL}  __ASSIGN_LEAF_NODE(MUL);
{DIV}  __ASSIGN_LEAF_NODE(DIV);
{AND}  __ASSIGN_LEAF_NODE(AND);
{OR}   __ASSIGN_LEAF_NODE(OR);
{NOT}  __ASSIGN_LEAF_NODE(NOT);
{LP}  __ASSIGN_LEAF_NODE(LP);
{RP}  __ASSIGN_LEAF_NODE(RP);
{LB}  __ASSIGN_LEAF_NODE(LB);
{RB}  __ASSIGN_LEAF_NODE(RB);
{LC}  __ASSIGN_LEAF_NODE(LC);
{RC}  __ASSIGN_LEAF_NODE(RC);
    /* comments */
{SINGLE_LINE_COMMENT}   ;
    /* multi-line comments */
"/*"  BEGIN(IN_COMMENT);
    /* whitespace */
\n      ;
[\t ]+ ;
.       {
    fprintf(fout, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    __ASSIGN_LEAF_NODE(ERROR);
}
<IN_COMMENT>{
"*/"      BEGIN(INITIAL);
[^*\n]+   // eat comment in chunks
"*"       // eat the lone star
\n        ;
}
%%
