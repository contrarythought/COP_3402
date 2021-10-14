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
#include "compiler.h"
#define MAX_NUMBER_TOKENS 500
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

lexeme *list;
int lex_index;

void printlexerror(int type);
void printtokens();

lexeme *lexanalyzer(char *input)
{
	int err_type = 0, index = 0;
	char *input_ptr = input;

	// loop through each character in the input
	while (*input_ptr)
	{
		// skip white space
		while (isspace(*input_ptr))
			input_ptr++;

		switch (*input_ptr)
		{

		// for single char terminals
		case ';':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = semicolonsym;
			input_ptr++;
			index++;
			break;
		case '+':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = addsym;
			input_ptr++;
			index++;
			break;
		case '-':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = subsym;
			input_ptr++;
			index++;
			break;
		case '*':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = multsym;
			input_ptr++;
			index++;
			break;
		case '%':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = modsym;
			input_ptr++;
			index++;
			break;
		case '(':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = lparensym;
			input_ptr++;
			index++;
			break;
		case ')':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = rparensym;
			input_ptr++;
			index++;
			break;
		case ',':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = commasym;
			input_ptr++;
			index++;
			break;
		case '.':
			list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
			list[index].type = periodsym;
			input_ptr++;
			index++;
			break;
		case '!':
			if (*(input_ptr + 1) == '=')
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = neqsym;
				input_ptr += 2; // advance pass the '='
				index++;
			}
			else
				printlexerror(1); // invalid symbol
			break;
		case ':':
			if (*(input_ptr + 1) == '=')
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = assignsym;
				input_ptr += 2;
				index++;
			}
			else
				printlexerror(1);
			break;
		case '=':
			if (*(input_ptr + 1) == '=')
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = eqlsym;
				input_ptr += 2;
				index++;
			}
			else
				printlexerror(1);
			break;
		case '<':
			if (*(input_ptr + 1) == '=')
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = leqsym;
				input_ptr += 2;
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
				index++;
			}
			else
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = gtrsym;
				input_ptr++;
				index++;
			}
			break;
		case '/':
			if (*(input_ptr + 1) == '/')
			{
				// for comments, skip everything until a newline is found
				while ((*input_ptr != '\n') && *input_ptr)
					input_ptr++;
			}
			else
			{
				list = (lexeme *)realloc(list, sizeof(lexeme) * (index + 1));
				list[index].type = divsym;
				input_ptr++;
				index++;
			}
			break;
		default:
			// if *input_ptr is a letter, check for identifier or reserved vocab (const, while, for, etc)
			if (isalpha(*input_ptr))
			{
				if (!ident_reserved(&input_ptr, &err_type))
					printlexerror(err_type);
			}

			// check if *input_ptr is a number
			else if (isdigit(*input_ptr))
			{
				if (!number(&input_ptr, &err_type))
					printlexerror(err_type);
			}

			// invalid input
			else
				printlexerror(5);
			break;
		}
	}

	return NULL;
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
