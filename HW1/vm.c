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
    ADD = 1, SUB, MUL, DIV, EQL, NEQ, LSS, LEQ, GTR, GEQ
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
        case LIT:
            /* code */
            break;
        
        case OPR:
            /* code */
            break;
        case LOD:
            /* code */
            break;
        case STO:
            /* code */
            break;
        case CAL:
            /* code */
            break;
        case INC:
            /* code */
            break;
        case JMP:
            /* code */
            break;

        case JPC:
            /* code */
            break;

        case 9: //used for SOU SIN OR EOP

            break;
        
        }

    }





    //cleanup
    fclose(in);
}
