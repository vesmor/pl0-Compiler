/*
    pm/0 Scanner
    COP3402 Euripedes Montagne
    Romsev Charles
    Brandon Sheridan

    April 14th 2023
*/ 


/*
    TODOS
    [] The Compiler output should Display on screen: 
        The input program and a message indicating that the program is syntactically correct. Otherwise, show an error message. 

*/

/*
    TODO: figure out which symbol table implementation we should use

    TODO: delete the symbol or mark the variable as deleted once we've reached the end of the scope of where it was declared
*/

/*

    TODO: add emit error function overloading when time permits so we dont keep forgetting the "\0"

*/

/*
    Correct output:
        1.- Print input (program in PL/0) 
    
        2.- Print out the message “No errors, program is syntactically correct”  
        
        2.- Print out the generated code 
        
        3.- Create file with executable for your VM virtual machine (HW1)

    Error code output:
        1. print input
        
        2. a message alon gthe lines of:
              ***** Error number xxx, period expected

*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


//these are here cuz romsev wants to be extra with terminal coloring
#define RED   "\x1B[31m"
#define RESET "\x1B[0m"

/*------Lexer Use----------*/
#define  nmax                     5       /* maximum amount of digits in a number*/
#define  cmax                    11       /* maximum number of chars for idents */
#define  strmax                 256       /* maximum length of strings */
#define  MAX_SYMBOL_TABLE_SIZE  500
#define  MAX_CODE_SIZE         1000
/*------------------------*/


/*----Symbol Tables use----*/
//for symbol table kinds
#define CONST   1
#define VAR     2
#define PROC    3

//symbol table marks
#define UNUSED  0
#define USED    1
/*----------------------*/

//added 9 to the token_type enum(???)
typedef enum token_type{
    skipsym = 1, identsym, numbersym, plussym, minussym, 
    multsym,  slashsym, oddsym, eqlsym, neqsym, lessym, leqsym, 
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym, 
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,  
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym, 
    readsym , elsesym9
}token_type;

//opcodes copied from HW1
typedef enum opcodes {
    LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SYS,
    SOU = 1, SIN, EOP   //these three codes depend on the M passed in

} opcodes;

//also copied from HW1
typedef enum OPRcodes{
    RTN = 0, ADD, SUB, MUL, DIV, EQL, NEQ, LSS, LEQ, GTR, GEQ, ODD
}OPRcodes;


typedef struct lexeme{

    char *token_name;
    int token_type;

}lexeme;

//instruction hold for parsing
typedef struct instruction{

    int op; //opcode
    int L;  // lexicographical level
    int M;  // modifier
    

}instruction;

/*
    For constants, you must store kind, name and value. 
    For variables, you must store kind, name, L and M.
*/
typedef struct symbol{

    int kind;       //ex: const = 1, var = 2, const = 3
    char name[12];  //name like str is variables name
    int val;        //literal value of a number
    int level;      //lexographical level
    int addr;       //M address
    int mark;       //marks when the symbol has been used

}symbol;



/* list of reserved keyword names */
const char  *word [] = { "const", "var", "begin", "end", "if", "then", "while", "do", "read", "write", "odd", "procedure", "call"}; 
/* list of ignored symbols */
const char ignoresym [] = { '\n', '\0', ' ', '\t', '\f', '\r', '\v'};                         
/* list of special symbols such as arithmetic*/
const char ssym[] = {'*', ')', '.', '>', ';', '-', '(', ',', '<', '%', '+', '/', '=', ':', '!'};
/*master list of all symbols or keywords, matching index with token_type enum*/
const char *sym[] = {"", "", "", "", "+", "-", "*", "/", "odd", "=", "<>", "<", "<=", ">", ">=", "(", ")", ",", ";", ".", 
    ":=", "begin", "end", "if", "then", "while", "do", "call", "const", "var", "procedure", "write", "read", ""};


const int norw        = sizeof(word)/sizeof(word[0]);             /* number of reserved words */
const int ignoresymlen= sizeof(ignoresym)/sizeof(ignoresym[0]);   /* length of ignoresym array*/
const int ssymlen     = sizeof(ssym)/sizeof(ssym[0]);             /*len of special symbol arr*/
const int symlen      = sizeof(sym)/sizeof(sym[0]);               /*master sym array length*/


#define NUM_ERRORS 29
const char *err_messages[NUM_ERRORS] =  {
 
        "Use = instead of := ", 
        "= must be followed by a number ", 
        "Identifier must be followed by = ", 
        "const , var , procedure must be followed by identifier" , 
        "Semicolon or comma missing" , 
        "Incorrect symbol after procedure declaration" , 
        "Statement expected",
        "Incorrect symbol after statement part in block",
        "Period expected ",
        "Semicolon between statements missing",
        "Undeclared identifier",
        "Assignment to constant or procedure is not allowed",
        "Assignment operator expected",
        "call must be followed by an identifier",
        "Call of a constant or variable is meaningless" , 
        "then  expected ", 
        "Semicolon or end expected ", 
        "do expected",
        "Incorrect symbol following statement",
        "Relational operator expected", 
        "Expression must not contain a procedure identifier", 
        "Right parenthesis missing",
        "The preceding factor cannot begin with this symbol", 

        //scanner errors
        "An expression cannot begin with this symbol", 
        "This number is too large", 
        "Identifier too long", 
        "Invalid symbol",

        //extra errors not given in instructions
        "Comment does not have ending '*/'",
        "Identifier already declared",

};



//Error Signals
//starts from -1 to leave space for the not found error
//doesn't match up with array indexing so that we can make all error signals a negative, less than 0 number
typedef enum errors{

    /* extra errors not given in instructions starts minus 1 to let*/
    IDENT_ALR_DECLARED_ERR = (-NUM_ERRORS - 1), //"Identifier already declared"
    END_OF_COMMENT_ERR, //"Comment does not have ending '*/'"

    /* Scanner Errors */
    INVALID_SYM_ERR, //"Invalid symbol"
    IDENT_TOO_LONG_ERR, //"Identifier too long"
    NUM_TOO_LONG_ERR, //"This number is too large"
    INVALID_VAR_ERR, //"An expression cannot begin with this symbol"

    /* Parser Errors */
    ARITHMETIC_ERR, //"The preceding factor cannot begin with this symbol"
    INCOMPLETE_PARENTHEIS_ERR, //"Right parenthesis missing"
    EXPRESSION_PROCEDURE_ERR, //"Expression must not contain a procedure identifier"
    NEEDS_COMPARE_ERR, //"Relational operator expected"
    WRONG_SYM_AFTER_STMNT_ERR, //"Incorrect symbol following statement"
    DO_MISSING_ERR, //"do expected"
    SEMI_OR_END_MISSING_ERR, //"Semicolon or end expected "
    THEN_MISSING_ERR, //"then  expected "
    IMPROPER_CALL_ERR, //"Call of a constant or variable is meaningless"
    CALL_NEEDS_IDENT_ERR, //"call must be followed by an identifier"
    ASSGN_MISSING_ERR, //"Assignment operator expected"
    ILLEGAL_CONST_CHANGE_ERR, //"Assignment to constant or procedure is not allowed"
    UNDECLARED_IDENT_ERR, //"Undeclared identifier"
    SEMICOLON_MISSING_ERR, //"Semicolon between statements missing"
    MISSING_PERIOD_ERR, //"Period expected "
    STMNT_BLOCK_INCORRECT_ERR, //"Incorrect symbol after statement part in block"
    STMN_EXPECTED_ERR, //"Statement expected"
    WRONG_SYM_AFTER_PROC_ERR, //"Incorrect symbol after procedure declaration"
    SEMI_OR_COLON_MISSING_ERR, //"Semicolon or comma missing"
    IDENT_AFTER_KEYWORD_ERR, // "const , var , procedure must be followed by identifier"
    CONST_NEEDS_EQ_ERR, //"Identifier must be followed by = "
    IDENTIFIER_EXPECTED_ERR, //"= must be followed by a number "
    USE_EQ_NOT_BECOME_ERR, //"Use = instead of := ", 


    // if identifier not found in symbol table
    NOT_FOUND = -1 
}errors;






FILE *in;
FILE *out;

//parser global variables

symbol table[MAX_SYMBOL_TABLE_SIZE];
instruction code[MAX_CODE_SIZE];
int cx; //working code arr index
int tableworkingIndex;//working index of symbol table
int tableSize; //size of symbol table
int token; 
// int LexLevel;


/*---------Function Declarations-----------------*/
//
int findSymVal(char *chunk);
int shouldBeIgnored(char c);
int isSpecialSym(char c);
char* readProgram(int *arrSize);
int chunkify(char buffer[], char arr[], int arrPointer, int arrSize);
int isWord (char *chunk);
int determinNonReserved(char *chunk);
int tokenize(char *chunk);
void printLexemes(lexeme *list, size_t size);
int var_declaration();
symbol initSymObj(int kind, char *name, int val, int level, int addr);
void printTable(symbol table[], int tableSize);
void emitError(int errorSignal, char *invalidIdent);
int isStartStatement();
void program(int LexLevel);
void statement(int LexLevel);
void term(int LexLevel);
void factor(int LexLevel);
void block(int LexLevel);
void const_declaration(int LexLevel);
int seekSymbol(char *targetName, int target_LexLevel);
int symboltablecheck(char *targetName);
void expression(int LexLevel);
void condition(int LexLevel);
void emit(int op, int L, int M);
void printInstructions();
void markTable(int current_LexLevel);
/*-----------------------------------------------*/


int main(int argc, char const *argv[])
{

    char tokenFileName[] = "tokens.txt"; //remember to change this before submission lol

    in = fopen(argv[1], "r");
    out = fopen(tokenFileName, "w");

    if(in == NULL){

        printf(RED "Fatal File Error: " RESET);
        printf("No input file provided or no input file of that name found.\n");
        printf("If a file was provided make sure you called it from the proper directory.\n");
        
        fclose(out);
        return EXIT_FAILURE;
    }

    if(out == NULL){

        printf(RED "Fatal Error: File unable to be created:\n" RESET);
        printf("Program failed to create the file " RED "%s" RESET " possibly due to a non-existent directory.\n", tokenFileName);
        printf("\t-Try changing the 'char tokenFileName[]' variable in main from HW3/tokens.txt' to 'tokens.txt\n");
        fclose(in);
        return EXIT_FAILURE;
    }

    int arrSize;    
    int indexPointer = 0;   //keeps track of index we're reading from from array

    char *charArr = readProgram(&arrSize);  //process file into one big array
    char bufferArr[strmax];     //used to help seperate into tokens
    lexeme *lex_list = NULL;

    // printf("Lexeme Table:\n\nlexeme\t\ttoken type\n");

    
    //tokenize program using a buffer array
    size_t lex_size;   //track size of lex_list
    for (size_t i = 0; (i < arrSize) && (indexPointer < arrSize); i++){

        indexPointer = chunkify(bufferArr, charArr, indexPointer, arrSize);   //returns the index where we left off
        
        lex_list = realloc(lex_list, (i+1) * sizeof(lexeme));
    
        int val = tokenize(bufferArr);

        //weeds out EOC error
        if(indexPointer == END_OF_COMMENT_ERR){
            strcpy(bufferArr, "EOCErr");
            val = END_OF_COMMENT_ERR;
            indexPointer = arrSize + 1; //end processing loop after executing these last lines
            lex_size = i;
        }

        if( val < 0 ){
            // printf("Token b4 emitError is [%s]\n", bufferArr);
            emitError(val, "\0");
        }


        if (val) {

            // printf("%s\t\t%d\n", bufferArr, val);
            // fprintf(out, "%s \t\t\t%d\n", bufferArr, val);

            int len = strlen(bufferArr);
            lex_list[i].token_name = malloc(len * sizeof(char));
            strcpy(lex_list[i].token_name, bufferArr);
            lex_list[i].token_type = val;
        }

        lex_size = i; //store index for lex_list size
    }
    
    // printf("\n");
    // printf("Lexeme List:\n");
    
    printLexemes(lex_list, lex_size);

    // printf("\n");
    // printf("\n");


     //________________________________END OF LEXXING SECTION____________________________________________//
    //                                     Time to parse                                                //

    
    fclose(in);
    fclose(out);
    in = NULL;
    out = NULL;
    
    char VMoutputName[] = "elf.text";

    in = fopen(tokenFileName, "r"); 
    out = fopen(VMoutputName, "w"); //possibly may need to comment this out for submission

    // out = fopen(VMoutputName, "w"); //possibly may need to comment this out for submission

    if(in == NULL){
        printf(RED "Fatal File Error: the expected tokens.txt file not found:\n" RESET);
        printf("the program should be reading the tokens.txt file but tried to open " RED "%s" RESET " instead.\nIf the names match, the file might have never been created.", tokenFileName);
        return EXIT_FAILURE;
    }

    if(out == NULL){
        printf(RED "File File Error: %s could not be located\n" RESET, VMoutputName);
        return EXIT_FAILURE;
    }


    //now what...
    int LexLevel = 0;
    tableSize = 0; //symbol table size
    tableworkingIndex = 0;
    cx = 0;
    // tokens = readTokens();

    int numVars;
    table[tableworkingIndex++] = initSymObj(PROC, "main", LexLevel, 0, 3); //adds main procedure to symbol table at index 0
    
    emit(JMP, 0, 3);

    program(LexLevel); //literally starts reading program

    // code[0].M = cx;//jmp to where main procedure is

    // markTable(); //since we only have 1 function this is called in main

    printInstructions();
    printTable(table, tableSize);
    
    


    //clean up
    // for (size_t index = 0; index < lex_size; index++)
    // {
    //     // if(lex_list[index].token_name == NULL)
    //     free(lex_list[index].token_name);
    
    // }
    // free(lex_list);
    // free(charArr);
    fclose(in);
    fclose(out);

    return 0;
}


//-------------------------------------FUNCTION DECLARATIONS-----------------------------


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

/* returns index + 1of delim symbol that should be ignored 0 otherwise */
int shouldBeIgnored(char c){

    if(isspace(c)){
        return 1;
    }

    // for (size_t i = 0; i < ignoresymlen; i++)
    // {
    //     if(c == ignoresym[i])
    //         return i + 1;
    // }
    
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

    // printf("Source Program:\n");
    // fprintf(out, "Source Program:\n");

    char *charArr = (char *) malloc(1 * sizeof(char));

    for (int i = 0; fscanf(in, "%c", &charArr[i]) > 0;  ){    //incrementor in statement
        
        
        if(feof(in)){
            break;
        }

        // printf("%c", charArr[i]);
        // fprintf(out, "%c", charArr[i]);
    
        i++;    //added in statement so it didnt mess up realloc 'math' for some reason
        charArr = realloc(charArr, sizeof(char) * (i + 1));
        if(charArr == NULL){
            free(charArr);
            printf("Ran out of memory dude. Gotta exit program\n");
            fprintf(out, "Ran out of memory dude. Gotta exit program\n");
            fclose(out);
            exit(EXIT_FAILURE); //exit program since we have no memory
        }
    
        (*arrSize) = i;

    }
    charArr = realloc(charArr, sizeof(char) * (*arrSize));
    charArr[*arrSize] = '\0'; //signify end of arr

    // printf("\n\n");

    return charArr;
}


/*
    seperates known symbols into chunks to be organized later
    returns 'working' position in charArr
*/
int chunkify(char buffer[], char arr[], int arrPointer, int arrSize){

    
    int bufferSize = 0;  //keeps track of size buffer needs to be so we can allocate enough space for it
    int index = arrPointer; //track where we're working in the array
    const int init_arrPointer = arrPointer;   //holds where we initially started with the array

    //makes space in buffer arr until we land at the index of an ignorable element
    while( !shouldBeIgnored(arr[index]) && index < arrSize){


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
            if( (arr[index] == ':' && arr[index+1] == '=') || (arr[index] == '<' && arr[index+1] == '>') || 
                (arr[index] == '<' && arr[index+1] == '=') || (arr[index] == '>' && arr[index+1] == '=')){

                bufferSize += 2;
                index++;
                break;
            }

            bufferSize++;
            break;
            

        }

        //peek to see if next char is a spec sym so we just break it here
        if((index + 1) < arrSize){
            if( isSpecialSym(arr[index + 1]) ){
                bufferSize++;
                break;
            }
        }
        // printf("in yk char is %c next is %c\n\n", arr[index], arr[index+1]);

        bufferSize++;
        index++;
    }
    

    //copying the valid word chunk from arr to buffer
    // printf("\tbuff size %d\n", bufferSize);
    for(int k = 0; k < bufferSize; k++){
        buffer[k] = arr[init_arrPointer + k];
    }
    buffer[bufferSize] = '\0';  //always add terminator to end of strs in c

    // printf("bufferArr %s\n", buffer);
    arrPointer = index + 1;
    return arrPointer;   //continue to next array element
}



//checks if chunk is a keyword
//returns index number where found 0 if not
int isWord (char *chunk){

    for (size_t i = 0; i < norw; i++)
    {
        if( !strcmp(chunk, word[i]) ){
            return i + 1; // +1 so that const does not get looked over in the if statemnt for rtn 0 
        }
    }

    return 0;
}



//    determins if the chunk is an ident, int, or invalid. returns 2 for ident, 3 for int,
//    -17 for non num in int, -18 for int too long, -19 for name too long, -16 for invalid symbol
int determinNonReserved(char *chunk){
    int i = 1;
    if (isdigit(chunk[0])){
        for(i; i <= nmax; ++i) {
            if (chunk[i] == '\0') {
                return numbersym;
            }
            if (!isdigit(chunk[i])) {
                return INVALID_VAR_ERR;            
            }
        }
        return NUM_TOO_LONG_ERR;
    }
    else if (isalpha(chunk[0])) {
        for(i; i <= cmax; ++i) {
            if (chunk[i] == '\0') {
                return identsym;
            }
        }
        return IDENT_TOO_LONG_ERR;
    }

    return INVALID_SYM_ERR;

}


/*
   Organize word chunks into proper lexeme category
    returns token number
    returns 0 if empty token
*/
int tokenize(char *chunk){


    if (chunk == NULL || shouldBeIgnored(chunk[0]) || 
        (strcmp(chunk, "\0") == 0)){
        // printf(RED "\tbein ignored in tokenize\n" RESET);
        return 0;
    }


    // printf("tokenize chunk [%s]\n", chunk);
    int tokenVal = determinNonReserved(chunk);

    if (isWord(chunk) > 0 || isSpecialSym(chunk[0])){
        tokenVal = findSymVal(chunk);
    }

    // printf("\tchunk %s\n", chunk);
    return tokenVal;
}


void printLexemes(lexeme *list, size_t size){

    for (size_t i = 0; i <= size; i++)  //changed due to only printing last lexeme to terminal but not to file sometimes but only sometimes. Im so fucking baffled why
    {
        if(list[i].token_name == NULL || list[i].token_type <= 0){
            continue;
        }  

        // printf("|%d ", list[i].token_type);
        fprintf(out, "%d ", list[i].token_type);
        if (list[i].token_type == identsym || list[i].token_type == numbersym){
            // printf("%s ", list[i].token_name);
            fprintf(out, "%s ", list[i].token_name);
        }
    }
    

}

//________________________________END LEXER FUNCS_______________________//
/*                               START PARSER FUNCS                    */

//TODO: possibly search backwards for symbol and check if the lexlevel matches too
//searches thru symbol table for a targetName name and checks if theyre on the same lexlevel 
//returns index if name and level match, if not returns NOT_FOUND
int seekSymbol(char *targetName, int target_LexLevel){

    //moves backward thru symbol table
    for (int index = tableSize - 1; index >= 0; index--)
    {
        
        // printf("\tI: %d\n", index);
        if(strcmp(table[index].name, targetName) == 0 && 
            table[index].level == target_LexLevel &&
            table[index].mark == UNUSED)
        {
            return index;
        }

    }
    // printf("not found\n");
    return NOT_FOUND;
    

}

//checks if a symbol with targetName exists in general
//returns index if found, returns NOT_FOUND if not
int symboltablecheck(char *targetName){

    
    //moves backward thru symbol table
    for (int index = tableSize - 1; index >= 0; index--)
    {
        // printf("\tI: %d\n", index);
        if(strcmp(table[index].name, targetName) == 0){
            return index;
        }

    }

    return NOT_FOUND;
}



//make symbol struct with values, defaults mark to 0
symbol initSymObj(int kind, char *name, int val, int level, int addr){

    symbol s;
    s.kind = kind;
    strcpy(s.name, name);
    s.val = val;
    s.level = level;
    s.addr = addr;
    s.mark = 0;

    tableSize++; //we're probably adding something to the table so increase the size

    return s;

}

void printTable(symbol table[], int tableSize){

    printf("\nSymbol Table:\n\n");
    // fprintf(out, "\nSymbol Table:\n\n");
    // printf("table size %d\n", tableSize);
    printf("Kind | Name   \t| Value   | Level | Address | Mark\n");
    printf("---------------------------------------------------\n");
    // fprintf(out, "Kind | Name   \t| Value   | Level | Address | Mark\n");
    // fprintf(out, "---------------------------------------------------\n");

    for (size_t i = 0; (i < tableSize) && (table[i].name[0] != '\0'); i++)
    {
        printf("   %d |\t%7s |\t%d |\t%d |\t%d   |\t%d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);
        // fprintf(out, "   %d |\t%7s |\t%d |\t%d |\t%d   |\t%d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);
    }
    
}



//Print error message to console corresponding to error signal passed in. pass "\0" into invalidIdent if not an undeclared_ident error
void emitError(int errorSignal, char *invalidIdent){

    int offset = 2; // offset factors in that array starts at 0 and the enums start 1 less than the amount of errors

    char error_message[strmax] = "";
    errorSignal = abs(errorSignal) - offset; //get index of err message w offset of 
    strcat(error_message, err_messages[errorSignal]);
    
    if(invalidIdent[0] != '\0'){
        strcat(error_message, " "); //add space
        strcat(error_message, invalidIdent); //insert the wrong variable
    }


    // out = fopen("output.txt", "w");

    printf("Error number %d, %s\n", (errorSignal + 1), error_message);
    fprintf(out, "%s\n", error_message);


    
    fclose(out);
    // printTable(table, tableSize);
    // printInstructions();
    _Exit(EXIT_SUCCESS);

}

int isStartStatement(){

    if (token == identsym || token == beginsym || token == ifsym || token == whilesym || token == readsym || token == writesym){
        return 1;
    }

    return 0;

}

void program(int LexLevel){
    // printf("starting program\n");
    

    if(fscanf(in, "%d", &token) <= 0){ //get first token
        printf(RED "\nError: token list is empty\n" RESET);
        printf("Internal program logic might either did not write to/had trouble reading from the token.txt file or the source code was empty.\n");
        _Exit(EXIT_FAILURE);
    }

    block(LexLevel);

    if(token != periodsym){
        emitError(MISSING_PERIOD_ERR, "\0");
    }

    //mark off all global variables from main
    markTable(LexLevel);

    emit(SYS, 0, EOP);//HALT
}

void block(int LexLevel){

    // printf("in block\n");

    //grab number of variables and constants we need
    int numVars = 0;
    while (token == varsym || token == constsym) {
        const_declaration(LexLevel);
        numVars += var_declaration(LexLevel);
    }

    if( numVars < 0 ){  //all error signals are negative numbers
        
        int error = numVars;
        emitError(error, "\0");
    
    }

    //continously read procedure blocks
    while (token == procsym){
        
        fscanf(in, "%d", &token);
        
        if(token != identsym){
            // printf("In proc from from block:\n");
            //procedure missing identifier
            emitError(IDENT_AFTER_KEYWORD_ERR, "\0");
        }

        //grab procedure name
        char procName[cmax + 1];
        fscanf(in, "%s", procName);

        if( seekSymbol(procName, LexLevel) != NOT_FOUND){ //if no name exists in the same lex level we can create one
            // printf("In proc from from block:\n");
            emitError(IDENT_ALR_DECLARED_ERR, procName);
        }

        symbol procsym = initSymObj(PROC, procName, 0, LexLevel, 0);

        table[tableworkingIndex] = procsym;
        tableworkingIndex++;
        
        
        fscanf(in, "%d", &token);
        if (token != semicolonsym){
            // printf("In proc from from block:\n");
            emitError(SEMI_OR_COLON_MISSING_ERR, "\0");
        }

        fscanf(in, "%d", &token);    //expecting an isStartStatement type keyword
        
        
       //increase lexlevel by 1 in block and markTable since we're going into a procedure scope
        block(LexLevel + 1); 

        if (token != semicolonsym){
            // printf("In proc from from block:\n");
            emitError(SEMI_OR_COLON_MISSING_ERR, "\0");
        }

        //mark off (clean) variables in scope of procedures
        markTable(LexLevel + 1);
        
        fscanf(in, "%d", &token); //expecting another procedure keyword or begin to signify "main" function

    }


    emit(INC, 0, numVars + 3);
    
    statement(LexLevel);

}

void const_declaration(int LexLevel){

    if(token == constsym){
        // printf("in const is const\n");
        do
        {
            
            if(fscanf(in, "%d", &token) <= 0){ //check infinite loop
                // printf("in const_decl\n");
                emitError(IDENTIFIER_EXPECTED_ERR,"\0");
            }   
            if (token != identsym){
                emitError(IDENTIFIER_EXPECTED_ERR, "\0");  // i think this error needs to be changed
            }
            
            char identSymStr[cmax + 1];
            fscanf(in, "%s", identSymStr);
            if (seekSymbol(identSymStr, LexLevel) != NOT_FOUND){
                // printf("in const_decl\n");
                emitError(IDENT_ALR_DECLARED_ERR, identSymStr);
                
            }

            
            //save ident name 
            char identName[cmax + 1];
            strcpy(identName, identSymStr);
        
            fscanf(in, "%d", &token);
            if (token != eqlsym) {
                // printf("in const_decl\n");
                emitError(CONST_NEEDS_EQ_ERR, "\0");
            }
            // get next token 
            fscanf(in, "%d", &token);
            if(token != numbersym){
                // printf("in const_decl\n");
                emitError(IDENT_AFTER_KEYWORD_ERR, "\0");
            }
            
            int actualNumber;
            fscanf(in ,"%d", &actualNumber);
            
            // add to symbol table 
            symbol constSym = initSymObj(CONST, identName, actualNumber, LexLevel, 0);
            table[tableworkingIndex] = constSym;
            tableworkingIndex++;

            // get next token
            fscanf(in, "%d", &token); //hoping for a comma
          

        } while (token == commasym);
        
        if(token != semicolonsym){
            // printf("in const_decl\n");
            emitError(SEMI_OR_COLON_MISSING_ERR, "\0");
        }

        fscanf(in, "%d", &token);

    }

}


//    Function for when a variable is about to be declared
//    returns number of vars
int var_declaration(int LexLevel){
    // printf("in var declaration\n");
    int numVars = 0;

    if(token == varsym){
        do
        {   
            if(fscanf(in, "%d", &token) <= 0){ //check infinite loop
                emitError(IDENTIFIER_EXPECTED_ERR,"\0");
            }
            
            if( token != identsym){
                emitError(IDENT_AFTER_KEYWORD_ERR, "\0");
            }
        
            char name[12];
            if(fscanf(in, "%s", name) <= 0){
                emitError(IDENT_AFTER_KEYWORD_ERR, "\0");
            }
            
            if(seekSymbol(name, LexLevel) != NOT_FOUND){
                // printTable(table, tableworkingIndex);
                emitError(IDENT_ALR_DECLARED_ERR, name);
            }

            numVars++;
            
            //Add to symbol table
            symbol varSym = initSymObj(VAR, name, 0, LexLevel, numVars + 2);
            table[tableworkingIndex] = varSym;
            tableworkingIndex++;


            //get next token and hope its a comma
            
            fscanf(in, "%d", &token);
                
        }while (token == commasym);

        
        if (token != semicolonsym){
            // printf("in var_decl:\n");
            emitError(SEMI_OR_COLON_MISSING_ERR, "\0");
        }
        fscanf(in, "%d", &token);
    }


    if( numVars < 0 ){  //all error signals are negative numbers
        
        int error = numVars;
        emitError(error, "\0");
    
    }

    return numVars;
}

//still need if, while, read, write
void statement(int LexLevel){

    // printf("%d in statement\n", token);

    if(token == identsym){

        // printf("in identsym statement\n");
        char identName[cmax];
        fscanf(in, "%s", identName);
        int symIdx = symboltablecheck(identName);
        if (symIdx == NOT_FOUND){
            emitError(UNDECLARED_IDENT_ERR, identName);
        }

        if (table[symIdx].kind != VAR && table[symIdx].kind != PROC){
            emitError(ILLEGAL_CONST_CHANGE_ERR, "\0");
        }

        fscanf(in, "%d", &token); //expecting := sym
        if(token != becomessym){ //expecting to assign a variable
            // printf("was expecting := got %d\n", token);
            //it might also be the arithmetic error
            emitError(ASSGN_MISSING_ERR, "\0");
        }

        fscanf(in, "%d", &token);
        tableworkingIndex = symIdx;
        expression(LexLevel);

        // if (token != semicolonsym && token != endsym){
        //     // printf("missing semicolon %d\n", token);
        //     emitError(ARITHMETIC_ERR, "\0");
        // }

        //emit STO (M = table[symIdx].addr)
        // printf("emitting STO\n");
        emit(STO, LexLevel, table[symIdx].addr);
        // printf("token is after STO %d\n", token);
        return;

    }

    if(token == beginsym){
        // printf("begin in statement token\n");
        do
        {

            if (fscanf(in, "%d", &token) <= 0){ //makes sure we dont get stuck in recursive hell if theres nothing after
                break;
            }

            // printf("out of statement BEFORE recurse token is %d\n", token);
            statement(LexLevel);
            // printf("out of statement after recurse token is %d\n", token);

        } while (token == semicolonsym);
        
        //if statements need a semicolon check for it here and do an error mess


        if(token != endsym){    //make sure end is followed by begin
            // printf("in begin of statement: %d\n", token);
            emitError(SEMI_OR_END_MISSING_ERR, "\0");
        }
         
        fscanf(in, "%d", &token);
        return;
    }

    //if statement
    if(token == ifsym){
        // printf("in if statement\n");

        fscanf(in, "%d", &token);

        condition(LexLevel);

        int jpcIndex = cx;

        // printf("emit JPC\n");
        emit(JPC, LexLevel, 0);

        if(token != thensym){
            emitError(THEN_MISSING_ERR, "\0");
        }

        fscanf(in, "%d", &token);
        statement(LexLevel);
        code[jpcIndex].M = cx * 3; //I think this has something to do with making a new instructions struct

        return;
    }

    if(token == whilesym){

        fscanf(in, "%d", &token);
        int loopIndex = cx; //the counter where the loop is happening
        // printf("loop index in whilesym: %d\n", loopIndex);
        condition(LexLevel);

        if (token != dosym){
            emitError(DO_MISSING_ERR, "\0");
        }

        fscanf(in, "%d", &token);
        int jpcIndex = cx;
        // printf("emit JPC\n"); //M = loopIndex
        emit(JPC, LexLevel, 0);

        statement(LexLevel);

        emit(JMP, LexLevel, loopIndex);
        code[jpcIndex].M = cx * 3;
        return;

    }

    if(token == readsym){

        fscanf(in, "%d", &token);

        if(token != identsym){
            emitError(IDENTIFIER_EXPECTED_ERR, "\0");
        }
        
        char tokenName[cmax];
        fscanf(in, "%s", tokenName);
        int symIndex = symboltablecheck(tokenName);
        if(symIndex == NOT_FOUND){
            emitError(UNDECLARED_IDENT_ERR, tokenName);
        }

        if(table[symIndex].kind != VAR){
            emitError(IDENTIFIER_EXPECTED_ERR, "\0");
        }

        fscanf(in, "%d", &token);
        
        // printf("emit READ\n");
        emit(SYS, 0, SIN); //READ

        // printf("emit STO\n"); //M = table[symIndex].addr
        emit(STO, LexLevel, table[symIndex].addr);

        return;

    }

    if(token == writesym){
        // printf("in write statement\n");
        fscanf(in, "%d", &token);
        expression(LexLevel);
        // printf("emit WRITE\n");
        emit(SYS, 0, SOU); //WRITE

        return;
    }

    if (token == callsym){
        fscanf(in, "%d", &token);
        if (token != identsym){
            emitError(CALL_NEEDS_IDENT_ERR, "\0");
        }
        
        //get procedures name
        char procName[cmax + 1];
        fscanf(in, "%s", procName);

        if (symboltablecheck(procName) == NOT_FOUND){
            // printf("in callsym of statement:\n");
            emitError(UNDECLARED_IDENT_ERR, procName);
        }

        fscanf(in, "%d", &token);

        return;
    }

    // printf("\nThis is a statement.\n\n");

}

void condition(int LexLevel){

    if(token == oddsym){
        fscanf(in, "%d", &token);
        expression(LexLevel);
        // printf("emit ODD\n");
        emit(OPR, 0, ODD);
    }

    else{

        expression(0);
        if(token == eqlsym){
            fscanf(in, "%d", &token);
            expression(LexLevel);
            // printf("emit EQL\n");
            emit(OPR, 0, EQL);
        }
        else if(token == neqsym){
            fscanf(in, "%d", &token);
            expression(LexLevel);
            // printf("emit NEQ\n");
            emit(OPR, 0, NEQ);
        }
        else if(token == lessym){
            fscanf(in, "%d", &token);
            expression(LexLevel);
            // printf("emit LSS\n");
            emit(OPR, 0, LSS);
        }
        else if(token == leqsym){
            fscanf(in, "%d", &token);
            expression(LexLevel);
            // printf("emit LEQ\n");
            emit(OPR, 0, LEQ);
        }
        else if(token == gtrsym){
            fscanf(in, "%d", &token);
            expression(LexLevel);
            // printf("emit GTR\n");
            emit(OPR, 0, GTR);
        }
        else if(token == geqsym){
            fscanf(in, "%d", &token);
            expression(LexLevel);
            // printf("emit GEQ\n");
            emit(OPR, 0, GEQ);
        }
        else{
            emitError(NEEDS_COMPARE_ERR, "\0");
        }

    }

}

void expression(int LexLevel){

    // printf("%d in expression\n", token);

    term(LexLevel);

    while (token == plussym || token == minussym){
        // fscanf(in, "%d", &token);
        // printf("token is plus or minus\n");
    
        if (token == plussym){
            fscanf(in, "%d", &token);
            term(LexLevel);
            //emit add
            // printf("token in expression is add\n");
            emit(OPR, LexLevel, ADD);
        }
        else if (token == minussym){
            fscanf(in, "%d", &token);
            term(LexLevel);
            //emit sub
            // printf("token in expression is sub\n");
            emit(OPR, LexLevel, SUB);
        }    
    }
    


}

void term(int LexLevel){

    // printf("in term %d\n", token);
    factor(LexLevel);

    while(token == multsym || token == slashsym){

        if(token == multsym){

            fscanf(in, "%d", &token);
            factor(LexLevel);
            //emit MUL
            emit(OPR, LexLevel, MUL);

        }

        else if(token == slashsym){

            fscanf(in, "%d", &token);
            factor(LexLevel);
            //emit DIV 
            // printf("emit DIV in term\n");
            emit(OPR, LexLevel, DIV);

        }

    }

}

//
void factor(int LexLevel){
    // printf("in factor %d\n", token);
    if (token == identsym){

        char identifierStr[cmax];
        fscanf(in, "%s", identifierStr);
        int symIdx = symboltablecheck(identifierStr);
        if(symIdx == NOT_FOUND){
            emitError(UNDECLARED_IDENT_ERR, identifierStr);
        }

        if(table[symIdx].kind == CONST){
            //emit LIT(m = table[sym.idx].val)
            // printf("Emit LIT in factor\n");
            emit(LIT, 0, table[symIdx].val);
        }
        else if(table[symIdx].kind == VAR){
            //emit LOD (M = table[symIdx].addr)
            // printf("Emit LOD in factor\n");
            emit(LOD, LexLevel, table[symIdx].addr);
        }

        // table[tableworkingIndex].val = table[symIdx].val;

        fscanf(in, "%d", &token);
    }

    else if(token == numbersym){
        int actualNumber;
        fscanf(in, "%d", &actualNumber); //get the actual number after numbersym

        // table[tableworkingIndex].val = actualNumber;

        //emit LIT
        // printf("Emitting numbersym LIT %d from factor\n", actualNumber);
        emit(LIT, 0, actualNumber);

        fscanf(in, "%d", &token);
    }

    else if(token == lparentsym){
        fscanf(in, "%d", &token);
        expression(LexLevel);
        // printf("exiting expresion to factor parentheses token: %d\n", token);
        if(token != rparentsym){
            emitError(INCOMPLETE_PARENTHEIS_ERR, "\0");
        }
        fscanf(in, "%d", &token);
    }
    else{
        emitError(ARITHMETIC_ERR, "\0");
    }

}

//pushes opcode into text space array
void emit(int op, int L, int M){

    // #define RED   "\x1B[31m"
    // #define RESET "\x1B[0m"

    //def dont want to get bigger than the allocated size
    if(cx > MAX_CODE_SIZE){ 
        printf(RED "Fatal Error: " RESET);  //i just like being extra and wanted to color ur console red
        printf("ran out of allocated text space.\n");
    }

    code[cx].op = op;  //opcode
    code[cx].L = L; // lexicographical level
    code[cx].M = M; // modifier
    cx++;

}

//prints the vm instructions in human readable format
void printInstructions(){

    const char *op_code_names[] = { "LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP", "JPC", "SYS" };

    printf("Assembly Code:\n\n");
    // fprintf(out, "Assembly Code:\n\n");
    
    printf("Line\tOP\tL\tM\n");
    // fprintf(out, "Line\tOP\tL\tM\n");
    for (size_t i = 0; i < cx; i++)
    {
        char op_name[4];
        strcpy(op_name, op_code_names[code[i].op - 1]); //translate op number into name from above arr
        
        //translates the sub-instructions for opr
        if(code[i].op == OPR){
            char *opr_all_names[] = {"RTN", "ADD", "SUB", "MUL", "DIV", "EQL", "NEQ", "LSS", "LEQ", "GTR", "GEQ", "ODD"};
            char opr_name[4];
            strcpy(opr_name, opr_all_names[code[i].M]);
            printf("%3ld %6s %6d %7s\n", i, op_name, code[i].L, opr_name);
        }
        //translates for SYS stuff
        else if(code[i].op == SYS){
            char *sys_names[] ={ "SOU", "SIN", "EOP"};
            char sys_name[4];
            strcpy(sys_name, sys_names[code[i].M - 1]);
            printf("%3ld %6s %6d %7s\n", i, op_name, code[i].L, sys_name);
        }
        else{
            printf("%3ld %6s %6d %7d\n", i, op_name, code[i].L, code[i].M);
            // fprintf(out, "%3ld %6s %6d %7d\n", i, op_name, code[i].L, code[i].M); //prettified output file
        }
        fprintf(out, "%d %d %d\n", code[i].op, code[i].L, code[i].M); //write op codes in plain numbers to file for VM to run
    
    }
    

}

void markTable(int current_LexLevel) {

    for (int i = tableSize - 1; i >= 0; i--) {
        
        //break the loop once we get to any levels below us
        if(table[i].level < current_LexLevel)
            break;

        if(table[i].level == current_LexLevel)
            table[i].mark = 1;

    }
}
