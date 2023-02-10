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
            - everything has been broken down into chunks but next it just needs to be organized
                into the proper tokenType
            - While getting organized it needs to make sure errors get thrown when they happen
            - Gotta revisit the chunkify function because some words are getting cut off
            - "begin" and "end" keyword get the wrong token# or none at all for some reason?

*/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



//look in Lecture 5 Lexical Analysis

#define  norw          10         /* number of reserved words */
#define  imax       32767       /* maximum integer value */
#define  cmax          11         /* maximum number of chars for idents */
#define  strmax       256         /* maximum length of strings */
#define  ignoresymlen   4          /* length of ignoresym array*/
#define  ssymlen       16          /*len of special symbol arr*/
#define  symlen        33

typedef enum{
    skipsym = 1, identsym, numbersym, plussym, minussym, 
    multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym, 
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym, 
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,  
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym, 
    readsym , elsesym
}token_type;

/* list of reserved keyword names */
const char  *word [ ] = { "begin", "end", "if", "then", "while", "do", "call", "read", "write", "else"}; 

/* list of ignored symbols */
const char ignoresym [] = { '\n', '\0', ' ', '\t'};
                         
/* internal representation  of reserved words */
// int  wsym [ ] =  { nulsym, beginsym, callsym, constsym, dosym, elsesym, endsym, ifsym, oddsym, procsym, readsym, thensym, varsym, whilesym, writesym};


/* list of special symbols such as arithmetic*/
char ssym[ssymlen] = {'*', ')', '.', '>', ';', '-', '(', ',', '<', '%', '+', '/', '=', '#', '$', ':'};
char *sym[] = {"", "", "", "", "+", "-", "*", "/", "odd", "=", "!=", "<", "<=", ">", ">=", "", "",",", ";", ".", ":=", "begin", "end", "if", "then", "while", "do", "call", "const", "var", "procedure", "write", "read", "else"};


typedef struct lexeme{

    char *tokenName;
    int tokenType;
    int val;
    struct lexeme *next;

}lexeme;


FILE *f;


int findSymVal(char *chunk);
int shouldBeIgnored(char c);
int isSpecialSym(char c);
char* readProgram(int *arrSize);
int chunkify(char buffer[], char arr[], int arrPointer);
int isWord (char *chunk);
int isNumber (char *chunk);
int isIdentifier(char *chunk);
lexeme* tokenize(char *chunk);


void printToken(lexeme* t){

    if(t != NULL){
        printf("Token: %s\n\ttype:%d\n\tval: %d\n", t->tokenName, t->tokenType, t->val);
    }
}


int main(int argc, char const *argv[])
{

    // ssym[plussym]=  ('+');  ssym[minussym]=    ('-');  ssym[multsym]=      ('*');
    // ssym[slashsym]= ('/');  ssym[lparentsym]=  ('(');  ssym[rparentsym]=   (')'); 
    // ssym[eqsym]=    ('=');  ssym[commasym]=    (',');  ssym[periodsym]=    ('.'); 
    // ssym[neqsym]=   ('#');  ssym[lessym]=      ('<');  ssym[gtrsym]=       ('>'); 
    // ssym[leqsym]=   ('$');  ssym[geqsym]=      ('%');  ssym[semicolonsym]= (';');



    f = fopen(argv[1], "r");

    int arrSize;    
    int indexPointer = 0;   //keeps track of index we're reading from from array

    char *charArr = readProgram(&arrSize);  //process file into one big array
    char bufferArr[strmax];     //used to help seperate into tokens

    printf("Lexeme Table:\n\nlexeme\ttoken type\n");

    
    lexeme **tokens = malloc(arrSize * sizeof(lexeme*));  //arr of lexemes 1
    // tokens[0] = malloc(sizeof(lexeme)); //init first lexeme in arr
    
    //tokenize program using a buffer array
    for (size_t i = 0; (i < arrSize) && (indexPointer < arrSize); i++){
        
        indexPointer = chunkify(bufferArr, charArr, indexPointer);   //returns the index where we left off
        // printf("buffer holds %s indexptr is %d\n", bufferArr, indexPointer);


        tokenize(bufferArr);


        // printToken(tokens[i]);
        // printf("%ld\n", i);

        // tokens = realloc(tokens, sizeof(lexeme*) * (i+1));

    }

    

     
    




    //clean up
    // free(bufferArr);
    free(charArr);
    fclose(f);

    return 0;
}


lexeme* makeLexNode(char *token, int tokenType, int value){

    lexeme *node = malloc(sizeof(lexeme));

    strcpy(node->tokenName, token);
    node->tokenType = tokenType;
    node->val = value;
    node->next = NULL;

    return node;
}



int findSymVal(char *chunk){

    for (size_t i = 0; i < symlen; i++)
    {
        if( !strcmp(chunk, sym[i]) ){
            return i;
        }
    }

    return 0;    

}

/* returns 1 if symbol should be ignored 0 otherwise */
int shouldBeIgnored(char c){
    for (size_t i = 0; i < ignoresymlen; i++)
    {
        if(c == ignoresym[i])
            return 1;
    }
    
    return 0;
}

/*checks if character exists in special symbol array*/
int isSpecialSym(char c){

    for (size_t i = 0; i < ssymlen; i++)
    {
        if (c == ssym[i]){
            return 1;
        }
    }  
    
    return 0;
}


/* reads pl0 input file and puts every character into one huge array and updates the array size
    returns a char array with everything from file
*/
char* readProgram(int *arrSize){

    printf("Source Program:\n");

    char *charArr = (char *) malloc(1 * sizeof(char));
    // int arrSize;
    for (int i = 0; fscanf(f, "%c", &charArr[i]) > 0; ){    //incrementor in statement


        // printf("at index %d adding + 1\n",i);

        printf("%c", charArr[i]);

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
    printf("\n\n");

    return charArr;
}


/*
    seperates known symbols into chunks to be organized later
    returns 'working' position in charArr
*/
int chunkify(char buffer[], char arr[], int arrPointer){

    
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


        //if the character we landed on is a special sym we break the chunk here
        if (isSpecialSym(arr[index])){
            if( arr[index] == ':' && arr[index+1] == '='){
                bufferSize += 2;
                index++;
                break;
            }

            else{
                bufferSize++;
                index++;
                break;
            }

        }

        //peek to see if next char is a spec sym so we just break it here
        if( isSpecialSym(arr[index + 1]) ){
            bufferSize++;
            break;
        }


        // printf("arr %c\n", arr[index]);
        bufferSize++;
        index++;
    }
    

    // printf("buffersize %d\n", bufferSize);

    //copying the valid word chunk from arr to buffer
    for(int k = 0; k < bufferSize; k++){
        buffer[k] = arr[init_arrPointer + k];
    }
    buffer[bufferSize] = '\0';  //always add terminator to end of strs in c


    return index + 1;   //continue to next array element


}

/*
    checks if chunk is a keyword
    returns 0 if not, and index number if found
*/
int isWord (char *chunk){

    for (size_t i = 0; i < norw; i++)
    {
        if( !strcmp(chunk, word[i]) ){
            return i;
        }
    }

    return 0;
    

}

/*
    checks if chunk is a number
    1 if yes, index where it isnt if not
*/
int isNumber (char *chunk){

    int len = strlen(chunk);
    for (size_t i = 0; i < len; i++)
    {
        if( !isdigit(chunk[i]) || (chunk[i] == '-' && i > 0) ){   //OR stmnt checks that there isnt a minus sign past the first index
            return i;
        }
    }
    
    return 1;

}

/*
    if variable starts with number or special sym return 0
    otherwise return 1
*/
int isIdentifier(char *chunk){

    if ( isdigit(chunk[0]) || isSpecialSym(chunk[0]) ){
        return 0;
    }

    return 1;

}


/*Organize word chunks into proper lexeme category*/
lexeme* tokenize(char *chunk){

    if (chunk == NULL || chunk[0] == '\0'){
        return NULL;
    }

    printf("%s\t", chunk);

    char err[strmax]; //for error messages maybe?

    lexeme* t = NULL;


    /*
        having trouble with begin not getting the proper token type
        and end not getting a number at all
    */

    int tokenVal = findSymVal(chunk);

    if (isWord(chunk)){
        // t = makeLexNode(chunk, tokenVal, tokenVal);
        printf("\n");
        return t;
    }

    //literally just checks for var symbol since its not a keyword ig?
    else if ( !strcmp(chunk, "var") ){
        
    }

    else if( isNumber(chunk) ){
        tokenVal = numbersym;
    }

    else if( isIdentifier(chunk) ){
        tokenVal = identsym;
    }



    printf("%d", tokenVal);
    printf("\n"); //remove this once token type is determined

    return t;
}
