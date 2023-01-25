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


/*-----------------*/





typedef enum opcodes {
	LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SOU,
    SIN = 9, EOP = 9 //these two codes depend on the M passed in
} opcodes;


//instruction register struct
typedef struct IR{
	int op;	
	int l;
	int m;
}IR;



int main(int argc, char *argv[]){


    //open file
    in = fopen(argv[1], "r");
    
    int pas[ARRAY_SIZE]; //process address space array

    
    IR IR;

    for (int i = 0; i < ARRAY_SIZE; i++){

        //breaks loop if theres nothing else in input file to read
        if (fscanf(in, "%d", &pas[i]) < 1){
            break;
        }
        printf("ran %d time\n", (i + 1));
    }

    // okay so now that we finished reading the entire instruction file into the PAS
    // how do we keep track of where the instruction register starts?
    // do we use the program counter for that?


    //cleanup
    fclose(in);
}
