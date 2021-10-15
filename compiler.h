/* 
	This is the header file for the UCF Fall 2021 Systems Software Project.
	If you choose to alter this, you MUST make a note of that in your
	readme file, otherwise you will lose 5 points.
*/

/* reserved words */
#define CONST "const"
#define WHILE "while"
#define VAR "var"
#define BEGIN "begin"
#define END "end"
#define DO "do"
#define IF "if"
#define THEN "then"
#define ELSE "else"
#define CALL "call"
#define WRITE "write"
#define READ "read"
#define NUM "numbers"
#define ODD "odd"

typedef enum token_type {
	constsym = 1, varsym, procsym, beginsym, endsym, whilesym, dosym, 
	ifsym, thensym, elsesym, callsym, writesym, readsym, identsym, 
	numbersym, assignsym, addsym, subsym, multsym, divsym, modsym, 
	eqlsym, neqsym, lsssym, leqsym, gtrsym, geqsym, oddsym, 
	lparensym, rparensym, commasym, periodsym, semicolonsym
} token_type;

typedef struct lexeme {
	char name[12];
	int value;
	token_type type;
} lexeme;

typedef struct instruction {
	int opcode;
	int l;
	int m;
} instruction;

lexeme *lexanalyzer(char *input);
instruction *parse(lexeme *list);
void execute_program(instruction *code);
