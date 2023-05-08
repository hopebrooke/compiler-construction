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

FILE * fp;
extern int compileNum;

int InitCompiler ()
{
	return 1;
}

ParserInfo compile (char* dir_name)
{
	ParserInfo p;
	p.er = none;

	compileNum = 0;

	// Start symbol tables:
	Constructor();
	
	InitLexer("Sys.jack");
    fp = fopen("Sys.vm", "w");
    p = Parse();
	fclose(fp);
	InitLexer("String.jack");
    fp = fopen("String.vm", "w");
    p = Parse();
	fclose(fp);
	InitLexer("Screen.jack");
    fp = fopen("Screen.vm", "w");
    p = Parse();
	fclose(fp);
	InitLexer("Output.jack");
   	fp = fopen("Output.vm", "w");
    p = Parse();
	fclose(fp);
	InitLexer("Memory.jack");
   	fp = fopen("Memory.vm", "w");
    p = Parse();
	fclose(fp);
	InitLexer("Math.jack");
   	fp = fopen("Math.vm", "w");
    p = Parse();
	fclose(fp);
	InitLexer("Keyboard.jack");
   	fp = fopen("Keyboard.vm", "w");
    p = Parse();
	fclose(fp);
	InitLexer("Array.jack");
    fp = fopen("Array.vm", "w");
    p = Parse();
	fclose(fp);

	struct dirent *de; 
	DIR *dr = opendir(dir_name); 

    if (dr == NULL){
        printf("Could not open current directory");
    }

    // Referencing each file/folder within the directory
    while ((de = readdir(dr)) != NULL) {
		if( (!strcmp(de->d_name + strlen(de->d_name) - 5, ".jack"))) {
			char route[128];
			strcpy(route, dir_name);
			strcat(route, "/");
			strcat(route, de->d_name);
			InitLexer(route);

			// open write file
			char fileName[128] = "";
			for(int i=0; '.' != route[i]; i++) {
				fileName[i] = route[i];
			}
			strcat(fileName, ".vm");
			compileNum = 0;
			fp = fopen(fileName, "w");
			p = Parse();
			fclose(fp);
			if(p.er != 0) {
				return p;
			}

		}
	}
    closedir(dr); 
	
	if(p.er == 0) {
		p = checkUndec();
		if( p.er != 0 ) {
			return p;
		}
	}

	// Parse again:
	dr = opendir(dir_name); 

    if (dr == NULL){
        printf("Could not open current directory");
    }

    // Referencing each file/folder within the directory
    while ((de = readdir(dr)) != NULL) {
		if( (!strcmp(de->d_name + strlen(de->d_name) - 5, ".jack"))) {
			char route[128];
			strcpy(route, dir_name);
			strcat(route, "/");
			strcat(route, de->d_name);
			InitLexer(route);

			// open write file
			char fileName[128] = "";
			for(int i=0; '.' != route[i]; i++) {
				fileName[i] = route[i];
			}
			strcat(fileName, ".vm");
			compileNum = 1;
			fp = fopen(fileName, "w");
			p = Parse();
			fclose(fp);
			if(p.er != 0) {
				break;
			}

		}
	}
    closedir(dr); 
	
	return p;
}


int StopCompiler ()
{
	return 1;
}

int writePush(Segment seg, int index) {
	char line[128];
	strcpy(line, "push");

	switch (seg) {
		case 0: strcat(line, " constant "); break;
		case 1: strcat(line, " argument "); break;
		case 2: strcat(line, " local "); break;
		case 3: strcat(line, " static "); break;
		case 4: strcat(line, " this "); break;
		case 5: strcat(line, " that "); break;
		case 6: strcat(line, " pointer "); break;
		case 7: strcat(line, " temp "); break;
	}

	char indexString[8];
	sprintf(indexString,"%d",index); 
	strcat(line, indexString);
	strcat(line, "\n");
	fputs(line, fp);
	return 0;
}



int writePop(Segment seg, int index) {
	char line[128];
	strcpy(line, "pop");

	switch (seg) {
		case 0: strcat(line, " constant "); break;
		case 1: strcat(line, " argument "); break;
		case 2: strcat(line, " local "); break;
		case 3: strcat(line, " static "); break;
		case 4: strcat(line, " this "); break;
		case 5: strcat(line, " that "); break;
		case 6: strcat(line, " pointer "); break;
		case 7: strcat(line, " temp "); break;
	}

	char indexString[8];
	sprintf(indexString,"%d",index); 
	strcat(line, indexString);
	strcat(line, "\n");
	fputs(line, fp);
	return 0;
}


int writeArithmetic(Command com) {
	char line[128];

	switch (com) {
		case 0: strcpy(line, "add"); break;
		case 1: strcpy(line, "sub"); break;
		case 2: strcpy(line, "neg"); break;
		case 3: strcpy(line, "eq"); break;
		case 4: strcpy(line, "gt"); break;
		case 5: strcpy(line, "lt"); break;
		case 6: strcpy(line, "and"); break;
		case 7: strcpy(line, "or"); break;
		case 8: strcpy(line, "not"); break;
	}

	strcat(line, "\n");
	fputs(line, fp);
	return 0;
}

int writeLabel(char* label) {
	char line[128] = "label ";
	strcat(line, label);
	strcat(line, "\n");
	fputs(line, fp);
	return 0;
}

int writeGoto(char* label) {
	char line[128] = "goto ";
	strcat(line, label);
	strcat(line, "\n");
	fputs(line, fp);
	return 0;
}

int writeIf(char* label) {
	char line[128] = "if-goto ";
	strcat(line, label);
	strcat(line, "\n");
	fputs(line, fp);
	return 0;
}

int writeCall(char* name, int nArgs) {
	char line[128] = "call ";
	strcat(line, name);
	char index[128];
	sprintf(index," %d\n",nArgs); 
	strcat(line, index);
	fputs(line, fp);
	return 0;
}

int writeFunction(char* name, int nLocals) {
	char line[128] = "function ";
	strcat(line, name);
	char index[128];
	sprintf(index," %d\n",nLocals); 
	strcat(line, index);
	fputs(line, fp);
	return 0;
}

int writeReturn() {
	fputs("return\n", fp);
	return 0;
}



#ifndef TEST_COMPILER
// int main ()
// {
// 	InitCompiler ();
// 	ParserInfo p = compile ("Average");
// 	printf("Token: %s, error: %i, at line: %i\n", p.tk.lx, p.er, p.tk.ln);
	
// 	// PrintError (p);
// 	StopCompiler ();
// 	return 1;
// }
#endif
