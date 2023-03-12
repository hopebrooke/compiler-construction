/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
Lexer Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Hope Brooke
Student ID: sc21hb
Email: sc21hb@leeds.ac.uk
Date Work Commenced: 20/02
*************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

#include <stdbool.h>

//***********************************
// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE



// VARIABLES AND DECLARATIONS
//***********************************
FILE* file;
int lineNum;    // Current line number
Token peeked;       // Token that has been peeked at but not consumed
char fileName[32];  // Filename
const char * KeyWords[21] = { "class", "constructor", "method", "function", "int", "boolean", "char", "void",
                              "var", "static", "field", "let", "do", "if", "else", "while", "return", "true", 
                              "false", "null", "this" };  // Array of all keywords
const int NumKeyWords = 21; // Amount of keywords
const char Symbols[19] = { '(', ')', '[', ']', '{', '}', ',', ';', '=', '.', '+', '-', '*', '/', '&', '|', 
                          '~', '<', '>'}; // Array of all allowed symbols
const int NumSymbols = 19; // Amount of keywords
const char * TokenString[7] = {"RESWORD","ID","INT","SYMBOL","STRING","EOFile","ERR"}; // Create strings for each token type.



// FUNCTIONS
//***********************************
// Function to check if lexeme is a keyword
int IsKeyword(char * str)
{
  // Go through array of keywords checking if lexeme is contained
  for( int i=0; i<NumKeyWords; i++)
  {
    // Return 1 if lexeme is keyword
    if( strcmp(str, KeyWords[i]) == 0) return 1;
  }
  // Return 0 if not
  return 0;
}

// Function to check if lexeme is an allowed symbol
int IsSymbol(char c)
{
  // Go through array of symbols checking if lexeme is contained
  for( int i=0; i<NumSymbols; i++)
  {
    // Return 1 if lexeme is allowed symbol
    if( Symbols[i] == c ) return 1;
  }
  // Return 0 if not
  return 0;
}



// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name)
{
  // Open the file
  file = fopen(file_name, "r");
  // Check file has been opened successfully
  if(file == 0)
  {
    // Return 0 if unsucessful
    return 0;
  }

  // Set Peeked line number to -1 to start with
  peeked.ln = -1;

  // Set file line number to 1
  lineNum = 1;

  // Set global filename
  strcpy(fileName, file_name);

  // Return 1 if successful
  return 1;
}


// Get the next token from the source file
Token GetNextToken ()
{
  // Create new token
  Token t;
  // Token default type is error
  t.tp = ERR;
  t.ln = 1;
  // Set token filename
  strcpy(t.fl, fileName);
  // Create temporary lexeme to store characters read in
  char tempLex[128];

  // First check if there is a value that has been extracted but not consumed
  if( peeked.ln != -1 )
  { // Return that token if there is
    t = peeked;

    // Set temporary token line number to -1
    peeked.ln = -1;
    return t;
  }


  // If no value to peek at, read in first character
  int c;
  c = getc(file);


  //_________WHITESPACE_________
  // Consume leading white space
  while( isspace(c) )
  {
    // Check if any whitespace is new line:
    if( c == '\n')
      lineNum ++;

    c = getc(file);
  }

  //_________COMMENTS_________
  // Create string array to store comment line if needed
  char line[256];
  
  //Continue until line doesn't start with '/'
  while( c == '/')
  {
    // Consume any comments
    if( c == '/' )
    {
      // If '//' then skip rest of line
      c = getc(file);
      if( c == '/')
      {
        fgets(line, sizeof(line), file);
        // Add to the line number
        lineNum ++;
        c = getc(file);
      }
    
      // If '/*' check for end of comment - '*/'
      else if( c == '*' )
      {

        int nextc;
        nextc = getc(file);
        bool commentLeft = true;

        // Keep reading in comments until a '*/' is found
        while( commentLeft )
        {
          c = nextc;
          nextc = getc(file);
          // Continually check for new line character
          if( c == '\n' )
            lineNum ++;
          // Continually check for end of file character
          if( c == -1 )
          {
            // Set error token values and return
            t.tp = ERR;
            strcpy(t.lx, "Error: unexpected eof in comment");
            t.ec = EofInCom;
            t.ln = lineNum;
            return t;
          }
          // 'c' and 'nextc' contain two most recent characters, check if they are '*/'
          // If end of comment then exit while loop
          if( c == '*' && nextc == '/')
          {
            commentLeft = false;
          }
        }
        // Read in next character
        c = getc(file);
      }
      
      // If not a comment then create '/' symbol token
      else
      {// Put last character back into input stream
        ungetc(c, file);
        // Set and return token for '/' symbol
        t.tp = SYMBOL;
        strcpy(t.lx, "/");
        t.ln = lineNum;
        return t;
      }
    }


    while( isspace(c) ) {
      // Check if c is new line char
      if( c == '\n' )
        lineNum ++;
      c = getc(file);
    }
  }

  //_________WHITESPACE-2_________
  // Consume leading white space after comments
  while( isspace(c) ) 
  {
    // Check for new line character
    if( c == '\n' )
      lineNum ++;
    c = getc(file);
  }

  //_________EOF_________
  // Check for end of file
  if( c == -1 )
  {
    // Set and return EOF token
    t.tp = EOFile;
    strcpy(t.lx, "EOF");
    // Set line number?
    return t;
  }


  //_________STRING_________
  // Check if first character is '"'
  else if( c == '"')
  {
    c = getc(file);
    int i = 0;
    // Keep reading into tempLex until another '"' character
    while( c != '"' )
    {
      // Check for end of file in string:
      if( c == -1)
      {
        // Set error token values and return
        t.tp = ERR;
        strcpy(t.lx, "Error: unexpected eof in string constant");
        t.ec = EofInStr;
        t.ln = lineNum;
        return t;
      }
      // Check for new line in string
      if( c == '\n' )
      {
        // Set error token values and return
        t.tp = ERR;
        strcpy(t.lx, "Error: new line in string constant");
        t.ec = NewLnInStr;
        t.ln = lineNum;
        lineNum++;
        return t;
      }
      tempLex[i] = c;
      c = getc(file);
      i ++;
    }
    // Add end of string character when done
    tempLex[i] = '\0';
    // Set and return string token
    t.tp = STRING;
    strcpy(t.lx, tempLex);
    t.ln = lineNum;
    return t;
  }


  //_________KEYWORDS/IDS_________
  // Check is first character is a letter or underscore
  else if( isalpha(c) || c == '_')
  {
    int i = 0;
    // Keep reading in characters until char that is not letter, digit or '_'
    while( isalpha(c) || isdigit(c) || c == '_' )
    {
      tempLex[i] = c;
      i++;
      c = getc(file);
    }
    // Put last character back into input stream
    ungetc(c, file);
    // Add end of string character
    tempLex[i] = '\0';

    // Check if tempLex is a key/reserved word
    if( IsKeyword(tempLex) == 1 )
    {
      // Set and return keyword token
      t.tp = RESWORD;
      strcpy(t.lx, tempLex);
      t.ln = lineNum;
      return t;
    } 
    else {
      // Set and return ID token
      t.tp = ID;
      strcpy(t.lx, tempLex);
      t.ln = lineNum;
      return t;

    }
  }


  //_________DIGITS_________
  // Check is first character is a digit
  else if( isdigit(c) )
  {
    int i = 0;
    // Keep reading in characters until a non-digit is encountered
    while( isdigit(c) )
    {
      tempLex[i] = c;
      c = getc(file);
      i++;
    }
    // Put last character back into input stream
    ungetc(c, file);
    // Add end of string character
    tempLex[i] = '\0';

    // Set and return int token
    t.tp = INT;
    strcpy(t.lx, tempLex);
    t.ln = lineNum;
    return t;
  }


  //_________SYMBOLS_________
  // If not digit or letter then must be symbol
  else {
    // Check if symbol is allowed
    if( IsSymbol(c) == 1 )
    {
      t.tp = SYMBOL;
      t.lx[0] = c;
      t.lx[1] = '\0';
      t.ln = lineNum;
      return t;
    }
    else {
      // If symbol not allowed return error token
      t.tp = ERR;
      printf(" symbol is %c\n", c);
      strcpy(t.lx, "Error: illegal symbol in source file");
      t.ec = 3;
      t.ln = lineNum;
      return t;
    }
  }

  
  return t;
}


// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken ()
{
  // First get the next token
  Token t = GetNextToken();

  // Store the token in temporary token
  peeked = t;

  // Return the token
  return t;
}


// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
  // Close the opened file
  fclose(file);

	return 0;
}


// do not remove the next line
#ifndef TEST
int main ()
{
	// implement your main function here
  // NOTE: the autograder will not use your main function

  // Initialise lexer
  InitLexer("closeBracketExpected.jack");

  // Read all tokens
  Token t = GetNextToken();
  while(t.tp != 5)
  {
    printf("<%s, %d, %s, %s>\n", t.fl, t.ln, t.lx, TokenString[t.tp]);
    t = GetNextToken();
   
  }
  
	return 0;
}
// do not remove the next line
#endif
