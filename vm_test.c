#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// for debugging
#define LOG(MSG)             \
    {                        \
        printf("%s\n", MSG); \
    }

// size of process address space
#define MAX_PAS_LENGTH 500

// process address space
int pas[MAX_PAS_LENGTH];

// assumed max byte size of an instruction line
#define MAX_INSTR_SIZE 8

// instruction register
struct IR
{
    int OP;
    int L;
    int M;
};
typedef struct IR IR;

int SP;     // stack pointer (points to top of stack)
int BP;     // base pointer (points to beginning of activation record?)
int PC;     // program counter (points to NEXT instruction in text mem)
int DP;     // data pointer (IC - 1)
int GP;     // global pointer (points to data section of memory - I think data is accessed at GP + IC)
int FREE;   // heap pointer (IC + 40)
int IC = 0; // instruction counter (incr by 3 for every instruction in text mem)

enum ISA
{
    LIT = 1,
    OPR,
    LOD,
    STO,
    CAL,
    INC,
    JMP,
    JPC,
    SYS
};

enum Operations
{
    RTN,
    NEG,
    ADD,
    SUB,
    MUL,
    DIV,
    ODD,
    MOD,
    EQL,
    NEQ,
    LSS,
    LEQ,
    GTR,
    GEQ
};

// print function
void print_execution(int line, char *opname, IR *instr, int PC, int SP, int DP, int *pas, int GP);

// find base L levels down
int base(int L);

/** ENTRY POINT **/
int main(int argc, char **argv)
{

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(-1);
    }

    // zero out process memory
    memset(pas, 0, sizeof(pas));

    // read instruction file (assuming 1 file )
    FILE *ifp = fopen(argv[1], "r");
    if (!ifp)
    {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        exit(-1);
    }

    // read instructions into text

    char buffer[MAX_INSTR_SIZE];

    int OP = 0, L = 0, M = 0, i = 0;

    while (fgets(buffer, MAX_INSTR_SIZE, ifp))
    {

        sscanf(buffer, "%d %d %d", &OP, &L, &M);

        pas[i] = OP;
        pas[i + 1] = L;
        pas[i + 2] = M;

        IC += 3;
        i += 3;
    }

    fclose(ifp);

    // set up the pointers

    PC = 0;              // program counter points to beginning of text
    GP = IC;             // global pointer points to data part of memory (doc says GP = IC, so I assume pas + IC)
    DP = IC - 1;         // data pointer (need to clarify about this one)
    FREE = IC + 40;      // FREE points to heap
    BP = IC;             // base pointer points to base of data or activation records
    SP = MAX_PAS_LENGTH; // stack pointer points to top of the stack

    int halt = 0; // halt flag
    IR instr;     // instruction register

    /** TODO - NEED TO PRINT OUT HEADER **/

    while (!halt)
    {

        // read instruction into IR
        instr.OP = pas[PC];

        instr.L = pas[PC + 1];

        instr.M = pas[PC + 2];

        int line = PC / 3;

        PC += 3;

        char opname[4];

        switch (instr.OP)
        {

        case LIT:
        {
            strcpy(opname, "LIT");

            if (BP == GP)
            {

                DP++;
                pas[DP] = instr.M;
            }
            else
            {

                SP--;
                pas[SP] = instr.M;
            }

            break;
        }
        case OPR:
        {
            switch (instr.M)
            {
            case RTN:
            {
                strcpy(opname, "RTN");
                SP = BP + 1;
                BP = pas[SP - 2];
                PC = pas[SP - 3];
                break;
            }
            case NEG:
            {
                strcpy(opname, "NEG");
                if (BP == GP)
                {
                    pas[DP] *= -1;
                }
                else
                {
                    pas[SP] *= -1;
                }
                break;
            }
            case ADD:
            {
                strcpy(opname, "ADD");
                if (BP == GP)
                {
                    DP--;
                    pas[DP] += pas[DP + 1];
                }
                else
                {
                    SP++;
                    pas[SP] += pas[SP - 1];
                }
                break;
            }
            case SUB:
            {
                strcpy(opname, "SUB");
                if (BP == GP)
                {
                    DP--;
                    pas[DP] -= pas[DP + 1];
                }
                else
                {
                    SP++;
                    pas[SP] -= pas[SP - 1];
                }
                break;
            }
            case MUL:
            {
                strcpy(opname, "MUL");
                if (BP == GP)
                {
                    DP--;
                    pas[DP] *= pas[DP + 1];
                }
                else
                {
                    SP++;
                    pas[SP] *= pas[SP - 1];
                }
                break;
            }
            case DIV:
            {
                strcpy(opname, "DIV");
                if (BP == GP)
                {
                    DP--;
                    pas[DP] /= pas[DP + 1];
                }
                else
                {
                    SP++;
                    pas[SP] /= pas[SP - 1];
                }
                break;
            }
            case ODD:
            {
                strcpy(opname, "ODD");
                if (BP == GP)
                {
                    pas[DP] %= 2;
                }
                else
                {
                    pas[SP] %= 2;
                }
                break;
            }
            case MOD:
            {
                strcpy(opname, "MOD");
                if (BP == GP)
                {
                    DP--;
                    pas[DP] %= pas[DP + 1];
                }
                else
                {
                    SP++;
                    pas[SP] %= pas[SP - 1];
                }
                break;
            }
            case EQL:
            {
                strcpy(opname, "EQL");
                if (BP == GP)
                {
                    DP--;
                    pas[DP] = (pas[DP] == pas[DP + 1]) ? 0 : 1;
                }
                else
                {
                    SP++;
                    pas[SP] = (pas[SP] == pas[SP - 1]) ? 0 : 1;
                }
                break;
            }
            case NEQ:
            {
                strcpy(opname, "NEQ");
                if (BP == GP)
                {
                    DP--;
                    pas[DP] = (pas[DP] != pas[DP + 1]) ? 0 : 1;
                }
                else
                {
                    SP++;
                    pas[SP] = (pas[SP] != pas[SP - 1]) ? 0 : 1;
                }
                break;
            }
            case LSS:
            {
                strcpy(opname, "LSS");
                if (BP == GP)
                {
                    DP--;
                    pas[DP] = (pas[DP] < pas[DP + 1]) ? 0 : 1;
                }
                else
                {
                    SP++;
                    pas[SP] = (pas[SP] < pas[SP - 1]) ? 0 : 1;
                }
                break;
            }
            case LEQ:
            {
                strcpy(opname, "LEQ");
                if (BP == GP)
                {
                    DP--;
                    pas[DP] = (pas[DP] <= pas[DP + 1]) ? 0 : 1;
                }
                else
                {
                    SP++;
                    pas[SP] = (pas[SP] <= pas[SP - 1]) ? 0 : 1;
                }
                break;
            }
            case GTR:
            {
                strcpy(opname, "GTR");
                if (BP == GP)
                {
                    DP--;
                    pas[DP] = (pas[DP] > pas[DP + 1]) ? 0 : 1;
                }
                else
                {
                    SP++;
                    pas[SP] = (pas[SP] > pas[SP - 1]) ? 0 : 1;
                }
                break;
            }
            case GEQ:
            {
                strcpy(opname, "GEQ");
                if (BP == GP)
                {
                    DP--;
                    pas[DP] = (pas[DP] >= pas[DP + 1]) ? 0 : 1;
                }
                else
                {
                    SP++;
                    pas[SP] = (pas[SP] >= pas[SP - 1]) ? 0 : 1;
                }
                break;
            }
            default:
                printf("Invalid Instruction: %d %d %d", instr.OP, instr.L, instr.M);
                break;
            }
            break;
        }
        case LOD:
        {
            strcpy(opname, "LOD");
            if (BP == GP)
            {

                DP++;
                pas[DP] = pas[GP + M];
            }
            else
            {

                if (base(instr.L))
                {

                    SP--;
                    pas[SP] = pas[GP + instr.M];
                }
                else
                {

                    SP--;
                    pas[SP] = pas[base(instr.L) - M];
                }
            }

            break;
        }
        case STO:
        {
            strcpy(opname, "STO");
            if (BP == GP)
            {

                pas[GP + instr.M] = pas[SP];
                SP++;
            }
            else
            {

                if (base(instr.L) == GP)
                {

                    pas[GP + instr.M] = pas[SP];
                    SP++;
                }
                else
                {

                    pas[base(instr.L) - M] = pas[SP];
                    SP++;
                }
            }

            break;
        }
        case CAL:
        {
            strcpy(opname, "CAL");
            pas[SP - 1] = base(instr.L); // static link
            pas[SP - 2] = BP;            // dynamic link
            pas[SP - 3] = PC;            // return address
            BP = SP - 1;
            PC = instr.M;

            break;
        }
        case INC:
        {
            strcpy(opname, "INC");
            if (BP == GP)
                DP += instr.M;
            else
                SP -= instr.M;

            break;
        }
        case JMP:
        {

            PC = instr.M;
            strcpy(opname, "JMP");
            break;
        }
        case JPC:
        {
            strcpy(opname, "JPC");
            if (BP == GP)
            {

                if (pas[DP] == 0)
                    PC = instr.M;
                DP--;
            }
            else
            {

                if (pas[SP] == 0)
                    PC = instr.M;
                SP++;
            }

            break;
        }
        case SYS:
        {
            strcpy(opname, "SYS");
            if (instr.L != 0)
            {
                printf("Invalid Call, L needs to be 0.");
                continue;
            }
            switch (instr.M)
            {
            case 1:
                printf("Top of Stack Value: ");
                if (BP == GP)
                {
                    printf("%d", pas[DP]);
                    DP--;
                }
                else
                {
                    printf("%d", pas[SP]);
                    SP++;
                }
                printf("\n");
                break;
            case 2:
                printf("Please Enter an Integer: ");
                if (BP == GP)
                {
                    DP++;
                    scanf("%d", &pas[DP]);
                }
                else
                {
                    SP--;
                    scanf("%d", &pas[SP]);
                }
                break;
            case 3:
                printf("End of Program\n");
                halt = 1;
            }
            break;
        }
        }

        print_execution(line, opname, &instr, PC, SP, DP, pas, GP);
    }

    return 0;
}

void print_execution(int line, char *opname, IR *instr, int PC, int SP, int DP, int *pas, int GP)
{

    int i;

    printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, instr->L, instr->M, PC, BP, SP, DP);

    for (i = GP; i <= DP; i++)
    {
        printf("%d ", pas[i]);
    }
    printf("\n");

    // print stack
    printf("\tstack : ");
    for (i = MAX_PAS_LENGTH - 1; i >= SP; i--)
    {
        printf("%d ", pas[i]);
    }

    printf("\n");
}

int base(int L)
{

    int arb = BP; // arb = activation record base

    while (L > 0)
    { // find base L levels down

        arb = pas[arb];
        L--;
    }

    return arb;
}
