#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// size of process address space
#define MAX_PAS_LENGTH 500

// assumed max byte size of an instruction line
#define MAX_INSTR_SIZE 8

// instruction register
struct IR {
    int OP;  
    int L;   
    int M;      
};
typedef struct IR IR;

int *SP;        // stack pointer (points to top of stack)
int *BP;        // base pointer (points to beginning of activation record?)
int *PC;        // program counter (points to NEXT instruction in text mem)
int *DP;        // data pointer (IC - 1)
int *GP;        // global pointer (points to data section of memory - I think data is accessed at GP + IC)
int *FREE;      // heap pointer (IC + 40)
int IC = 0;     // instruction counter (incr by 3 for every instruction in text mem)

// function to read the instructions into text part of memory
void read_instructions_into_text(FILE *ifp, int *pas);

int main(int argc, char **argv) {

    if(argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(-1);
    }

    // set up the Process Address Space (PAS)
    int pas[MAX_PAS_LENGTH];

    // zero out process memory
    memset(pas, 0, sizeof(pas));

    // read instruction file (assuming 1 file )
    FILE *ifp = fopen(argv[1], "r");
    if(!ifp) {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        exit(-1);
    }

    read_instructions_into_text(ifp, pas);

    // set up the pointers

    PC = pas;                   // program counter pounts to beginning of text part of memory
    GP = pas + IC;              // global pointer points to data part of memory (doc says GP = IC, so I assume pas + IC)
    DP = pas + (IC - 1);        // data pointer (need to clarify about this one)
    FREE = pas + (IC + 40);     // FREE points to heap
    BP = pas + IC;              // base pointer points to base of data or activation records
    SP = pas + MAX_PAS_LENGTH;  // stack pointer points to top of the stack
    

    fclose(ifp);
    return 0;
}

void read_instructions_into_text(FILE *ifp, int *pas) {

    // buffer that stores a line of instruction
    char buffer[MAX_INSTR_SIZE];

    int OP = 0, L = 0, M = 0, i = 0;


    while(fgets(buffer, MAX_INSTR_SIZE, ifp)) {

        sscanf(buffer, "%d %d %d", &OP, &L, &M);

        // pas + 0 = OP
        *(pas + i) = OP;
        i++;

        // pas + 1 = L
        *(pas + i) = L;
        i++;

        // pas + 2 = M
        *(pas + i) = M;

        // increment instruction counter by 3
        IC += 3;

        // increment i for the next round of instructions
        i++;

    }
}
