#ifndef COMPILER_H
#define COMPILER_H

// #define TEST_COMPILER    // uncomment to run the compiler autograder

#include "parser.h"
#include "symbols.h"

typedef enum {CONST, ARGU, LOC, STAT, THIS, THAT, POINTER, TEMP} Segment;
typedef enum {ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT} Command;


int InitCompiler ();
ParserInfo compile (char* dir_name);
int StopCompiler();
int writePush(Segment, int);    // Writes a push command
int writePop(Segment, int);     // Writes a pop command
int writeArithmetic(Command);   // Writes an arithmetic command
int writeLabel(char*);  
int writeGoto(char*);  
int writeIf(char*); 
int writeCall(char*, int); 
int writeFunction(char*, int); 
int writeReturn();

#endif
