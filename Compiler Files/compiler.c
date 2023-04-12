/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Compiler Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Hope Brooke
Student ID: 201438799
Email: hopembrooke@gmail.com
Date Work Commenced: 03/04/2023
*************************************************************************/

#include "compiler.h"

#include <stdio.h>
#include <string.h>
#include <dirent.h>


int InitCompiler ()
{
	return 1;
}

ParserInfo compile (char* dir_name)
{
	ParserInfo p;

	// Start symbol tables:
	Constructor();

	struct dirent *de; 
	 DIR *dr = opendir(dir_name); 

    if (dr == NULL){
        printf("Could not open current directory");
    }

    // Referencing each file/folder within the directory
    while ((de = readdir(dr)) != NULL) {
		if( (strcmp(de->d_name, ".")!=0) && (strcmp(de->d_name, "..")!= 0)) {
			char route[128];
			strcpy(route, dir_name);
			strcat(route, "/");
			strcat(route, de->d_name);
			InitLexer(route);
			p = Parse();
			if(p.er != 0) {
				return p;
			}
		}
	}
    closedir(dr); 
	// p.er = none;
	return p;
}

int StopCompiler ()
{


	return 1;
}


#ifndef TEST_COMPILER
int main ()
{
	InitCompiler ();
	ParserInfo p = compile ("redclarClass");
	printf("Token: %s, error: %i\n", p.tk.lx, p.er);
	
	// PrintError (p);
	StopCompiler ();
	return 1;
}
#endif
