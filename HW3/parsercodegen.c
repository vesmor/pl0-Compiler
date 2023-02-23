/*
    pm/0 Scanner
    COP3402 Euripedes Montagne
    Romsev Charles
    Brandon Sheridan

    February 14 2023
*/ 

/*  ________________ERROR CODE DOCUMENTATION____________________ 
        -1 - Identigiers cannot begin with a digit.
        -2 - Number too long.
        -3 - Name too long.
        -4 - Invalid symbols.
        -5 - The input ends during a comment (i.e. a comment was started but not 
            fnished when an end-of-file was seen reading from the input file).

    ________________WILL BE IMPORTIANT TO SEE NEXT PROJECT_________________
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>




//Error signals
typedef enum errors{
    SEMICOLON_MISSING_ERR = -9,
    END_OF_COMMENT_ERR,
    INVALID_SYM_ERR,
    NAME_TOO_LONG_ERR,
    INT_TOO_LONG_ERR, 
    INVALID_INT_ERR,

    IDENTIFIER_EXPECTED_ERR,
    IDENT_ALR_DECLARED_ERR,
    NOT_FOUND = -1/*Error if there is no variable after Var declaration*/
}errors;

#define  norw                    14       /* number of reserved words */
#define  cmax                    11       /* maximum number of chars for idents */
#define  strmax                 256       /* maximum length of strings */
#define  ignoresymlen             4       /* length of ignoresym array*/
#define  ssymlen                 17       /*len of special symbol arr*/
#define  symlen                  34       /*master sym array length*/
#define  MAX_SYMBOL_TABLE_SIZE  500

typedef enum token_type{
    skipsym = 1, identsym, numbersym, plussym, minussym, 
    multsym,  slashsym, oddsym, eqlsym, neqsym, lessym, leqsym, 
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym, 
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,  
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym, 
    readsym , elsesym
}token_type;


typedef struct lexeme{

    char *token_name;
    int token_type;

}lexeme;

/*
    For constants, you must store kind, name and value. 
    For variables, you must store kind, name, L and M.
*/
typedef struct symbol{

    int kind;       //ex: const = 1, varsym = 29
    char name[12];  //name like str is variables name
    int val;        //literal value of a number
    int level;      //lexographical level
    int addr;       //M address
    int mark;

}symbol;



/* list of reserved keyword names */
const char  *word [ ] = { "const", "var", "procedure", "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write", "odd"}; 
/* list of ignored symbols */
const char ignoresym [] = { '\n', '\0', ' ', '\t'};                         
/* list of special symbols such as arithmetic*/
char ssym[ssymlen] = {'*', ')', '.', '>', ';', '-', '(', ',', '<', '%', '+', '/', '=', '#', '$', ':', '!'};
/*master list of all symbols or keywords, matching index with token_type enum*/
char *sym[] = {"", "", "", "", "+", "-", "*", "/", "odd", "=", "!=", "<", "<=", ">", ">=", "(", ")", ",", ";", ".", 
    ":=", "begin", "end", "if", "then", "while", "do", "call", "const", "var", "procedure", "write", "read", "else"};


char *err_messages[] =  {
                            "program must end with period",
                            "const, var, and read keywords must be followed by identifier",
                            "symbol name has already been declared",
                            "constants must be assigned with =",
                            "constants must be assigned an integer value",
                            "constant and variable declarations must be followed by a semicolon",
                            "undeclared identifier",
                            "only variable values may be altered",
                            "assignment statements must use :=",
                            "begin must be followed by end",
                            "if must be followed by then",
                            "while must be followed by do",
                            "condition must contain comparison operator",
                            "right parenthesis must follow left parenthesis",
                            "arithmetic equations must contain operands, parentheses, numbers, or symbols"
                        };


FILE *in;
FILE * out;




/*---------Function Declarations-----------------*/
//
int findSymVal(char *chunk);
int shouldBeIgnored(char c);
int isSpecialSym(char c);
char* readProgram(int *arrSize);
int chunkify(char buffer[], char arr[], int arrPointer);
int isWord (char *chunk);
int determinNonReserved(char *chunk);
int tokenize(char *chunk);
void printLexemes(lexeme *list, size_t size);
int symboltablecheck(symbol *table ,char *target);
void readTokens(symbol *table, int *tableSize);
int var_declaration(symbol table[], int *workingIndex);
symbol initSymObj(int kind, char *name, int val, int level, int addr);
void printTable(symbol table[], int tableSize);
/*-----------------------------------------------*/



int main(int argc, char const *argv[])
{

    char tokenFileName[] = "HW3/tokens.txt";

    in = fopen(argv[1], "r");
    out = fopen(tokenFileName, "w");

    if(in == NULL){
        printf("No input file provided \n");
        return EXIT_FAILURE;
    }

    int arrSize;    
    int indexPointer = 0;   //keeps track of index we're reading from from array

    char *charArr = readProgram(&arrSize);  //process file into one big array
    char bufferArr[strmax];     //used to help seperate into tokens
    lexeme *lex_list = NULL;

    printf("Lexeme Table:\n\nlexeme\ttoken type\n");

    
    //tokenize program using a buffer array
    size_t lex_size;   //track size of lex_list
    for (size_t i = 0; (i < arrSize) && (indexPointer < arrSize); i++){


        indexPointer = chunkify(bufferArr, charArr, indexPointer);   //returns the index where we left off
        
        lex_list = realloc(lex_list, (i+1) * sizeof(lexeme));
    
        int val = tokenize(bufferArr);

        //weeds out EOC error
        if(indexPointer == END_OF_COMMENT_ERR){
            strcpy(bufferArr, "EOCErr");
            val = END_OF_COMMENT_ERR;
            indexPointer = arrSize + 1; //end processing loop after executing these last lines
            lex_size = i;
        }

        if (val) {

            printf("%s\t%d\n", bufferArr, val);
            // fprintf(out, "%s \t\t\t%d\n", bufferArr, val);

            int len = strlen(bufferArr);
            lex_list[i].token_name = malloc(len * sizeof(char));
            strcpy(lex_list[i].token_name, bufferArr);
            lex_list[i].token_type = val;
        }

        lex_size = i; //store index for lex_list size
    }
    
    printf("\n");
    printf("Lexeme List:\n");
    
    printLexemes(lex_list, lex_size);

    printf("\n");
    printf("\n");


    //________________________________END OF LEXXING SECTION____________________________________________//
    //                                     Time to parse                                                //

    
    fclose(in);
    fclose(out);
    in = NULL;
    out = NULL;

    in = fopen("HW3/tokens.txt", "r");

    if(in == NULL){
        perror("File not found");
        return EXIT_FAILURE;
    }


    //now what...

    int tableSize = 0; //symbol table size
    symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

    int token;
    int workingIndex = 0;
    int numVars;
    for (size_t i = 0; fscanf(in, "%d", &token) > 0; i++){

        if (token == varsym){
            printf("working in var: %d\n", workingIndex);
            numVars = var_declaration(symbol_table, &workingIndex);

            //TODO: assign proper error messages to errors thrown
            if( numVars == IDENT_ALR_DECLARED_ERR || numVars == IDENTIFIER_EXPECTED_ERR || numVars == SEMICOLON_MISSING_ERR){
                printf("vars error\n");
            }

        }

        (tableSize) = workingIndex;

    }

    printTable(symbol_table, tableSize);


    


    //clean up
    for (size_t index = 0; index < lex_size; index++)
    {
        free(lex_list[index].token_name);
    }
    free(lex_list);
    free(charArr);
    // fclose(in);
    // fclose(out);

    return 0;
}


/*returns index of symbol found from masterlist*/
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


/* 
    reads pl0 input file and puts every character into one huge array;
    updates array size variable to proper value
    returns a char array with everything from file
*/
char* readProgram(int *arrSize){

    printf("Source Program:\n");
    // fprintf(out, "Source Program:\n");

    char *charArr = (char *) malloc(1 * sizeof(char));

    for (int i = 0; fscanf(in, "%c", &charArr[i]) > 0; ){    //incrementor in statement



        printf("%c", charArr[i]);
        // fprintf(out, "%c", charArr[i]);
    
        i++;    //added in statement so it didnt mess up realloc 'math' for some reason
        charArr = realloc(charArr, sizeof(char) * (i + 1));
        if(charArr == NULL){
            free(charArr);
            printf("Ran out of memory dude. Gotta exit program\n");
            fprintf(out, "Ran out of memory dude. Gotta exit program\n");
            fclose(out);
            exit(-1); //exit program with error
        }
    
        (*arrSize) = i + 1;

    }
    charArr = realloc(charArr, sizeof(char) * (*arrSize));
    charArr[*arrSize] = '\0'; //signify end of arr

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

    //makes space in buffer arr until we land at the index of an ignorable element
    while( !shouldBeIgnored(arr[index]) ){


        //checks for signal to start comment and skips portion of array until end of comment signal
        if (arr[index] == '/' && arr[index + 1] == '*'){

            arrPointer = index;
            arrPointer += 2;

            while ( !(arr[arrPointer] == '*' && arr[arrPointer + 1] == '/') ){

                arrPointer++;
                if (arr[arrPointer + 1] == '\0'){
                    return END_OF_COMMENT_ERR;
                }
            }
            arrPointer++;
            index = arrPointer;
            
            break;

        }


        //if the character we landed on is a special sym we break the chunk here
        if (isSpecialSym(arr[index])){
            if( (arr[index] == ':' && arr[index+1] == '=') || (arr[index] == '!' && arr[index+1] == '=') 
                || (arr[index] == '<' && arr[index+1] == '=') || (arr[index] == '>' && arr[index+1] == '=')){
                bufferSize += 2;
                index++;
                break;
            }

            bufferSize++;
            break;
            

        }

        //peek to see if next char is a spec sym so we just break it here
        if( isSpecialSym(arr[index + 1]) ){
            bufferSize++;
            break;
        }


        bufferSize++;
        index++;
    }
    

    //copying the valid word chunk from arr to buffer
    for(int k = 0; k < bufferSize; k++){
        buffer[k] = arr[init_arrPointer + k];
    }
    buffer[bufferSize] = '\0';  //always add terminator to end of strs in c


    arrPointer = index + 1;
    return arrPointer;   //continue to next array element
}



/*
    checks if chunk is a keyword
    returns 0 if not, and index number if found
*/
int isWord (char *chunk){

    for (size_t i = 0; i < norw; i++)
    {
        if( !strcmp(chunk, word[i]) ){
            return i + 1; // +1 so that const does not get looked over in the if statemnt for rtn 0 
        }
    }

    return 0;
}



/*
    determins if the chunk is an ident, int, or invalid. returns 2 for ident, 3 for int,
    -1 for non num in int, -2 for int too long, -3 for name too long, -4 for invalid symbol
*/
int determinNonReserved(char *chunk){
    int i = 1;
    if (isdigit(chunk[0])){
        for(i; i <= 5; ++i) {
            if (chunk[i] == '\0') {
                return numbersym;
            }
            if (!isdigit(chunk[i])) {
                return INVALID_INT_ERR;            
            }
        }
        return INT_TOO_LONG_ERR;
    }
    else if (isalpha(chunk[0])) {
        for(i; i <= cmax; ++i) {
            if (chunk[i] == '\0') {
                return identsym;
            }
        }
        return NAME_TOO_LONG_ERR;
    }

    return INVALID_SYM_ERR;

}


/*Organize word chunks into proper lexeme category*/
int tokenize(char *chunk){

    if (chunk == NULL || chunk[0] == '\0'){
        return 0;
    }

    int tokenVal = determinNonReserved(chunk);

    if (isWord(chunk) > 0 || isSpecialSym(chunk[0])){
        tokenVal = findSymVal(chunk);
    }


    return tokenVal;
}


void printLexemes(lexeme *list, size_t size){

    for (size_t i = 0; i < size; i++)
    {
        if(list[i].token_name == NULL || list[i].token_type == 0){
            continue;
        }  

        printf("|%d ", list[i].token_type);
        fprintf(out, "%d ", list[i].token_type);
        if (list[i].token_type == identsym || list[i].token_type == numbersym){
            printf("%s ", list[i].token_name);
            fprintf(out, "%s ", list[i].token_name);
        }
    }
    

}

/*searches thru symbol table for a target name, returns -1 if not found*/
int symboltablecheck(symbol *table ,char *target){


    int size = sizeof(table)/sizeof(symbol);
    for (size_t index = 0; index < size; index++)
    {
        if(strcmp(table[index].name, target)){
            return index;
        }
    }

    return NOT_FOUND;
    

}

/*read tokens from file and inserts into symbol table struct, update table size*/
void readTokens(symbol *table, int *tableSize){



    // printf("table size %d\n", *tableSize);
    
}

int var_declaration(symbol table[], int *workingIndex){
    
    int numVars = 0;
    int token;

    do
    {   
        char name[12];
        
        int scanned = fscanf(in, "%d", &token); //how many items got fscanned
        scanned += fscanf(in, "%s", name);

        if(token != identsym || !scanned){
            return IDENTIFIER_EXPECTED_ERR;
        }
        
        if(symboltablecheck(table, name) != NOT_FOUND){
            return IDENT_ALR_DECLARED_ERR;
        }

        numVars++;
        
        //Add to symbol table
        symbol newSym = initSymObj(identsym, name, 0, 0, numVars + 2);
        table[*workingIndex] = newSym;
        (*workingIndex)++;


        //get next token and hope its a comma
        scanned = fscanf(in, "%d", &token);
        if(!scanned){
            return IDENTIFIER_EXPECTED_ERR;
        }

    }while (token == commasym);
    
    if (token != semicolonsym){
        return SEMICOLON_MISSING_ERR;
    }
   
    return numVars;
}

symbol initSymObj(int kind, char *name, int val, int level, int addr){

    symbol s;
    s.kind = kind;
    strcpy(s.name, name);
    s.val = val;
    s.level = level;
    s.addr = addr;
    s.mark = 0;

    return s;

}

void printTable(symbol table[], int tableSize){

    printf("table size %d\n", tableSize);
    printf("Kind | Name       | Value | Level | Address | Mark\n");
    printf("---------------------------------------------------\n");

    for (size_t i = 0; i < tableSize; i++)
    {
        printf("   %d |\t\t%s |\t%d |\t%d |\t%d   |\t%d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);
    }
    

}