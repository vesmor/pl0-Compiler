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
            - further seperate tokens that may be stuck together such as semicolons and a variable
            - organize tokens into different identifiers
                -Someone mentioned making a struct for everything? like for the lexemes
            - can we have 1+2 instead of 1 + 2
            


*/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//look in Lecture 5 Lexical Analysis

#define  norw      10         /* number of reserved words */
#define  imax   32767       /* maximum integer value */
#define  cmax      11         /* maximum number of chars for idents */
#define  strmax   256         /* maximum length of strings */
#define  ignoreyml  4          /* length of ignoresym array*/

typedef enum{
    skipsym = 1,identsym, numbersym, plussym, minussym, 
    multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym, 
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym, 
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,  
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym, 
    readsym , elsesym
}token_type;

/* list of reserved word names */
const char  *word [ ] = {  "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write"}; 

/* list of ignored symbols */
const char ignoresym [] = {'\n', '\0', ' ', '\t'};
                         
/* internal representation  of reserved words */
// int  wsym [ ] =  { nulsym, beginsym, callsym, constsym, dosym, elsesym, endsym, ifsym, oddsym, procsym, readsym, thensym, varsym, whilesym, writesym};


/* list of special symbols */
// int ssym[256];
// ssym['+']=plus; ssym['-']=minus; ssym['*']=mult; 
// ssym['/']=slash;  ssym['(']=lparen;  ssym[')']=rparen; 
// ssym['=']=eql;      ssym[',']=comma;  ssym['.']=period; 
// ssym['#']=neq;  ssym['<']=lss;  ssym['>']=gtr; 
// ssym['$']=leq;  ssym['%']=geq;  ssym[';']=semicolon;


typedef struct lexeme{

    char *tokenName;
    int tokenType;
    int val;

}lexeme;


FILE *f;


int cpytilspace(char buffer[], char arr[], int arrPointer);
char* readProgram(int *arrSize);
int shouldBeIgnored(char c);
void tokenize(char *chunk);


int main(int argc, char const *argv[])
{



    f = fopen(argv[1], "r");

    int arrSize;    
    int indexPointer = 0;   //keeps track of index we're reading from from array

    char *charArr = readProgram(&arrSize);  //process file into one big array
    char bufferArr[strmax];     //used to help seperate into tokens
    
    //tokenize program using a buffer array
    for (size_t i = 0; (i < arrSize) && (indexPointer < arrSize); i++){
        
        indexPointer = cpytilspace(bufferArr, charArr, indexPointer);   //returns the index where we left off
        // printf("buffer holds %s indexptr is %d\n", bufferArr, indexPointer);


        tokenize(bufferArr);

    }

     
    




    //clean up
    // free(bufferArr);
    free(charArr);
    fclose(f);

    return 0;
}


/* returns 1 if symbol should be ignored 0 otherwise */
int shouldBeIgnored(char c){
    for (size_t i = 0; i < ignoreyml; i++)
    {
        if(c == ignoresym[i])
            return 1;
    }
    
    return 0;
}


/* reads pl0 input file and puts every character into one huge array and updates the array size
    returns a char array with everything from file
*/
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
    returns position in charArr
*/
int cpytilspace(char buffer[], char arr[], int arrPointer){

    
    int bufferSize = 0;  //keeps track of size buffer needs to be so we can allocate enough space for it
    int index = arrPointer; //track where we're working in the array
    const int init_arrPointer = arrPointer;   //holds where we initially started with the array

    while( !shouldBeIgnored(arr[index]) ){  //runs until an ignorable space


        //checks for comments to ignore and then breaks buffer at index after comment
        if (arr[index] == '/' && arr[index + 1] == '*'){

            arrPointer = index;
            arrPointer += 2;

            while (arr[arrPointer] != '*' && arr[arrPointer + 1] != '/'){
                arrPointer++;
            }
            
            arrPointer++;
            index = arrPointer;
            
            break;

        }

        bufferSize++;
        index++;
    }
    

    // printf("buffersize %d\n", bufferSize);

    //copying the valid word chunk from arr to buffer
    for(int k = 0; k < bufferSize; k++){
        buffer[k] = arr[init_arrPointer + k];
    }
    buffer[bufferSize] = '\0';  //always add terminator to strs in c

    // printf("functionm buffer holds %s index is %d\n", buffer, index);
    return index + 1;   //return where we will continue workin with array


}



/*Organize word chunks into proper lexeme*/
void tokenize(char *chunk){

    if (chunk == NULL || chunk[0] == '\0'){
        return;
    }

    printf("chunk %s\n", chunk);

    // if (isWord(chunk)){
        
    // }

}