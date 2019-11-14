#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "semantic.h"
#include "symbol.h"

/*
%type <AptNode> Program ExtDefList ExtDef ExtDecList;
%type <AptNode> Specifier StructSpecifier;
%type <AptNode> VarDec FunDec VarList ParamDec;
%type <AptNode> CompSt StmtList Stmt;
%type <AptNode> DefList Def DecList Dec;
%type <AptNode> Exp Args;
*/

extern FILE* fout;

uint32_t indent;

void parseProgram(Node program) {
    #ifdef VERBOSE
    fprintf(fout, "parse <Program>\n"); ++indent;
    #endif
    // Program: ExtDefList
    parseExtDefList(program->left);
    #ifdef VERBOSE
    --indent;
    #endif
}

void parseExtDefList(Node ext_def_list) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <ExtDefList>\n"); ++indent;
    #endif
    // ExtDefList:
    //     Epsilon
    if (!strcmp(ext_def_list->name, "Epsilon")) return;
    //     ExtDef ExtDefList
    if (!strcmp(ext_def_list->left->name, "ExtDef")) {
        Node ext_def = ext_def_list->left;
        parseExtDef(ext_def);
        parseExtDefList(ext_def->right);
    }
    #ifdef VERBOSE
    --indent;
    #endif
}

void parseExtDef(Node ext_def) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <ExtDef>\n"); ++indent;
    #endif
    // ExtDef:
    Node specifier = ext_def->left;

    if (specifier == NULL || specifier->right == NULL) return;

    Type* spec_type = parseSpecifier(specifier);

    Node child = specifier->right;
    if (!strcmp(child->name, "ExtDecList")) {
    //     Specifier ExtDecList SEMI
        Node ext_def_list = child;
        parseExtDecList(ext_def_list, spec_type);
    } else if (!strcmp(child->name, "FunDec")) {
    // |   Specifier FunDec CompSt  
        Node fun_dec = child;
        Node comp_st = fun_dec->right;
        Type* fun_dec_type = parseFunDec(fun_dec, spec_type);

        if(fun_dec_type != NULL){
            char* func_name = fun_dec_type->structure->name;
            Symbol* func_symbol = findSymbol(func_name);
            if(func_symbol != NULL) {
                fprintf(fout, "Error type 4 at Line %d: function is redefined \"%s\"\n",
                        fun_dec->lineno, func_symbol->name);
            } else {
                func_symbol = (Symbol*) malloc(sizeof(Symbol));
                char* buf = (char*) malloc(sizeof(char) * strlen(func_name));
                strcpy(buf, func_name);
                func_symbol->name = buf;
                func_symbol->type = fun_dec_type;
                insertSymbol(func_symbol);

                parseCompSt(comp_st, spec_type);
            }
		}
    } else if (!strcmp(child->name, "SEMI")) {
    // |   Specifier SEMI
    }
    #ifdef VERBOSE
    --indent;
    #endif
}

Type* parseSpecifier(Node specifier) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <Specifier>\n"); ++indent;
    #endif
    // Specifier:
    Type* type = NULL;
    Node child = specifier->left;
    if (!strcmp(child->name, "TYPE")) {
    //     TYPE
        type = (Type*) malloc(sizeof(Type));
        type->category = PRIMITIVE;
        if (!strcmp(child->string_value, "int")) {
            type->primitive = _INT;
        }
        else if (!strcmp(child->string_value, "float")) {
            type->primitive = _FLOAT;
        }
    } else if (!strcmp(child->name, "StructSpecifier")) {
    // |   StructSpecifier
        #ifdef VERBOSE
        fprintf(fout, "%*c", indent * 2, ' ');
        fprintf(fout, "parse <StructSpecifier>\n"); ++indent;
        #endif
        // StructSpecifier:
        Node _struct = child->left;
        Node struct_id = _struct->right;
        char* struct_name = struct_id->string_value;

        Symbol* struct_symbol = findSymbol(struct_name);
        if (struct_id->right != NULL) {
        //     STRUCT ID LC DefList RC
            if (struct_symbol != NULL) {
                fprintf(fout, "Error type 15 at Line %d: redefine the same structure type \"%s\"\n",
                        struct_id->lineno, struct_name);
                indent -= 2;
                return NULL;
            }

            Node def_list = struct_id->right->right;

            type = (Type*) malloc(sizeof(Type));
            type->category = STRUCTURE;
            type->structure = parseDefList(def_list, NULL, 1);
            
            struct_symbol = (Symbol*) malloc(sizeof(Symbol));
            char* buf = (char*) malloc(sizeof(char) * strlen(struct_name));
            strcpy(buf, struct_name);
            struct_symbol->name = buf;
            struct_symbol->type = type;
            insertSymbol(struct_symbol);
        } else {
        // |   STRUCT ID (Not declaration)
            if (struct_symbol == NULL) {
                fprintf(fout, "Error type 16 at Line %d: struct is used without definition \"%s\"\n",
                        struct_id->lineno, struct_name);
                indent -= 2;
                return NULL;
            }
            if (struct_symbol->type->category != STRUCTURE) {
                fprintf(fout, "Error type 17 at Line %d: \"%s\" is not a struct\n",
                        struct_id->lineno, struct_name);
                indent -= 2;
                return NULL;
            }
            type = struct_symbol->type;
        }
        #ifdef VERBOSE
        --indent;
        #endif
    }
    #ifdef VERBOSE
    --indent;
    #endif
    return type;
}

FieldList* parseDefList(Node def_list, FieldList* field, uint8_t instruct) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <Specifier>\n"); ++indent;
    #endif
    // DefList: 
    //     Def DefList
    // |
    if(!strcmp(def_list->name, "Epsilon")){
        #ifdef VERBOSE
        --indent;
        #endif
        return field;
	}

    Node def = def_list->left;
    if (field == NULL) {
        field = parseDef(def, field, instruct);
    } else {
        field->next = parseDef(def, field, instruct);
    }
    parseDefList(def->right, field, instruct);
    #ifdef VERBOSE
    --indent;
    #endif
    return field;
}

FieldList* parseDef(Node def, FieldList* field, uint8_t instruct) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <Def>\n"); ++indent;
    #endif
    // Def:
    //     Specifier DecList SEMI
    Node specifier = def->left;
    Node dec_list = specifier->right;

    Type* type = parseSpecifier(specifier);
    field = parseDecList(dec_list, field, type, instruct);

    #ifdef VERBOSE
    --indent;
    #endif
	return field;
}

FieldList* parseDecList(Node dec_list, FieldList* field, Type* type, uint8_t instruct) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <DecList>\n"); ++indent;
    #endif
    // DecList: 
    //     Dec
    // |   Dec COMMA DecList

    Node dec = dec_list->left;

    field = parseDec(dec, field, type, instruct);
    if (dec->right != NULL) {
    // |   Dec COMMA DecList
        field = parseDecList(dec->right->right, field, type, instruct);
    }
    #ifdef VERBOSE
    --indent;
    #endif
    return field;
}

FieldList* parseDec(Node dec, FieldList* field, Type* type, uint8_t instruct) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <Dec>\n"); ++indent;
    #endif
    // Dec: 
    //     VarDec
    // |   VarDec ASSIGN Exp
    Node var_dec = dec->left;
    FieldList* var_dec_fields = parseVarDec(var_dec, type, instruct);

    if(var_dec->right != NULL) {
        // TODO: maybe initialization in struct
        Node exp = var_dec->right->right;
        Type* exp_type = parseExp(exp);
        if(!sameType(type, exp_type)) {
            fprintf(fout, "Error type 5 at Line %d: unmatching types on both sides of assignment operator\n",
                    var_dec->lineno);
        }
    }
    if(var_dec_fields != NULL) {
        FieldList* temp = (FieldList*) malloc(sizeof(FieldList));
        temp->name = var_dec_fields->name;
        temp->type = var_dec_fields->type;
        temp->next = field;
    }
    #ifdef VERBOSE
    --indent;
    #endif
    return var_dec_fields;
}

FieldList* parseVarDec(Node var_dec, Type* type, uint8_t instruct) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <VarDec>\n"); ++indent;
    #endif
    // VarDec:
    Node child = var_dec->left;
    //     ID
    // |   VarDec LB INT RB
    // |   VarDec LB HEX_INT RB 
    Type* lasttype = type;
	FieldList* result = (FieldList*) malloc(sizeof(FieldList));
	Node next;

	while(!strcmp(child->name, "VarDec"))
	{
		Type* newtype = (Type*) malloc(sizeof(Type));
        newtype->array = (Array*) malloc(sizeof(Array)); // IMPORTANT: initalize Array pointer
		newtype->category = ARRAY;
		next = child->right->right;
        
		newtype->array->size = next->int_value;
		newtype->array->base = lasttype;
		lasttype = newtype;
		child = child->left; // Goto ID
	}

    result->name = child->string_value;
	result->type = lasttype;
	result->next = NULL;
    
    Symbol* var_symbol = findSymbol(result->name);

    if (var_symbol != NULL) {
        fprintf(fout, "Error type 3 at Line %d: variable is redefined in the same scope \"%s\"\n",
                child->lineno, result->name);
	}

    var_symbol = (Symbol*) malloc(sizeof(Symbol));
    char* buf = (char*) malloc(sizeof(char) * strlen(result->name));
    strcpy(buf, result->name);
    var_symbol->name = buf;
    var_symbol->type = result->type;
    insertSymbol(var_symbol);

    #ifdef VERBOSE
    --indent;
    #endif
    return result;
}

FieldList* parseVarList(Node var_dec_list, FieldList* field) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <VarList>\n"); ++indent;
    #endif
    // VarList:
    //     ParamDec COMMA VarList
    // |   ParamDec
    Node param_dec = var_dec_list->left;

    field = parseParamDec(param_dec, field);
    if (param_dec->right != NULL) {
        field = parseVarList(param_dec->right->right, field);
    }
    #ifdef VERBOSE
    --indent;
    #endif
    return field;
}

FieldList* parseParamDec(Node param_dec, FieldList* field) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <ParamDec>\n"); ++indent;
    #endif
    // ParamDec:
    //     Specifier VarDec
    Type* spec = parseSpecifier(param_dec->left);
    if(spec != NULL) {
        FieldList* temp = parseVarDec(param_dec->left->right, spec, 0);
        field->next = temp;
        field = temp;
    }
    #ifdef VERBOSE
    --indent;
    #endif
    return field;
}

Type* parseExp(Node exp) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <Exp>\n"); ++indent;
    #endif
    // Exp:
    Node child = exp->left;
    Type* type = (Type*) malloc(sizeof(Type));
    if (!strcmp(child->name, "Exp")) {
        if (!strcmp(child->right->name, "ASSIGN")) {
    //     Exp ASSIGN Exp
            Node exp1 = child;
            Node exp2 = child->right->right;
            Type* exp1_type = parseExp(exp1);
            Type* exp2_type = parseExp(exp2);
            if (!((!strcmp(exp1->left->name, "ID") && exp1->left->right == NULL) ||
                  (!strcmp(exp1->left->name, "Exp") && !strcmp(exp1->left->right->name, "DOT")) ||
                  (!strcmp(exp1->left->name, "Exp") && !strcmp(exp1->left->right->name, "LB")))) {
                fprintf(fout, "Error type 6 at Line %d: rvalue on the left side of assignment operator\n", child->lineno);
            } else if (!sameType(exp1_type, exp2_type)) {
                fprintf(fout, "Error type 5 at Line %d: unmatching types on both sides of assignment operator\n", child->lineno);
            } else {
                type = exp1_type;
            }
        } else if (!strcmp(child->right->name, "AND") ||
                   !strcmp(child->right->name, "OR")) {
    // |   Exp AND Exp
    // |   Exp OR Exp
            Node exp1 = child;
            Node exp2 = child->right->right;
            Type* exp1_type = parseExp(exp1);
            Type* exp2_type = parseExp(exp2);
            if (!sameType(exp1_type, exp2_type) &&
                exp1_type->category == PRIMITIVE &&
                exp1_type->primitive == _INT) {
                fprintf(fout, "Error type 7 at Line %d: unmatching operands\n", child->lineno);
            } else {
                type = exp1_type;
            }
        } else if (!strcmp(child->right->name, "LT") ||
                   !strcmp(child->right->name, "LE") ||
                   !strcmp(child->right->name, "GT") ||
                   !strcmp(child->right->name, "GE") ||
                   !strcmp(child->right->name, "NE") ||
                   !strcmp(child->right->name, "EQ") ||
                   !strcmp(child->right->name, "PLUS") ||
                   !strcmp(child->right->name, "MINUS") ||
                   !strcmp(child->right->name, "MUL") ||
                   !strcmp(child->right->name, "DIV")) {
    // |   Exp LT Exp
    // |   Exp LE Exp
    // |   Exp GT Exp
    // |   Exp GE Exp
    // |   Exp NE Exp
    // |   Exp EQ Exp
    // |   Exp PLUS Exp
    // |   Exp MINUS Exp
    // |   Exp MUL Exp
    // |   Exp DIV Exp
            Node exp1 = child;
            Node exp2 = child->right->right;
            Type* exp1_type = parseExp(exp1);
            Type* exp2_type = parseExp(exp2);
            if (!sameType(exp1_type, exp2_type) &&
                exp1_type->category == PRIMITIVE &&
                (exp1_type->primitive == _INT || exp1_type->primitive == _FLOAT)) {
                fprintf(fout, "Error type 7 at Line %d: unmatching operands\n", child->lineno);
            } else {
                type = exp1_type;
            }
        } else if (!strcmp(child->right->name, "LB")) {
    // |   Exp LB Exp RB
            Node exp1 = child;
            Node exp2 = child->right->right;
            Type* exp1_type = parseExp(exp1);
            Type* exp2_type = parseExp(exp2);
			if (exp1_type->category != ARRAY) {
				fprintf(fout, "Error type 10 at Line %d: applying indexing operator "
                              "on non-array type variables\n",
                        child->lineno);
			} else if (!(exp2_type->category == PRIMITIVE && exp2_type->primitive == _INT)) {
                fprintf(fout, "Error type 12 at Line %d: array indexing with non-integer type expression\n",
                        child->lineno);
                type = exp1_type->array->base;
            } else {
                type = exp1_type->array->base;
            }
        } else if (!strcmp(child->right->name, "DOT")) {
    // |   Exp DOT ID
            Node exp1 = child;
            Node member_id = child->right->right;
            Type* exp1_type = parseExp(exp1);
            if (exp1_type->category != STRUCTURE) {
				fprintf(fout, "Error type 13 at Line %d: accessing member of non-structure variable\n",
                        child->lineno);
			} else {
                FieldList* field;
				for(field = exp1_type->structure; field != NULL; field = field->next) {
					if(!strcmp(field->name, member_id->string_value)) break;
                }
				if(field == NULL) {
					fprintf(fout, "Error type 14 at Line %d: accessing an undefined structure member \"%s\".\n",
                            member_id->lineno, member_id->string_value);
				} else {
					type = field->type;
				}
			}
        }
    } else if (!strcmp(child->name, "LP")) { 
    // |   LP Exp RP
        type = parseExp(child->right);
    } else if (!strcmp(child->name, "MINUS")) {
    // |   MINUS Exp
        Type* exp_type = parseExp(child->right);
        if(exp_type->category != PRIMITIVE) {
            fprintf(fout, "Error type 7 at Line %d: unmatching operands\n", child->lineno);
        } else {
            type = exp_type;
        }
    } else if (!strcmp(child->name, "NOT")) {
    // |   NOT Exp
        Type* exp_type = parseExp(child->right);
        if(!(exp_type->category == PRIMITIVE || exp_type->primitive == _INT)) {
            fprintf(fout, "Error type 7 at Line %d: unmatching operands\n", child->lineno);
        } else {
            type->category = PRIMITIVE;
            type->primitive = _INT;
        }
    } else if (!strcmp(child->name, "ID")) {
        Symbol* id_symbol = findSymbol(child->string_value);
        if (child->right != NULL) {
    // |   ID LP Args RP
    // |   ID LP RP
            if (id_symbol == NULL) {
				fprintf(fout, "Error type 1 at Line %d: function is invoked without definition \"%s\"\n",
                        child->lineno, child->string_value);
                #ifdef VERBOSE
                --indent;
                #endif
                return NULL;
			} else if (id_symbol->type->category != FUNCTION) {
                fprintf(fout, "Error type 11 at Line %d: applying function invocation "
                              "operator on non-function names \"%s\"\n",
                        child->lineno, child->string_value);
                #ifdef VERBOSE
                --indent;
                #endif
                return NULL;
            }
            Type* func_type = id_symbol->type;
            if (!strcmp(child->right->right->name, "Args")) {
                Node args = child->right->right;
                FieldList* args_type = func_type->structure->next;
                if (args_type == NULL) {
                    fprintf(fout, "Error type 9 at Line %d: the function "
                                  "%s’s arguments mismatch the declared parameters (too many)\n",
                                  child->lineno, child->string_value);
                } else {
                    // Args: Exp COMMA Args
                    //   |   Exp
                    Node exp = args->left;
                    while (1) {
                        Type* exp_type = parseExp(exp);
                        if (exp_type != NULL) {
                            if(!sameType(exp_type, args_type->type)) {
                                fprintf(fout, "Error type 9 at Line %d: the function "
                                        "%s’s arguments mismatch the declared parameters (type mismatch)\n",
                                        child->lineno, child->string_value);
                                break;
                            } else {
                                args_type = args_type->next;
                                if (args_type == NULL && exp->right == NULL) {
                                    type = func_type->structure->type;
                                    break;
                                } else if (args_type != NULL && exp->right == NULL) {
                                    fprintf(fout, "Error type 9 at Line %d: the function "
                                            "%s’s arguments mismatch the declared parameters (too few)\n",
                                            child->lineno, child->string_value);
                                    break;
                                } else if (args_type == NULL && exp->right != NULL) {
                                    fprintf(fout, "Error type 9 at Line %d: the function "
                                            "%s’s arguments mismatch the declared parameters (too many)\n",
                                            child->lineno, child->string_value);
                                    break;
                                }
                                exp = exp->right->right->left;
                            }
                        }
                    }
                }
            } else {
                if(func_type->structure->next == NULL) {
                    type = func_type->structure->type;
                } else {
                    fprintf(fout, "Error type 9 at Line %d: the function "
                                  "\"%s\"’s arguments mismatch the declared parameters (too few)\n",
                            child->lineno, child->string_value);
                }
            }
        } else {
    // |   ID
			if (id_symbol == NULL) {
				fprintf(fout, "Error type 1 at Line %d: variable is used without definition \"%s\".\n",
                        child->lineno, child->string_value);
			} else {
				type = id_symbol->type;
			}
        }
    } else if (!strcmp(child->name, "INT") ||
               !strcmp(child->name, "HEX_INT")) {
    // |   INT
    // |   HEX_INT
        type->category = PRIMITIVE;
		type->primitive = _INT;
    } else if(!strcmp(child->name, "FLOAT")) {
    // |   FLOAT
        type->category = PRIMITIVE;
		type->primitive = _FLOAT;
    } else if(!strcmp(child->name, "CHAR")) {
    // |   CHAR
        type->category = PRIMITIVE;
		type->primitive = _CHAR;
    }
    #ifdef VERBOSE
    --indent;
    #endif
    return type;
}

Type* parseFunDec(Node fun_dec, Type* lasttype) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <FunDec>\n"); ++indent;
    #endif
    // FunDec:
    //     ID LP VarList RP
    // |   ID LP RP

    Type* type = (Type*) malloc(sizeof(Type));
    type->structure = (FieldList*) malloc(sizeof(FieldList));

	Node func_id = fun_dec->left;
	type->category = FUNCTION;
	type->structure->name = func_id->string_value;
	type->structure->type = lasttype;
	FieldList* field = type->structure;
	
    Node child = func_id->right->right;
	if(!strcmp(child->name, "RP")) {
    //    ID LP RP
		field->next = NULL;
	} else {
    //    ID LP VarList RP
		parseVarList(child, field);
	}

    #ifdef VERBOSE
    --indent;
    #endif
    return type;
}

void parseCompSt(Node compst, Type* returntype) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <CompSt>\n"); ++indent;
    #endif
    // CompSt: 
    //     LC DefList StmtList RC

    Node def_list = compst->left->right;
    FieldList* def_list_field = parseDefList(def_list, NULL, 0);
    parseStmtList(def_list->right, returntype);

    #ifdef VERBOSE
    --indent;
    #endif
}

void parseStmtList(Node stmt_list, Type* returntype) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <StmtList>\n"); ++indent;
    #endif

    // StmtList: 
    //     Stmt StmtList
    // |
    if (stmt_list->left != NULL) {
        Node stmt = stmt_list->left;
        parseStmt(stmt, returntype);
        parseStmtList(stmt->right, returntype);
    }

    #ifdef VERBOSE
    --indent;
    #endif
}

void parseStmt(Node stmt, Type* returntype) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <Stmt>\n"); ++indent;
    #endif
    // Stmt:
    Node child = stmt->left;
	if(!strcmp(child->name, "Exp")) {
    //     Exp SEMI
        parseExp(child);
    } else if(!strcmp(child->name, "CompSt")) {
    // |   CompSt
		parseCompSt(child, returntype);
	} else if(!strcmp(child->name, "RETURN")) {
    // |   RETURN Exp SEMI
        Node exp = child->right;
		Type* exp_type = parseExp(exp);
		if (exp_type != NULL && !sameType(returntype, exp_type)) {
			fprintf(fout, "Error type 8 at Line %d: the function’s return "
                          "value type mismatches the declared type\n",
                    child->lineno);
		}
	} else if(!strcmp(child->name, "IF")) {
    // |   IF LP Exp RP Stmt
    // |   IF LP Exp RP Stmt ELSE Stmt
        Node exp = child->right->right;
		Type* exp_type = parseExp(exp);
		if(exp_type != NULL) {
            if(!(exp_type->category == PRIMITIVE &&
                 exp_type->primitive == _INT)) {
                fprintf(fout, "Error type 7 at Line %d: unmatching operands\n", exp->lineno);
            } else {
                Node stmt = exp->right->right;
                parseStmt(stmt, returntype);
                if(stmt->right != NULL) parseStmt(stmt->right->right, returntype);
            }
		}
	} else if(!strcmp(child->name, "WHILE")) {
    // |   WHILE LP Exp RP Stmt 
		Node exp = child->right->right;
		Type* exp_type = parseExp(exp);
		if(exp_type != NULL) {
			if(!(exp_type->category == PRIMITIVE &&
                 exp_type->primitive == _INT)) {
                fprintf(fout, "Error type 7 at Line %d: unmatching operands\n", exp->lineno);
            } else {
                Node stmt = exp->right->right;
                parseStmt(stmt, returntype);
            }
		}
	}

    #ifdef VERBOSE
    --indent;
    #endif
}

void parseExtDecList(Node ext_dec_list, Type* type) {
    #ifdef VERBOSE
    fprintf(fout, "%*c", indent * 2, ' ');
    fprintf(fout, "parse <ExtDecList>\n"); ++indent;
    #endif
    // ExtDecList:
    Node var_dec = ext_dec_list->left;
    parseVarDec(var_dec, type, 0);
    if (var_dec->right != NULL) {
    // |   VarDec COMMA ExtDecList
        parseExtDecList(var_dec->right->right, type);
    }
    //     VarDec
    #ifdef VERBOSE
    --indent;
    #endif
}

uint8_t sameType(Type* type1, Type* type2) {
    if (type1 == NULL || type2 == NULL) {
        return 0;
    }
    if (type1->category == type2->category) {
        if (type1->category == PRIMITIVE && type1->primitive == type2->primitive) {
            return 1;
        } else if (type1->category == ARRAY &&
            sameType(type1->array->base, type2->array->base) &&
            type1->array->size == type2->array->size) {
            return 1;
        } else if(type1->category == STRUCTURE || type1->category == FUNCTION) {
			FieldList* field1 = type1->structure;
			FieldList* field2 = type2->structure;
			while(field1 != NULL && field2 != NULL)
			{
				if(!sameType(field1->type, field2->type)) {
					return 0;
                }
				field1 = field1->next;
				field2 = field2->next;
			}
			if(field1 != NULL || field2 != NULL) {
				return 0;
            }
			return 1;
		}
    }
    return 0;
}