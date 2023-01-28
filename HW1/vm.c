/*
    pm/0
    COP3402 Euripedes Montagne
    Romsev Charles
    Brandon Sheridan
*/


/*
    TODO:
    1) Scan everything to stack first
        Stack size is [look up in instructions]
    2) initialize all counters
        OP L M, all init to 0;
        BP = 499, SP = 500, PC = 0;
    3) Start Fetch cycle
        Scan from stack at program counter, into IR and increment by 3
    
    things to implement:
    Error handling
    base function (there was a question about it being wrong, it uses static link instead of dynamic link 
        **I AM USURE IF THIS IS TRUE after looking at the CAL instructions,- it was a question in the lab**)


*/
//only 2 functions MAIN and BASE



#include <stdlib.h>
#include <stdio.h>


/* GLOBAL VARIABLES */

#define ARRAY_SIZE 500

FILE *in; //input file variable

int pas[ARRAY_SIZE]; //process address space array

/*-----------------*/





typedef enum opcodes {
	LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC,
    SOU = 9, SIN = 9, EOP = 9 //these three codes depend on the M passed in
} opcodes;

typedef enum OPRcodes{
    RTN = 0, ADD, SUB, MUL, DIV, EQL, NEQ, LSS, LEQ, GTR, GEQ
}OPRcodes;

//instruction register struct
typedef struct IR{
	int op;	
	int l;
	int m;
}IR;



int main(int argc, char *argv[]){


    //open file
    in = fopen(argv[1], "r");
    

    
    IR IR; //instruction register

    for (int i = 0; i < ARRAY_SIZE; i++){

        //breaks loop if theres nothing else in input file to read
        if (fscanf(in, "%d", &pas[i]) < 1){
            break;
        }
        printf("ran %d time\n", (i + 1));
    }


    //initialize SP BP and PC


    // okay so now that we finished reading the entire instruction file into the PAS
    // how do we keep track of where the instruction register starts?
    // do we use the program counter for that?

    //no u just use the IR struct for this


    int sp = 500; //stack pointer
    int bp = sp - 1; //base pointer
    int pc = 0; //program counter

    int eop = 1; //end of program
    
    while (eop){ //keep running until end of program

        IR.op = pas[pc];
        IR.l = pas[pc + 1];
        IR.m = pas[pc + 2];

        switch (IR.op)
        {
        case LIT:       //pushes value of m onto stack
            --sp;
            pas[sp] = IR.m;
            break;
        case OPR:
            switch (IR.m) {
                case RTN:
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
                    pas[sp + 1] = pas[sp + 1] == pas[sp];
                    ++sp;
                    break;
                case NEQ:
                    pas[sp + 1] = pas[sp + 1] != pas[sp];
                    ++sp;
                    break;
                case LSS:
                    pas[sp + 1] = pas[sp + 1] < pas[sp];
                    ++sp;
                    break;
                case LEQ:
                    pas[sp + 1] = pas[sp + 1] <= pas[sp];
                    ++sp;
                    break;
                case GTR:
                    pas[sp + 1] = pas[sp + 1] > pas[sp];
                    ++sp;
                    break;
                case GEQ:
                    pas[sp + 1] = pas[sp + 1] >= pas[sp];
                    ++sp;
                    break;
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
            break;
        case INC:
            sp = sp - IR.m;
            break;
        case JMP:
            pc = IR.m;
            break;
        case JPC:
            if (pas[sp] == 0) pc = IR.m;
            ++sp;
            break;
        case 9: //used for SOU SIN OR EOP
            switch (IR.m) {
                case 1:
                    printf("%d", pas[sp]);
                    ++sp;
                    break;
                case 2:
                    --sp;
                    scanf("%d\n", &pas[sp]);
                    break;
                case 3:
                    eop = 0;
                    break;
            }
            break;
        }

    }





    //cleanup
    fclose(in);
}
