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
int err_flag;
int end_flag;

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
	//printf("Here findSymbol(): Looking for %s, Kind: %d\n", token.name, kind);
	// start the search backwards from the most recent addition in the symbol table
	int index = tIndex - 1;

	while(index >= 0) {

		// See if the symbol's name and kind value match the token input, AND is unmarked
		if(strcmp(table[index].name, token.name) == 0 && table[index].kind == kind && table[index].mark == UNMARKED) {

			// Maximize the level
			if(table[index].level <= level) {
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
	//printf("Here program(): %d, %s\n", list[listIndex].type, list[listIndex].name);
	emit(7, 0, 0); // EMIT JMP - NOT SURE IF CORRECT
	addToSymbolTable(3, "main", 0, 0, 0, UNMARKED);

	level = -1;

	// execute block
	block(list);
	if(err_flag)
		return;

	//listIndex++;
	//printf("here\n");
	if(list[listIndex].type != periodsym) {
		printparseerror(1);
		err_flag = 1;
		return;
	}
		

	emit(9, 0, 3); // EMIT HALT - NOT SURE IF CORRECT

	/* 
	for each line in code
		if line has OPR 5 (CALL)
			code[line].m = table[code[line].m].addr
	code[0].m = table[0].addr
	*/

	//printf("here2\n");
	int i;
	for(i = 0; i < cIndex; i++) {
		if(code[i].opcode == 5) {
			code[i].m = table[code[i].m].addr;
		}
	}
	code[0].m = table[0].addr;
	//printf("finished program()\n");
}

void block(lexeme *list) {
	//printf("Here block(): %d, %s\n", list[listIndex].type, list[listIndex].name);
	level++;
	
	// DON'T KNOW WHAT THIS IS
	int procedure_idx = tIndex - 1;

	constDeclare(list);
	if(err_flag)
		return;

	// numVars = number of variables declared
	int numVars = varDeclare(list);
	if(err_flag)
		return;

	procDeclare(list);
	if(err_flag)
		return;

	table[procedure_idx].addr = cIndex * 3; // DON'T UNDERSTAND THIS

	if(level == 0)
		emit(6, level, numVars); // WHY DOES M = numVars?
	else
		emit(6, level, numVars + 3);
	
	statement(list);
	if(err_flag)
		return;

	mark(list[listIndex]); // NEED TO IMPLEMENT

	level--;

}

void constDeclare(lexeme *list) {
	//printf("Here constDeclare(): %d, %s\n", list[listIndex].type, list[listIndex].name);
	if(list[listIndex].type == constsym) {
		//printf("Inside if\n");
		do {
			// get next token
			listIndex++;

			if(list[listIndex].type != identsym) {
				printparseerror(2); // NOT SURE IF RIGHT
				//printf("HERE 234\n");
				// EXIT PROGRAM
				err_flag = 1;
				return;
			}
				

			symIdx = multipleDeclareCheck(list[listIndex]); 
			if(symIdx == -1) {
				printparseerror(19); // NOT SURE IF RIGHT
				// EXIT PROGRAM
				err_flag = 1;
				return;
			}
			// save ident name
			int len = strlen(list[listIndex].name);
			char *saveName = (char *) malloc(len + 1);
			strcpy(saveName, list[listIndex].name);

			// get next token
			listIndex++;

			if(list[listIndex].type != assignsym) {
				printparseerror(2); // NOT SURE IF RIGHT
				//printf("HERE 257\n");
				// EXIT PROGRAM
				err_flag = 1;
				return;
			}
				
			// get next token
			listIndex++;

			if(list[listIndex].type != numbersym) {
				printparseerror(2); // NOT SURE IF RIGHT
				//printf("HERE 267\n");
				// EXIT PROGRAM
				err_flag = 1;
				return;
			}
				
			
			// add to symbol table
			addToSymbolTable(1, saveName, list[listIndex].value, level, 0, UNMARKED); // PLEASE CHECK

			free(saveName); // free here, because don't think I have any use for saveName after inserting into symbol table

			// get next token
			listIndex++;
		} while(list[listIndex].type == commasym);

		if(list[listIndex].type != semicolonsym) {
			if(list[listIndex].type == identsym) {
				printparseerror(2);
				err_flag = 1;
				return;
			} else {
				printparseerror(2);
				err_flag = 1;
				return;
			}
		}
	} else {
		//printf("Not a const\n");
	}
	// get next token
	//listIndex++;
}

int varDeclare(lexeme *list) {
	//printf("Here varDeclare(): %d, %s\n", list[listIndex].type, list[listIndex].name);
	int numVars = 0;
	if(list[listIndex].type == varsym) {
		//printf("Inside var\n");
		do {
			numVars++;

			// get next token
			listIndex++;

			if(list[listIndex].type != identsym) {
				printparseerror(3); // PLEASE CHECK
				// EXIT PROGRAM
				err_flag = 1;
				return -1;
			}
				

			symIdx = multipleDeclareCheck(list[listIndex]); // NEED TO IMPLEMENT
			if(symIdx != -1) {
				printparseerror(19); // NOT SURE IF CORRECT
				// EXIT PROGRAM
				err_flag = 1;
				return -1;
			}
			if(level == 0) {
				//printf("Adding %s to symbol table\n", list[listIndex].name);
				addToSymbolTable(2, list[listIndex].name, 0, level, numVars - 1, UNMARKED);
			} else {
				//printf("Adding %s to symbol table\n", list[listIndex].name);
				addToSymbolTable(2, list[listIndex].name, 0, level, numVars + 2, UNMARKED);
			}
			//printsymboltable(); // DELETE
		
			// get next token
			listIndex++;
		} while(list[listIndex].type == commasym);

		if(list[listIndex].type != semicolonsym) {
			if(list[listIndex].type == identsym) {
				printparseerror(3); // PLEASE CHECK
				// EXIT PROGRAM
				err_flag = 1;
				return -1;
			} else {
				printparseerror(3); // PLEASE CHECK
				// EXIT PROGRAM
				err_flag = 1;
				return -1;
			}
		}

		// get next token
		listIndex++;
	} else {
		//printf("not a var\n");
	}
	
	return numVars;
}

void procDeclare(lexeme *list) {
	//printf("Here procDeclare(): %d, %s\n", list[listIndex].type, list[listIndex].name);
	while(list[listIndex].type == procsym) {
		// get next token
		listIndex++;
		
		if(list[listIndex].type != identsym) {
			printparseerror(4); // DOUBLE CHECK
			// EXIT PROGRAM
			err_flag = 1;
			return;
		}
		symIdx = multipleDeclareCheck(list[listIndex]); // NEED TO IMPLEMENT
		if(symIdx != -1) {
			printparseerror(19); // NOT SURE IF CORRECT
			// EXIT PROGRAM
			err_flag = 1;
			return;
		}	
		
		//printf("Adding procedure %s to symbol table\n", list[listIndex].name);
		addToSymbolTable(3, list[listIndex].name, 0, level, 0, UNMARKED);
		//printsymboltable(); // DELETE

		// get next token
		listIndex++;

		if(list[listIndex].type != semicolonsym) {
			printparseerror(4); // DOUBLE CHECK
			// EXIT PROGRAM
			err_flag = 1;
			return;
		}

		// get next token
		listIndex++;

		block(list);
		if(err_flag)
			return;

		//printf("Back in procDeclare()\n");
		if(list[listIndex].type != semicolonsym) {
			printparseerror(4); // NOT SURE IF CORRECT
			// EXIT PROGRAM
			err_flag = 1;
			return;
		}

		// get next token
		listIndex++;

		emit(2, 0, 0); // THIS IS PROBABLY NOT CORRECT - DON'T UNDERSTAND
	}
}

void statement(lexeme *list) {
	//printf("Here statement(): %d, %s\n", list[listIndex].type, list[listIndex].name);

	if(list[listIndex].type == identsym) {
		//printf("\tidentsym\n");
		symIdx = findSymbol(list[listIndex], 2); // NEED TO IMPLEMENT
		//printsymboltable();

		if(symIdx == -1) {
			if(findSymbol(list[listIndex], 1) != findSymbol(list[listIndex], 3)) {
				//printf("ERROR 417\n");
				printparseerror(18); // PROBABLY WRONG
				// EXIT PROGRAM
				err_flag = 1;
				return;
			} else {
				//printf("ERROR 423\n");
				printparseerror(18); // PROBABLY WRONG
				// EXIT PROGRAM
				err_flag = 1;
				return;
			} 
		}
		// get next token
		listIndex++;
		if(list[listIndex].type != assignsym) {
			printparseerror(5);
			err_flag = 1;
			return;
		}
		// get next token
		listIndex++;

		expression(list); 
		if(err_flag)
			return;

		emit(4, level - table[symIdx].level, table[symIdx].addr); // NOT SURE WHERE TO ACCESS OPCODES...code array???
		return;
	}
	if(list[listIndex].type == beginsym) {
		//printf("\tbeginsym\n");
		do {
			//printf("in begin loop\n");
			// get next token
			listIndex++;
			statement(list);
			//printf("Returned back to begin in statement()\n");
			if(err_flag)
				return;	
			if(end_flag)
				return;			
		} while(list[listIndex].type == semicolonsym);

		if(list[listIndex].type != endsym) {
			if(list[listIndex].type == identsym || list[listIndex].type == beginsym || list[listIndex].type == ifsym
			|| list[listIndex].type == whilesym || list[listIndex].type == callsym) { 
				//printf("ERROR 452\n");
				printparseerror(15); // NOT SURE IF CORRECT

				// FIND A WAY TO BREAK OUT OF THE PROGRAM HERE
				err_flag = 1;
				return;
			} else {
				//printf("ERROR 459\n");
				printparseerror(15); // NOT SURE IF CORRECT

				// FIND A WAY TO BREAK OUT OF THE PROGRAM HERE
				err_flag = 1;
				return;
			}
		} 
		// get next token
		listIndex++;
		return;
	}
	if(list[listIndex].type == ifsym) {
		//printf("\tifsym\n");
		// get next token
		listIndex++;
		
		condition(list);
		if(err_flag)
			return;

		int jpcidx = cIndex; // NOT SURE HOW TO USE cIndex/WHEN TO INCREMENT, ETC.

		emit(7, 0, 0); // PROBABLY WRONG
		
		if(list[listIndex].type != thensym) {
			//printf("\tthensym\n");
			//printf("ERROR 484\n");
			printparseerror(8); // DOUBLE CHECK
			// EXIT OUT OF PROGRAM HERE
			err_flag = 1;
			return;
		}

		// get next token
		listIndex++;

		statement(list);
		if(err_flag)
			return;

		if(list[listIndex].type == elsesym) {
			//printf("\telsesym\n");
			int jmpidx = cIndex; 
			emit(7, 0, 0); // HELP HERE
			code[jpcidx].m = cIndex * 3;

			// get next token
			listIndex++;

			statement(list);
			if(err_flag)
				return;

			code[jmpidx].m = cIndex * 3;
		} else {
			code[jpcidx].m = cIndex * 3;
		}

		return;
	}
	if(list[listIndex].type == whilesym) {
		//printf("\twhilesym\n");
		// get next token
		listIndex++;

		int loopidx = cIndex;

		condition(list);
		if(err_flag)
			return;

		if(list[listIndex].type != dosym) {
			//printf("\tdosym\n");
			//printf("ERROR 528\n");
			printparseerror(9); // DOUBLE CHECK
			// EXIT FROM PROGRAM HERE
			err_flag = 1;
			return;
		}
		// get next token
		listIndex++;
		int jpcidx = cIndex;
		emit(8, 0, 0); // HELP HERE

		statement(list);
		if(err_flag)
			return;

		emit(7, 0, loopidx * 3); //  HELP HERE
		code[jpcidx].m = cIndex * 3;
		return;
	}
	if(list[listIndex].type == readsym) {
		//printf("\treadsym\n");
		// get next token
		listIndex++;

		if(list[listIndex].type != identsym) {
			//printf("ERROR 552\n");
			printparseerror(6); // NOT SURE IF CORRECT
			// EXIT PROGRAM
			err_flag = 1;
			return;
		}
		
		symIdx = findSymbol(list[listIndex], 2); // NEED TO IMPLEMENT
		if(symIdx == -1) {
			if(findSymbol(list[listIndex], 1) != findSymbol(list[listIndex], 3)) {
				//printf("ERROR 562\n");
				printparseerror(18); // DON'T KNOW WHAT TO PUT HERE
				// EXIT PROGRAM
				err_flag = 1;
				return;
			} else {
				//printf("ERROR 568\n");
				printparseerror(18); // PROBABLY WRONG
				// EXIT PROGRAM
				err_flag = 1;
				return;
			}
			// get next token
			listIndex++;
			emit(9, 0, 2); // DON'T KNOW 
			emit(4, level - table[symIdx].level, table[symIdx].addr); // DON'T KNOW
			return;
		}
	}
	if(list[listIndex].type == writesym) {
		//printf("\twritesym\n");
		// get next token
		listIndex++;
		expression(list);
		if(err_flag)
			return;

		emit(9, 0, 1); // DON'T KNOW
		return;
	}
	if(list[listIndex].type == callsym) {
		//printf("\tcallsym\n");
		// get next token
		listIndex++;
		symIdx = findSymbol(list[listIndex], 3); // NEED TO IMPLEMENT
		if(symIdx == -1) {
			if(findSymbol(list[listIndex], 1) != findSymbol(list[listIndex], 2)) {
				//printf("ERROR 597\n");
				printparseerror(18); // DON'T KNOW
				// EXIT PROGRAM
				err_flag = 1;
				return;
			} else {
				//printf("ERROR 603\n");
				printparseerror(18); // DON'T KNOW
				// EXIT PROGRAM
				err_flag = 1;
				return;
			}
			// get next token
			listIndex++;
			emit(5, level - table[symIdx].level, symIdx); // DON'T KNOW, IS symidx SUPPOSED TO BE THE M VALUE??
		}
	}
	//printf("end of statement with %d\n", list[listIndex].type);
}

void condition(lexeme *list) {
	//printf("Here condition(): %d, %s\n", list[listIndex].type, list[listIndex].name);
	if(list[listIndex].type == oddsym) {
		// get next token
		listIndex++;
		expression(list);
		if(err_flag)
			return;

		emit(2, 0, 6); // DON'T KNOW
	} else {
		expression(list);
		if(err_flag)
			return;

		if(list[listIndex].type == eqlsym) {
			// get next token
			listIndex++;			
			expression(list);
			if(err_flag)
				return;

			emit(2, 0 , 8); // DON'T KNOW
		} else if(list[listIndex].type == neqsym) {
			// get next token
			listIndex++;
			expression(list);
			if(err_flag)
				return;

			emit(2, 0, 9); // DON'T KNOW
		} else if(list[listIndex].type == lsssym) {
			// get next token
			listIndex++;
			expression(list);
			if(err_flag)
				return;

			emit(2, 0, 10); // DON'T KNOW
		} else if(list[listIndex].type == leqsym) {
			// get next token
			listIndex++;
			expression(list);
			if(err_flag)
				return;

			emit(2, 0, 11); // DON'T KNOW
		} else if(list[listIndex].type == gtrsym) {
			// get next token
			listIndex++;
			expression(list);
			if(err_flag)
				return;
				
			emit(2, 0 , 12); // DON'T KNOW
		} else if(list[listIndex].type == geqsym) {
			// get next token
			listIndex++;
			expression(list);
			if(err_flag)
				return;

			emit(2, 0, 13); // DON'T KNOW
		} else {
			printparseerror(10); // NOT SURE IF CORRECT
			// EXIT PROGRAM
			err_flag = 1;
			return;
		}
	}
}

void expression(lexeme *list) {
	//printf("Here expression(): %d, value: %d\n", list[listIndex].type, list[listIndex].value);
	if(list[listIndex].type == subsym) {
		// get next token
		listIndex++;
		term(list);
		if(err_flag)
			return;

		emit(2, 0, 1); // DON'T KNOW

		while(list[listIndex].type == addsym || list[listIndex].type == subsym) {
			if(list[listIndex].type == addsym) {
				// get next token
				listIndex++;
				term(list);
				if(err_flag)
					return;

				emit(2, 0, 2); // DON'T KNOW
			} else {
				// get next token
				listIndex++;
				term(list);
				if(err_flag)
					return;

				emit(2, 0, 3); // DON'T KNOW
			}
		}
	} else {
		if(list[listIndex].type == addsym) {
			//get next token
			listIndex++;
		}
		term(list);
		if(err_flag)
			return;
		//printf("Back in expression() with %d\n", list[listIndex].type);
		while(list[listIndex].type == addsym || list[listIndex].type == subsym) {
			if(list[listIndex].type == addsym) {
				// get next term
				listIndex++;
				term(list);
				if(err_flag)
					return;

				emit(2, 0, 2); // DON'T KNOW
			} else {
				// get next token
				listIndex++;
				term(list);
				if(err_flag)
					return;

				emit(2, 0, 3); // DON'T KNOW
			}
		}
	}
	if(list[listIndex].type == 28) { // DON'T THINK THIS IS CORRECT. PSEUDOCODE: "if token == (identifier number odd"
		printparseerror(17); // DON'T KNOW WHAT ERROR - MAYBE 17??
		// EXIT PROGRAM
		err_flag = 1;
		return;
	}
}

void term(lexeme *list) {
	//printf("Here term(): %d, value: %d\n", list[listIndex].type, list[listIndex].value);
	factor(list);
	//printf("Back in term() with %d\n", list[listIndex].type);
	while(list[listIndex].type == multsym || list[listIndex].type == divsym || list[listIndex].type == modsym) {
		if(list[listIndex].type == multsym) {
			// get next token
			listIndex++;
			factor(list);
			if(err_flag)
				return;

			emit(2, 0, 4); // DON'T KNOW
		} else if(list[listIndex].type == divsym) {
			// get next token
			listIndex++;
			factor(list);
			if(err_flag)
				return;

			emit(2, 0, 5); // DON'T KNOW
		} else {
			// get next token
			listIndex++;
			factor(list);
			if(err_flag)
				return;

			emit(2, 0, 7); // DON'T KNOW
		}
	}
}

void factor(lexeme *list) {
	//printf("Here factor(): %d, value: %d\n", list[listIndex].type, list[listIndex].value);
	if(list[listIndex].type == identsym) {
		int symidx_var = findSymbol(list[listIndex], 2); // NOT SURE IF ARGUMENTS ARE CORRECT - NEED TO IMPLEMENT 
		int symidx_const = findSymbol(list[listIndex], 1); // NOT SURE IF ARGUMENTS ARE CORRECT - NEED TO IMPLEMENT

		if(symidx_var == -1 && symidx_const == -1) {
			if(findSymbol(list[listIndex], 3) != -1) {
				printparseerror(18); // NOT SURE IF CORRECT
				// EXIT PROGRAM
				err_flag = 1;
				return;
			} else {
				printparseerror(18); // NOT SURE IF CORRECT
				// EXIT PROGRAM
				err_flag = 1;
				return;
			}
		}
		if(symidx_var == -1) { // PSEUDOCODE SAYS: "if symIdx_var == -1 (const)" 
			emit(1, 0, table[symidx_const].val); // DON'T KNOW	
		} else if(symidx_const == -1 || table[symidx_var].level > table[symidx_const].level) {
			emit(3, level - table[symidx_var].level, table[symidx_var].addr); // DON'T KNOW
		} else {
			emit(1, 0, table[symidx_const].val); // DON'T KNOW
		}
		// get next token
		listIndex++;
	} else if(list[listIndex].type == numbersym) {
		emit(1, 0, list[listIndex].value); // DON'T KNOW
		// get next token
		listIndex++;
	} else if(list[listIndex].type == lparensym) {
		// get next token
		listIndex++;
		expression(list);
		if(err_flag)
			return;

		if(list[listIndex].type != rparensym) {
			printparseerror(12); // DOUBLE CHECK
			// EXIT PROGRAM
			err_flag = 1;
			return;
		}
		// get next token
		listIndex++;
	} else {
		printparseerror(11); // NOT SURE
		// EXIT PROGRAM
		err_flag = 1;
		return;
	}
}

instruction *parse(lexeme *list, int printTable, int printCode)
{
	code = NULL;

	// malloc MAX_CODE_LENGTH amount of memory for code
	code = (instruction *) malloc(sizeof(instruction) * MAX_CODE_LENGTH);

	// malloc MAX_SYMBOL_COUNT for table
	table = (symbol *) malloc(sizeof(symbol) * MAX_SYMBOL_COUNT);

	err_flag = 0;
	listIndex = 0;
	cIndex = 0;
	tIndex = 0;
	
	/* START THE PARSING HERE - I THINK THIS IS ALL WE CALL TO START PARSING, BUT DOUBLE CHECK */
	program(list);
	if(err_flag)
		return NULL;
	/* this line is EXTREMELY IMPORTANT, you MUST uncomment it
		when you test your code otherwise IT WILL SEGFAULT in 
		vm.o THIS LINE IS HOW THE VM KNOWS WHERE THE CODE ENDS
		WHEN COPYING IT TO THE PAS
	*/

	// I believe this is the end of the function
	code[cIndex].opcode = -1;
	//printsymboltable();
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
