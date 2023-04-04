
/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Symbol Tables Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Hope Brooke
Student ID: 201438799
Email: hopembrooke@gmail.com
Date Work Commenced: 03/04/2023
*************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbols.h"

// Need to create list of all identifiers and attributes
// Must facilitate searching for a symbol in the tables
// And inserting a symbol into the table


// Initialise class symbol table
int Constructor() {
    classTable = NULL;
    subroutineTable = NULL;
    return 1;
}


// Empty subroutine table
int startSubroutine() {
    symbol * symbols = subroutineTable;
    // Go through all freeing
    while(symbols != NULL){
        symbol* next = symbols->next;
        free(symbols);
        symbols = next;
    }
    subroutineTable = NULL;
}


// Define new symbol
int Define(char* name, char* type, Kind kind) {

    symbol * newSymbol;

 
    // Check for scope
    if((kind==0) || (kind==1)){
        newSymbol = classTable;

    } else {
        newSymbol = subroutineTable;

    }

    // Search through scope table for variable to make sure it doesn't already exist
    while(newSymbol!=NULL){
    
        if(strcmp(newSymbol->name, name) == 0){
     
            printf("symbol already defined in scope");
            return 0; //return 0 for above error
 
        }
        newSymbol = newSymbol->next;
  
    }


    // If no error, create symbol
    newSymbol = (symbol *) malloc(sizeof(symbol));

    strcpy(newSymbol->name, name);
    strcpy(newSymbol->type, type);
    newSymbol->kind = kind;

    // Work out index of symbol
    int num = VarCount(kind);
    newSymbol->index = num;


    // Add symbol to table
    if((newSymbol->kind == 0) || (newSymbol->kind ==1)){
        newSymbol->next = classTable;
        classTable=newSymbol;
    } else {
        newSymbol->next = subroutineTable;
        subroutineTable=newSymbol;
    }
}


// Count number of that kind
int VarCount(Kind kind){
    symbol * symbols;
    int count = 0;
    if((kind==0) || (kind==1)){
        symbols = classTable;
    } else {
        symbols = subroutineTable;
    }
    while(symbols != NULL){
        if(symbols->kind == kind ){
            count ++;
        }
        symbols = symbols->next;
    }
    return count;
}

// Returns the kind of symbol of named identifier
Kind KindOf(char * name){
    // Search linked list for name
    symbol * kindSymbol = subroutineTable;
    
    Kind kind = NONE;

    // First search subroutine table if not null:
    while(kindSymbol != NULL) {
        if(strcmp(name, kindSymbol->name) == 0){
            kind = kindSymbol->kind;
            return kind;
        }
        kindSymbol = kindSymbol->next;
    }
    
    // If not found in subroutine, repeat for class
    kindSymbol = classTable;
    while(kindSymbol != NULL) {
        if(strcmp(name, kindSymbol->name) == 0){
            kind = kindSymbol->kind;
            return kind;
        }
        kindSymbol = kindSymbol->next;
    }

    // Return kind
    return kind;

}


// Returns type of named identifier
char* TypeOf(char* name){
    // Search linked list for name
    symbol * typeSymbol = subroutineTable;
    
    char* type = NULL;

    // First search subroutine table if not null:
    while(typeSymbol != NULL) {
        if(strcmp(name, typeSymbol->name) == 0){
            strcpy(type, typeSymbol->type);
            return type;
        }
        typeSymbol = typeSymbol->next;
    }
    
    // If not found in subroutine, repeat for class
    typeSymbol = classTable;
    while(typeSymbol != NULL) {
        if(strcmp(name, typeSymbol->name) == 0){
            strcpy(type, typeSymbol->type);
            return type;
        }
        typeSymbol = typeSymbol->next;
    }
    // Return type
    return type;
}



// Returns index of named identifier
int IndexOf(char* name){
    // Search linked list for name
    symbol * symbol = subroutineTable;
    
    int index = -1;

    // First search subroutine table if not null:
    while(symbol != NULL) {
        if(strcmp(name, symbol->name) == 0){
            index = symbol->index;
            return index;
        }
        symbol = symbol->next;
    }
    
    // If not found in subroutine, repeat for class
    symbol = classTable;
    while(symbol != NULL) {
        if(strcmp(name, symbol->name) == 0){
            index = symbol->index;
            return index;
        }
        symbol = symbol->next;
    }
    // Return type
    return index;
}



int main() {

    // Initialise
    printf("Initialising constructor \n");
    Constructor();

    // Define some kinds
    Kind class1 = STATIC;
    Kind class2 = FIELD;
    Kind sub1 = ARG;
    Kind sub2 = VAR;
    
    // Add some class identifiers
    Define("classvar1", "String", class1);
    Define("classvar2", "String", class2);
    Define("classVar3", "String", class1);
    Define("classVar4", "String", class2);
    Define("classvar5", "String", class1);
    
    printf("Printing class table: \n");
    symbol * table = classTable;
    while(table != NULL){
        printf("%s, %s, %i, %i \n", table->name, table->type, table->kind, table->index);
        table = table-> next;
    }

    // Start a subroutine
    startSubroutine();
    Define("subvar1", "String", sub1);
    Define("subvar2", "String", sub1);
    Define("subvar3", "String", sub2);

    printf("Printing subroutine table \n");
    table = subroutineTable;
    while(table != NULL){
        printf("%s, %s, %i, %i \n", table->name, table->type, table->kind, table->index);
        table = table-> next;
    }

    // Create new subroutine table
    // Start a subroutine
    startSubroutine();
    Define("newsubvar1", "String", sub2);
    Define("newsubvar2", "String", sub1);
    Define("newsubvar3", "String", sub2);

    printf("Printing new subroutine table \n");
    table = subroutineTable;
    while(table != NULL){
        printf("%s, %s, %i, %i \n", table->name, table->type, table->kind, table->index);
        table = table-> next;
    }
    
    printf("Printing class table: \n");
    table = classTable;
    while(table != NULL){
        printf("%s, %s, %i, %i \n", table->name, table->type, table->kind, table->index);
        table = table-> next;
    }

    return 1;

}
// VARIABLES:
// Should be declared before using
// Should be initialised before using it's value
// Scope resolution

// TYPE CHECKING:
// RHS vs. LHS types must be compatable
// Corecion/casting
// Expressions used as array indices must be integers

// FUNCTION CALLING:
// Has to be declared before called
// Must have same number and type of declarations as declaration
// Returns value compatable with return type
// All paths in a function should return a value

// OTHER:
// Deal with unreachable code


