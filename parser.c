#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

instruction *code;
int cIndex;
symbol *table;
int tIndex; // TODO - increment this every time something is added to the symbol table

/* added these variable */
int level;
int listIndex;
int symIdx; // I think this keeps track of the current table index

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
void statement(lexeme *list);
void expression(lexeme *list);
void condition(lexeme *list);
void expression(lexeme *list);
void term(lexeme *list);
void factor(lexeme *list);

/* helper function declarations */
void mark(lexeme token); // TODO
int multipleDeclareCheck(lexeme token); // IMPLEMENTED - CHECK TO SEE IF CORRECT
int findSymbol(lexeme token, int kind); // NEED TO IMPLEMENT

enum
{
	UNMARKED, 
	MARKED
};

/*
	findSymbol: 

	This function does a linear search for the given name. 
	An entry only matches if it has the correct name AND kind value AND is unmarked. 
	Then it tries to maximize the level value
*/

// TODO - DON'T THINK THIS IS CORRECT
int findSymbol(lexeme token, int kind) {
	// start the search backwards from the most recent addition in the symbol table
	int index = tIndex - 1;

	while(index >= 0) {

		// See if the symbol's name and kind value match the token input, AND is unmarked
		if(strcmp(table[index].name, token.name) == 0 && table[index].kind == kind && table[index].mark == UNMARKED) {

			// Maximize the level
			if(table[index].level > level) {
				return index;
			}
		}	
		index--;
	}
	return -1;
}

/* 
	mark():

	This function starts at the end of the table and works backward. 
	It ignores marked entries. It looks at an entry’s level and if it is equal to the 
	current level it marks that entry. It stops when it finds an unmarked entry whose 
	level is less than the current level
*/

void mark(lexeme token) {
	// tIndex keeps track of the current index that is available to be inserted at in the symbol table
	// so I think I should start the linear search at tIndex - 1 to start at an index that has data in it
	int index = tIndex - 1;

	while(index >= 0) {

		// only check entries that are UNMARKED
		if(table[index].mark == UNMARKED) {

			// if the current symbol i'm looking at's level is equal to the current level, MARK it
			if(table[index].level == level) {
				table[index].mark = MARKED;
			} 

			// if the current symbol's level is lower than the current level, exit
			else if(table[index].level < level) {
				break;
			}
		}

		// decrement through the symbol table
		index--;
	}
}

/* 
	multipleDeclareCheck():

	This function should do a linear pass through the symbol table 
	looking for the symbol name given. If it finds that name, it 
	checks to see if it’s unmarked (no? keep searching). If it finds an 
	unmarked instance, it checks the level. If the level is equal to the 
	current level, it returns that index. Otherwise it keeps searching until 
	it gets to the end of the table, and if nothing is found, returns -1
*/
int multipleDeclareCheck(lexeme token) {
	int index = tIndex - 1;

	while(index >= 0) {
		// check to see if the name of the token matches the symbol that tIndex is pointing to 
		if(strcmp(table[index].name, token.name) == 0) {

			// if the names match, check to see if it is unmarked
			if(table[index].mark == UNMARKED) {

				// if unmarked, check the level and see if it equals the current level
				if(table[index].level == level) {

					// return the index of the symbol table if the levels match
					return index;
				}
			}
		}

		// keep decrementing through the table, comparing each entry in the symbol table to the token input
		index--;
	}

	// if the program reaches here, nothing in the symbol table matched the input, and so return -1
	return -1;
}

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
	
	statement(list); 
	mark(list[listIndex]); // NEED TO IMPLEMENT

	level--;

}

void constDeclare(lexeme *list) {
	if(list[listIndex].type == constsym) {
		do {
			// get next token
			listIndex++;

			if(list[listIndex].type != identsym) {
				printparseerror(2); // NOT SURE IF RIGHT
				// EXIT PROGRAM
			}
				

			symIdx = multipleDeclareCheck(list[listIndex]); 
			if(symIdx == -1) {
				printparseerror(19); // NOT SURE IF RIGHT
				// EXIT PROGRAM
			}
			// save ident name
			int len = strlen(list[listIndex].name);
			char *saveName = (char *) malloc(len + 1);
			strcpy(saveName, list[listIndex].name);

			// get next token
			listIndex++;

			if(list[listIndex].type != assignsym) {
				printparseerror(2); // NOT SURE IF RIGHT
				// EXIT PROGRAM
			}
				
			// get next token
			listIndex++;

			if(list[listIndex].type != numbersym) {
				printparseerror(2); // NOT SURE IF RIGHT
				// EXIT PROGRAM
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
			// EXIT PROGRAM
		} else {
			printparseerror(2); // NOT SURE IF RIGHT
			// EXIT PROGRAM
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
				// EXIT PROGRAM
			}
				

			symIdx = multipleDeclareCheck(list[listIndex]); // NEED TO IMPLEMENT
			if(symIdx != -1) {
				printparseerror(19); // NOT SURE IF CORRECT
				// EXIT PROGRAM
			}
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
				// EXIT PROGRAM
			} else {
				printparseerror(3); // PLEASE CHECK
				// EXIT PROGRAM
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
			// EXIT PROGRAM
		}
		symIdx = multipleDeclareCheck(list[listIndex]); // NEED TO IMPLEMENT
		if(symIdx != -1) {
			printparseerror(19); // NOT SURE IF CORRECT
			// EXIT PROGRAM
		}	
			
		addToSymbolTable(3, list[listIndex].name, 0, level, 0, UNMARKED);

		// get next token
		listIndex++;

		if(list[listIndex].type != semicolonsym) {
			printparseerror(4); // DOUBLE CHECK
			// EXIT PROGRAM
		}

		// get next token
		listIndex++;

		block(list);

		if(list[listIndex].type != semicolonsym) {
			printparseerror(4); // NOT SURE IF CORRECT
			// EXIT PROGRAM
		}

		// get next token
		listIndex++;

		emit(code[cIndex].opcode, code[cIndex].l, code[cIndex].m); // THIS IS PROBABLY NOT CORRECT - DON'T UNDERSTAND
	}
}

void statement(lexeme *list) {
	if(list[listIndex].type == identsym) {
		symIdx = findSymbol(list[listIndex], 2); // NEED TO IMPLEMENT

		if(symIdx == -1) {
			if(findSymbol(list[listIndex], 1) != findSymbol(list[listIndex], 3)) {
				printparseerror(18); // PROBABLY WRONG
				// EXIT PROGRAM
			} else {
				printparseerror(18); // PROBABLY WRONG
				// EXIT PROGRAM
			} 
			// get next token
			listIndex++;

			expression(list); 

			emit(STO, level - table[symIdx].level, table[symIdx].addr); // NOT SURE WHERE TO ACCESS OPCODES...code array???
			return;
		}
	}
	if(list[listIndex].type == beginsym) {
		do {
			// get next token
			listIndex++;
			statement(list);
		} while(list[listIndex].type == semicolonsym);

		if(list[listIndex].type != endsym) {
			if(list[listIndex].type == identsym || list[listIndex].type == beginsym || list[listIndex].type == ifsym
			|| list[listIndex].type == whilesym || list[listIndex].type == callsym) { 
				printparseerror(15); // NOT SURE IF CORRECT

				// FIND A WAY TO BREAK OUT OF THE PROGRAM HERE
			} else {
				printparseerror(15); // NOT SURE IF CORRECT

				// FIND A WAY TO BREAK OUT OF THE PROGRAM HERE
			}
		}
		// get next token
		listIndex++;
		return;
	}
	if(list[listIndex].type == ifsym) {
		// get next token
		listIndex++;
		
		condition(list);

		int jpcidx = cIndex; // NOT SURE HOW TO USE cIndex/WHEN TO INCREMENT, ETC.

		emit(JPC, 0, 0); // PROBABLY WRONG
		
		if(list[listIndex].type != thensym) {
			printparseerror(8); // DOUBLE CHECK
			// EXIT OUT OF PROGRAM HERE
		}

		// get next token
		listIndex++;

		statement(list);

		if(list[listIndex].type == elsesym) {
			int jmpidx = cIndex; 
			emit(JMP, 0, 0); // HELP HERE
			code[jpcidx].m = cIndex * 3;

			// get next token
			listIndex++;

			statement(list);
			
			code[jmpidx].m = cIndex * 3;
		} else {
			code[jpcidx].m = cIndex * 3;
		}

		return;
	}
	if(list[listIndex].type == whilesym) {
		// get next token
		listIndex++;

		int loopidx = cIndex;

		condition(list);
		if(list[listIndex].type != dosym) {
			printparseerror(9); // DOUBLE CHECK
			// EXIT FROM PROGRAM HERE
		}
		// get next token
		listIndex++;
		int jpcidx = cIndex;
		emit(JPC, level, 0); // HELP HERE

		statement(list);
		emit(JMP, level, loopidx * 3); //  HELP HERE
		code[jpcidx].m = cIndex * 3;
		return;
	}
	if(list[listIndex].type == readsym) {
		// get next token
		listIndex++;

		if(list[listIndex].type != identsym) {
			printparseerror(6); // NOT SURE IF CORRECT
			// EXIT PROGRAM
		}
		
		symIdx = findSymbol(list[listIndex], 2); // NEED TO IMPLEMENT
		if(symIdx == -1) {
			if(findSymbol(list[listIndex], 1) != findSymbol(list[listIndex], 3)) {
				printparseerror(18); // DON'T KNOW WHAT TO PUT HERE
				// EXIT PROGRAM
			} else {
				printparseerror(18); // PROBABLY WRONG
				// EXIT PROGRAM
			}
			// get next token
			listIndex++;
			emit(READ, , ); // DON'T KNOW 
			emit(STO, level - table[symIdx].level, table[symIdx].addr); // DON'T KNOW
			return;
		}
	}
	if(list[listIndex].type == writesym) {
		// get next token
		listIndex++;
		expressio(list);
		emit(WRITE, ,); // DON'T KNOW
		return;
	}
	if(list[listIndex].type == callsym) {
		// get next token
		listIndex++;
		symIdx = findSymbol(list[listIndex], 3); // NEED TO IMPLEMENT
		if(symIdx == -1) {
			if(findSymbol(list[listIndex], 1) != findSymbol(list[listIndex], 2)) {
				printparseerror(18); // DON'T KNOW
				// EXIT PROGRAM
			} else {
				printassemblycode(18); // DON'T KNOW
				// EXIT PROGRAM
			}
			// get next token
			listIndex++;
			emit(CAL, level - table[symIdx].level, symIdx); // DON'T KNOW, IS symidx SUPPOSED TO BE THE M VALUE??
		}
	}
}

void condition(lexeme *list) {
	if(list[listIndex].type == oddsym) {
		// get next token
		listIndex++;
		expression(list);
		emit(ODD, ,); // DON'T KNOW
	} else {
		expression(list);
		if(list[listIndex].type == eqlsym) {
			// get next token
			listIndex++;			
			expression(list);
			emit(EQL,,); // DON'T KNOW
		} else if(list[listIndex].type == neqsym) {
			// get next token
			listIndex++;
			expression(list);
			emit(NEQ,,); // DON'T KNOW
		} else if(list[listIndex].type == lsssym) {
			// get next token
			listIndex++;
			expression(list);
			emit(LSS, ,); // DON'T KNOW
		} else if(list[listIndex].type == leqsym) {
			// get next token
			listIndex++;
			expression(list);
			emit(LEQ,,); // DON'T KNOW
		} else if(list[listIndex].type == gtrsym) {
			// get next token
			listIndex++;
			expression(list);
			emit(GTR,,); // DON'T KNOW
		} else if(list[listIndex].type == geqsym) {
			// get next token
			listIndex++;
			expression(list);
			emit(GEQ, ,); // DON'T KNOW
		} else {
			printparseerror(10); // NOT SURE IF CORRECT
			// EXIT PROGRAM
		}
	}
}

void expression(lexeme *list) {
	if(list[listIndex].type == subsym) {
		// get next token
		listIndex++;
		term(list);
		emit(NEG,,); // DON'T KNOW

		while(list[listIndex].type == addsym || list[listIndex].type == subsym) {
			if(list[listIndex].type == addsym) {
				// get next token
				listIndex++;
				term(list);
				emit(ADD, ,); // DON'T KNOW
			} else {
				// get next token
				listIndex++;
				term(list);
				emit(SUB,,); // DON'T KNOW
			}
		}
	} else {
		if(list[listIndex].type == addsym) {
			//get next token
			listIndex++;
		}
		term(list);
		while(list[listIndex].type == addsym || list[listIndex].type == subsym) {
			if(list[listIndex].type == addsym) {
				// get next term
				listIndex++;
				term(list);
				emit(ADD, , ); // DON'T KNOW
			} else {
				// get next token
				listIndex++;
				term(list);
				emit(SUB,,); // DON'T KNOW
			}
		}
	}
	if(list[listIndex].type == 28) { // DON'T THINK THIS IS CORRECT. PSEUDOCODE: "if token == (identifier number odd"
		printparseerror(17); // DON'T KNOW WHAT ERROR - MAYBE 17??
		// EXIT PROGRAM
	}
}

void term(lexeme *list) {
	factor(list);
	while(list[listIndex].type == multsym || list[listIndex].type == divsym || list[listIndex].type == modsym) {
		if(list[listIndex].type == multsym) {
			// get next token
			listIndex++;
			factor(list);
			emit(MUL,,); // DON'T KNOW
		} else if(list[listIndex].type == divsym) {
			// get next token
			listIndex++;
			factor(list);
			emit(DIV,,); // DON'T KNOW
		} else {
			// get next token
			listIndex++;
			factor(list);
			emit(MOD,,); // DON'T KNOW
		}
	}
}

void factor(lexeme *list) {
	if(list[listIndex].type == identsym) {
		int symidx_var = findSymbol(list[listIndex], 2); // NOT SURE IF ARGUMENTS ARE CORRECT - NEED TO IMPLEMENT 
		int symidx_const = findSymbol(list[listIndex], 1); // NOT SURE IF ARGUMENTS ARE CORRECT - NEED TO IMPLEMENT

		if(symidx_var == -1 && symidx_const == -1) {
			if(findSymbol(list[listIndex], 3) != -1) {
				printparseerror(18); // NOT SURE IF CORRECT
				// EXIT PROGRAM
			} else {
				printparseerror(18); // NOT SURE IF CORRECT
				// EXIT PROGRAM
			}
		}
		if(symidx_var == -1) { // PSEUDOCODE SAYS: "if symIdx_var == -1 (const)" 
			emit(LIT, , table[symidx_const].val); // DON'T KNOW	
		} else if(symidx_const == -1 || table[symidx_var].level > table[symidx_const].level) {
			emit(LOD, level - table[symidx_var].level, table[symidx_var].addr); // DON'T KNOW
		} else {
			emit(LIT, , table[symidx_const].val); // DON'T KNOW
		}
		// get next token
		listIndex++;
	} else if(list[listIndex].type == numbersym) {
		emit(LIT,,); // DON'T KNOW
		// get next token
		listIndex++;
	} else if(list[listIndex].type == lparensym) {
		// get next token
		listIndex++;
		expression(list);
		if(list[listIndex].type != rparensym) {
			printparseerror(12); // DOUBLE CHECK
			// EXIT PROGRAM
		}
		// get next token
		listIndex++;
	} else {
		printparseerror(11); // NOT SURE
		// EXIT PROGRAM
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
	
	/* START THE PARSING HERE - I THINK THIS IS ALL WE CALL TO START PARSING, BUT DOUBLE CHECK */
	program(list);

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
