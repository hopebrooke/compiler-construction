#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "symbols.h"


// Make parameter list struct:
typedef struct param {
	char args[50][128];
	char argTypes[50][128];
	int num;
} param;

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
void expressionList();
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
		if(exists) {
			status.er = redecIdentifier;
			status.tk = t;
		} else {
			char empty[10][128];
			Define(t.lx, typeSymbol, kind, 1, empty, empty);
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
			if(exists) {
				status.er = redecIdentifier;
				status.tk = t;
			} else {
				char empty[10][128];
				Define(t.lx, typeSymbol, kind, 1, empty, empty);
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
		if( !exists ){
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
	Kind kind;
	char name[128];
	char typeSymbol[128];
	int index;
	char args[50][128];
	char argTypes[50][128];

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
		if(exists) {
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
	Define(name, typeSymbol, kind, index, args, argTypes);
	// Start subroutine:
	startSubroutine();
	// Add parameters as arguments:
	char emptyArgs[50][128];
	for(int i=0; i<parameters.num; i++){
		Define(parameters.args[i], parameters.argTypes[i], ARG, i, emptyArgs, emptyArgs);
	}

	//______ SUBROUTINEBODY ______
	subroutineBody();
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
	char name[128];
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
		if(exists){
			status.er = redecIdentifier;
			status.tk = t;
			return;
		} else {
			Define(t.lx, typeSymbol, kind, 0, args, args);
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
			if(exists){
				status.er = redecIdentifier;
				status.tk = t;
				return;
			} else {
				Define(t.lx, typeSymbol, kind, 0, args, args);
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
	if( t.tp == 1 ){
		//check if exists:
		int index = IndexOf(t.lx);
		if(index == -1) {
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

	if( (t.tp == 0) && !strcmp(t.lx, "while"));
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
}


// Function for subroutine call
void subroutineCall()
{
	Token one;
	Token two;
	Token t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	//______IDENTIFIER______
	if( t.tp == 1) {
		one = t;
		int index = IndexOf(t.lx);
		if(index==-1) {
			int cExists = classExists(t.lx);
			if(!cExists) {
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
			// If already declared, pass 'type of' one
			int index = IndexOf(one.lx);
			if(index != -1) {
				strcpy(one.lx, TypeOf(one.lx));
			}
			addUndec(one, two, 2);
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

	//______ EXPRESSION LIST ______
	t = PeekNextToken();
	if( (t.tp == 3) && (t.lx[0] == ')'));
	else {
		expressionList();
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
}


// Function for expression list
void expressionList() 
{
	//______ ) ______
	Token t = PeekNextToken();
	if( (t.tp == 3) && (t.lx[0] == ')')) return;

	//_____ EXPRESSION ______
	expression();
	if(status.er != 0) return;

	//______ {, EXPRESSION} ______
	t = PeekNextToken();
	int loop = 0;
	if((t.tp == 3) && (t.lx[0] == ',')) loop = 1;
	while (loop){
		t = GetNextToken();
		expression();
		if(status.er != 0 ) return;

		t = PeekNextToken();
		if((t.tp == 3) && (t.lx[0] == ','));
		else {
			loop = 0;
		}
	}
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
	if( (t.tp == 3) && (t.lx[0] == ';'));
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
	if( (t.tp == 3) && ((t.lx[0] == '&') || (t.lx[0] == '|') )) loop = 1;
	while (loop)
	{
		t = GetNextToken();
		t = PeekNextToken();
		relationalExpression();
		if( status.er != 0) return;
		if( (t.tp == 3) && ((t.lx[0] == '&') || (t.lx[0] == '|')));
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
	if( (t.tp == 3) && ( (t.lx[0] == '=') || (t.lx[0] == '<') || (t.lx[0] == '>') )) loop = 1;
	while (loop){
		t = GetNextToken();
		t = PeekNextToken();
		arithmeticExpression();
		if( status.er != 0) return;
		if( (t.tp == 3) && ( (t.lx[0] == '=') || (t.lx[0] == '<') || (t.lx[0] == '>')));
		else loop = 0;
	}
}


// Function for arithmetic expression
void arithmeticExpression()
{	
	//______ TERM ______
	term();
	if( status.er != 0) return;
	//______ {(+ | -) TERM} ______
	Token t = PeekNextToken();
	int loop = 0;
	if( (t.tp == 3) && ( (t.lx[0] == '+') || (t.lx[0] == '-') ))loop = 1;
	while (loop) {
		t = GetNextToken();
		term();
		if( status.er != 0) return;
		t = PeekNextToken();
		if( (t.tp == 3) && ( (t.lx[0] == '+') || (t.lx[0] == '-') ));
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
	if( (t.tp == 3) && ( (t.lx[0] == '*') || (t.lx[0] == '/') )) loop = 1;
	while (loop) {
		t = GetNextToken();
		factor();
		if( status.er != 0) return;
		t = PeekNextToken();
		if( (t.tp == 3) && ( (t.lx[0] == '*') || (t.lx[0] == '/') ));
		else loop = 0;
	}

}


// Function for factor
void factor()
{
	//______ -|~______
	Token t = PeekNextToken();
	if( (t.tp == 3) && ( (t.lx[0] == '-') || (t.lx[0] == '~'))) t = GetNextToken();

	//______ OPERAND ______
	operand();
	if (status.er != 0 ) return;
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
	if( t.tp == 2){;}
	//_______IDENTIFIER________
	else if( t.tp == 1 ){
		one = t;
		int index = IndexOf(t.lx);
		// if not there, check class names:
		if( index == -1) {
			int cExists = classExists(t.lx);
			if( !cExists) {
				addUndec(one, two, 1);
			}
		} 
		
		//______ . ______
		t = PeekNextToken();
		if( (t.tp == 3) && (t.lx[0] == '.')){
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
				addUndec(one, two, 2);
			}
			else {
				status.er = idExpected;
				status.tk = t;
				return;
			}
		}

		//______ [______
		t = PeekNextToken();
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
		}

		//______ ( ______
		else if( (t.tp == 3) && (t.lx[0] == '(')){
			t = GetNextToken();
			//______ EXPRESSION LIST ______
			expressionList();
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
	else if (t.tp == 4);

	//________ TRUE/FALSE/NULL/THIS _______
	else if(( t.tp == 0) && ( !strcmp(t.lx, "true") || !strcmp(t.lx, "false") || 
			!strcmp(t.lx, "null") || !strcmp(t.lx, "this") ));
	else {
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
		if(exists) {
			status.er = redecIdentifier;
			status.tk = t;
			return status;
		} else {
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

// #ifndef TEST_PARSER
// int main ()
// {
// 	InitLexer("semicolonExpected.jack");

// 	ParserInfo popping = Parse();

// 	printf("Error %i: Line %i at or near %s\n", popping.er, popping.tk.ln, popping.tk.lx);


// 	return 1;
// }
// #endif
