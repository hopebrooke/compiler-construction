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
		return;
	}

	//___________CLASSVAR OR SUBROUTINE___________
	// First check if token is 'static' or 'field' for classVarDeclar
	if( t.tp == 0 && (!strcmp(t.lx, "static") || !strcmp(t.lx, "field"))){
		// If it is a classVarDeclar call function
		classVarDeclar();
		// If any errors, exit current function
		if( status.er != 0 )return;
	}
	// Check if token is 'constructor', 'function' or 'method'
	else if( t.tp == 0 && (!strcmp(t.lx, "constructor") || !strcmp(t.lx, "function") || !strcmp(t.lx, "method")))
	{
		// If it is a subroutineDeclar call function
		subroutineDeclar();
		// If any errors, exit current function
		if( status.er != 0 ) return;
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
		return;
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
		return;
	}

	//___________TYPE___________
	// If no errors, continue and call 'type' function
	type();
	// Check for errors form this
	if( status.er != 0) return;

	//___________IDENTIFIER___________
	// Get next token
	t = GetNextToken();
	// Check for lexer error
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	// Check token is identifier
	if( t.tp == 1 ){
		;
	}
	else {
		status.er = idExpected;
		status.tk = t;
		return;
	}

	//_______{, IDENTIFIER}________
	// First check if next token is a comma
	t = PeekNextToken();
	int loop = 0;
	if( (t.tp == 3) && (t.lx[0] == ',')){
		loop = 1;
	}
	// Write loop to keep checking for comma + identifier
	while( loop )
	{
		// Discard comma and get token after
		t = GetNextToken();

		t = GetNextToken();
		// Check for lexer errors
		if( t.tp == 6 )
		{
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		// If no errors, check it's an identifier
		if( t.tp == 1 ){
			;
		}
		else {
			status.er = idExpected;
			status.tk = t;
			return;
		}

		// Check next token for a comma
		t = PeekNextToken();
		if( (t.tp == 3 ) && (t.lx[0] == ',')){
			;
		}
		else {
			loop = 0;
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
		return;
	}
	// If no errors, check it's a semi colon
	if( (t.tp == 3) && (t.lx[0] == ';') ){
		;
	}
	else {
		status.er = semicolonExpected;
		status.tk = t;
		return;
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
		return;
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
		return;
	}
	// Give error if token is not 'constructor', 'function' or 'method'
	if( !strcmp(t.lx, "constructor") || !strcmp(t.lx, "function") || !strcmp(t.lx, "method"))
	{
		;
	}
	else {
		status.er = subroutineDeclarErr;
		status.tk = t;
		return;
	}

	//______TYPE OR VOID___________
	// Peek the next token to check if it is a type, or 'void'
	t = PeekNextToken();
	// If it is void, continue, if not, call type function
	if( !(strcmp(t.lx, "void")) )
	{
		t = GetNextToken();
	}
	else {
		type();
		// If type returns error, exit current function
		if ( status.er != 0) return;
	}

	//_______IDENTIFIER_________
	t = GetNextToken();
	// Return error if lexer error
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( t.tp == 1)
	{
		;
	}
	else {
		status.er = idExpected;
		status.tk = t;
		return;
	}

	//_______(__________
	t = GetNextToken();
	// Return error if lexer error
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	// Return error if not an '('
	if( (t.tp == 3) && (t.lx[0] == '('))
	{
		;
	}
	else {
		status.er = openParenExpected;
		status.tk = t;
		return;
	}

	// Call param list function
	paramList();
	if( status.er != 0 ) return;

	//_________)_________
	t = GetNextToken();
	// Return error if lexer error
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	// Return error if not an ')'
	if( (t.tp == 3) && (t.lx[0] == ')'))
	{
		;
	}
	else {
		status.er = closeParenExpected;
		status.tk = t;
		return;
	}

	// Call subroutine body
	subroutineBody();
	if( status.er != 0 ) return;
}


// Function for parameter list
void paramList()
{
	// Check if next token is type
	Token t = PeekNextToken();
	if( !strcmp(t.lx, "int") || !strcmp(t.lx, "char") || !strcmp(t.lx, "boolean") || (t.tp == 1))
	{
		//________TYPE____________
		type();
		if( status.er != 0) return;

		//_______IDENTIFIER________
		t = GetNextToken();
		if( t.tp == 6 )
		{
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		if( t.tp == 1 ){
			;
		}
		else {
			status.er = idExpected;
			status.tk = t;
			return;
		}

		//_____,_____
		// Check if next token is a comma
		t = PeekNextToken();
		int loop = 0;
		if( (t.tp == 3) && (t.lx[0] == ',')){
			loop = 1;
		}

		// If next token is comma, keep looping
		while( loop ){
			// Consume the comma
			t = GetNextToken();
			if( t.tp == 6 )
			{
				status.er = lexerErr;
				status.tk = t;
				return;
			}
			// Check next token is type
			type();
			if( status.er != 0) return;

			// Check next token is identifier
			t = GetNextToken();
			if( t.tp == 6 )
			{
				status.er = lexerErr;
				status.tk = t;
				return;
			}
			if( t.tp == 1 ){
				;
			}
			else {
				status.er = idExpected;
				status.tk = t;
				return;
			}

			// Check if next token is comma
			t = PeekNextToken();
			// If it is comma continue looping, if not then exit while loop
			if( (t.tp == 3) && (t.lx[0] == ',')){
				;
			}
			else {
				loop = 0;
			}
		}
	}
	// If no type, then just return to previous function
}


// Function for subroutine body
void subroutineBody(){
	// Check for open curly bracket
	Token t = GetNextToken();
	if( t.tp == 6 ){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] = '{'))
	{
		;
	}
	else {
		status.er = openBraceExpected;
		status.tk = t;
		return;
	}

	//Loop for statements
	int loop = 1;
	while( loop )
	{
		// Check next token
		t = PeekNextToken();
		// All tokens start with a reserved word
		if( t.tp == 0 )
		{
			statement();
			if( status.er != 0 ) return;
		}
		else {
			break;
		}
	}

	// Check for closing curly bracket
	t = GetNextToken();
	// Check for lexer error
	if( t.tp == 6) {
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '}'))
	{
		;
	}
	else {
		status.er = closeBraceExpected;
		status.tk = t;
		return;
	}
}


// Function to check for statements
void statement() 
{
	// Peek next token to check which statement it is
	Token t = PeekNextToken();
	if( (t.tp == 0) && !strcmp(t.lx, "var")){
		varDeclarStatement();
		if ( status.er != 0 ) return;
	}
	else if( (t.tp == 0) && !strcmp(t.lx, "let")){
		letStatement();
		if ( status.er != 0 ) return;
	}
	else if( (t.tp == 0) && !strcmp(t.lx, "if")){
		ifStatement();
		if ( status.er != 0 ) return;
	}
	else if( (t.tp == 0) && !strcmp(t.lx, "while")){
		whileStatement();
		if ( status.er != 0 ) return;
	}
	else if( (t.tp == 0) && !strcmp(t.lx, "do")){
		doStatement();
		if ( status.er != 0 ) return;
	}
	else if( (t.tp == 0) && !strcmp(t.lx, "return")){
		returnStatement();
		if ( status.er != 0 ) return;
	}
	else {
		if( t.tp == 6 ){
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		else {
			status.er = syntaxError;
			status.tk = t;
			return;
		}
	}
}


// Function for variable declaration statements
void varDeclarStatement()
{
	// Check first token is 'var'
	Token t = GetNextToken();
	// Check for lexer errors
	if ( t.tp == 6 ){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( ( t.tp == 0 ) && ( !strcmp(t.lx, "var")))
	{
		;
	}
	else {
		status.er = syntaxError;
		status.tk = t;
		return;
	}

	// Check next token is type
	type();
	if( status.er != 0 ) return;

	// Check next token is identifier
	t = GetNextToken();
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( t.tp == 1 ){
		;
	}
	else {
		status.er = idExpected;
		status.tk = t;
		return;
	}

	// Peek at next token to check if it is a comma
	t = PeekNextToken();
	int loop = 0;
	if( (t.tp== 3) && (t.lx[0] == ','))
	{
		loop = 1;
	}
	while (loop)
	{
		// Discard comma
		t = GetNextToken();

		// Check for identifier
		t = GetNextToken();
		if( t.tp == 6 )
		{
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		if( t.tp == 1 ){
			;
		}
		else {
			status.er = idExpected;
			status.tk = t;
			return;
		}

		// Peek to see if next token is comma
		t = PeekNextToken();
		if( (t.tp == 3) && (t.lx[0] == ','))
		{
			;
		}
		else{
			loop = 0;
		}
	}

	// Check for semicolon
	t = GetNextToken();
	if( t.tp == 6)
	{
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == ';')){
		;
	}
	else{
		status.er = semicolonExpected;
		status.tk = t;
		return;
	}
}


// Function for let statment
void letStatement()
{
	Token t = GetNextToken();
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 0) && (!strcmp(t.lx, "let"))){
		;
	}
	else {
		status.er = syntaxError;
		status.tk = t;
		return;
	}

	// Check next token is identifier
	t = GetNextToken();
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( t.tp == 1 ){
		;
	}
	else {
		status.er = idExpected;
		status.tk = t;
		return;
	}

	// Check if next token is '['
	t = PeekNextToken();
	if(t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '[')){
		// Discard [
		t = GetNextToken();
		// Continue checking for expression
		expression();
		if( status.er != 0 ) return;

		// Check for closing parenthesis
		t = GetNextToken();
		if( t.tp == 6){
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		if( (t.tp == 3)&&(t.lx[0] == ']')){
			;
		}
		else {
			status.er = closeBracketExpected;
			status.tk = t;
			return;
		}
	}

	// Check for equals
	t = GetNextToken();
	// Check for lexer error
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	if((t.tp == 3)&& (t.lx[0] == '=')){
		;
	}
	else{
		status.er = equalExpected;
		status.tk = t;
		return;
	}

	// Check for expression
	expression();
	if( status.er != 0 ) return;

	//Check for semicolon
	t = GetNextToken();
	if( t.tp == 6 ){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	if( (t.tp == 3) && (t.lx[0] == ';')){
		;
	}
	else {
		status.er = semicolonExpected;
		status.tk = t;
		return;
	}
}


// Function for if statement
void ifStatement()
{	
	// Check for if
	Token t = GetNextToken();
	if ( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	if( (t.tp == 0) && !strcmp(t.lx, "if"))
	{
		;
	}
	else {
		status.er = syntaxError;
		status.tk = t;
		return;
	}


	// Check for (
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '(')){
		;
	}
	else {
		status.er = openParenExpected;
		status.tk = t;
		return;
	}

	// Check for expression
	expression();
	if( status.er != 0 ) return;

	// Check for )
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == ')')){
		;
	}
	else {
		status.er = closeParenExpected;
		status.tk = t;
		return;
	}


	// Check for {
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '{')){
		;
	}
	else {
		status.er = openBraceExpected;
		status.tk = t;
		return;
	}

	// Check for statement
	statement();
	if( status.er != 0 ) return;

	// Check for more statements
	t = PeekNextToken();
	int loop = 0;
	if( !strcmp(t.lx, "var") || !strcmp(t.lx, "let") || !strcmp(t.lx, "if") || !strcmp(t.lx, "while") || !strcmp(t.lx, "do") || !strcmp(t.lx, "return"))
	{
		loop = 1;
	}
	while (loop){
		statement();
		if( status.er != 0) return;
		t = PeekNextToken();
		if( !strcmp(t.lx, "var") || !strcmp(t.lx, "let") || !strcmp(t.lx, "if") || !strcmp(t.lx, "while") || !strcmp(t.lx, "do") || !strcmp(t.lx, "return"))
		{
			;
		}
		else {
			loop = 0;
		}
	}

	// Check for }
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '}')){
		;
	}
	else {
		status.er = closeBraceExpected;
		status.tk = t;
		return;
	}

	// Check if there is an else
	t = PeekNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	if( !strcmp(t.lx, "else")){
		// Consume else
		t = GetNextToken();

		//Check for {
		t = GetNextToken();
		if( t.tp == 6){
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		if((t.tp == 3) && (t.lx[0] == '{'))
		{
			;
		}
		else {
			status.er = openBraceExpected;
			status.tk = t;
			return;
		}

			
		// Check for statement
		statement();
		if( status.er != 0 ) return;

		// Check for more statements
		t = PeekNextToken();
		int loop = 0;
		if( !strcmp(t.lx, "var") || !strcmp(t.lx, "let") || !strcmp(t.lx, "if") || !strcmp(t.lx, "while") || !strcmp(t.lx, "do") || !strcmp(t.lx, "return"))
		{
			loop = 1;
		}
		while (loop){
			statement();
			if( status.er != 0) return;
			t = PeekNextToken();
			if( !strcmp(t.lx, "var") || !strcmp(t.lx, "let") || !strcmp(t.lx, "if") || !strcmp(t.lx, "while") || !strcmp(t.lx, "do") || !strcmp(t.lx, "return"))
			{
				;
			}
			else {
				loop = 0;
			}
		}

		// Check for }
		t = GetNextToken();
		if( t.tp == 6){
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		if( (t.tp == 3) && (t.lx[0] == '}')){
			;
		}
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
	// Check for while
	Token t = GetNextToken();
	if ( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	if( (t.tp == 0) && !strcmp(t.lx, "while"))
	{
		;
	}
	else {
		status.er = syntaxError;
		status.tk = t;
		return;
	}


	// Check for (
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '(')){
		;
	}
	else {
		status.er = openParenExpected;
		status.tk = t;
		return;
	}

	// Check for expression
	expression();
	if( status.er != 0 ) return;

	// Check for )
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == ')')){;}
	else {
		status.er = closeParenExpected;
		status.tk = t;
		return;
	}


	// Check for {
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '{')){;}
	else {
		status.er = openBraceExpected;
		status.tk = t;
		return;
	}

	// Check for statement
	statement();
	if( status.er != 0 ) return;

	// Check for more statements
	t = PeekNextToken();
	int loop = 0;
	if( !strcmp(t.lx, "var") || !strcmp(t.lx, "let") || !strcmp(t.lx, "if") || !strcmp(t.lx, "while") || !strcmp(t.lx, "do") || !strcmp(t.lx, "return"))
	{
		loop = 1;
	}
	while (loop){
		statement();
		if( status.er != 0) return;
		t = PeekNextToken();
		if( !strcmp(t.lx, "var") || !strcmp(t.lx, "let") || !strcmp(t.lx, "if") || !strcmp(t.lx, "while") || !strcmp(t.lx, "do") || !strcmp(t.lx, "return"))
		{
			;
		}
		else {
			loop = 0;
		}
	}

	// Check for }
	t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	if( (t.tp == 3) && (t.lx[0] == '}')){
		;
	}
	else {
		status.er = closeBraceExpected;
		status.tk = t;
		return;
	}

}


// Function for do statement
void doStatement()
{
	Token t = GetNextToken();
	// Check for lexer errors
	if( t.tp == 6 )
	{
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	// Check it is 'do'
	if( (t.tp == 0) && !strcmp(t.lx, "do"))
	{
		;
	}
	else {
		status.er = syntaxError;
		status.tk = t;
		return;
	}

	subroutineCall();
	if( status.er != 0 ) return;

	// Check for semi colon
	t = GetNextToken();
	if( t.tp == 6)
	{
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	if( (t.tp == 3) && (t.lx[0] == ';')){
		;
	}
	else {
		status.er = semicolonExpected;
		status.tk = t;
		return;
	}
}


// Function for subroutine call
void subroutineCall()
{
	Token t = GetNextToken();
	// Check for lexer errors
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	// Check for identifier
	if( t.tp == 1)
	{
		;
	}
	else{
		status.er = idExpected;
		status.tk = t;
		return;
	}

	t = PeekNextToken();
	// Check for lexer errors
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	// If next token is full stop, check for another identifier
	if( (t.tp == 3) && (t.lx[0] == '.'))
	{
		// Consume '.'
		t = GetNextToken();
		t = GetNextToken();
		// Check for lexer errors
		if( t.tp == 6){
			status.er = lexerErr;
			status.tk = t;
			return;
		}

		// Check for identifier
		if( t.tp == 1)
		{
			;
		}
		else{
			status.er = idExpected;
			status.tk = t;
			return;
		}
	}

	t = GetNextToken();
	// Check for lexer errors
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	if( (t.tp == 3) && (t.lx[0] == '('))
	{
		;
	}
	else {
		status.er = openParenExpected;
		status.tk = t;
		return;
	}

	// Only call expression list if next token isnt ')'
	t = PeekNextToken();
	if( (t.tp == 3) && (t.lx[0] == ')')){
		;
	}
	else {
		expressionList();
		if( status.er != 0) return;
	}

	t = GetNextToken();
	// Check for lexer errors
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	if( (t.tp == 3) && (t.lx[0] == ')'))
	{
		;
	}
	else {
		status.er = closeParenExpected;
		status.tk = t;
		return;
	}
}


// Function for expression list
void expressionList() 
{
	// We know if no expression list if first token is ')'
	Token t = PeekNextToken();
	if( (t.tp == 3) && (t.lx[0] == ')'))
	{
		return;
	}

	// Otherwise
	expression();
	if(status.er != 0) return;

	//Loop check for more expressions
	t = PeekNextToken();
	int loop = 0;
	if((t.tp == 3) && (t.lx[0] == ',')){
		loop = 1;
	}
	while (loop){
		// Consume comma
		t = GetNextToken();
		expression();
		if(status.er != 0 ) return;

		t = PeekNextToken();
		if((t.tp == 3) && (t.lx[0] == ',')){
			;
		}
		else {
			loop = 0;
		}
	}
}


// Function for return statements
void returnStatement()
{
	Token t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}

	if( (t.tp == 0)&& !strcmp(t.lx, "return") ){
		;
	}
	else {
		status.er = syntaxError;
		status.tk = t;
		return;
	}

	// Peek at next token to see if it is a semicolon
	t = PeekNextToken();
	if( (t.tp == 3) && (t.lx[0] == ';')){
		;
	}
	else {
		expression();
		if( status.er != 0) return;
	}

	t = GetNextToken();
	if( (t.tp == 3) && (t.lx[0] == ';')){
		;
	}
	else {
		status.er = semicolonExpected;
		status.tk = t;
		return;
	}
}


// Function for an expression
void expression()
{
	relationalExpression();
	if( status.er != 0) return;

	// Check for more relational expressions
	Token t = PeekNextToken();
	int loop = 0;

	if( (t.tp == 3) && ((t.lx[0] == '&') || (t.lx[0] == '|') )){
		loop = 1;
	}
	while (loop)
	{
		// Consume '&' or '|'
		t = GetNextToken();

		relationalExpression();
		
		if( status.er != 0) return;

		if( (t.tp == 3) && ((t.lx[0] == '&') || (t.lx[0] == '|'))){
			;
		}
		else {
			loop = 0;
		}
	}
}


// Function for relational expressions
void relationalExpression()
{	
	Token t = PeekNextToken();
	arithmeticExpression();
	if( status.er != 0) return;

	// Check for more relational expressions
	t = PeekNextToken();
	int loop = 0;

	if( (t.tp == 3) && ( (t.lx[0] == '=') || (t.lx[0] == '<') || (t.lx[0] == '>') )){
		loop = 1;
	}

	while (loop)
	{
		// Consume '=', '<' or '>'
		t = GetNextToken();
		t = PeekNextToken();
		arithmeticExpression();
		if( status.er != 0) return;

		if( (t.tp == 3) && ( (t.lx[0] == '=') || (t.lx[0] == '<') || (t.lx[0] == '>'))){
			;
		}
		else {
			loop = 0;
		}
	}
}


// Function for arithmetic expression
void arithmeticExpression()
{	
	term();
	if( status.er != 0) return;

	// Check for more relational expressions
	Token t = PeekNextToken();
	int loop = 0;

	if( (t.tp == 3) && ( (t.lx[0] == '+') || (t.lx[0] == '-') )){
		loop = 1;
	}

	while (loop)
	{
		// Consume '+' or '-'
		t = GetNextToken();

		term();
		if( status.er != 0) return;

		t = PeekNextToken();
		if( (t.tp == 3) && ( (t.lx[0] == '+') || (t.lx[0] == '-') )){
			;
		}
		else {
			loop = 0;
		}
	}

}


// Function for term
void term()
{	
	factor();
	if( status.er != 0) return;

	// Check for more relational expressions
	Token t = PeekNextToken();
	int loop = 0;

	if( (t.tp == 3) && ( (t.lx[0] == '*') || (t.lx[0] == '/') )){
		loop = 1;
	}

	while (loop)
	{
		// Consume '*' or '/'
		t = GetNextToken();

		factor();
		if( status.er != 0) return;

		t = PeekNextToken();
		if( (t.tp == 3) && ( (t.lx[0] == '*') || (t.lx[0] == '/') )){
			;
		}
		else {
			loop = 0;
		}
	}

}


// Function for factor
void factor()
{
	// Peek first to see if token is - or ~
	Token t = PeekNextToken();
	
	if( (t.tp == 3) && ( (t.lx[0] == '-') || (t.lx[0] == '~'))){
		t = GetNextToken();
	}

	operand();
	if (status.er != 0 ) return;
}


// Function for operand
void operand()
{
	Token t = GetNextToken();
	if( t.tp == 6){
		status.er = lexerErr;
		status.tk = t;
		return;
	}
	//_____INTEGER CONSTANT______
	if( t.tp == 2){;}
	//_______IDENTIFIER________
	else if( t.tp == 1 )
	{
		// Peek next token for '.'
		t = PeekNextToken();
		if( (t.tp == 3) && (t.lx[0] == '.')){
			// Consume '.'
			t = GetNextToken();

			// Check for another identifier
			t = GetNextToken();
			if( t.tp == 6){
				status.er = lexerErr;
				status.tk = t;
				return;
			}
			if( t.tp == 1){;}
			else {
				status.er = idExpected;
				status.tk = t;
				return;
			}

		}

		// Peek next token for a '[' or '('
		t = PeekNextToken();
		if( (t.tp == 3) && (t.lx[0] == '[')){
			// Consume '['
			t = GetNextToken();
			expression();
			if( status.er != 0) return;
			// Check for closing ']'
			t = GetNextToken();
			if( t.tp == 6){
				status.er = lexerErr;
				status.tk = t;
				return;
			}
			if( (t.tp == 3) && (t.lx[0] == ']')){;}
			else{
				status.er = closeBracketExpected;
				status.tk = t;
				return;
			}
		}
		else if( (t.tp == 3) && (t.lx[0] == '(')){
			// Consume '(
			t = GetNextToken();
			expressionList();
			if( status.er != 0) return;
			// Check for closing ')'
			t = GetNextToken();
			if( t.tp == 6){
				status.er = lexerErr;
				status.tk = t;
				return;
			}
			if( (t.tp == 3) && (t.lx[0] == ')')){;}
			else{
				status.er = closeParenExpected;
				status.tk = t;
				return;
			}
		}
	}
	//_______EXPRESSION________
	else if( (t.tp == 3) && (t.lx[0] == '(')){
		printf("%i: open bracket\n", t.ln);
		expression();
		if(status.er != 0 ) return;
		// Check for )
		t = GetNextToken();
		if( t.tp == 6){
			status.er = lexerErr;
			status.tk = t;
			return;
		}
		if( (t.tp == 3) && (t.lx[0] == ')')){
			printf("%i: closing bracket\n", t.ln);
		}
		else {
			status.er = closeParenExpected;
			status.tk = t;
			return;
		}
	}
	//________STRING LITERAL_______
	else if (t.tp == 4) {;}
	//________TRUE/FALSE/NULL/THIS_______
	else if(( t.tp == 0) && ( !strcmp(t.lx, "true") || !strcmp(t.lx, "false") || 
			!strcmp(t.lx, "null") || !strcmp(t.lx, "this") )){;}
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
	// Can be 0+ member declarations
	t = PeekNextToken();
	int loop = 0;
	if( (t.tp == 3 ) && (t.lx[0] == '}')){
		;
	}
	else {
		loop = 1;
	}

	// Only loop if next token is not '}'
	while( loop ){
		memberDeclar();
		// Check if any errors
		
		if( status.er != 0)	{
			return status;
		}
		// Check next token
		t = PeekNextToken();
		// End loop when next token is '}'
		if( (t.tp == 3 ) && (t.lx[0] == '}')){
			loop = 0;
		}
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
	InitLexer("Ball.jack");

	ParserInfo popping = Parse();

	printf("Error %i: Line %i at or near %s\n", popping.er, popping.tk.ln, popping.tk.lx);


	return 1;
}
#endif
