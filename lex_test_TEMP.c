/*
	This is the lex.c file for the UCF Fall 2021 Systems Software Project.
	For HW2, you must implement the function lexeme *lexanalyzer(char *input).
	You may add as many constants, global variables, and support functions
	as you desire.

	If you choose to alter the printing functions or delete list or lex_index,
	you MUST make a note of that in you readme file, otherwise you will lose
	5 points.
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h> /* DELETE */
#include "compiler.h"
#define MAX_NUMBER_TOKENS 500
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5
#define NUM_RESERVED 14

lexeme *list;
int lex_index;
char iden_buff[MAX_IDENT_LEN];
const char *rw[NUM_RESERVED]; // reserved words

void printlexerror(int type);
void printtokens();
int ident_reserved(char **input_ptr, int *err_type, const char **reserved_words, char *result, int *rw_index);
int number(char **input_ptr, int *err_type, int *result);

int number(char **input_ptr, int *err_type, int *result)
{
	int i;
	for(i = 0; isdigit(**input_ptr); i++, (*input_ptr)++) {
		*result = *result * 10 + (**input_ptr - '0');
	}

	// length of number cannot surpass 5
	if((i + 1) > MAX_NUMBER_LEN) {
		*err_type = 3;
		return 0;
	}

	
	// if alpha before whitespace, it is illegal identifier
	if(isalpha(**input_ptr)) {
		*err_type = 2;
		return 0;
	} else {

		// get to the next token
		while(isspace(**input_ptr))
			(*input_ptr)++;

		return 1;
	}
	
	return 0;
}

// TODO
int ident_reserved(char **input_ptr, int *err_type, const char **reserved_words, char *result, int *rw_index)
{
	char word_buffer[MAX_IDENT_LEN + 1]; // will hold an identifier
	//char *ptr = *input_ptr;
	int iden_len = 0; // cannot be > MAX_IDENT_LEN

	memset(word_buffer, 0, sizeof(word_buffer)); // zero out word_buffer
	
	// extract word
	int i;

	// RUSHED THE LOGIC HERE, PROBABLY BETTER WAY TO WRITE THIS
	for(i = 0; (isalpha(**input_ptr) || isdigit(**input_ptr)) && **input_ptr; (*input_ptr)++, i++) {
		if((i + 1) > MAX_IDENT_LEN) {
			*err_type = 4;
			return 0;
		}

		word_buffer[i] = **input_ptr;
	}

	// get to start of next token
	while(isspace(**input_ptr))
		(*input_ptr)++;

	for(i = 0; i < NUM_RESERVED; i++) {
		if(strcmp(word_buffer, reserved_words[i]) == 0)
			*rw_index = i;
	}

	strcpy(result, word_buffer);
	printf("%s\n", result); 
	return 1;
}

lexeme *lexanalyzer(char *input)
{
	int err_type = 0, index = 0;
	char *input_ptr = input;

	// initialize string array
	rw[0] = "const";
	rw[1] = "var";
	rw[2] = "procedure";
	rw[3] = "begin";
	rw[4] = "end";
	rw[5] = "while";
	rw[6] = "do";
	rw[7] = "if";
	rw[8] = "then";
	rw[9] = "else";
	rw[10] = "call";
	rw[11] = "write";
	rw[12] = "read";
	rw[13] = "odd";

	// list = (lexeme *) malloc(sizeof(lexeme) * MAX_NUMBER_TOKENS);

	while (*input_ptr)
	{	
		/*
		if(list) {
			for(int i = 0; i < index; i++) {
				printf("%d | %d | %s \n", list[i].value, list[i].type, list[i].name);
			}
		}
		*/
		printf("%c\n", *input_ptr); /* DELETE */
		//sleep(1);

		switch (*input_ptr)
		{
		case ';':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = semicolonsym;
			input_ptr++;

			// get to next token
				while(isspace(*input_ptr))
					input_ptr++;

			index++;
			break;
		case '+':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = addsym;
			input_ptr++;

			// get to next token
				while(isspace(*input_ptr))
					input_ptr++;

			index++;
			break;
		case '-':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = subsym;
			input_ptr++;

			// get to next token
				while(isspace(*input_ptr))
					input_ptr++;
			
			index++;
			break;
		case '*':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = multsym;
			input_ptr++;

			// get to next token
				while(isspace(*input_ptr))
					input_ptr++;

			index++;
			break;
		case '%':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = modsym;
			input_ptr++;

			// get to next token
				while(isspace(*input_ptr))
					input_ptr++;
		
			index++;
			break;
		case '(':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = lparensym;
			input_ptr++;

			// get to next token
				while(isspace(*input_ptr))
					input_ptr++;

			index++;
			break;
		case ')':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = rparensym;
			input_ptr++;

			// get to next token
			while(isspace(*input_ptr))
				input_ptr++;

			index++;
			break;
		case ',':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = commasym;
			input_ptr++;
			// get to next token
				while(isspace(*input_ptr))
					input_ptr++;
			index++;
			break;
		case '.':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = periodsym;
			input_ptr++;
			// get to next token
				while(isspace(*input_ptr))
					input_ptr++;
			index++;
			break;
		// multichar symbols
		case '!':
			if (*(input_ptr + 1) == '=')
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = neqsym;
				input_ptr += 2; // advance pass the '='

				// get to next token
				while(isspace(*input_ptr))
					input_ptr++;

				index++;
			}
			else {
				//printf("here !\n");
				printlexerror(1); // invalid symbol
				exit(EXIT_FAILURE);
			}
				
			break;
		case ':':
			if (*(input_ptr + 1) == '=')
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = assignsym;
				input_ptr += 2;

				// get to next token
				while(isspace(*input_ptr))
					input_ptr++;

				index++;
			}
			else {
				//printf("here :\n");
				printlexerror(1);
				exit(EXIT_FAILURE);
			}
				
			break;
		case '=':
			if (*(input_ptr + 1) == '=')
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = eqlsym;
				input_ptr += 2;

				// get to next token
				while(isspace(*input_ptr))
					input_ptr++;

				index++;
			}
			else {
				//printf("here =\n");
				printlexerror(1);
				exit(EXIT_FAILURE);
			}
				
			break;
		case '<':
			if (*(input_ptr + 1) == '=')
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = leqsym;
				input_ptr += 2;

				// get to next token
				while(isspace(*input_ptr))
					input_ptr++;

				index++;
			}
			else
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = lsssym;
				input_ptr++;
				index++;
			}
			break;
		case '>':
			if (*(input_ptr + 1) == '=')
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = geqsym;
				input_ptr += 2;

				// get to next token
				while(isspace(*input_ptr))
					input_ptr++;

				index++;
			}
			else
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = gtrsym;
				input_ptr++;
				// get to next token
				while(isspace(*input_ptr))
					input_ptr++;
				index++;
			}
			break;
		case '/':
			if (*(input_ptr + 1) == '/')
			{
				for (; *input_ptr != '\n'; input_ptr++)
					;
				
				// get to next token
				while(isspace(*input_ptr))
					input_ptr++;
			}
			else
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = divsym;
				index++;
				// get to next token
				while(isspace(*input_ptr))
					input_ptr++;
			}
			break;
		default:
			// if *input_ptr is a letter, check for identifier or reserved vocab (const, while, for, etc)
			if (isalpha(*input_ptr))
			{
				char result[MAX_IDENT_LEN + 1];
				int rw_index = -1;
				if(!ident_reserved(&input_ptr, &err_type, rw, result, &rw_index)) {
					printlexerror(err_type);
					exit(EXIT_FAILURE);
				} else {
					list = (lexeme *) realloc(list, sizeof(lexeme) * (index + 1));

					// copy result into name
					strcpy(list[index].name, result);

					// rw_index is still -1, then result is an identifier
					if(rw_index == -1) 
						list[index].type = identsym;
					
					// otherwise, assign type to the correct reserved word
					else {
						
						switch(rw_index) {
							case 0:
								list[index].type = constsym;
								break;
							case 1:
								list[index].type = varsym;
								break;
							case 2:
								list[index].type = procsym;
								break;
							case 3:
								list[index].type = beginsym;
								break;
							case 4:
								list[index].type = endsym;
								break;
							case 5:
								list[index].type = whilesym;
								break;
							case 6:
								list[index].type = dosym;
								break;
							case 7:
								list[index].type = ifsym;
								break;
							case 8:
								list[index].type = thensym;
								break;
							case 9:
								list[index].type = elsesym;
								break;
							case 10:
								list[index].type = callsym;
								break;
							case 11:
								list[index].type = writesym;
								break;
							case 12:
								list[index].type = readsym;
								break;
							case 13:
								list[index].type = oddsym;
								break;
						}

					}
					
					// increment index for the next lexeme
					index++;
				}
			}

			// check if *input_ptr is a number
			else if (isdigit(*input_ptr))
			{
				int result = 0;
				if(!number(&input_ptr, &err_type, &result)) {
					printlexerror(err_type);
					exit(EXIT_FAILURE);
				}
				else {
					list = (lexeme *) realloc(list, sizeof(lexeme) * (index + 1));
					list[index].type = numbersym;
					list[index].value = result;
					index++;
				}
			}

			// invalid input
			else
				//printlexerror(5);
			break;
		}

		//input_ptr++;
	}

	lex_index = index;
	return list;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
		case oddsym:
			printf("%11s\t%d", "odd", oddsym);
			break;
		case eqlsym:
			printf("%11s\t%d", "==", eqlsym);
			break;
		case neqsym:
			printf("%11s\t%d", "!=", neqsym);
			break;
		case lsssym:
			printf("%11s\t%d", "<", lsssym);
			break;
		case leqsym:
			printf("%11s\t%d", "<=", leqsym);
			break;
		case gtrsym:
			printf("%11s\t%d", ">", gtrsym);
			break;
		case geqsym:
			printf("%11s\t%d", ">=", geqsym);
			break;
		case modsym:
			printf("%11s\t%d", "%", modsym);
			break;
		case multsym:
			printf("%11s\t%d", "*", multsym);
			break;
		case divsym:
			printf("%11s\t%d", "/", divsym);
			break;
		case addsym:
			printf("%11s\t%d", "+", addsym);
			break;
		case subsym:
			printf("%11s\t%d", "-", subsym);
			break;
		case lparensym:
			printf("%11s\t%d", "(", lparensym);
			break;
		case rparensym:
			printf("%11s\t%d", ")", rparensym);
			break;
		case commasym:
			printf("%11s\t%d", ",", commasym);
			break;
		case periodsym:
			printf("%11s\t%d", ".", periodsym);
			break;
		case semicolonsym:
			printf("%11s\t%d", ";", semicolonsym);
			break;
		case assignsym:
			printf("%11s\t%d", ":=", assignsym);
			break;
		case beginsym:
			printf("%11s\t%d", "begin", beginsym);
			break;
		case endsym:
			printf("%11s\t%d", "end", endsym);
			break;
		case ifsym:
			printf("%11s\t%d", "if", ifsym);
			break;
		case thensym:
			printf("%11s\t%d", "then", thensym);
			break;
		case elsesym:
			printf("%11s\t%d", "else", elsesym);
			break;
		case whilesym:
			printf("%11s\t%d", "while", whilesym);
			break;
		case dosym:
			printf("%11s\t%d", "do", dosym);
			break;
		case callsym:
			printf("%11s\t%d", "call", callsym);
			break;
		case writesym:
			printf("%11s\t%d", "write", writesym);
			break;
		case readsym:
			printf("%11s\t%d", "read", readsym);
			break;
		case constsym:
			printf("%11s\t%d", "const", constsym);
			break;
		case varsym:
			printf("%11s\t%d", "var", varsym);
			break;
		case procsym:
			printf("%11s\t%d", "procedure", procsym);
			break;
		case identsym:
			printf("%11s\t%d", list[i].name, identsym);
			break;
		case numbersym:
			printf("%11d\t%d", list[i].value, numbersym);
			break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);
	return;
}
