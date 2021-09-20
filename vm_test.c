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



/** ENTRY POINT **/
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


    /** NEED TO PRINT OUT HEADER **/

    while(!halt) {

        // read instruction into IR
        instr.OP = pas[PC];

        instr.L = pas[PC + 1];

        instr.M = pas[PC + 2];

        int line = PC / 3;

        PC += 3;

        char opname[4];

        switch(instr.OP) {

            case LIT: {

                if(BP == GP) {

                    DP++;
                    pas[DP] = instr.M;

                } else {

                    SP--;
                    pas[SP] = instr.M;

                }

                break;
            }
            case OPR: {
                switch (instr.M) {
                    case RTN : {
                        SP = BP + 1;
                        BP = pas[SP - 2];
                        PC = pas[SP - 3];
                        break;
                    }
                    case NEG : {
                        if (BP == GP) {pas[DP] *= -1;}
                        else {pas[SP] *= -1;}
                        break;
                    }
                    case ADD : {
                        if (BP == GP) {
                            DP--;
                            pas[DP] += pas[DP + 1];
                        } else {
                            SP++;
                            pas[SP] += pas[SP - 1];
                        }
                        break;
                    }
                    case SUB : {
                        if (BP == GP) {
                            DP--;
                            pas[DP] -= pas[DP + 1];
                        } else {
                            SP++;
                            pas[SP] -= pas[SP - 1];
                        }
                        break;
                    }
                    case MUL : {
                        if (BP == GP) {
                            DP--;
                            pas[DP] *= pas[DP + 1];
                        } else {
                            SP++;
                            pas[SP] *= pas[SP - 1];
                        }
                        break;
                    }
                    case DIV : {
                        if (BP == GP) {
                            DP--;
                            pas[DP] /= pas[DP + 1];
                        } else {
                            SP++;
                            pas[SP] /= pas[SP - 1];
                        }
                        break;
                    }
                    case ODD : {
                        if (BP == GP) {pas[DP] %= 2;}
                        else {pas[SP] %= 2;}
                        break;
                    }
                    case MOD : {
                            if (BP == GP) {
                                DP--;
                                pas[DP] %= pas[DP + 1];
                            } else {
                                SP++;
                                pas[SP] %= pas[SP - 1];
                            }
                            break;
                    }
                    case EQL : {
                        if (BP == GP) {
                            DP--;
                            pas[DP] = (pas[DP] == pas[DP + 1]) ? 0 : 1;

                        } else {
                            SP++;
                            pas[SP] = (pas[SP] == pas[SP - 1]) ? 0 : 1;
                        }
                        break;
                    }
                    case NEQ : {
                        if (BP == GP) {
                            DP--;
                            pas[DP] = (pas[DP] != pas[DP + 1]) ? 0 : 1;

                        } else {
                            SP++;
                            pas[SP] = (pas[SP] != pas[SP - 1]) ? 0 : 1;
                        }
                        break;
                    }
                    case LSS : {
                        if (BP == GP) {
                            DP--;
                            pas[DP] = (pas[DP] < pas[DP + 1]) ? 0 : 1;

                        } else {
                            SP++;
                            pas[SP] = (pas[SP] < pas[SP - 1]) ? 0 : 1;
                        }
                        break;
                    }
                    case LEQ : {
                        if (BP == GP) {
                            DP--;
                            pas[DP] = (pas[DP] <= pas[DP + 1]) ? 0 : 1;

                        } else {
                            SP++;
                            pas[SP] = (pas[SP] <= pas[SP - 1]) ? 0 : 1;
                        }
                        break;
                    }
                    case GTR : {
                        if (BP == GP) {
                            DP--;
                            pas[DP] = (pas[DP] > pas[DP + 1]) ? 0 : 1;

                        } else {
                            SP++;
                            pas[SP] = (pas[SP] > pas[SP - 1]) ? 0 : 1;
                        }
                        break;
                    }
                    case GEQ : {
                        if (BP == GP) {
                            DP--;
                            pas[DP] = (pas[DP] >= pas[DP + 1]) ? 0 : 1;

                        } else {
                            SP++;
                            pas[SP] = (pas[SP] >= pas[SP - 1]) ? 0 : 1;
                        }
                        break;
                    }
                    default :
                    printf("Invalid Instruction: %d %d %d", instr.OP, instr.L, instr.M);
                    break;
                }
                break;
            }
            case LOD: {

                if(BP == GP) {

                    DP++;
                    pas[DP] = pas[GP + M];

                } else {

                    if(base(instr.L)) {

                        SP--;
                        pas[SP] = pas[GP + instr.M];

                    } else {

                        SP--;
                        pas[SP] = pas[base(instr.L) - M];

                    }

                }

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
                
                PC = instr.M;
                strcpy(opname, "JMP");
                break;
            }
            case JPC: {
                break;
            }
            case SYS: {
                if (instr.L != 0) {
                    printf("Invalid Call, L needs to be 0.");
                    continue;
                }
                switch(instr.M) {
                    case 1 :
                        if (BP == GP) {
                            printf("%d", pas[DP]);
                            DP--;
                        } else {
                            printf("%d", pas[SP]);
                            SP++;
                        }
                        break;
                    case 2 :
                        if (BP == GP) {
                            DP++;
                            scanf("%d", &pas[DP]);
                        } else {
                            SP--;
                            scanf("%d", &pas[SP]);
                        }
                        break;
                    case 3 :
                        printf("End of Program");
                        halt = 1;
                }
                break;
            }
        }  

        print_execution(line, opname, &instr, PC, SP, DP, pas, GP);  
    }

    printf("\n");
    
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
