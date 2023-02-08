/*
    pm/0 Scanner
    COP3402 Euripedes Montagne
    Romsev Charles
    Brandon Sheridan
*/



/*
    TODO:
        - lol idk ???

        -Error handling 
            1. Variable does not start with letter. 
            2. Number too long. 
            3. Name too long. 
            4. Invalid symbols. 

        -"parsing" thru the code and seperating each thing into tokens
            - we can seperate using a space or "|"
            


*/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//look in Lecture 5 Lexical Analysis

#define  norw      10         /* number of reserved words */
#define  imax   32767       /* maximum integer value */
#define  cmax      11         /* maximum number of chars for idents */
#define  strmax   256         /* maximum length of strings */


typedef enum{
    skipsym = 1,identsym, numbersym, plussym, minussym, 
    multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym, 
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym, 
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,  
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym, 
    readsym , elsesym
}token_type;

/* list of reserved word names */
char  *word [ ] = {  "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write"}; 
                         
/* internal representation  of reserved words */
// int  wsym [ ] =  { nulsym, beginsym, callsym, constsym, dosym, elsesym, endsym, ifsym, oddsym, procsym, readsym, thensym, varsym, whilesym, writesym};


/* list of special symbols */
// int ssym[256];
// ssym['+']=plus; ssym['-']=minus; ssym['*']=mult; 
// ssym['/']=slash;  ssym['(']=lparen;  ssym[')']=rparen; 
// ssym['=']=eql;      ssym[',']=comma;  ssym['.']=period; 
// ssym['#']=neq;  ssym['<']=lss;  ssym['>']=gtr; 
// ssym['$']=leq;  ssym['%']=geq;  ssym[';']=semicolon;

FILE *f;


int cpytilspace(char buffer[], char arr[], int arrPointer);
char* readProgram(int *arrSize);

int main(int argc, char const *argv[])
{

    f = fopen(argv[1], "r");

    int arrSize;    
    int indexPointer = 0;   //keeps track of index we're reading from from array

    char *charArr = readProgram(&arrSize);
    char *bufferArr = malloc(1 * sizeof(char));
    
    
    indexPointer = cpytilspace(bufferArr, charArr, indexPointer);
    

    printf("buffer holds %s\n", bufferArr);



    //clean up
    free(bufferArr);
    free(charArr);
    fclose(f);

    return 0;
}


//reads pl0 input file and puts every character into an array, keeps track of a
char* readProgram(int *arrSize){

    char *charArr = (char *) malloc(1 * sizeof(char));
    // int arrSize;
    for (int i = 0; fscanf(f, "%c", &charArr[i]) > 0; ){    //incrementor in statement


        // printf("at index %d adding + 1\n",i);

        i++;    //added in statement so it didnt mess up realloc 'math' for some reason
        charArr = realloc(charArr, sizeof(char) * (i + 1));
        if(charArr == NULL){
            free(charArr);
            printf("Ran out of memory dude. Gotta exit program\n");
            exit(-1); //exit program with error
        }
        
        // printf("%c\n", charArr[i - 1]);
        
        *arrSize = i;

    }

    return charArr;
}


/*
    copys array to buffer until it reaches a "space" or bar "|"
    returns arrPointer
*/
int cpytilspace(char buffer[], char arr[], int arrPointer){

    // int lenFrom = strlen(arr);
    // int lenTo = strlen(buffer);
    
    int bufferSize = 0;  //keeps track of size buffer needs to be so we can allocate enough space for it
    int index = arrPointer; //start where we left off last time on the 

    while(arr[index] != '\0' && arr[index] != '|' && arr[index] != ' '){
        bufferSize++;
        index++;
    }


    buffer = realloc(buffer, sizeof(char) * bufferSize);
    for(int k = 0; k < bufferSize; k++){
        buffer[k] = arr[arrPointer + k];
    }

    return arrPointer;


}
