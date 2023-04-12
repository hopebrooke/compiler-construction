#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"

typedef enum {STATIC , FIELD , ARG , VAR , CONSTRUCTOR , FUNCTION , METHOD , NONE} Kind;  // enumerated types
// typedef enum {INT, FLOAT, CHAR, VOID, NONE } Type;

typedef struct symbol {
    char name[128]; 
    int vf;        // whether symbol is variable (0) or function (1)
    char type[128]; // data type for variable, return type (type/void) for function/method
    Kind kind;      // Kind of varibale/function
    int index; // Index for variables, num of arguments for functions
    // memory size?

    // list of arguments and return types:
    char args[10][128];
    char argTypes[10][128];
} symbol;

typedef struct table {
    char name[128];
    symbol classTable[128];
    int ctCount;
    //memmory size needed?
} classTable;


classTable programTable[128];
// Program table is an array of class tables
int ptCount;
// ptCount is num of class tables in program
// class table contains a name, an array of symbols, 
// and a count of symbols
// symbol classTable[128];
// int ctCount;
symbol subroutineTable[128];
int stCount;

int Constructor();          // Creates new empty symbol tables
int newClass(char* name);   // Add class to program table
int startSubroutine();      // Starts new subroutine scope (resets subroutines symbol table)
int Define(char* name, char* type, Kind kind, int index, char args[10][128], char argTypes[10][128]);  // Assigns new identifier of given name, type and kind, assigns it running index
                            // STATIC/FIELD -> class scope, ARG/VAR -> subroutine scope
int VarCount(Kind kind);    // Returns num of variables of given kind in current scope
Kind KindOf(char* name);    // Returns kind of named identifier in current scope, if doesn't exist, returns none
char* TypeOf(char* name);   // Returns type of named identifier in current scope
int IndexOf(char* name);    // Returns index assigned to named identifier




#endif
