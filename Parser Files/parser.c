#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"


// you can declare prototypes of parser functions below
void memberDeclar();
void classVarDeclar();
void type();
void subroutineDeclar();
void paramList();
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

// Function for member declaration checking
void memberDeclar()
{
	// First peek at token to check whether it is a class 
	// variable declaration or a subroutine declaration
	Token t = PeekNextToken();

	// If token is an error set status
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		exit(0);
	}

	//___________CLASSVAR OR SUBROUTINE___________
	// First check if token is 'static' or 'field' for classVarDeclar
	if( t.tp == 0 && (!strcmp(t.lx, "static") || !strcmp(t.lx, "field"))){
		// If it is a classVarDeclar call function
		classVarDeclar();
		// If any errors, exit
		if( status.er != 0 ) exit(0);
	}
	// Check if token is 'constructor', 'function' or 'method'
	else if( t.tp == 0 && (!strcmp(t.lx, "constructor" || !strcmp(t.lx, "function") || !strcmp(t.lx, "method"))))
	{
		// If it is a subroutineDeclar call function
		subroutineDeclar();
		// If any errors, exit
		if( status.er != 0 ) exit(0);
	}
	// If not subroutineDeclar or classVarDeclar set errors
	else {
		status.er = memberDeclarErr;
		status.tk = t;
	}
}

// Function for classVarDeclar
void classVarDeclar() 
{
	// Get the next token
	Token t = GetNextToken();
	// Return error if lexer error
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		exit(0);
	}

	//___________FIELD/STATIC___________
	// Give error if token is not 'field' or 'static'
	if( !strcmp(t.lx, "field") || !strcmp(t.lx, "static"))
	{
		;
	}
	else {
		status.er = classVarErr;
		status.tk = t;
		exit(0);
	}

	//___________TYPE___________
	// If no errors, continue and call 'type' function
	type();
	// Check for errors form this
	if( status.er != 0) exit(0);

	//___________IDENTIFIER___________
	// Get next token
	t = GetNextToken();
	// Check for lexer error
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		exit(0);
	}
	// Check token is identifier
	if( t.tp == 1 ){
		;
	}
	else {
		status.er = idExpected;
		status.tk = t;
		exit(0);
	}

	//_______{, IDENTIFIER}________
	int continueSearch = 1;
	// Write loop to keep checking for comma + identifier
	while( continueSearch )
	{
		// First check if next token is a comma
		t = PeekNextToken();
		if(!((t.tp == 3) && (t.lx[0] = ',')))
		{
			// If not, break out of loop
			continueSearch = 0;
			break;
		}
		// Discard comma and get token after
		t = GetNextToken();
		t = GetNextToken();
		// Check for lexer errors
		if( t.tp == 6 )
		{
			status.er = lexerErr;
			status.tk = t;
			exit(0);
		}
		// If no errors, check it's an identifier
		if( t.tp == 1 ){
			;
		}
		else {
			status.er = idExpected;
			status.tk = t;
			exit(0);
		}
	}

	//_______;________
	// Get the next token
	t = GetNextToken();
	// Check for lexer errors
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		exit(0);
	}
	// If no errors, check it's a semi colon
	if( (t.tp == 3) && (t.lx[0] == ';') ){
		;
	}
	else {
		status.er = semicolonExpected;
		status.tk = t;
		exit(0);
	}
}


//Function for type
void type() {
	// Get the next token
	Token t = GetNextToken();
	// Check for lexer errors
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		exit(0);
	}

	// Check that token is either int, char, boolean or an identifier
	if( t.tp == 1)
	{
		// Pass if identifier
		;
	}
	else if ((t.tp == 0) &&(	!strcmp(t.lx, "int") ||
								!strcmp(t.lx, "char") ||
								!strcmp(t.lx, "boolean")))
	{
		// Pass if one of the above reserved words
		;
	}
	else {
		// If not the above, change parser info status error
		status.er = illegalType;
		status.tk = t;
	}
}


// Function for subroutineDeclar
void subroutineDeclar()
{
	
	//___________CONSTRUCTOR/FUNCTION/METHOD___________
	// Get the next token
	Token t = GetNextToken();
	// Return error if lexer error
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		exit(0);
	}
	// Give error if token is not 'constructor', 'function' or 'method'
	if( !strcmp(t.lx, "constructor") || !strcmp(t.lx, "function" || !strcmp(t.lx, "method")))
	{
		;
	}
	else {
		status.er = subroutineDeclarErr;
		status.tk = t;
		exit(0);
	}

	//______TYPE OR VOID___________

}

// Function to initialise parser
int InitParser (char* file_name)
{
	return 1;
}

ParserInfo Parse ()
{
	// Start by setting global parser status error to none
	status.er = none;

	//___________CLASS___________
	// Get the first token
	Token t = GetNextToken();
	// First check that the first token is the class identifier
	if( (t.tp == 0) && !strcmp(t.lx, "class"))
	{
		;
	}
	// If not set status error using error function
	else 
	{
		// First check if it's a lexer error 
		if (t.tp == 6)
		{
			status.er = lexerErr;
			status.tk = t;
		}
		else {
			status.er = classExpected;
			status.tk = t;
		}
		// Return error ParserInfo
		return status;

	}

	//___________IDENTIFIER___________
	// Get the next token
	t = GetNextToken();
	// Check next token is identifier
	if( t.tp == 1)
	{
		;
	}
	// If not set status error using error function
	else 
	{
		// First check if it's a lexer error 
		if (t.tp == 6)
		{
			status.er = lexerErr;
			status.tk = t;
		}
		else {
			status.er = idExpected;
			status.tk = t;
		}
		// Return error ParserInfo
		return status;
	}

	//___________{___________
	// Get the next token
	t = GetNextToken();
	// Check if it is an open curly bracket
	if( (t.tp == 3) && (t.lx[0] = '{'))
	{
		;
	}
	// If not set status error using error function
	else 
	{
		// First check if it's a lexer error 
		if (t.tp == 6)
		{
			status.er = lexerErr;
			status.tk = t;
		}
		else {
			status.er = openBraceExpected;
			status.tk = t;
		}
		// Return error ParserInfo
		return status;
	}

	//___________MEMBER DECLAR___________
	memberDeclar();
	// Check if any errors
	if( status.er == 0)
	{
		return status;
	}

	//___________}___________
	// Get the next token
	t = GetNextToken();
	// Check if it is an open curly bracket
	if((t.tp == 3) && (t.lx[0] = '}'))
	{
		;
	}
	// If not set status error using error function
	else 
	{
		// First check if it's a lexer error 
		if (t.tp == 6)
		{
			status.er = lexerErr;
			status.tk = t;
		}
		else {
			status.er = closeBraceExpected;
			status.tk = t;
		}
		// Return error ParserInfo
		return status;
	}

	return status;
}


int StopParser ()
{
	return 1;
}

#ifndef TEST_PARSER
int main ()
{

	return 1;
}
#endif
