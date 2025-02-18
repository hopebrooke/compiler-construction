#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"

typedef enum {STATIC , FIELD , ARG , VAR , CONSTRUCTOR , FUNCTION , METHOD , NONE} Kind;  // enumerated types

typedef struct symbol {
    char name[128]; 
    int vf;        // whether symbol is variable (0) or function (1)
    char type[128]; // data type for variable, return type (type/void) for function/method
    Kind kind;      // Kind of varibale/function
    int index; // Index for variables, num of arguments for functions
    int vars;
    char args[50][128];
    char argTypes[50][128];
} symbol;

typedef struct undeclared {
    Token first;
    Token second;
    int count;
} undeclared;

typedef struct classTable {
    char name[128];
    symbol classTable[128];
    int ctCount;
} classTable;

int Constructor();          // Creates new empty symbol tables
int newClass(char* name);   // Add class to program table
int startSubroutine();      // Starts new subroutine scope (resets subroutines symbol table)
int Define(char* name, char* type, Kind kind, int index, char args[50][128], char argTypes[50][128]);  // Assigns new identifier of given name, type and kind, assigns it running index
                            // STATIC/FIELD -> class scope, ARG/VAR -> subroutine scope
int search(char* name, Kind kind);
int VarCount(Kind kind);    // Returns num of variables of given kind in given class
Kind KindOf(char* name);    // Returns kind of named identifier in current scope, if doesn't exist, returns none
char* TypeOf(char* name);   // Returns type of named identifier in current scope
int IndexOf(char* name);    // Returns index assigned to named identifier
int classExists(char *name);
int addUndec(Token, Token, int);
ParserInfo checkUndec();
symbol FindSymbol(char * className, char * name);

#endif
