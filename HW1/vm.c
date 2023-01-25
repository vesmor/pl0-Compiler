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

#include <stdlib.h>
#include <stdio.h>

//only 2 functions MAIN and BASE


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


//file IO
FILE *in;

int main(int argc, char *argv[]){

    IR IR;

    in = fopen(argv[1], "r");

    //scan the file 3 times
    for (int i = 0; i < 3; i++){
        fscanf(in, "%d %d %d", &IR.op, &IR.l, &IR.m);
    }

    printf("%d %d %d\n", IR.op, IR.l, IR.m);


    //cleanup
    fclose(in);
}
