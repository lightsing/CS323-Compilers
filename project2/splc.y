%{
    #include "APT.h"
    #include "semantic.h"
    #include "lex.yy.c"

    FILE* fout = NULL;

    #define MISSING_SEMI_ERROR(e) { \
        ++errors; \
        fprintf(fout, "Error type B at Line %d: Missing semicolon ';'\n", e->lineno); \
    }

    #define MISSING_RP_ERROR(e) { \
        ++errors; \
        fprintf(fout, "Error type B at Line %d: Missing closing parenthesis ')'\n", e->lineno); \
    }

    uint32_t errors = 0;

    void yyerror (const char*);
%}

%union {
    AnnotatedParseTreeNode* AptNode;
}

%token <AptNode> INT     // /* integer in 32-bits (decimal or hexadecimal) */
%token <AptNode> HEX_INT
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
%token <AptNode> ERROR   // Lexical error
//%token <AptNode> NEWLINE

%type <AptNode> Program ExtDefList ExtDef ExtDecList;
%type <AptNode> Specifier StructSpecifier;
%type <AptNode> VarDec FunDec VarList ParamDec;
%type <AptNode> CompSt StmtList Stmt;
%type <AptNode> DefList Def DecList Dec;
%type <AptNode> Exp Args;

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
        ExtDefList    { 
                $$ = newAnnotatedParseNode("Program", 1, $1);
                if (errors == 0) {
                    //printAnnotatedParseTree($$, 0);
                    parseProgram($$);
                }
            }
    ;
ExtDefList: 
        ExtDef ExtDefList    { $$ = newAnnotatedParseNode("ExtDefList", 2, $1, $2); }
    |      { $$ = newAnnotatedParseNode("Epsilon", -1); }
    ;
ExtDef:
        Specifier ExtDecList SEMI    { $$ = newAnnotatedParseNode("ExtDef", 3, $1, $2, $3); }
    |   Specifier SEMI    { $$ = newAnnotatedParseNode("ExtDef", 2, $1, $2); }
    |   Specifier FunDec CompSt    { $$ = newAnnotatedParseNode("ExtDef", 3, $1, $2, $3); }
    |   Specifier error {
        $$ = newAnnotatedParseNode("ExtDef", 1, $1);
        MISSING_SEMI_ERROR($1);
    }
    |   Specifier ExtDecList error {
        $$ = newAnnotatedParseNode("ExtDef", 2, $1, $2);
        MISSING_SEMI_ERROR($1);
    }
    |   error SEMI {
        ++errors;
    }
    ;
ExtDecList:
        VarDec    { $$ = newAnnotatedParseNode("ExtDecList", 1, $1); }
    |   VarDec COMMA ExtDecList    { $$ = newAnnotatedParseNode("ExtDecList", 3, $1, $2, $3); }
    ;

    /* specifier */
Specifier:
        TYPE    { $$ = newAnnotatedParseNode("Specifier", 1, $1); }
    |   StructSpecifier    { $$ = newAnnotatedParseNode("Specifier", 1, $1); }
    ;
StructSpecifier:
        STRUCT ID LC DefList RC    { $$ = newAnnotatedParseNode("StructSpecifier", 5, $1, $2, $3, $4, $5); }
    |   STRUCT ID    { $$ = newAnnotatedParseNode("StructSpecifier", 2, $1, $2); }
    ;

    /* declarator */
VarDec: 
        ID    { $$ = newAnnotatedParseNode("VarDec", 1, $1); }
    |   ERROR {
        ++errors;
        $$ = newAnnotatedParseNode("VarDec", 1, $1);
    }
    |   VarDec LB INT RB    { $$ = newAnnotatedParseNode("VarDec", 3, $1, $2, $3, $4); }
    |   VarDec LB HEX_INT RB    { $$ = newAnnotatedParseNode("VarDec", 3, $1, $2, $3, $4); }
    ;
FunDec:
        ID LP VarList RP    { $$ = newAnnotatedParseNode("FunDec", 4, $1, $2, $3, $4); }
    |   ID LP RP    { $$ = newAnnotatedParseNode("FunDec", 3, $1, $2, $3); }
    |   ID LP VarList error    {
        $$ = newAnnotatedParseNode("FunDec", 3, $1, $2, $3);
        MISSING_RP_ERROR($3);
    }
    |   ID LP error    {
        $$ = newAnnotatedParseNode("FunDec", 2, $1, $2);
        MISSING_RP_ERROR($2);
    }
    ;
VarList:
        ParamDec COMMA VarList    { $$ = newAnnotatedParseNode("VarList", 3, $1, $2, $3); }
    |   ParamDec    { $$ = newAnnotatedParseNode("VarList", 1, $1); }
    ;
ParamDec:
        Specifier VarDec    { $$ = newAnnotatedParseNode("ParamDec", 2, $1, $2); }
    ;

    /* statement */
CompSt: 
        LC DefList StmtList RC    { $$ = newAnnotatedParseNode("CompSt", 4, $1, $2, $3, $4); }
    ;
StmtList: 
        Stmt StmtList    { $$ = newAnnotatedParseNode("StmtList", 2, $1, $2); }
    |      { $$ = newAnnotatedParseNode("Epsilon", -1); }
    ;
Stmt:
        Exp SEMI    { $$ = newAnnotatedParseNode("Stmt", 2, $1, $2); }
    |   CompSt    { $$ = newAnnotatedParseNode("Stmt", 1, $1); }
    |   RETURN Exp SEMI    { $$ = newAnnotatedParseNode("Stmt", 3, $1, $2, $3); }
    |   IF LP Exp RP Stmt    { $$ = newAnnotatedParseNode("Stmt", 5, $1, $2, $3, $4, $5); }
    |   IF LP Exp RP Stmt ELSE Stmt    { $$ = newAnnotatedParseNode("Stmt", 7, $1, $2, $3, $4, $5, $6, $7); }
    |   WHILE LP Exp RP Stmt    { $$ = newAnnotatedParseNode("Stmt", 5, $1, $2, $3, $4, $5); }
    |   Exp error   {
        $$ = newAnnotatedParseNode("Stmt", 1, $1);
        MISSING_SEMI_ERROR($1);
    }
    |   RETURN Exp error    {
        $$ = newAnnotatedParseNode("Stmt", 2, $1, $2);
        MISSING_SEMI_ERROR($2);
    }
    |   IF LP Exp error Stmt    { 
        $$ = newAnnotatedParseNode("Stmt", 4, $1, $2, $3, $5);
        MISSING_RP_ERROR($3);
    }
    |   WHILE LP Exp error Stmt    {
        $$ = newAnnotatedParseNode("Stmt", 3, $1, $2, $3, $5);
        MISSING_RP_ERROR($3);
    }
    ;

    /* local definition */
DefList: 
        Def DefList   { $$ = newAnnotatedParseNode("DefList", 2, $1, $2); }
    |      { $$ = newAnnotatedParseNode("Epsilon", -1); }
    ;
Def:
        Specifier DecList SEMI    { $$ = newAnnotatedParseNode("Def", 3, $1, $2, $3); }
    |   Specifier DecList error    {
        $$ = newAnnotatedParseNode("Def", 2, $1, $2);
        MISSING_SEMI_ERROR($1);
    }
    ;
DecList: 
        Dec    { $$ = newAnnotatedParseNode("DecList", 1, $1); }
    |   Dec COMMA DecList    { $$ = newAnnotatedParseNode("DecList", 3, $1, $2, $3); }
    ;
Dec: 
        VarDec    { $$ = newAnnotatedParseNode("Dec", 1, $1); }
    |   VarDec ASSIGN Exp    { $$ = newAnnotatedParseNode("Dec", 3, $1, $2, $3); }
    ;
    /* Expression */
Exp:  
        Exp ASSIGN Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp AND Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp OR Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp LT Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp LE Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp GT Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp GE Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp NE Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp EQ Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp PLUS Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp MINUS Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp MUL Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp DIV Exp    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   LP Exp RP    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   MINUS Exp    { $$ = newAnnotatedParseNode("Exp", 2, $1, $2); }
    |   NOT Exp    { $$ = newAnnotatedParseNode("Exp", 2, $1, $2); }
    |   ID LP Args RP    { $$ = newAnnotatedParseNode("Exp", 4, $1, $2, $3, $4); }
    |   ID LP RP    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   Exp LB Exp RB    { $$ = newAnnotatedParseNode("Exp", 4, $1, $2, $3, $4); }
    |   Exp DOT ID    { $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); }
    |   ID    { $$ = newAnnotatedParseNode("Exp", 1, $1); }
    |   INT    { $$ = newAnnotatedParseNode("Exp", 1, $1); }
    |   HEX_INT    { $$ = newAnnotatedParseNode("Exp", 1, $1); }
    |   FLOAT    { $$ = newAnnotatedParseNode("Exp", 1, $1); }
    |   CHAR    { $$ = newAnnotatedParseNode("Exp", 1, $1); }
    |   Exp ERROR Exp   {
        ++errors;
        $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3);
    }
    |   ERROR   {
        ++errors;
        $$ = newAnnotatedParseNode("Exp", 1, $1);
    }
    |   LP Exp error    {
        $$ = newAnnotatedParseNode("Exp", 2, $1, $2);
        MISSING_RP_ERROR($2);
    }
    |   ID LP Args error    {
        $$ = newAnnotatedParseNode("Exp", 3, $1, $2, $3); 
        MISSING_RP_ERROR($3);
    }
    |   ID LP error    {
        $$ = newAnnotatedParseNode("Exp", 3, $1, $2);
        MISSING_RP_ERROR($2);
    }
    ;
Args: Exp COMMA Args    { $$ = newAnnotatedParseNode("Args", 3, $1, $2, $3); }
    |   Exp    { $$ = newAnnotatedParseNode("Args", 1, $1); }
    ;
%%
void
yyerror (char const *s)
{
  #ifdef VERBOSE
  fprintf (stderr, "ERROR: Line %d, %s \"%s\"\n", yylineno, s, yytext);
  #endif
}

int
main (int argc, char **argv)
{
    #ifdef DEBUG
    fout = stdout;
    #else
    char* tmp = (char *)malloc(sizeof(char) * (strlen(argv[1]) + 3));
    strcpy(tmp, argv[1]);
    char *end = tmp + strlen(tmp);

    while (end > tmp && *end != '.') {
        --end;
    }
    if (end > tmp) {
        *(end + 1)= 'o';
        *(end + 2)= 'u';
        *(end + 3)= 't';
    }
    fout = fopen(tmp, "w");
    printf("%s\n", tmp);
    #endif

    yyin = fopen(argv[1], "r");
    #ifdef VERBOSE
    yydebug = 1;
    #endif
    yyparse();
    /*while(1) {
        int token = yylex();
        if (token == 0) break;
        printf("Token: %d, value: '%s'\n", token, yytext);
    }*/
    #ifndef DEBUG
    fclose(fout);
    #endif
}