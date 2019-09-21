%{
    #include "APT.h"
    #include "lex.yy.c"
    void yyerror (const char*);
%}

%union {
    AnnotatedParseTreeNode* AptNode;
}

%token <AptNode> INT     // /* integer in 32-bits (decimal or hexadecimal) */
%token <AptNode> FLOAT   // /* floating point number (only dot-form) */
%token <AptNode> CHAR    // /* single character (printable or hex-form) */
%token <AptNode> ID      // /* identifier */
%token <AptNode> TYPE    // int | float | char
%token <AptNode> STRUCT  // struct
%token <AptNode> IF      // if
%token <AptNode> ELSE    // else
%token <AptNode> WHILE   // while
%token <AptNode> RETURN  // return
%token <AptNode> DOT     // .
%token <AptNode> SEMI    // ;
%token <AptNode> COMMA   // ,
%token <AptNode> ASSIGN  // =
%token <AptNode> LT      // <
%token <AptNode> LE      // <=
%token <AptNode> GT      // >
%token <AptNode> GE      // >=
%token <AptNode> NE      // !=
%token <AptNode> EQ      // ==
%token <AptNode> PLUS    // +
%token <AptNode> MINUS   // -
%token <AptNode> MUL     // *
%token <AptNode> DIV     // /
%token <AptNode> AND     // &&
%token <AptNode> OR      // ||
%token <AptNode> NOT     // !
%token <AptNode> LP      // (
%token <AptNode> RP      // )
%token <AptNode> LB      // [
%token <AptNode> RB      // ]
%token <AptNode> LC      // {
%token <AptNode> RC      // }
//%token <AptNode> NEWLINE
%right ASSIGN
%right NOT

%left OR
%left AND
%left LT LE GT GE NE EQ
%left PLUS MINUS
%left MUL DIV
%left COMMA DOT

%nonassoc LP RP LB RB LC RC
%nonassoc SEMI
%nonassoc ELSE
%%
    /* high-level definition */
Program: 
        ExtDefList
    ;
ExtDefList: 
        ExtDef ExtDefList
    |   
    ;
ExtDef:
        Specifier ExtDecList SEMI
    |   Specifier SEMI
    |   Specifier FunDec CompSt
    ;
ExtDecList:
        VarDec
    |   VarDec COMMA ExtDecList
    ;

    /* specifier */
Specifier:
        TYPE
    |   StructSpecifier
    ;
StructSpecifier:
        STRUCT ID LC DefList RC
    |   STRUCT ID
    ;

    /* declarator */
VarDec: 
        ID
    |   VarDec LB INT RB
    ;
FunDec:
        ID LP VarList RP
    |   ID LP RP
    ;
VarList:
        ParamDec COMMA VarList
    |   ParamDec
    ;
ParamDec:
        Specifier VarDec
    ;

    /* statement */
CompSt: 
        LC DefList StmtList RC
    |   error RC
    ;
StmtList: 
        Stmt StmtList
    |   
    ;
Stmt:
        Exp SEMI
    |   CompSt
    |   RETURN Exp SEMI
    |   IF LP Exp RP Stmt
    |   IF LP Exp RP Stmt ELSE Stmt
    |   WHILE LP Exp RP Stmt
    |   error SEMI
    ;

    /* local definition */
DefList: 
        Def DefList
    |   
    ;
Def:
        Specifier DecList SEMI
    ;
DecList: 
        Dec
    |   Dec COMMA DecList
    ;
Dec: 
        VarDec
    |   VarDec ASSIGN Exp
    ;
    /* Expression */
Exp:  
        Exp ASSIGN Exp
    |   Exp AND Exp
    |   Exp OR Exp
    |   Exp LT Exp
    |   Exp LE Exp
    |   Exp GT Exp
    |   Exp GE Exp
    |   Exp NE Exp
    |   Exp EQ Exp
    |   Exp PLUS Exp
    |   Exp MINUS Exp
    |   Exp MUL Exp
    |   Exp DIV Exp
    |   LP Exp RP
    |   MINUS Exp
    |   NOT Exp
    |   ID LP Args RP
    |   ID LP RP
    |   Exp LB Exp LB
    |   Exp DOT ID
    |   ID
    |   INT
    |   FLOAT
    |   CHAR
    |   error RP
    ;
Args: Exp COMMA Args
    |   Exp
    ;
%%
void
yyerror (char const *s)
{
  //fprintf (stderr, "ERROR:%s\n", s);
  fprintf (stderr, "ERROR: Line %d, %s \"%s\"\n", yylineno, s, yytext);
  
}

int
main (int argc, char **argv)
{
    yyin = fopen(argv[1], "r");
    //yydebug = 1;
    yyparse();
    /*while(1) {
        int token = yylex();
        if (token == 0) break;
        printf("Token: %d, value: '%s'\n", token, yytext);
    }*/
}