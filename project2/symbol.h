/*
 * Symbol Table
 */
#ifndef _LT_SYMBOL_H
#define _LT_SYMBOL_H

#include "uthash/uthash.h"
#include "type.h"

typedef struct Symbol {
    char* name;
    Type* type;
    UT_hash_handle hh; /* makes this structure hashable */
} Symbol;

extern Symbol* symbols;

// Create a new symbol table.
void createSymbolTable();

// Free a symbol table, note that it is your responsibility to free the keys.
void freeSymbolTable();

void insertSymbol(Symbol* symbol);

// Lookup a key in table.
// Returns value if key is known.
// Returns NULL if key is not known.
Symbol* findSymbol(const char *key);

// Call callback for every known key.
Symbol* symbolTableForeach(void (*callback)(void *key, void *value));

#endif