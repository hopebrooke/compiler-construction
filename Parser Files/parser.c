#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"


// you can declare prototypes of parser functions below
void classDeclar();
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
ParserInfo error(char * err_message, SyntaxErrors err_num,Token t);

// Initialise global ParserInfo struct
ParserInfo status;

ParserInfo error(char * err_message, SyntaxErrors err_num, Token t)
{
	status.err = err_num
}
int InitParser (char* file_name)
{
	return 1;
}

ParserInfo Parse ()
{
	// Start by setting global parser status error to none
	status.er = none;

	// Get the first token
	Token t = GetNextToken();
	
	// First check that the first token is the class identifier
	if( t.tp == 0 && !strcmp(t.lx, "class"))
	{
		;
	}
	// If not set status error using error function
	else 
	{
		error()
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
