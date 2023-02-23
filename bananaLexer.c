#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum TT {keyword, id, num, sym, eof, err} TokenTypes;

typedef struct {
	char lex[128];
	TokenTypes t;
} Token;

FILE* f;

// define all the keywords
const char* KeyWords[12] = { "bello", "para", "tu", "minion", "banana", "bapple", "gelato", "bee", "do", "stopa", "bank", "yu" };
const char Symbols[6] = { ':', '@', '+', '-', '*'};

// open the file
int Init()
{
	f = fopen("bob.txt", "r");
	if (f == 0)
	{
		//printf("Error: can't open file\n");
		return 0; // 0 means error
	}
    //printf("File opened\n");
	return 1; // sucess
}


// checks if lexeme is keyword, returns 1 if yes
int IsKeyWord(char * str)
{
    // iterate through KeyWords array comparing string
    for(int i=0; i<12; i++)
    {
        if(strcmp(str, KeyWords[i]) == 0)
        {
            //printf("%s is a keyword: %s\n", str, KeyWords[i]);
            return 1;
        }
    }
    //printf("%s is not a keyword\n", str);
    return 0;
}


// checks if lexeme is allowed symbol, returns 1 if yes
int IsAllowedSymbol(char c)
{
    // iterate through symbol array comparing character
    for(int i=0; i<12; i++)
    {
        if(c == Symbols[i])
        {
            //printf("%c is an allowed symbol: %c\n",c, Symbols[i]);
            return 1;
        }
    }
    //printf("%c is not an allowed symbol\n", c);
    return 0;
}


// get the next token
Token GetToken()
{

    //create new token
	Token t;
	
    // get first character
	int c;
    char line[256];
	c = getc(f);
    //printf("%c has been read \n", c);

    // consume leading white space
	while (isspace(c))
		c = getc(f);

    // consume any comments
    if(c == '!')
    {
        fgets(line, sizeof(line), f); 
        //printf("comment detected: %s\n", line);
        // get next char
        c = getc(f);
    }

    // if end of file then return end of file token
	if (c == EOF)
	{
        //printf("end of file detected \n");
        strcpy (t.lex , "EOF");
		t.t = eof;
		return t;
	}

	char temp[128];
	int i = 0;

	if (isalpha(c))
	{
        //printf("alpha detected: %c\n", c);
        // if lexeme starts with alphabetic character, keep reading in until not an alphabetic character
		while (isalpha(c))
		{
            //printf("continuing to read in alpha chars: %c\n", c);
			temp[i] = c;
			i++;
			c = getc(f);
		}
		temp[i] = '\0';

		// if lexeme is keyword return keyword token
        if(IsKeyWord(temp)){
            strcpy(t.lex, temp);
            t.t = keyword;
            //printf("identified keyword, returning token\n");
            return t;
        }
        // if not, return id token
		else
        {
            strcpy(t.lex, temp);
            t.t = id;
            //printf("not a keyword so returning identifier token for %s\n", t.lex);
            return t;
        }
	}
	else if (isdigit(c))
	{
		// if lexeme starts with digit, read in full number
        while (isdigit(c))
        {
            temp[i] = c;
            i++;
            c = getc(f);
        }
        // check if decimal
        if (c == '.')
        {
            // keep reading after decimal
            while (isdigit(c))
            {
                temp[i] = c;
                i++;
                c = getc(f);
            }
        }

        // check for suffixes, if b or p then add to number
        if (c == 'b' || c == 'p')
        { 
            temp[i] = c;
        }
        strcpy(t.lex, temp);
        t.t = num;
        return t;
	}

	// if character is symbol, return symbol token
    else if(IsAllowedSymbol(c))
    {
        t.lex[0] = c;
        t.lex[1] = '\0';
        t.t = sym;
        return t;
    }

    // if its a symbol that isn't allowed, return error
    else
    {
        t.lex[0] = c;
        t.lex[1] = '\0';
        t.t = err;
        return t;
    }
	
}


int main()
{

    // open file
	Init();
    Token t;

    t = GetToken();

    // keep reading in tokens until end of file
    while(strcmp(t.lex, "EOF") != 0){
        printf("%s\n", t.lex);
        t = GetToken();
    }

}
