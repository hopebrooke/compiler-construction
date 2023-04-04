#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"

typedef enum {STATIC , FIELD , ARG , VAR , NONE} Kind;  // enumerated types

typedef struct symbol {
    char name[128];
    char type[128];
    Kind kind;
    int index;
    struct symbol* next;
} symbol;

symbol* classTable;
symbol* subroutineTable;

int Constructor();          // Creates new empty symbol tables
int startSubroutine();      // Starts new subroutine scope (resets subroutines symbol table)
int Define(char* name, char* type, Kind kind);  // Assigns new identifier of given name, type and kind, assigns it running index
                            // STATIC/FIELD -> class scope, ARG/VAR -> subroutine scope
int VarCount(Kind kind);    // Returns num of variables of given kind in current scope
Kind KindOf(char* name);    // Returns kind of named identifier in current scope, if doesn't exist, returns none
char* TypeOf(char* name);   // Returns type of named identifier in current scope
int IndexOf(char* name);    // Returns index assigned to named identifier




#endif
