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
int *DP;        // data pointer ()
int *GP;        // global pointer ()
int *FREE;      // heap pointer ()
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

    // test display
    int i;
    for(i = 0; i < MAX_PAS_LENGTH; i++) {
        if(i % 3 == 0 && i != 0) {
            printf("\n");
        }
        printf("%d ", pas[i]);
    }
    
    
    printf("\n");

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