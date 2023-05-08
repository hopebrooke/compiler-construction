#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "symbols.h"
#include "compiler.h"

// Make parameter list struct:
typedef struct param {
	char args[25][128];
	char argTypes[25][128];
	int num;
} param;

undeclared undeclaredTable[128];
int utCount;
classTable programTable[128];
int ptCount;
symbol subroutineTable[128];
int stCount;
int compileNum;


// Parser Function Declarations
void memberDeclar();
void classVarDeclar();
void type();
void subroutineDeclar();
param paramList();
void subroutineBody();
void statement();
void varDeclarStatement();
void letStatement();
void ifStatement();
void whileStatement();
void doStatement();
void subroutineCall();
int expressionList();
void returnStatement();
void expression();
void relationalExpression();
void arithmeticExpression();
void term();
void factor();
void operand();

// Initialise global ParserInfo struct
ParserInfo status;
char currentClass[128];
int ifNum;
int whileNum;

// Function for member declaration checking
void memberDeclar()
{
	Token t = PeekNextToken();
	if(t.tp == 6) {
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	//___________ CLASSVAR ___________
	if(t.tp == 0 && (!strcmp(t.lx, "static") || !strcmp(t.lx, "field"))){
		classVarDeclar();
		if(status.er != 0) return;
	}
	//___________ SUBROUTINE ___________
	else if(t.tp == 0 && (!strcmp(t.lx, "constructor") || !strcmp(t.lx, "function") || !strcmp(t.lx, "method"))){
		subroutineDeclar();
		if(status.er != 0) return;
	}
	//___________ ERROR ___________
	else {
		status.er = memberDeclarErr;
		status.tk = t;
	}
}


// Function for class variable declaration
void classVarDeclar() 
{
	Token t = GetNextToken();
	if(t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	Kind kind;
	//___________ FIELD/STATIC ___________
	if(!strcmp(t.lx, "field")){ kind = FIELD;}
	else if(!strcmp(t.lx, "static")){kind = STATIC;}
	else {
		status.er = classVarErr;
		status.tk = t;
		return;
	}

	t = PeekNextToken();
	char typeSymbol[128];
	strcpy(typeSymbol, t.lx);

	//___________ TYPE ___________
	type();
	if(status.er != 0) return;

	//___________ IDENTIFIER ___________
	t = GetNextToken();
	if(t.tp == 6) {
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if(t.tp == 1){
		// Check if the identifier already exists
		int exists = search(t.lx, kind);
		if(exists && (compileNum == 0)) {
			status.er = redecIdentifier;
			status.tk = t;
			return;
		} else if ((kind == 2) || (kind == 3) || (compileNum == 0)) {
			char empty[10][128];
			Define(t.lx, typeSymbol, kind, -1, empty, empty);
		} 
	}
	else {
		status.er = idExpected;
		status.tk = t;
		return;
	}

	//_______ {, IDENTIFIER} ________
	t = PeekNextToken();
	int loop = 0;
	if((t.tp == 3) && (t.lx[0] == ',')) loop = 1;
	while(loop) {
		// Discard comma
		t = GetNextToken();
		t = GetNextToken();
		if(t.tp == 6){
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		if(t.tp == 1){
			// Check if the identifier already exists
			int exists = search(t.lx, kind);
			if(exists && (compileNum == 0)) {
				status.er = redecIdentifier;
				status.tk = t;
			} else if( (kind == 2) || (kind == 3) || (compileNum == 0)) {
				char empty[10][128];
				Define(t.lx, typeSymbol, kind, -1, empty, empty);
			}
		}
		else {
			status.er = idExpected;
			status.tk = t;
			return;
		}
		// Check for another comma
		t = PeekNextToken();
		if((t.tp == 3) && (t.lx[0] == ','));
		else loop = 0;
	}

	//_______ ; ________
	t = GetNextToken();
	if(t.tp == 6) {
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if((t.tp == 3) && (t.lx[0] == ';'));
	else {
		status.er = semicolonExpected;
		status.tk = t;
		return;
	}
}


// Function for type
void type() 
{
	Token t = GetNextToken();
	if(t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	//_______ID_______
	if(t.tp == 1) {
		// Check if identifier exists in classes
		int exists = classExists(t.lx);
		if( !exists && (compileNum==0)){
			// Add to undeclared id list
			Token two;
			addUndec(t, two, 1);
		}
	} else if ((t.tp == 0) && ((!strcmp(t.lx, "int") ||
		!strcmp(t.lx, "char") || !strcmp(t.lx, "boolean")))){
	} else {
		status.er = illegalType;
		status.tk = t;
	}
}


// Function for subroutineDeclar
void subroutineDeclar()
{
	ifNum = 0;
	whileNum = 0;
	Kind kind;
	char name[128];
	char typeSymbol[128];
	int index;
	char args[50][128];
	char argTypes[50][128];
	int vars = 0;

	//___________ CONSTRUCTOR/FUNCTION/METHOD ___________
	Token t = GetNextToken();
	if( t.tp == 6 ) {
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if (!strcmp(t.lx, "constructor")){kind = CONSTRUCTOR;} 
	else if (!strcmp(t.lx, "function")){kind = FUNCTION;} 
	else if (!strcmp(t.lx, "method")){kind = METHOD;} 
	else {
		status.er = subroutineDeclarErr;
		status.tk = t;
		return;
	}

	//______ TYPE OR VOID ___________
	t = PeekNextToken();
	strcpy(typeSymbol, t.lx);

	if(!(strcmp(t.lx, "void"))) t = GetNextToken();
	else {
		type();
		if ( status.er != 0) return;
	}

	//_______ IDENTIFIER _________
	t = GetNextToken();
	if( t.tp == 6 ) {
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( t.tp == 1) {
		// Check if the identifier already exists
		int exists = search(t.lx, kind);
		if(exists && (compileNum == 0)) {
			status.er = redecIdentifier;
			status.tk = t;
		} else {
			strcpy(name, t.lx);
		}
	}
	else {
		status.er = idExpected;
		status.tk = t;
		return;
	}

	//_______ ( __________
	t = GetNextToken();
	if( t.tp == 6 ) {
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '('));
	else {
		status.er = openParenExpected;
		status.tk = t;
		return;
	}

	//______ PARAMLIST ______
	param parameters = paramList();
	if( status.er != 0 ) return;

	// If no problem, copy args and argtypes
	for(int i=0; i<parameters.num; i++){
		strcpy(args[i], parameters.args[i]);
		strcpy(argTypes[i], parameters.argTypes[i]);
	}
	index = parameters.num;
	//_________ ) _________
	t = GetNextToken();
	if( t.tp == 6 ){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == ')'));
	else {
		status.er = closeParenExpected;
		status.tk = t;
		return;
	}
	// Add to symbol table:
	if ((kind == 2) || (kind == 3) ||(compileNum == 0)) { // removed : (kind == 2) || (kind == 3) || from if statement cos why?
		Define(name, typeSymbol, kind, index, args, argTypes);
	}
	char funcName[128] = "";
	strcat(funcName, currentClass);
	strcat(funcName, ".");
	strcat(funcName, name);
	// Get number of variables declared:
	// Find class:
	if( compileNum == 1) {
		for( int i=0; i<ptCount; i++){
			if(!strcmp(currentClass, programTable[i].name)){
				for(int j=0; j<programTable[i].ctCount; j++){
					if(!strcmp(name, programTable[i].classTable[j].name)){
						vars = programTable[i].classTable[j].vars;
					}
				}
			}
		}
	}
	writeFunction(funcName, vars);

	if( kind == CONSTRUCTOR) {
		int objSize = 0;
		for(int i=0; i<ptCount; i++) {
			if(!strcmp(currentClass, programTable[i].name)){
				for( int j=0; j<programTable[i].ctCount; j++){
					if(programTable[i].classTable[j].kind == FIELD) {
						objSize ++;
					}
				}
			}
		}
		writePush(CONST, objSize);
		writeCall("Memory.alloc", 1);
		writePop(POINTER, 0);
	} else if (kind == METHOD) {
		writePush(ARGU, 0);
		writePop(POINTER, 0);
	}

	// Start subroutine:
	startSubroutine();
	// Add parameters as arguments:
	char emptyArgs[50][128];
	for(int i=0; i<parameters.num; i++){
		if( kind == METHOD) {
			Define(parameters.args[i], parameters.argTypes[i], ARG, i+1, emptyArgs, emptyArgs);
		} else {
			Define(parameters.args[i], parameters.argTypes[i], ARG, i, emptyArgs, emptyArgs);
		}
	}
	
	
	//______ SUBROUTINEBODY ______
	subroutineBody();
	// Set num of local vars:
	if(compileNum == 0){
		vars = VarCount(VAR);
		programTable[ptCount-1].classTable[programTable[ptCount-1].ctCount-1].vars = vars;
	}
	if( status.er != 0 ) return;
}


// Function for parameter list
param paramList(){
	param parameters;
	parameters.num = 0;
	Token t = PeekNextToken();
	if( !strcmp(t.lx, "int") || !strcmp(t.lx, "char") || !strcmp(t.lx, "boolean") || (t.tp == 1)) {
		
		strcpy(parameters.argTypes[parameters.num], t.lx);
		//________ TYPE ____________
		type();
		if( status.er != 0) return parameters;

		//_______ IDENTIFIER ________
		t = GetNextToken();
		if( t.tp == 6 ) {
			status.er = lexerErr;
			status.tk = t;
			return parameters;
		}
		if( t.tp == 1 ) {
			strcpy(parameters.args[parameters.num], t.lx);
			parameters.num ++;
		}
		else {
			status.er = idExpected;
			status.tk = t;
			return parameters;
		}

		//_________ {, TYPE IDENTIFIER}_________
		t = PeekNextToken();
		int loop = 0;
		if((t.tp == 3) && (t.lx[0] == ',')) loop = 1;
		while( loop ){
			t = GetNextToken();
			if( t.tp == 6 ){
				status.er = lexerErr;
				status.tk = t;
				return parameters;
			}
			t = PeekNextToken();
			strcpy(parameters.argTypes[parameters.num], t.lx);
		
			//______ TYPE ______
			type();
			if( status.er != 0) return parameters;

			//______ IDENTIFIER ______
			t = GetNextToken();
			if( t.tp == 6 ){
				status.er = lexerErr;
				status.tk = t;
				return parameters;
			}
			if( t.tp == 1 ){
				strcpy(parameters.args[parameters.num], t.lx);
				parameters.num ++;
		
			}
			else {
				status.er = idExpected;
				status.tk = t;
				return parameters;
			}

			//______ , ______
			t = PeekNextToken();
			if( (t.tp == 3) && (t.lx[0] == ','));
			else loop = 0;
		}
	}
	// If no type, then just return to previous function
	return parameters;
}


// Function for subroutine body
void subroutineBody()
{
	Token t = GetNextToken();
	if( t.tp == 6 ){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	//______ { ______
	if( (t.tp == 3) && (t.lx[0] = '{'));
	else {
		status.er = openBraceExpected;
		status.tk = t;
		return;
	}

	//______ STATEMENTS ______
	int loop = 1;
	while( loop ) {
		t = PeekNextToken();
		// All tokens start with a reserved word
		if( t.tp == 0 ) {
			statement();
			if( status.er != 0 ) return;
		}
		else break;
	}

	//______ } ______
	t = GetNextToken();
	if( t.tp == 6) {
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '}'));
	else {
		status.er = closeBraceExpected;
		status.tk = t;
		return;
	}
}


// Function to check for statements
void statement() 
{
	Token t = PeekNextToken();
	//______ VARDECLAR STATEMENT ______
	if( (t.tp == 0) && !strcmp(t.lx, "var")){
		varDeclarStatement();
		if ( status.er != 0 ) return;
	}
	//______ LET STATEMENT ______
	else if( (t.tp == 0) && !strcmp(t.lx, "let")){
		letStatement();
		if ( status.er != 0 ) return;
	}
	//______ IF STATEMENT ______
	else if( (t.tp == 0) && !strcmp(t.lx, "if")){
		ifStatement();
		if ( status.er != 0 ) return;
	}
	//______ WHILE STATEMENT ______
	else if( (t.tp == 0) && !strcmp(t.lx, "while")){
		whileStatement();
		if ( status.er != 0 ) return;
	}
	//______ DO STATEMENT ______
	else if( (t.tp == 0) && !strcmp(t.lx, "do")){
		doStatement();
		if ( status.er != 0 ) return;
	}
	//______ RETURN STATEMENT ______
	else if( (t.tp == 0) && !strcmp(t.lx, "return")){
		returnStatement();
		if ( status.er != 0 ) return;
	}
	else {
		if( t.tp == 6 ) status.er = lexerErr;
		else status.er = syntaxError;
		status.tk = t;
		return;
	}
}


// Function for variable declaration statements
void varDeclarStatement()
{
	char typeSymbol[128];
	char args[50][128];
	Kind kind = VAR;

	Token t = GetNextToken();
	if ( t.tp == 6 ){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	//______ VAR ______
	if( ( t.tp == 0 ) && ( !strcmp(t.lx, "var")));
	else {
		status.er = syntaxError;
		status.tk = t;
		return;
	}

	t = PeekNextToken();
	strcpy(typeSymbol, t.lx);  
	//______ TYPE ______
	type();
	if( status.er != 0 ) return;

	//______ IDENTIFIER ______
	t = GetNextToken();
	if( t.tp == 6) {
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( t.tp == 1 ) {
		int exists = search(t.lx, kind);
		if(exists && (compileNum == 0)){
			status.er = redecIdentifier;
			status.tk = t;
			return;
		} else {
			Define(t.lx, typeSymbol, kind, -1, args, args);
		}
	}
	else {
		status.er = idExpected;
		status.tk = t;
		return;
	}

	//______ {, IDENTIFIER} ______
	t = PeekNextToken();
	int loop = 0;
	if( (t.tp== 3) && (t.lx[0] == ',')) loop = 1;
	while (loop){
		t = GetNextToken();

		//______ IDENTIFIER ______
		t = GetNextToken();
		if( t.tp == 6 ){
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		if( t.tp == 1 ){
			int exists = search(t.lx, kind);
			if(exists && (compileNum == 0)){
				status.er = redecIdentifier;
				status.tk = t;
				return;
			} else {
				Define(t.lx, typeSymbol, kind, -1, args, args);
			}
		}
		else {
			status.er = idExpected;
			status.tk = t;
			return;
		}

		//______ , ______
		t = PeekNextToken();
		if( (t.tp == 3) && (t.lx[0] == ','));
		else loop = 0;
	}

	//______ ; ______
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == ';'));
	else{
		status.er = semicolonExpected;
		status.tk = t;
		return;
	}
}


// Function for let statment
void letStatement()
{
	//______ LET ______
	Token t = GetNextToken();
	if( t.tp == 6 ) {
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 0) && (!strcmp(t.lx, "let"))) {}
	else {
		status.er = syntaxError;
		status.tk = t;
		return;
	}

	//______ IDENTIFIER ______
	t = GetNextToken();
	if( t.tp == 6 ){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	Kind kind;
	int index;
	if( t.tp == 1 ){
		//check if exists:

		symbol sym = FindSymbol(currentClass, t.lx);

		if(!strcmp(sym.name, " ")){
			// WORKS FOR LOCAL VARS/ARGS:
			index = IndexOf(t.lx);
			kind = KindOf(t.lx);
		} else {
			// WORKS FOR FIELD/STATIC:
			index = sym.index;
			kind = sym.kind;
		}
		
		if((index == -1) && (compileNum == 0)) {
			status.er = undecIdentifier;
			status.tk = t;
			return;
		}
	}
	else {
		status.er = idExpected;
		status.tk = t;
		return;
	}

	//______ [ ______
	t = PeekNextToken();
	if(t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '[')){
		t = GetNextToken();
		expression();
		if( status.er != 0 ) return;
		
		if(kind == STATIC) {
			writePush(STAT, index);
		} else if (kind == FIELD) {
			writePush(THIS, index);
		} else if (kind == ARG) {
			writePush(ARGU, index);
		} else if (kind == VAR) {
			writePush(LOC, index);
		}
		kind = NONE;
		writeArithmetic(ADD);
		//______ ] ______
		t = GetNextToken();
		if( t.tp == 6){
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		if( (t.tp == 3)&&(t.lx[0] == ']'));
		else {
			status.er = closeBracketExpected;
			status.tk = t;
			return;
		}
	}

	//______ = ______
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if((t.tp == 3)&& (t.lx[0] == '='));
	else {
		status.er = equalExpected;
		status.tk = t;
		return;
	}

	//______ EXPRESSION ______
	expression();
	if( status.er != 0 ) return;

	//______ ; ______
	t = GetNextToken();
	if( t.tp == 6 ){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	if( (t.tp == 3) && (t.lx[0] == ';'));
	else {
		status.er = semicolonExpected;
		status.tk = t;
		return;
	}


	if(kind == STATIC) {
		writePop(STAT, index);
	} else if (kind == FIELD) {
		writePop(THIS, index);
	} else if (kind == ARG) {
		writePop(ARGU, index);
	} else if (kind == VAR) {
		writePop(LOC, index);
	} else {
		writePop(TEMP, 0);
		writePop(POINTER, 1);
		writePush(TEMP, 0);
		writePop(THAT, 0);
	}
}


// Function for if statement
void ifStatement()
{	
	//______ IF ______
	Token t = GetNextToken();
	if ( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 0) && !strcmp(t.lx, "if"));
	else {
		status.er = syntaxError;
		status.tk = t;
		return;
	}

	//______ ( ______
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '('));
	else {
		status.er = openParenExpected;
		status.tk = t;
		return;
	}

	//______ EXPRESSION ______
	expression();
	if( status.er != 0 ) return;

	//______ ) ______
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == ')'));
	else {
		status.er = closeParenExpected;
		status.tk = t;
		return;
	}


	//______ { ______
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '{'));
	else {
		status.er = openBraceExpected;
		status.tk = t;
		return;
	}


	char ifTrue[128] = "IF_TRUE";
	char ifFalse[128] = "IF_FALSE";

	char numString[128];
	sprintf(numString, "%d", ifNum);
	strcat(ifTrue, numString);
	strcat(ifFalse, numString);

	writeIf(ifTrue);
	writeGoto(ifFalse);
	ifNum ++;

	writeLabel(ifTrue);

	//______ STATEMENT ______
	statement();
	if( status.er != 0 ) return;

	//______ {STATEMENT} ______
	t = PeekNextToken();
	int loop = 0;
	if( !strcmp(t.lx, "var") || !strcmp(t.lx, "let") || !strcmp(t.lx, "if") || !strcmp(t.lx, "while") 
				|| !strcmp(t.lx, "do") || !strcmp(t.lx, "return")) loop = 1;

	while (loop){
		statement();
		if( status.er != 0) return;
		t = PeekNextToken();
		if( !strcmp(t.lx, "var") || !strcmp(t.lx, "let") || !strcmp(t.lx, "if") || !strcmp(t.lx, "while") 
				|| !strcmp(t.lx, "do") || !strcmp(t.lx, "return"));
		else loop = 0;
	}

	//______ } ______
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '}'));
	else {
		status.er = closeBraceExpected;
		status.tk = t;
		return;
	}

	writeLabel(ifFalse);
	//______ [ELSE] ______
	t = PeekNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( !strcmp(t.lx, "else")){
		t = GetNextToken();

		//______ { ______
		t = GetNextToken();
		if( t.tp == 6){
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		if((t.tp == 3) && (t.lx[0] == '{'));
		else {
			status.er = openBraceExpected;
			status.tk = t;
			return;
		}

		//______ STATEMENT ______
		statement();
		if( status.er != 0 ) return;

		//______ {STATEMENT} ______
		t = PeekNextToken();
		int loop = 0;
		if( !strcmp(t.lx, "var") || !strcmp(t.lx, "let") || !strcmp(t.lx, "if") || !strcmp(t.lx, "while") 
				|| !strcmp(t.lx, "do") || !strcmp(t.lx, "return")) loop = 1;

		while (loop){
			statement();
			if( status.er != 0) return;
			t = PeekNextToken();
			if( !strcmp(t.lx, "var") || !strcmp(t.lx, "let") || !strcmp(t.lx, "if") || !strcmp(t.lx, "while") 
					|| !strcmp(t.lx, "do") || !strcmp(t.lx, "return"));
			else loop = 0;
		}

		//______ } ______
		t = GetNextToken();
		if( t.tp == 6){
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		if( (t.tp == 3) && (t.lx[0] == '}'));
		else {
			status.er = closeBraceExpected;
			status.tk = t;
			return;
		}
	}
	
}


// Function for while statement
void whileStatement()
{
	//______ WHILE ______
	Token t = GetNextToken();
	if ( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	char whileExpression[128] = "WHILE_EXP";
	char whileEnd[128] = "WHILE_END";

	char numString[128];
	sprintf(numString, "%d", whileNum);
	strcat(whileExpression, numString);
	strcat(whileEnd, numString);

	whileNum ++;

	if( (t.tp == 0) && !strcmp(t.lx, "while"));
	else {
		status.er = syntaxError;
		status.tk = t;
		return;
	}

	writeLabel(whileExpression);

	//______ ( ______
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '('));
	else {
		status.er = openParenExpected;
		status.tk = t;
		return;
	}

	//______ EXPRESSION ______
	expression();
	if( status.er != 0 ) return;

	//______ ) ______
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == ')'));
	else {
		status.er = closeParenExpected;
		status.tk = t;
		return;
	}
	writeArithmetic(NOT);
	writeIf(whileEnd);
	//______ { ______
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '{'));
	else {
		status.er = openBraceExpected;
		status.tk = t;
		return;
	}

	//_______ STATEMENT _______
	statement();
	if( status.er != 0 ) return;

	//_______ {STATEMENT} ______
	t = PeekNextToken();
	int loop = 0;
	if( !strcmp(t.lx, "var") || !strcmp(t.lx, "let") || !strcmp(t.lx, "if") || !strcmp(t.lx, "while") 
			|| !strcmp(t.lx, "do") || !strcmp(t.lx, "return")) loop = 1;
	
	while (loop){
		statement();
		if( status.er != 0) return;
		t = PeekNextToken();
		if( !strcmp(t.lx, "var") || !strcmp(t.lx, "let") || !strcmp(t.lx, "if") || !strcmp(t.lx, "while") 
				|| !strcmp(t.lx, "do") || !strcmp(t.lx, "return"));
		else loop = 0;
	}

	//______ } ______
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '}'));
	else {
		status.er = closeBraceExpected;
		status.tk = t;
		return;
	}
	writeGoto(whileExpression);
	writeLabel(whileEnd);

}


// Function for do statement
void doStatement()
{
	//______ DO ______
	Token t = GetNextToken();
	if( t.tp == 6 ){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 0) && !strcmp(t.lx, "do"));
	else {
		status.er = syntaxError;
		status.tk = t;
		return;
	}

	//______ SUBROUTINE CALL ______
	subroutineCall();
	if( status.er != 0 ) return;

	//______ ; ______
	t = GetNextToken();
	if( t.tp == 6) {
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == ';'));
	else {
		status.er = semicolonExpected;
		status.tk = t;
		return;
	}
	writePop(TEMP, 0);
}


// Function for subroutine call
void subroutineCall()
{
	Token one;
	Token two;
	Token t = GetNextToken();
	int idCount = 0;
	char firstId[128];
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	//______IDENTIFIER______
	if( t.tp == 1) {
		one = t;
		int index = IndexOf(t.lx);
		strcpy(firstId, one.lx);
		if(index==-1) {
			int cExists = classExists(t.lx);
			if(!cExists && (compileNum == 0)) {
				strcpy(two.lx, currentClass);
				addUndec(one, two, 1);
			}
		}
		
	}
	else{
		status.er = idExpected;
		status.tk = t;
		return;
	}

	//______[.IDENTIFIER]______
	t = PeekNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '.')) {
		// If next is a dot then call first one if it's not a class:
		int cExists = classExists(one.lx);
		if (!cExists) {
			Kind kind = KindOf(one.lx);	
			int index = IndexOf(one.lx);	
			if(kind == 0){
				writePush(STAT, index);
			} else if (kind == 1) {
				writePush(THIS, index);
			} else if (kind == 2) {
				writePush(ARGU, index);
			} else if (kind == 3) {
				writePush(LOC, index);
			}
		}

		t = GetNextToken();
		t = GetNextToken();
		if( t.tp == 6){
			status.er = lexerErr;
			status.tk = t;
			return;
		}

		//______IDENTIFIER______
		if( t.tp == 1){
			// add '.identifier' to undeclared for later checking
			two = t;
			idCount = 1;
			// If already declared, pass 'type of' one
			int index = IndexOf(one.lx);
			if(index != -1) {
				strcpy(one.lx, TypeOf(one.lx));
			} 
			if (compileNum == 0){
				addUndec(one, two, 2);
			}
		}
		else{
			status.er = idExpected;
			status.tk = t;
			return;
		}
	}

	//______ ( ______
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '('));
	else {
		status.er = openParenExpected;
		status.tk = t;
		return;
	}
	int expressions;
	//______ EXPRESSION LIST ______
	t = PeekNextToken();
	if( (t.tp == 3) && (t.lx[0] == ')')) expressions = 0;
	else {
		expressions = expressionList();
		if( status.er != 0) return;
	}

	//______ ) ______
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == ')'));
	else {
		status.er = closeParenExpected;
		status.tk = t;
		return;
	}

	char funcCall[128] = "";
	if(idCount == 0) {
		strcpy(funcCall, currentClass);
		strcat(funcCall, ".");
		strcat(funcCall, firstId);
		if(FindSymbol(currentClass, firstId).kind == METHOD) {
			expressions ++;
			writePush(POINTER, 0);
		}
	} else {
		strcpy(funcCall, one.lx);
		strcat(funcCall, ".");
		strcat(funcCall, two.lx);
		if(FindSymbol(one.lx, two.lx).kind == METHOD) {
			expressions ++;
			// writePush(POINTER, 0);
		}
	}
	writeCall(funcCall, expressions);
}


// Function for expression list
int expressionList() 
{
	int numExpressions = 0;

	//______ ) ______
	Token t = PeekNextToken();
	if( (t.tp == 3) && (t.lx[0] == ')')) return 0;

	//_____ EXPRESSION ______
	expression();
	if(status.er != 0) return 0;
	numExpressions ++;

	//______ {, EXPRESSION} ______
	t = PeekNextToken();
	int loop = 0;
	if((t.tp == 3) && (t.lx[0] == ',')) loop = 1;
	while (loop){
		t = GetNextToken();
		expression();
		numExpressions ++;
		if(status.er != 0 ) return 0;

		t = PeekNextToken();
		if((t.tp == 3) && (t.lx[0] == ','));
		else {
			loop = 0;
		}
	}
	return numExpressions;
}


// Function for return statements
void returnStatement()
{
	//______ RETURN ______
	Token t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 0)&& !strcmp(t.lx, "return") );
	else {
		status.er = syntaxError;
		status.tk = t;
		return;
	}

	//______ ; | EXPRESSION ______
	t = PeekNextToken();
	if( (t.tp == 3) && (t.lx[0] == ';')) {
		writePush(CONST, 0);
	}
	else if ((t.tp == 0) || (t.tp == 1) || (t.tp == 2) || (t.tp == 4) || ((t.tp == 3)&&(t.lx[0] == '(')) ){
		expression();
		if( status.er != 0) return;
	}
	t = GetNextToken();
	if( (t.tp == 3) && (t.lx[0] == ';'));
	else {
		status.er = semicolonExpected;
		status.tk = t;
		return;
	}
	writeReturn();
}


// Function for an expression
void expression()
{
	//______ RELATIONAL EXPRESSION ______
	Token t = PeekNextToken();
	relationalExpression();
	if( status.er != 0) return;

	//______ {(& | |) RELATIONAL EXPRESSION} ______
	t = PeekNextToken();
	int loop = 0;
	char relEx;
	if( (t.tp == 3) && ((t.lx[0] == '&') || (t.lx[0] == '|') )){
		relEx = t.lx[0];
		loop = 1;
	}
	while (loop)
	{
		t = GetNextToken();
		t = PeekNextToken();
		relationalExpression();
		if( status.er != 0) return;

		if(relEx == '&'){
			writeArithmetic(AND);
		} else {
			writeArithmetic(OR);
		}
		if( (t.tp == 3) && ((t.lx[0] == '&') || (t.lx[0] == '|'))){
			relEx = t.lx[0];
		}
		else loop = 0;
	}
}


// Function for relational expressions
void relationalExpression()
{	
	//______ ARITHMETIC EXPRESSION ______
	Token t = PeekNextToken();
	arithmeticExpression();
	if( status.er != 0) return;
	
	//______ {(=|<|>) ARITHMETIC EXPRESSION} ______
	t = PeekNextToken();
	int loop = 0;
	char arithEx;
	if( (t.tp == 3) && ( (t.lx[0] == '=') || (t.lx[0] == '<') || (t.lx[0] == '>') )) {
		loop = 1;
		arithEx = t.lx[0];
	}
	while (loop){
		t = GetNextToken();
		t = PeekNextToken();
		arithmeticExpression();

		if(arithEx == '=') {
			writeArithmetic(EQ);
		} else if (arithEx == '<') {
			writeArithmetic(LT);
		} else {
			writeArithmetic(GT);
		}
		if( status.er != 0) return;
		if( (t.tp == 3) && ( (t.lx[0] == '=') || (t.lx[0] == '<') || (t.lx[0] == '>'))){
			arithEx = t.lx[0];
		}
		else loop = 0;
	}
}


// Function for arithmetic expression
void arithmeticExpression()
{	
	char addSub;
	//______ TERM ______
	term();
	if( status.er != 0) return;
	//______ {(+ | -) TERM} ______
	Token t = PeekNextToken();
	int loop = 0;
	if( (t.tp == 3) && ( (t.lx[0] == '+') || (t.lx[0] == '-') )) {
		addSub = t.lx[0];
		loop = 1;
	}
	while (loop) {
		t = GetNextToken();
		term();
		if(addSub == '+') {
			writeArithmetic(ADD);
		} else {
			writeArithmetic(SUB);
		}
		if( status.er != 0) return;
		t = PeekNextToken();
		if( (t.tp == 3) && ( (t.lx[0] == '+') || (t.lx[0] == '-') )){
			addSub = t.lx[0];
		}
		else loop = 0;
	}
}


// Function for term
void term()
{	
	//______ FACTOR ______
	factor();
	if( status.er != 0) return;

	//______{(*|/) FACTOR}
	Token t = PeekNextToken();
	int loop = 0;
	char multdiv;

	if( (t.tp == 3) && ( (t.lx[0] == '*') || (t.lx[0] == '/') )){
		loop = 1;
		multdiv = t.lx[0];
	} 
	while (loop) {
		t = GetNextToken();
		factor();
		if( status.er != 0) return;
		if(multdiv == '*') {
			writeCall("Math.multiply", 2);
		} else {
			writeCall("Math.divide", 2);
		}

		t = PeekNextToken();
		if( (t.tp == 3) && ( (t.lx[0] == '*') || (t.lx[0] == '/') )){
			multdiv = t.lx[0];
		}
		else loop = 0;
	}

}


// Function for factor
void factor()
{
	//______ -|~______
	Token t = PeekNextToken();
	int doArith = 0;
	char arith;
	if( (t.tp == 3) && ( (t.lx[0] == '-'))) {
		arith = t.lx[0];
		doArith = 1;
		t = GetNextToken();
	} else if ((t.tp == 3) && (t.lx[0] == '~') ){
		t = GetNextToken();
		doArith = 1;
		arith = t.lx[0];
	} 

	//______ OPERAND ______
	operand();
	if (status.er != 0 ) return;

	if(doArith) {
		if(arith == '-'){
			writeArithmetic(NEG);
		} else {
			writeArithmetic(NOT);
		}
	}
}


// Function for operand
void operand()
{
	Token one;
	Token two;
	Token t = GetNextToken();

	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	//_____INTEGER CONSTANT______
	if( t.tp == 2){
		writePush(CONST, atoi(t.lx));
	}
	//_______IDENTIFIER________
	else if( t.tp == 1 ){
		char funcCall[128];
		int type = 0; //0 for 1 id or array, 1 for call
		one = t;
		int expList = 0;
		int index = IndexOf(t.lx);

		strcpy(funcCall, one.lx);
		// if not there, check class names:
		if( index == -1) {
			int cExists = classExists(t.lx);
			if( !cExists && (compileNum == 0)) {
				addUndec(one, two, 1);
			}
		} 

		//______ . ______
		t = PeekNextToken();
		if( (t.tp == 3) && (t.lx[0] == '.')){

			// If next is a dot then call first one if it's not a class:
			int cExists = classExists(one.lx);
			if (!cExists) {
				Kind kind = KindOf(one.lx);	
				int index = IndexOf(one.lx);	
				if(kind == 0){
					writePush(STAT, index);
				} else if (kind == 1) {
					writePush(THIS, index);
				} else if (kind == 2) {
					writePush(ARGU, index);
				} else if (kind == 3) {
					writePush(LOC, index);
				}
			}
			t = GetNextToken();
			//_______ IDENTIFIER ______
			t = GetNextToken();
			if( t.tp == 6){
				status.er = lexerErr;
				status.tk = t;
				return;
			}
			if( t.tp == 1) {
				two = t;
				type = type + 1; // ----> id.id = 1
				strcat(funcCall, ".");
				strcat(funcCall, two.lx);
				if( compileNum == 0){
					int index = IndexOf(one.lx);
					if(index != -1) {
						strcpy(one.lx, TypeOf(one.lx));
					} 
					addUndec(one, two, 2);
				}
			}
			else {
				status.er = idExpected;
				status.tk = t;
				return;
			}
		}

		t = PeekNextToken();
		//______ ( ______
		if( (t.tp == 3) && (t.lx[0] == '(')){
			t = GetNextToken();
			//______ EXPRESSION LIST ______
			expList = expressionList();
			type = type + 2; //------> id(explist) = 2, id[.id](explist) = 3
			if( status.er != 0) return;
			t = GetNextToken();
			if( t.tp == 6){
				status.er = lexerErr;
				status.tk = t;
				return;
			}
			//______ ) ______
			if( (t.tp == 3) && (t.lx[0] == ')'));
			else{
				status.er = closeParenExpected;
				status.tk = t;
				return;
			}
		}
		// TYPE 0: id
		// TYPE 1: id.id
		// TYPE 2: id(expList)
		// TYPE 3: id.id(expList) 
		if(type == 0){
			symbol sym = FindSymbol(currentClass, one.lx);
			Kind kind = KindOf(one.lx);
			if(strcmp(sym.name, " ")) {
				kind = sym.kind;
				index = sym.index;
			}
						
			if(kind == STATIC){
				writePush(STAT, index);
			} else if (kind == FIELD) {
				writePush(THIS, index);
			} else if (kind == ARG) {
				writePush(ARGU, index);
			} else if (kind == VAR) {
				writePush(LOC, index);
			}
		} else if (type == 2) {
			// Work out the kind of function:
			Kind kind = FindSymbol(currentClass, one.lx).kind;
			if(kind == METHOD) {
				expList = expList + 1;
			}
			writeCall(funcCall, expList);
		} else if( type == 1) {
			// not sure what should happen when it is id.id
			type = 1;
		} else if (type == 3) {
			char funcCall[128];
			if(classExists(one.lx)){
				strcpy(funcCall, one.lx);
			} else {
				strcpy(funcCall, TypeOf(one.lx));
				Kind twoKind = FindSymbol(TypeOf(one.lx), two.lx).kind;
				if(twoKind == METHOD) {
					expList = expList + 1;
				}
			}
			strcat(funcCall, ".");
			strcat(funcCall, two.lx);
			
			writeCall(funcCall, expList);
		}
		//______ [______
		if( (t.tp == 3) && (t.lx[0] == '[')){
			t = GetNextToken();
			//______ EXPRESSION ______
			expression();
			if( status.er != 0) return;
			//______ ] ______
			t = GetNextToken();
			if( t.tp == 6){
				status.er = lexerErr;
				status.tk = t;
				return;
			}
			if( (t.tp == 3) && (t.lx[0] == ']'));
			else{
				status.er = closeBracketExpected;
				status.tk = t;
				return;
			}
			writeArithmetic(ADD);
		}
	}

	//_______ ( ________
	else if( (t.tp == 3) && (t.lx[0] == '(')){
		//______ EXPRESSION ______
		expression();
		if(status.er != 0 ) return;
		//______ ) _______
		t = GetNextToken();
		if( t.tp == 6){
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		if( (t.tp == 3) && (t.lx[0] == ')'));
		else {
			status.er = closeParenExpected;
			status.tk = t;
			return;
		}
	}

	//________ STRING LITERAL _______
	else if (t.tp == 4) {
		int strlength = strlen(t.lx);
		writePush(CONST, strlength);
		writeCall("String.new", 1);
		for(int i=0; i<strlength; i++) {
			int asc = t.lx[i];
			writePush(CONST, asc);
			writeCall("String.appendChar", 2);
		}
	}

	//________ TRUE/FALSE/NULL/THIS _______
	else if(( t.tp == 0) && ( !strcmp(t.lx, "true"))) {
		writePush(CONST, 0);
		writeArithmetic(NOT);
	} else if(( t.tp == 0) && ( !strcmp(t.lx, "false"))) {
		writePush(CONST, 0);
	} else if(( t.tp == 0) && ( !strcmp(t.lx, "null"))) {
		writePush(CONST, 0);
	} else if(( t.tp == 0) && ( !strcmp(t.lx, "this"))) {
		writePush(POINTER, 0);
	} else {
		status.er = syntaxError;
		status.tk = t;
		return;
	}
}


// Function to initialise parser
int InitParser (char* file_name)
{
	return 1;
}

ParserInfo Parse ()
{
	// Default error is none
	status.er = none;

	ifNum = 0;
	whileNum = 0;
	//___________ CLASS ___________
	Token t = GetNextToken();
	if (t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return status;
	}
	if( (t.tp == 0) && !strcmp(t.lx, "class"));
	else {
		status.er = classExpected;
		status.tk = t;
		return status;
	}

	//___________ IDENTIFIER ___________
	t = GetNextToken();
	if (t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return status;
	}
	if( t.tp == 1) {
		// Check if class exists:
		strcpy(currentClass, t.lx);
		int exists = classExists(t.lx);
		if(exists && (compileNum == 0)) {
			status.er = redecIdentifier;
			status.tk = t;
			return status;
		} else if (compileNum == 0) {
			newClass(t.lx);
		}
		
	}
	else {
		status.er = idExpected;
		status.tk = t;
		return status;
	}

	//___________ { ___________
	t = GetNextToken();
	if (t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return status;
	}
	if( (t.tp == 3) && (t.lx[0] = '{'));
	else {
		status.er = openBraceExpected;
		status.tk = t;
		return status;
	}

	//___________ {MEMBER DECLAR} ___________
	t = PeekNextToken();
	int loop = 0;
	if( (t.tp == 3 ) && (t.lx[0] == '}'));
	else loop = 1;
	while( loop ){
		memberDeclar();
		if( status.er != 0) return status;
		t = PeekNextToken();
		if( (t.tp == 3 ) && (t.lx[0] == '}')) loop = 0;
	}
	
	//___________}___________
	t = GetNextToken();
	if (t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return status;
	}
	if((t.tp == 3) && (t.lx[0] = '}'));
	else {
		status.er = closeBraceExpected;
		status.tk = t;
		return status;
	}
	return status;
}


int StopParser ()
{
	return 1;
}

#ifndef TEST_PARSER
// int main ()
// {
// 	InitLexer("semicolonExpected.jack");

// 	ParserInfo popping = Parse();

// 	printf("Error %i: Line %i at or near %s\n", popping.er, popping.tk.ln, popping.tk.lx);


// 	return 1;
// }
#endif
