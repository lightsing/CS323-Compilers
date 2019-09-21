/*
 * Annotated Parse Tree
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "APT.h"

AnnotatedParseTreeNode* newAnnotatedParseNode(char* name, int cnt, ...) {
    AnnotatedParseTreeNode* node = (AnnotatedParseTreeNode*)malloc(sizeof(AnnotatedParseTreeNode));
    node->name = name;
    node->left = NULL;
    node->right = NULL;

    va_list childs;
    if (cnt < 0) return NULL; // fault
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

    // TODO: parse value from yytext
    
    return node;
}