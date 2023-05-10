
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

extern undeclared undeclaredTable[128];
extern int utCount;
extern classTable programTable[128];
extern int ptCount;
extern symbol subroutineTable[128];
extern int stCount;



// Initialise class symbol table
int Constructor() {
    ptCount = 0;
    stCount = 0;
    utCount = 0;
	
    return 1;
}

int newClass(char * name) {
    strcpy(programTable[ptCount].name, name);
    programTable[ptCount].ctCount = 0; 
    ptCount ++;
    return 0;
}

// Empty subroutine table
int startSubroutine() {
    stCount = 0;
    return 0;
}


// Define new symbol
int Define(char* name, char* type, Kind kind, int index, char args[50][128], char argTypes[50][128]){
    if ((kind==2) || (kind==3)) {
        // subroutine table
        subroutineTable[stCount].kind = kind;
        strcpy(subroutineTable[stCount].name, name);
        strcpy(subroutineTable[stCount].type, type);
        if(index == -1) {
            subroutineTable[stCount].index = VarCount(kind);
        } else {
            subroutineTable[stCount].index = index;
        }
        subroutineTable[stCount].vf = 0;
        subroutineTable[stCount].args[0][0] = '\0';
        subroutineTable[stCount].argTypes[0][0] = '\0';
        subroutineTable[stCount].vars = 0;
        stCount ++;
    } else {
        // class table 
        if((kind==0) || (kind==1)) {
            programTable[ptCount-1].classTable[programTable[ptCount-1].ctCount].vf = 0;
            programTable[ptCount-1].classTable[programTable[ptCount-1].ctCount].args[0][0] = '\0';
            programTable[ptCount-1].classTable[programTable[ptCount-1].ctCount].argTypes[0][0] = '\0';
            programTable[ptCount-1].classTable[programTable[ptCount-1].ctCount].index = VarCount(kind);
        } else {
            programTable[ptCount-1].classTable[programTable[ptCount-1].ctCount].vf = 1;
            programTable[ptCount-1].classTable[programTable[ptCount-1].ctCount].index = index;
            for (int i=0; i<10; i++){
                strcpy(programTable[ptCount-1].classTable[programTable[ptCount-1].ctCount].args[i], args[i]);
                strcpy(programTable[ptCount-1].classTable[programTable[ptCount-1].ctCount].argTypes[i], argTypes[i]);
            }
        }
        programTable[ptCount-1].classTable[programTable[ptCount-1].ctCount].kind = kind;
        programTable[ptCount-1].classTable[programTable[ptCount-1].ctCount].vars = 0; 
        strcpy(programTable[ptCount-1].classTable[programTable[ptCount-1].ctCount].name, name);
        strcpy(programTable[ptCount-1].classTable[programTable[ptCount-1].ctCount].type, type);
        programTable[ptCount-1].ctCount ++;
    }
    return 0;
}

int search (char* name, Kind kind){
    // int exists = 0;
    if((kind==2) || (kind ==3)) {
        for( int i =0; i <stCount; i++){
            if(!strcmp(subroutineTable[i].name, name)){
                return 1;
            }
        }
    } else {
        for(int i = 0; i < programTable[ptCount-1].ctCount; i++){
            if(!strcmp(programTable[ptCount-1].classTable[i].name, name)){
                return 1;
            }
        }
    }
    return 0;
}

// Count number of that kind
int VarCount(Kind kind){
    int count = 0;
    if((kind==0) || (kind==1)){
        for(int i=0; i < programTable[ptCount-1].ctCount; i++) {
            if(programTable[ptCount-1].classTable[i].kind == kind){
                count ++;
                // printf("%i\n", count);
            }  
        }
    } else {
        for(int i=0; i < stCount; i++) {
            if(subroutineTable[i].kind == kind)
                count ++;
        }
    }
    return count;
}

int addUndec(Token one, Token two, int count) {
    int found=0;
    for(int i=0; i<utCount; i++) {
        if(!strcmp(one.lx, undeclaredTable[i].first.lx)){
            if(count == 2) {
                if(!strcmp(two.lx, undeclaredTable[i].second.lx)){
                    found = 1;
                }
            } else {
                found = 1;
            }
        }
    }
    if( found == 0){
        undeclaredTable[utCount].first = one;
        undeclaredTable[utCount].second = two;
        undeclaredTable[utCount].count = count;
        utCount ++;
    }
    return 0;
}

// Returns the kind of symbol of named identifier
Kind KindOf(char * name) {
    Kind kind = NONE;
    // First search subroutine table:
    for(int i=0; i<stCount; i++){
        if(!strcmp(subroutineTable[i].name, name)){
            return subroutineTable[i].kind;
        }
    }
    // If not found in subroutine, repeat for class
    // First search subroutine table:
    for(int i=0; i<programTable[ptCount-1].ctCount; i++){
        if(!strcmp(programTable[ptCount-1].classTable[i].name, name)){
            return programTable[ptCount-1].classTable[i].kind;
        }
    }
    // Return kind
    return kind;
}


symbol FindSymbol(char * className, char * name) {
    symbol sym;
    strcpy(sym.name, " ");
    for(int i=0; i<ptCount; i++) {
        if(!strcmp(programTable[i].name, className)) {
            for(int j=0; j<programTable[i].ctCount; j++){
                if(!strcmp(programTable[i].classTable[j].name, name)) {
                    return programTable[i].classTable[j];
                }
            }
        }
    }
    return sym;
}

// Returns type of named identifier
char* TypeOf(char* name){
    char * type = "-1";
    // First search subroutine table:
    for(int i=0; i<stCount; i++){
        if(!strcmp(subroutineTable[i].name, name)){
            return subroutineTable[i].type;
        }
    }
    
    // If not found in subroutine, repeat for class
    // First search subroutine table:
    for(int i=0; i<programTable[ptCount-1].ctCount; i++){
        if(!strcmp(programTable[ptCount-1].classTable[i].name, name)){
            return programTable[ptCount-1].classTable[i].type;
        }
    }
    // Return type
    return type;
}


// Returns index of named identifier
int IndexOf(char* name){
    int index = -1;
   // First search subroutine table:
    for(int i=0; i<stCount; i++){
        if(!strcmp(subroutineTable[i].name, name)){
            return subroutineTable[i].index;
        }
    }
    
    // If not found in subroutine, repeat for class
    // First search subroutine table:
    for(int i=0; i<programTable[ptCount-1].ctCount; i++){
        if(!strcmp(programTable[ptCount-1].classTable[i].name, name)){
            return programTable[ptCount-1].classTable[i].index;
        }
    }
    // Return type
    return index;
}

int classExists(char* name){
    for(int i=0; i<ptCount; i++){
        if(!strcmp(programTable[i].name, name)){
            return 1;
        }
    }
    return 0;
}

ParserInfo checkUndec() {
    
    ParserInfo p;
    p.er = none;
    
    // Loop through undeclared:
    for( int i=0; i<utCount; i++ ){
        if(undeclaredTable[i].count == 1){
            int found = 0;
            int exists = classExists(undeclaredTable[i].first.lx);
            if(exists) {
                found = 1;
            }
            
            if( classExists(undeclaredTable[i].second.lx)){
                for(int j=0; j<ptCount; j++){
                    if(!strcmp(programTable[j].name, undeclaredTable[i].second.lx)) {
                        for(int k=0; k<programTable[j].ctCount; k++) {
                            if(!strcmp(programTable[j].classTable[k].name, undeclaredTable[i].first.lx)){
                                found = 1;
                                break;     
                            }
                        }
                    }
                    if(found == 1){
                        break;
                    }
                }               
            } if( found == 0){
                p.er = undecIdentifier;
                p.tk = undeclaredTable[i].first;
                return p;
            }

        } else {
            int found = 0;
            // Check if a class:
            if(classExists(undeclaredTable[i].first.lx)) {
                // If a class, check that second token is a function or constructor
                for(int j=0; j<ptCount; j++){
                    if(!strcmp(programTable[j].name, undeclaredTable[i].first.lx)) {
                        for(int k=0; k<programTable[j].ctCount; k++) {
                            if(!strcmp(programTable[j].classTable[k].name, undeclaredTable[i].second.lx)){
                                found = 1;
                                break;
                            
                            }
                        }
                    }
                    if(found == 1){
                        break;
                    }
                }
                if( found == 0){
                    p.tk = undeclaredTable[i].second;
                    p.er = undecIdentifier;
                    return p;
                }
            } else {
                // If not class, then should be object
                char * objectType;

        
                objectType = TypeOf(undeclaredTable[i].first.lx);

                
                for(int j=0; j<ptCount; j++){
                    if(!strcmp(programTable[j].name, objectType)) {
                        for(int k=0; k<programTable[j].ctCount; k++) {
                            if(!strcmp(programTable[j].classTable[k].name, undeclaredTable[i].second.lx)){
                                found = 1;
                                break;
                            }
                        }
                    }
                    if(found == 1) {
                        break;
                    }
                }
                if( found == 0){
                    p.tk = undeclaredTable[i].second;
                    p.er = undecIdentifier;
                    return p;
                }

            }

        }
    }
    return p;
}
// int main() {

//     // Initialise
//     printf("Initialising constructor \n");
//     Constructor();

//     int index = 0;
//     char args[10][128] = {"0", "0", "0", "0","0", "0","0", "0","0", "0"};
//     char argTypes[10][128] = {"0", "0", "0", "0","0", "0","0", "0","0", "0"};
//     char args1[10][128] = {"bob", "amy", "jeff", "happy","0", "0","0", "0","0", "0"};
//     char argTypes1[10][128] = {"char", "int", "string", "lemon","0", "0","0", "0","0", "0"};
    
//     // Define some kinds
//     Kind class1 = STATIC;
//     Kind class2 = FIELD;
//     Kind sub1 = ARG;
//     Kind sub2 = VAR;
//     Kind class3 = CONSTRUCTOR;
//     Kind class4 = FUNCTION;
//     Kind class5 = METHOD;
    
//     newClass("full");
//     // Add some class identifiers
//     Define("classvar1", "String", class1, index, args, argTypes);
//     Define("classvar2", "String", class2,index, args, argTypes);
//     Define("classVar3", "String", class1,index, args, argTypes);
//     Define("classVar4", "String", class2,index, args, argTypes);
//     Define("classvar5", "String", class1,index, args, argTypes);
//     Define("classvar6", "void", class3, 2, args1, argTypes1);
//     Define("classvar7", "int", class4, 4, args1, argTypes1);
    
//     newClass("empty");
    
//     printf("Printing program table: \n");
//     for(int i=0; i<ptCount; i++) {
//         printf("Name: %s, Count: %i \n", programTable[i].name, programTable[i].ctCount);
        
//     }
//     printf("Printing %s class table: \n", programTable[0].name);
//     printf("symbols in class: %i\n", programTable[0].ctCount);
//     for(int i=0; i<programTable[0].ctCount; i++) {
//         printf("%s, %s, %i, %i \n", programTable[0].classTable[i].name, programTable[0].classTable[i].type, programTable[0].classTable[i].kind, programTable[0].classTable[i].index);
//         if(programTable[0].classTable[i].vf == 1) {
//             for(int j=0; j<programTable[0].classTable[i].index; j++){
//                 printf("%s, %s\n", programTable[0].classTable[i].args[j], programTable[0].classTable[i].argTypes[j] );
//             }
//         }
//     }
//     printf("Printing %s class table: \n", programTable[1].name);
//     for(int i=0; i<programTable[1].ctCount; i++) {
//         printf("%s, %s, %i, %i \n", programTable[1].classTable[i].name, programTable[1].classTable[i].type, programTable[1].classTable[i].kind, programTable[1].classTable[i].index);
//         if(programTable[1].classTable[i].vf == 1) {
//             for(int j=0; j<programTable[1].classTable[i].index; i++){
//                 printf("%s, %s\n", programTable[1].classTable[i].args[j], programTable[1].classTable[i].argTypes[j] );
//             }
//         }
//     }

//     // Start a subroutine
//     startSubroutine();
//     Define("subvar1", "String", sub1,index, args, argTypes);
//     Define("subvar2", "String", sub1,index, args, argTypes);
//     Define("subvar3", "String", sub2,index, args, argTypes);

//     printf("Printing subroutine table \n");
//     for(int i=0; i<stCount; i++) {
//         printf("%s, %s, %i, %i \n", subroutineTable[i].name, subroutineTable[i].type, subroutineTable[i].kind, subroutineTable[i].index);
//     }

//     // Create new subroutine table
//     // Start a subroutine
//     startSubroutine();
//     Define("newsubvar1", "String", sub2,index, args, argTypes);
//     Define("newsubvar2", "String", sub1,index, args, argTypes);
//     Define("newsubvar3", "String", sub2,index, args, argTypes);

//     printf("Printing new subroutine table \n");
//     for(int i=0; i<stCount; i++) {
//         printf("%s, %s, %i, %i \n", subroutineTable[i].name, subroutineTable[i].type, subroutineTable[i].kind, subroutineTable[i].index);
//     }

    
//     // printf("Printing class table: \n");
//     // for(int i=0; i<ctCount; i++) {
//     //     printf("%s, %s, %i, %i \n", classTable[i].name, classTable[i].type, classTable[i].kind, classTable[i].index);
        
//     // }

//     return 1;

// }
