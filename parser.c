#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

instruction *code;
int cIndex;
symbol *table;
int tIndex;

/* added these variable */
int level;
int listIndex;

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

/* grammar function declarations */
void program(lexeme *list);
void block(lexeme *list);
void constDeclare(lexeme *list);
int varDeclare(lexeme *list);
void procDeclare(lexeme *list);
void statement();
void mark();

enum
{
	UNMARKED, 
	MARKED
};

/* grammar function definitions */
void program(lexeme *list) {
	emit(7, 0, 0); // EMIT JMP - NOT SURE IF CORRECT
	addToSymbolTable(3, "main", 0, 0, 0, UNMARKED);
	level = -1;

	// execute block
	block(list);

	if(list[listIndex].type != periodsym) {
		printparseerror(1);
		//return NULL;
	}
		

	emit(9, 0, 3); // EMIT HALT - NOT SURE IF CORRECT

	/* 
	TODO
	for each line in code
		if line has OPR 5 (CALL)
			code[line].m = table[code[line].m].addr
	code[0].m = table[0].addr
	*/

}

void block(lexeme *list) {
	level++;
	
	// DON'T KNOW WHAT THIS IS
	int procedure_idx = tIndex - 1;

	constDeclare(list);

	// numVars = number of variables declared
	int numVars = varDeclare(list);

	procDeclare(list);

	table[procedure_idx].addr = cIndex * 3; // DON'T UNDERSTAND THIS

	if(level == 0)
		emit(6, level, numVars); // WHY DOES M = numVars?
	else
		emit(6, level, numVars + 3);
	
	statement(); // NEED TO IMPLEMENT
	mark(); // NEED TO IMPLEMENT

	level--;

}

void constDeclare(lexeme *list) {
	if(list[listIndex].type == constsym) {
		do {
			// get next token
			listIndex++;

			if(list[listIndex].type != identsym) {
				printparseerror(2); // NOT SURE IF RIGHT
				//return NULL;
			}
				

			int symidx = multipleDeclareCheck(list[listIndex]); /** TODO **/ 
			if(symidx == -1) {
				printparseerror(19); // NOT SURE IF RIGHT
				//return NULL;
			}
				
			
			// save ident name
			int len = strlen(list[listIndex].name);
			char *saveName = (char *) malloc(len + 1);
			strcpy(saveName, list[listIndex].name);

			// get next token
			listIndex++;

			if(list[listIndex].type != assignsym) {
				printparseerror(2); // NOT SURE IF RIGHT
				//return NULL;
			}
				
			// get next token
			listIndex++;

			if(list[listIndex].type != numbersym) {
				printparseerror(2); // NOT SURE IF RIGHT
				//return NULL;
			}
				
			
			// add to symbol table
			addToSymbolTable(1, saveName, list[listIndex].value, level, 0, UNMARKED); // PLEASE CHECK
			free(saveName); // free here, because don't think I have any use for saveName after inserting into symbol table

			// get next token
			listIndex++;
		} while(list[listIndex].type == commasym);
	}

	if(list[listIndex].type != semicolonsym) {
		if(list[listIndex].type == identsym) {
			printparseerror(2); // NOT SURE IF RIGHT
			//return NULL;
		} else {
			printparseerror(2); // NOT SURE IF RIGHT
			//return NULL;
		}
			
	}

	// get next token
	listIndex++;
}

int varDeclare(lexeme *list) {
	int numVars = 0;
	if(list[listIndex].type == varsym) {
		do {
			numVars++;

			// get next token
			listIndex++;

			if(list[listIndex].type != identsym) {
				printparseerror(3); // PLEASE CHECK
				//return NULL;
			}
				

			int symidx = multipleDeclareCheck(list[listIndex]); // NEED TO IMPLEMENT
			if(symidx != -1)
				printparseerror(19); // NOT SURE IF CORRECT
			
			if(level == 0) {
				addToSymbolTable(2, list[listIndex].name, 0, level, numVars - 1, UNMARKED);
				//return NULL;
			} else {
				addToSymbolTable(2, list[listIndex].name, 0, level, numVars + 2, UNMARKED);
				//return NULL;
			}
		
			// get next token
			listIndex++;
		} while(list[listIndex].type == commasym);

		if(list[listIndex].type != semicolonsym) {
			if(list[listIndex].type == identsym) {
				printparseerror(3); // PLEASE CHECK
				//return;
			} else {
				printparseerror(3); // PLEASE CHECK
				//return;
			}
		}

		// get next token
		listIndex++;
	}
	
	return numVars;
}

void procDeclare(lexeme *list) {
	while(list[listIndex].type == procsym) {
		// get next token
		listIndex++;
		
		if(list[listIndex].type != identsym) {
			printparseerror(4); // DOUBLE CHECK
			//return;
		}
		int symidx = multipleDeclareCheck(list[listIndex]); // NEED TO IMPLEMENT
		if(symidx != -1) {
			printparseerror(19); // NOT SURE IF CORRECT
			//return;
		}	
			
		addToSymbolTable(3, list[listIndex].name, 0, level, 0, UNMARKED);

		// get next token
		listIndex++;

		if(list[listIndex].type != semicolonsym) {
			printparseerror(4); // DOUBLE CHECK
			//return;
		}

		// get next token
		listIndex++;

		block(list);

		if(list[listIndex].type != semicolonsym) {
			printparseerror(4); // NOT SURE IF CORRECT
			// return;
		}

		// get next token
		listIndex++;

		emit(code[cIndex].opcode, code[cIndex].l, code[cIndex].m); // THIS IS PROBABLY NOT CORRECT - DON'T UNDERSTAND
	}
}

instruction *parse(lexeme *list, int printTable, int printCode)
{
	code = NULL;

	// malloc MAX_CODE_LENGTH amount of memory for code
	code = (instruction *) malloc(sizeof(instruction) * MAX_CODE_LENGTH);

	// malloc MAX_SYMBOL_COUNT for table
	table = (symbol *) malloc(sizeof(symbol) * MAX_SYMBOL_COUNT);

	listIndex = 0;
	

	/* this line is EXTREMELY IMPORTANT, you MUST uncomment it
		when you test your code otherwise IT WILL SEGFAULT in 
		vm.o THIS LINE IS HOW THE VM KNOWS WHERE THE CODE ENDS
		WHEN COPYING IT TO THE PAS
	*/

	// I believe this is the end of the function
	code[cIndex].opcode = -1;
	
	//printf("finished\n");
	return code;
}


void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}


void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}
	
	free(code);
	free(table);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark); 
	
	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}
