/*
    pm/0
    COP3402 Euripedes Montagne
    Romsev Charles
    Brandon Sheridan
*/


/*
    TODO:
        Error Handling
        Print Instructions
        
    
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



int base(int BP, int L);


int main(int argc, char *argv[]){


    //open file
    in = fopen(argv[1], "r");
    printf("File opened\n");

    
    IR IR; //instruction register
    

    int temp_pas_size;
    for (int i = 0; i < ARRAY_SIZE; i++){

        //breaks loop if theres nothing else in input file to read
        if (fscanf(in, "%d", &pas[i]) < 1){
            
            temp_pas_size = i; //keeps track of how 
            break;
        }

        printf("ran %d times\n", i+1);


    }


    //print PAS
    for (int i = 0; i < temp_pas_size; i++){
        printf("[%d]", pas[i]);
    }
    printf("\n");



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

        pc = pc + 3; //move program counter up to next instruction

        char charInput;

        switch (IR.op)
        {
            case LIT:       //pushes value of m onto stack
                sp--;
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

                    //are we sure we're supposed to put this in as character thats what it says on instructions?
                    case 1:
                        printf("%c", pas[sp]);
                        sp++;
                        break;
                    case 2:
                        
                        --sp;
                        scanf(" %c", &pas[sp]);

                        printf("\n%d\n", pas[sp]);
                        break;
                    case 3:
                        eop = 0;    //end program
                        printf("\nending program\n");
                        break;
                }
                break;
        }

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


/*
void print_stack(int PC, int BP, int SP, int GP, int *pas, int *bars) {
	int i;
	printf("%d\t%d\t%d\t", PC, BP, SP);
	for (i = GP; i <= SP; i++)
	{
		if (bars[i] == 1)
		printf("| %d ", pas[i]);
		else
		printf("%d ", pas[i]);
	}
	printf("\n");
}

void print_instruction(int PC, int* IR){
	char opname[4];
	switch (IR[0])
	{
		case 1 : 
			strcpy(opname, "LIT"); 
			break;
		case 2 :
			switch (IR[2])
			{
				case 0 : strcpy(opname, "RTN"); break;
				case 1 : strcpy(opname, "ADD"); break;
				case 2 : strcpy(opname, "SUB"); break;
				case 3 : strcpy(opname, "MUL"); break;
				case 4 : strcpy(opname, "DIV"); break;
				case 5 : strcpy(opname, "EQL"); break;
				case 6 : strcpy(opname, "NEQ"); break;
				case 7 : strcpy(opname, "LSS"); break;
				case 8 : strcpy(opname, "LEQ"); break;
				case 9 : strcpy(opname, "GTR"); break;
				case 10 : strcpy(opname, "GEQ"); break;
				default : strcpy(opname, "err"); break;
			}
		break;
		case 3 : 
			strcpy(opname, "LOD"); 
			break;
		case 4 : 
			strcpy(opname, "STO"); 
			break;
		case 5 : 
			strcpy(opname, "CAL"); 
				break;
		case 6 : 
			strcpy(opname, "INC"); 
			break;
		case 7 : 
			strcpy(opname, "JMP"); 
			break;
		case 8 : 
			strcpy(opname, "JPC"); 
			break;
		case 9 :
			switch (IR[2])
			{
				case 1 :
					strcpy(opname, "WRT");
					break;
				case 2 :
					strcpy(opname, "RED"); 
					break;
				case 3 : 
					strcpy(opname, "HLT"); 
					break;
				default : 
					strcpy(opname, "err"); 
					break;
			}
		break;
	default : 
		strcpy(opname, "err"); 
		break;
	}

	printf("%d\t%s\t%d\t%d\t", (PC - 3)/3, opname, IR[1], IR[2]);
}
*/