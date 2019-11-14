/*
 * Semantic Analysis
 */
#ifndef _LT_SEMANTIC_H
#define _LT_SEMANTIC_H

#include <stdint.h>

#include "APT.h"
#include "type.h"

void parseProgram(Node program);
void parseExtDefList(Node ext_def_list);
void parseExtDef(Node ext_def);
Type* parseSpecifier(Node specifier);
FieldList* parseDefList(Node def_list, FieldList* field, uint8_t instruct);
FieldList* parseDef(Node def, FieldList* field, uint8_t instruct);
FieldList* parseDecList(Node dec_list, FieldList* field, Type* type, uint8_t instruct);
FieldList* parseDec(Node dec, FieldList* field, Type* type, uint8_t instruct);
FieldList* parseVarDec(Node var_dec, Type* type, uint8_t instruct);
FieldList* parseVarList(Node var_dec_list, FieldList* field);
FieldList* parseParamDec(Node param_dec, FieldList* field);
Type* parseExp(Node exp);
Type* parseFunDec(Node fun_dec, Type* lasttype);
void parseCompSt(Node compst, Type* returntype);
void parseStmt(Node stmt, Type* returntype);
void parseStmtList(Node stmt_list, Type* returntype);
void parseExtDecList(Node ext_dec_list, Type* type);
uint8_t sameType(Type* type1, Type* type2);

#endif