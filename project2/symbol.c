#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"

Symbol* symbols = NULL;

// Create a new symbol table.
void createSymbolTable() {
    symbols = (Symbol*)malloc(sizeof(Symbol));
}

// Free a symbol table, note that it is your responsibility to free the keys.
void freeSymbolTable() {
    free(symbols);
}

void insertSymbol(Symbol* symbol) {
    HASH_ADD_STR(symbols, name, symbol);
}

// Lookup a key in table.
// Returns value if key is known.
// Returns NULL if key is not known.
Symbol* findSymbol(const char *name) {
    Symbol* symbol;
    HASH_FIND_STR(symbols, name, symbol);
    return symbol;
}