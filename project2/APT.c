/*
 * Annotated Parse Tree
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "APT.h"

AnnotatedParseTreeNode* newAnnotatedParseNode(char* name, int cnt, ...) {
    AnnotatedParseTreeNode* node = (AnnotatedParseTreeNode*)malloc(sizeof(AnnotatedParseTreeNode));
    node->name = name;
    node->left = NULL;
    node->right = NULL;
    node->is_token = 0;

    va_list childs;
    if (cnt < 0) return node; // fault
    va_start(childs, cnt);

    AnnotatedParseTreeNode* child = va_arg(childs, AnnotatedParseTreeNode*);
    node->left = child;
    node->lineno = child->lineno;
    for(int i = 0; i < cnt - 1; ++i) {
        child->right = va_arg(childs, AnnotatedParseTreeNode*);
        child = child->right;
    }

    va_end(childs);
    return node;
}

AnnotatedParseTreeNode* newAnnotatedParseLeafNode(char* name, int lineno) {
    AnnotatedParseTreeNode* node = (AnnotatedParseTreeNode*)malloc(sizeof(AnnotatedParseTreeNode));
    node->lineno = lineno;
    node->name = name;
    node->left = NULL;
    node->right = NULL;
    node->is_token = 1;
    
    #ifdef VERBOSE
    printf("<%s, L%d>: %s\n", name, lineno, yytext);
    #endif

    if (!strcmp(node->name, "INT")) {
        node->int_value = atoi(yytext);
    } else if (!strcmp(node->name, "HEXINT")) {
        node->int_value = strtol(yytext, NULL, 16);
    } else if (!strcmp(node->name, "FLOAT")) {
        node->float_value = atof(yytext);
    } else {
        char* tmp = (char *)malloc(sizeof(char) * strlen(yytext));
        strcpy(tmp, yytext);
        node->string_value = tmp;
    }
    return node;
}

void printAnnotatedParseTree(AnnotatedParseTreeNode* apt, int indent) {
    if (apt == NULL) return;
    if (strcmp(apt->name, "Epsilon") != 0) {
        if (indent > 0) printf("%*c", indent * 2, ' '); // print indent using 2 spaces

        printf("%s", apt->name);

        if (apt->lineno == -1) {
            printf("\n");
            return;
        }

        if (!strcmp(apt->name, "ID") || !strcmp(apt->name, "TYPE") ||
            !strcmp(apt->name, "CHAR"))
            printf(": %s\n", apt->string_value);
        else if (!strcmp(apt->name, "INT"))
            printf(": %d\n", apt->int_value);
        else if (!strcmp(apt->name, "HEX_INT"))
            printf(": %d\n", apt->int_value);
        else if (!strcmp(apt->name, "FLOAT"))
            printf(": %f\n", apt->float_value);
        else if (apt->is_token == 0)
            printf(" (%d)\n", apt->lineno);
        else
            printf("\n");
    }
    
    printAnnotatedParseTree(apt->left, indent + 1);
    printAnnotatedParseTree(apt->right, indent);
}