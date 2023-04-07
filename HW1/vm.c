/*
    pm/0
    COP3402 Euripedes Montagne
    Romsev Charles
    Brandon Sheridan
*/






#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* GLOBAL VARIABLES */

#define ARRAY_SIZE 500

FILE *in; //input file variable

int pas[ARRAY_SIZE]; //process address space array
int pipe[ARRAY_SIZE]; //and indicator of where | needs to be printed in the output

/*-----------------*/





typedef enum opcodes {
    LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SYS,
    SOU = 9, SIN = 9, EOP = 9   //these three codes depend on the M passed in
} opcodes;

typedef enum OPRcodes{
    RTN = 0, ADD, SUB, MUL, DIV, EQL, NEQ, LSS, LEQ, GTR, GEQ, ODD
}OPRcodes;

//instruction register struct
typedef struct IR{
    int op; 
    int l;
    int m;
}IR;



//prototypes
int base(int BP, int L);
void print_stack(int PC, int BP, int SP);
void print_instruction(int PC, IR IR);


int main(int argc, char *argv[]){


    //open file
    in = fopen(argv[1], "r");


    
    IR IR; //instruction register
    

    int temp_pas_size;  //used to initialize second array laater
    for (int i = 0; i < ARRAY_SIZE; i++){
        //breaks loop if theres nothing else in input file to read
        if (fscanf(in, "%d", &pas[i]) < 1){
            
            temp_pas_size = i;
            break;
        }


    }




    for (int i = 0; i < temp_pas_size; i++){    //initialize array values
        pipe[i] = 0;
    }


    int bp = 499; //base pointer
    int sp = bp + 1; //stack pointer

    int pc = 0; //program counter

    printf("\t\t\tPC\tBP\tSP\tstack\nInitial values:\t%d\t%d\t%d\n", pc, bp, sp);

    int eop = 1; //end of program
    
    while (eop){ //keep running until end of program
        

        IR.op = pas[pc];
        IR.l = pas[pc + 1];
        IR.m = pas[pc + 2];
        pc += 3; //move program counter up to next instruction

        char charInput;
        int retval;
        switch (IR.op)
        {
            case LIT:       //pushes value of m onto stack
                sp--;
                pas[sp] = IR.m;
                break;

            case OPR:
                switch (IR.m) {
                    case RTN:
                        pipe[bp] = 0;
                        sp = bp + 1;
                        bp = pas[sp - 2];
                        pc = pas[sp - 3];
                        break;
                    case ADD:
                        pas[sp + 1] = pas[sp + 1] + pas[sp];
                        ++sp;
                        break;
                    case SUB:
                        pas[sp + 1] = pas[sp + 1] - pas[sp];
                        ++sp;
                        break;
                    case MUL:
                        pas[sp + 1] = pas[sp + 1] * pas[sp];
                        ++sp;
                        break;
                    case DIV:
                        pas[sp + 1] = pas[sp + 1] / pas[sp];
                        ++sp;
                        break;
                    case EQL:
                        retval = (pas[sp + 1] == pas[sp]) ? 1 : 0;
                        pas[sp + 1] = retval;
                        ++sp;
                        break;
                    case NEQ:
                        retval = (pas[sp + 1] != pas[sp]) ? 1 : 0;
                        pas[sp + 1] = retval;
                        ++sp;
                        break;
                    case LSS:
                        retval = (pas[sp + 1] < pas[sp]) ? 1 : 0;
                        pas[sp + 1] = retval;
                        ++sp;
                        break;
                    case LEQ:
                        retval = (pas[sp + 1] <= pas[sp]) ? 1 : 0;
                        pas[sp + 1] = retval;
                        ++sp;
                        break;
                    case GTR:
                        retval = (pas[sp + 1] > pas[sp]) ? 1 : 0;
                        pas[sp + 1] = retval;
                        ++sp;
                        break;
                    case GEQ:
                        retval = (pas[sp + 1] >= pas[sp]) ? 1 : 0;
                        pas[sp + 1] = retval;
                        ++sp;
                        break;
                    
                    default:    //error invalid command
                        printf("%d is an Invalid m for OPR opcode\nEnding process...\n", IR.m);
                        eop = 0;    //ending the loop
                }
                break;

            case LOD:
                --sp;
                pas[sp] = pas[base(bp, IR.l) - IR.m];
                break;

            case STO:
                pas[base(bp, IR.l) - IR.m] = pas[sp];
                ++sp;
                break;

            case CAL:
                pas[sp - 1] = base(bp, IR.l);
                pas[sp - 2] = bp;
                pas[sp - 3] = pc;
                bp = sp - 1;
                pc = IR.m; 
                pipe[bp] = 1;
                break;

            case INC:
                sp -= IR.m;
                break;

            case JMP:
                pc = IR.m; 
                break;

            case JPC:
                if (pas[sp] == 0){ pc = (IR.m);}
                sp++;
                break;

            case SYS: //used for SOU SIN OR EOP
                switch (IR.m) {

                    case 1: //SOU
                        printf("Output result is: %d\n", pas[sp]);
                        sp++;
                        break;
                    case 2: //SIN
                        printf("Please Enter an Integer: ");                        
                        --sp;
                        scanf(" %d", &pas[sp]);     //have to take an int
                        break;
                    case 3: //EOP
                        eop = 0;    //end program
                        break;

                    default:
                        printf("%d is an Invalid m for SYS opcode\nEnding process...\n", IR.m);
                        eop = 0;
                }
                break;
            case ODD:
                pas[sp] = pas[sp] % 2;
                break;

            default:
                printf("%d is an invalid OP Code\nEnding process...\n", IR.op);
                eop= 0;
        }
        print_instruction(pc, IR);
        print_stack(pc, bp, sp);
    }



    //cleanup
    fclose(in);
}




//Find base L levels down
int base(int BP, int L){
    int arb = BP;   //arb = activiation record base
    while (L > 0)   //find base L levels down
    {
        arb = pas[arb];
        L--;
    }

    return arb;
}


void print_stack(int PC, int BP, int SP) {
    int i;
    printf("%d\t%d\t%d\t", PC, BP, SP);
    for (i = ARRAY_SIZE - 1; i >= SP; --i)
    {
        if (pipe[i] == 1)
        printf("| %d ", pas[i]);
        else
        printf("%d ", pas[i]);
    }
    printf("\n");
}


void print_instruction(int PC, IR IR){
    char opname[4];
    switch (IR.op)
    {
        case 1: 
            strcpy(opname, "LIT"); 
            break;
        case 2:
            switch (IR.m)
            {
                case 0: strcpy(opname, "RTN"); break;
                case 1: strcpy(opname, "ADD"); break;
                case 2: strcpy(opname, "SUB"); break;
                case 3: strcpy(opname, "MUL"); break;
                case 4: strcpy(opname, "DIV"); break;
                case 5: strcpy(opname, "EQL"); break;
                case 6: strcpy(opname, "NEQ"); break;
                case 7: strcpy(opname, "LSS"); break;
                case 8: strcpy(opname, "LEQ"); break;
                case 9: strcpy(opname, "GTR"); break;
                case 10: strcpy(opname, "GEQ"); break;
                default: strcpy(opname, "ERR"); break;
            }
        break;
        case 3: 
            strcpy(opname, "LOD"); 
            break;
        case 4: 
            strcpy(opname, "STO"); 
            break;
        case 5: 
            strcpy(opname, "CAL"); 
                break;
        case 6: 
            strcpy(opname, "INC"); 
            break;
        case 7: 
            strcpy(opname, "JMP"); 
            break;
        case 8: 
            strcpy(opname, "JPC"); 
            break;
        case 9:
            switch (IR.m)
            {
                case 1:
                    strcpy(opname, "SOU");
                    break;
                case 2:
                    strcpy(opname, "SIN"); 
                    break;
                case 3: 
                    strcpy(opname, "EOP"); 
                    break;
                default: 
                    strcpy(opname, "ERR"); 
                    break;
            }
        break;
    default: 
        strcpy(opname, "ERR"); 
        break;
    }
    printf("%s\t%d\t%d\t", opname, IR.l, IR.m);
}