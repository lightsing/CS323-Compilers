/*
 * Type Struct
 */
#ifndef _LT_TYPE_H
#define _LT_TYPE_H

typedef struct Type {
    char name[32];
    enum { PRIMITIVE, ARRAY, STRUCTURE, FUNCTION} category;
    union {
        enum { _INT, _FLOAT, _CHAR } primitive;
        struct Array *array;
        struct FieldList *structure;
    };
} Type;

typedef struct Array {
    struct Type *base;
    int size;
} Array;

typedef struct FieldList {
    char* name;
    struct Type *type;
    struct FieldList *next;
} FieldList;

#endif