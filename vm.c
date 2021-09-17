#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// for debugging
#define LOG(MSG) {printf("%s\n", MSG);}

// size of process address space
#define MAX_PAS_LENGTH 500

// process address space
int pas[MAX_PAS_LENGTH];

// assumed max byte size of an instruction line
#define MAX_INSTR_SIZE 8

// instruction register
struct IR {
    int OP;  
    int L;   
    int M;      
};
typedef struct IR IR;

int SP;         // stack pointer (points to top of stack)
int BP;         // base pointer (points to beginning of activation record?)
int PC;         // program counter (points to NEXT instruction in text mem)
int DP;         // data pointer (IC - 1)
int GP;         // global pointer (points to data section of memory - I think data is accessed at GP + IC)
int FREE;       // heap pointer (IC + 40)
int IC = 0;     // instruction counter (incr by 3 for every instruction in text mem) 

enum ISA {
    LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SYS
};

enum Operations {
     RTN, NEG, ADD, SUB, MUL, DIV, ODD, MOD, EQL, NEQ, LSS, LEQ, GTR, GEQ
};

// print function
void print_execution(int line, char *opname, IR *instr, int PC, int SP, int DP, int *pas, int GP);

// find base L levels down
int base(int L);

int main(int argc, char **argv) {

    if(argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(-1);
    }

    // zero out process memory
    memset(pas, 0, sizeof(pas));

    // read instruction file (assuming 1 file )
    FILE *ifp = fopen(argv[1], "r");
    if(!ifp) {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        exit(-1);
    }

   // read instructions into text

    char buffer[MAX_INSTR_SIZE];

    int OP = 0, L = 0, M = 0, i = 0;

    while(fgets(buffer, MAX_INSTR_SIZE, ifp)) {

        sscanf(buffer, "%d %d %d", &OP, &L, &M);

        pas[i]         = OP;
        pas[i + 1]     = L;
        pas[i + 2]     = M;

        IC += 3;
        i += 3;

    }

    fclose(ifp);
    
    // set up the pointers

    PC = 0;                         // program counter points to beginning of text
    GP = IC;                        // global pointer points to data part of memory (doc says GP = IC, so I assume pas + IC)
    DP = IC - 1;                    // data pointer (need to clarify about this one)
    FREE = IC + 40;                 // FREE points to heap
    BP = IC;                        // base pointer points to base of data or activation records
    SP = MAX_PAS_LENGTH;            // stack pointer points to top of the stack

    int halt = 0;                   // halt flag
    IR instr;                       // instruction register


    while(!halt) {

        // read instruction into IR
        instr.OP = pas[PC];

        instr.L = pas[PC + 1];

        instr.M = pas[PC + 2];

        PC += 3;

        switch(instr.OP) {

            case LIT: {
                break;
            }
            case OPR: {
                break;
            }
            case LOD: {
                break;
            }
            case STO: {
                break;
            }
            case CAL: {
                break;
            }
            case INC: {
                break;
            }
            case JMP: {
                break;
            }
            case JPC: {
                break;
            }
            case SYS: {
                break;
            }
        }    
    }
    
    return 0;
}

void print_execution(int line, char *opname, IR *instr, int PC, int SP, int DP, int *pas, int GP) {

    int i;

    printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, instr->L, instr->M, PC, BP, SP, DP);

    for(i = GP; i <= DP; i++) {

        // print data section
        printf("%d ", pas[i]);
        printf("\n");

        // print stack
        printf("\tstack :");
        for(i = MAX_PAS_LENGTH - 1; i >= SP; i--) 
            printf("%d ", pas[i]);
        printf("\n");

    }
}

int base(int L) {

    int arb = BP;   // arb = activation record base

    while(L > 0) {  // find base L levels down

        arb = pas[arb];
        L--;

    }

    return arb;
}
