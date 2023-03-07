/*
    pm/0 Scanner
    COP3402 Euripedes Montagne
    Romsev Charles
    Brandon Sheridan

    March 8th 2023
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

/*
    TODO:
        - original program provided in syllabus isnt completely working
        - line number 4 is giving the wrong OPR code: its 3, should be 4
        - figure out the lex level stuff
        - add terminating errors for scanner portion
        - remove printf traces
        - prettify output
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


//these are here cuz romsev wants to be extra
#define RED   "\x1B[31m"
#define RESET "\x1B[0m"



#define  norw                    12       /* number of reserved words */
#define  cmax                    11       /* maximum number of chars for idents */
#define  strmax                 256       /* maximum length of strings */
#define  ignoresymlen             7       /* length of ignoresym array*/
#define  ssymlen                 17       /*len of special symbol arr*/
#define  symlen                  34       /*master sym array length*/
#define  MAX_SYMBOL_TABLE_SIZE  500
#define  MAX_CODE_SIZE         1000

//for symbol table kinds
#define CONST   1
#define VAR     2
#define PROC    3


//added 9 to the token_type enum(???)
typedef enum token_type{
    skipsym = 1, identsym, numbersym, plussym, minussym, 
    multsym,  slashsym, oddsym, eqlsym, neqsym, lessym, leqsym, 
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym, 
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,  
    whilesym, dosym, callsym9, constsym, varsym, procsym9, writesym, 
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

    int kind;       //ex: const = 1, varsym = 29
    char name[12];  //name like str is variables name
    int val;        //literal value of a number
    int level;      //lexographical level
    int addr;       //M address
    int mark;       //marks when the symbol has been used

}symbol;



/* list of reserved keyword names */
const char  *word [norw] = { "const", "var", "call", "begin", "end", "if", "else", "while", "do", "read", "write", "odd"}; 
/* list of ignored symbols */
const char ignoresym [ignoresymlen] = { '\n', '\0', ' ', '\t', '\f', '\r', '\v'};                         
/* list of special symbols such as arithmetic*/
const char ssym[ssymlen] = {'*', ')', '.', '>', ';', '-', '(', ',', '<', '%', '+', '/', '=', '#', '$', ':', '!'};
/*master list of all symbols or keywords, matching index with token_type enum*/
const char *sym[symlen] = {"", "", "", "", "+", "-", "*", "/", "odd", "=", "!=", "<", "<=", ">", ">=", "(", ")", ",", ";", ".", 
    ":=", "begin", "end", "if", "then", "while", "do", "call", "const", "var", "procedure", "write", "read", "else"};


//Error signals
typedef enum errors{

    //Scanner Errors
    END_OF_COMMENT_ERR = -20,
    NAME_TOO_LONG_ERR,
    INT_TOO_LONG_ERR, 
    INVALID_INT_ERR,
    INVALID_SYM_ERR,

    //Parser Errors
    ARITHMETIC_ERR = -15,
    INCOMPLETE_PARENTHEIS_ERR,
    NEEDS_COMPARE_ERR,
    DO_MISSING_ERR,
    THEN_MISSING_ERR,
    END_MISSING_ERR,
    ASSGN_MISSING_ERR,
    ILLEGAL_CONST_CHANGE_ERR,
    UNDECLARED_IDENT_ERR,
    SEMICOLON_MISSING_ERR, 
    CONST_NOT_INT_ERR, 
    CONST_NEEDS_EQ_ERR, 
    IDENT_ALR_DECLARED_ERR, 
    IDENTIFIER_EXPECTED_ERR, 
    MISSING_PERIOD_ERR, 

    //General errors
    NOT_FOUND = -1/*Error if there is no variable after Var declaration*/
}errors;
const char *err_messages[] =  {
                            "Error: program must end with period",
                            "Error: const, var, and read keywords must be followed by identifier",
                            "Error: symbol name has already been declared",
                            "Error: constants must be assigned with =",
                            "Error: constants must be assigned an integer value",
                            "Error: constant and variable declarations must be followed by a semicolon",
                            "Error: undeclared identifier",
                            "Error: only variable values may be altered",
                            "Error: assignment statements must use :=",
                            "Error: begin must be followed by end",
                            "Error: if must be followed by then",
                            "Error: while must be followed by do",
                            "Error: condition must contain comparison operator",
                            "Error: right parenthesis must follow left parenthesis",
                            "Error: arithmetic equations must contain operands, parentheses, numbers, or symbols"
                        };


FILE *in;
FILE *out;

//parser global variables

symbol table[MAX_SYMBOL_TABLE_SIZE];
lexeme *tokens;
instruction code[MAX_CODE_SIZE];
int cx; //working code arr index
int tokensIndex;//working index of tokens array
int tableworkingIndex;//working index of symbol table
int tableSize;
int token;
int LexLevel;


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
int symboltablecheck(char *target);
int var_declaration();
symbol initSymObj(int kind, char *name, int val, int level, int addr);
void printTable(symbol table[], int tableSize);
lexeme* readTokens();
void emitError(int errorSignal, char *invalidIdent);
int isStartStatement();
void program();
void statement();
void term();
void factor();
void block();
void const_declaration();
int symboltablecheck(char *target);
void expression();
void condition();
void emit(int op, int L, int M);
void printInstructions();
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

    in = fopen(tokenFileName, "r"); //possibly may need to change this for submission
    // out = fopen("HW1/input.txt", "w");

    if(in == NULL){
        printf(RED "Fatal Error: the expected tokens.txt file not found:\n" RESET);
        printf("the program should be reading the tokens.txt file but tried to open " RED "%s" RESET " instead.\nIf the names match, the file might have never been created.", tokenFileName);
        return EXIT_FAILURE;
    }


    //now what...
    LexLevel = 0;
    tableSize = 0; //symbol table size
    tableworkingIndex = 0;
    cx = 0;
    // tokens = readTokens();

    int numVars;
    table[tableworkingIndex++] = initSymObj(numbersym, "main", 0, LexLevel, 3);
    tableSize++;
    
    emit(JMP, 0, 3);

    program(); //literally starts reading program

    printTable(table, MAX_SYMBOL_TABLE_SIZE);
    printInstructions();
    
    


    //clean up
    for (size_t index = 0; index < lex_size; index++)
    {
        // free(lex_list[index].token_name);
    }
    // free(lex_list);
    // free(charArr);
    // free(tokens);
    fclose(in);
    // fclose(out);

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
    for (size_t i = 0; i < ignoresymlen; i++)
    {
        if(c == ignoresym[i])
            return i + 1;
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
            exit(EXIT_FAILURE); //exit program since we have no memory
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
            if( (arr[index] == ':' && arr[index+1] == '=') || (arr[index] == '!' && arr[index+1] == '=') || 
                (arr[index] == '<' && arr[index+1] == '=') || (arr[index] == '>' && arr[index+1] == '=')){

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


/*
   Organize word chunks into proper lexeme category
    returns token number
    returns 0 if empty token
*/
int tokenize(char *chunk){


    if (chunk == NULL || shouldBeIgnored(chunk[0])){
        return 0;
    }

    // printf("tokenize chunk %s\n", chunk);
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

        printf("|%d ", list[i].token_type);
        fprintf(out, "%d ", list[i].token_type);
        if (list[i].token_type == identsym || list[i].token_type == numbersym){
            printf("%s ", list[i].token_name);
            fprintf(out, "%s ", list[i].token_name);
        }
    }
    

}

//________________________________END LEXER FUNCS_______________________//
/*                               START PARSER FUNCS                    */

//searches thru symbol table for a target name, returns index if found, NOT_FOUND if not
int symboltablecheck(char *target){

    //also check that name isn't empty to avoid seg fault and to save time
    for (size_t index = 0; index < MAX_SYMBOL_TABLE_SIZE && table[index].name[0] != '\0'; index++)
    {
        // printf("\tI: %ld\n", index);
        if(strcmp(table[index].name, target) == 0){
            return index;
        }
    }
    // printf("not found\n");
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

    return s;

}

//TODO: Make the table print prettier, and align the tabs
void printTable(symbol table[], int tableSize){

    printf("table size %d\n", tableSize);
    printf("Kind | Name       | Value | Level | Address | Mark\n");
    printf("---------------------------------------------------\n");

    for (size_t i = 0; (i < tableSize) && (table[i].name[0] != '\0'); i++)
    {
        printf("   %d |\t\t%s |\t%d |\t%d |\t%d   |\t%d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);
    }
    
}


lexeme* readTokens(){

    lexeme *temp = NULL;

    int token_holder;

    for(int i = 0; fscanf(in, "%d", &token_holder) > 0; i++){

        temp = realloc(temp , (i+1) * sizeof(lexeme));

        // printf("tokenholder %d\n", token_holder);
        if(token_holder == numbersym || token_holder == identsym){

            char buffer[cmax+1];
            fscanf(in, "%s", buffer);
            int len = strlen(buffer);

            temp[i].token_name = (char*)malloc((len+1) * sizeof(char));
            strcpy(temp[i].token_name, buffer);

        }

        temp[i].token_type = token_holder;
        
    }

}


//Print error message to console corresponding to error signal passed in. pass "\0" into invalidIdent if not an undeclared_ident error
void emitError(int errorSignal, char *invalidIdent){

    char error_message[strmax] = "";
    errorSignal = abs(errorSignal) - 1; //get index of err message w offset
    strcat(error_message, err_messages[errorSignal]);
    
    if(invalidIdent[0] != '\0'){
        strcat(error_message, " "); //add space
        strcat(error_message, invalidIdent); //insert the wrong variable
        printf("%s\n", error_message);
    }

    else{
        printf("%s\n", error_message);
    }    
    
    printTable(table, tableSize);
    _Exit(EXIT_SUCCESS);

}

int isStartStatement(){

    if (token == identsym || token == beginsym || token == ifsym || token == whilesym || token == readsym || token == writesym){
        return 1;
    }

    return 0;

}

void program(){
    printf("starting program\n");
    

    block();
    printf("after block in program\n");
    printf("%d\n", token);
    if(token != periodsym){
        emitError(MISSING_PERIOD_ERR, "\0");
    }
    //emit halt
    printf("HALT\n");
    emit(SYS, 0, EOP);//HALT
}

void block(){

    printf("in block\n");

    if (fscanf(in, "%d", &token) <= 0){ //get first token
        printf("Error: token list is empty\n");
        _Exit(EXIT_SUCCESS);
    }
    // fscanf(in, "%d", &token):
    int numVars = 0;
    while (token == constsym || token == varsym) {
        const_declaration();
        
        numVars = var_declaration();
    } 

    if( numVars < 0 ){  //all error signals are negative numbers
        
        int error = numVars;
        emitError(error, "\0");
    
    }

    //emit INC(M= 3 + numVars);
    printf("emitting INC numVars: %d\n", numVars);
    emit(INC, LexLevel, numVars + 3);
    
    statement();
    
    printf("after statement call in block\n");

}

void const_declaration(){

    if(token == constsym){
        printf("in const is const\n");
        do
        {
            
            fscanf(in, "%d", &token);
            if (token != identsym){
                emitError(ILLEGAL_CONST_CHANGE_ERR, "\0");  // i think this error needs to be changed
            }
            
            char identSymStr[cmax];
            fscanf(in, "%s", identSymStr);
            if (symboltablecheck(identSymStr) != NOT_FOUND){
                emitError(IDENT_ALR_DECLARED_ERR, '\0');
                
            }

            
            //save ident name 
            char identName[cmax];
            strcpy(identName, identSymStr);
        
            fscanf(in, "%d", &token);
            if (token != eqlsym) {
                emitError(CONST_NEEDS_EQ_ERR, "\0");
            }
            // get next token 
            fscanf(in, "%d", &token);
            if(token != numbersym){
                emitError(CONST_NOT_INT_ERR, "\0");
            }
            
            int actualNumber;
            fscanf(in ,"%d", &actualNumber);
            // add to symbol table (kind 1, saved name, number, 0, 0) 
            symbol newSym = initSymObj(1, identName, actualNumber, 0, 0);
            table[tableworkingIndex] = newSym;
            tableworkingIndex++;
            tableSize++;

            // get next token
            fscanf(in, "%d", &token); //hoping for a comma
          

        } while (token == commasym);
        
        if(token != semicolonsym){
            emitError(SEMICOLON_MISSING_ERR, "\0");
        }

        fscanf(in, "%d", &token);

    }

}


//    Function that for when a "var" is about to be declared
//    returns number of vars
//    Can emit: IDENTIFIER_EXPECTED_ERR, IDENT_ALR_DECLARED_ERR, SEMICOLON_MISSING_ERR
int var_declaration(){
    printf("in var declaration\n");
    int numVars = 0;

    if(token == varsym){
        do
        {   
            fscanf(in, "%d", &token);
            if( token != identsym){
                return IDENTIFIER_EXPECTED_ERR;
            }
        
            char name[12];
            if(fscanf(in, "%s", name) <= 0){
                return IDENTIFIER_EXPECTED_ERR;
            }
            
            // printTable(table, tableworkingIndex);
            if(symboltablecheck(name) != NOT_FOUND){
                return IDENT_ALR_DECLARED_ERR;
            }

            numVars++;
            
            //Add to symbol table
            symbol newSym = initSymObj(2, name, 0, 0, numVars + 2);
            table[tableworkingIndex] = newSym;
            tableworkingIndex++;
            tableSize++;


            //get next token and hope its a comma
            
            fscanf(in, "%d", &token);
                
        }while (token == commasym);
        
        if (token != semicolonsym){
            return SEMICOLON_MISSING_ERR;
        }
        fscanf(in, "%d", &token);
    }

    return numVars;
}

//still need if, while, read, write
void statement(){

    printf("%d in statement\n", token);

    if(token == identsym){

        printf("identsym in statement\n");
        char identName[cmax];
        fscanf(in, "%s", identName);
        int symIdx = symboltablecheck(identName);
        if (symIdx == NOT_FOUND){
            emitError(UNDECLARED_IDENT_ERR, identName);
        }

        if (table[symIdx].kind != 2){
            emitError(ILLEGAL_CONST_CHANGE_ERR, "\0");
        }

        fscanf(in, "%d", &token); //expecting := sym
        if(token != becomessym){ //expecting to assign a variable
            //it might also be the arithmetic error
            emitError(ASSGN_MISSING_ERR, "\0");
        }

        fscanf(in, "%d", &token);
        tableworkingIndex = symIdx;
        expression();

        //emit STO (M = table[symIdx].addr)
        printf("emitting STO\n");
        emit(STO, LexLevel, table[symIdx].addr);
        printf("token is after STO %d\n", token);
        return;

    }

    if(token == beginsym){
        printf("begin in statement\n");
        do
        {

            if (fscanf(in, "%d", &token) <= 0){ //makes sure we dont get stuck in recursive hell if theres nothing after
                break;
            }

            printf("out of statement BEFORE recurse token is %d\n", token);
            statement(token);

            printf("out of statement after recurse token is %d\n", token);
        } while (token == semicolonsym);
        
        //if statements need a semicolon check for it here and do an error mess


        if(token != endsym){    //make sure end is followed by beginning
            emitError(END_MISSING_ERR, "\0");
        }
         
        fscanf(in, "%d", &token);
        return;
    }

    //if statement
    if(token == ifsym){

        fscanf(in, "%d", &token);

        condition();

        int jpcIndex = cx; //IDK what this means yet

        printf("emit JPC");
        emit(JPC, LexLevel, 0);

        if(token != thensym){
            emitError(THEN_MISSING_ERR, "\0");
        }

        fscanf(in, "%d", &token);
        statement();
        code[jpcIndex].M = cx; //I think this has something to do with making a new instructions struct

        return;
    }

    if(token == whilesym){

        fscanf(in, "%d", &token);
        int loopIndex = cx; //idk what this means yet

        condition();

        if (token != dosym){
            emitError(DO_MISSING_ERR, "\0");
        }

        fscanf(in, "%d", &token);
        int jpcIndex = cx;
        printf("emit JPC\n"); //M = loopIndex
        emit(JPC, LexLevel, 0);

        statement();

        emit(JMP, LexLevel, loopIndex);
        code[jpcIndex].M = cx;
        return;

    }

    if(token == readsym){

        fscanf(in, "%d", &token);

        if(token != identsym){
            emitError(ILLEGAL_CONST_CHANGE_ERR, "\0");
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
        
        printf("emit READ\n");
        emit(SYS, LexLevel, 2); //READ

        printf("emit STO\n"); //M = table[symIndex].addr
        emit(STO, LexLevel, table[symIndex].addr);

        return;

    }

    if(token == writesym){
        fscanf(in, "%d", &token);
        expression();
        printf("emit WRITE\n");
        emit(SYS, LexLevel, SOU); //WRITE

        return;
    }

    // printf("\nThis is a statement.\n\n");

}

void condition(){

    if(token == oddsym){
        fscanf(in, "%d", &token);
        expression();
        printf("emit ODD\n");
        emit(ODD, LexLevel, 0);
    }

    else{

        expression();
        if(token == eqlsym){
            fscanf(in, "%d", &token);
            expression();
            printf("emit EQL\n");
            emit(OPR, LexLevel, EQL);
        }
        else if(token == neqsym){
            fscanf(in, "%d", &token);
            expression();
            printf("emit NEQ\n");
            emit(OPR, LexLevel, NEQ);
        }
        else if(token == lessym){
            fscanf(in, "%d", &token);
            expression();
            printf("emit LSS\n");
            emit(OPR, LexLevel, LSS);
        }
        else if(token == leqsym){
            fscanf(in, "%d", &token);
            expression();
            printf("emit LEQ\n");
            emit(OPR, LexLevel, LEQ);
        }
        else if(token == gtrsym){
            fscanf(in, "%d", &token);
            expression();
            printf("emit GTR\n");
            emit(OPR, LexLevel, GTR);
        }
        else if(token == geqsym){
            fscanf(in, "%d", &token);
            expression();
            printf("emit GEQ\n");
            emit(OPR, LexLevel, GEQ);
        }
        else{
            emitError(NEEDS_COMPARE_ERR, "\0");
        }

    }

}

void expression(){

    printf("%d in expression\n", token);

    if (token == minussym){
        printf("token in exp is minus sym\n");
        
        fscanf(in, "%d", &token);
        term();

        //emit neg ?
        printf("Emit neg\n");
        emit(OPR, LexLevel, SUB);

        while (token == plussym || token == minussym){
            fscanf(in, "%d", &token);
            printf("token is plus or minus\n");

            if (token == plussym){
                fscanf(in, "%d", &token);
                term();
                //emit add
                printf("token in expression is add\n");
                emit(OPR, LexLevel, ADD);
            }
            else{
                fscanf(in, "%d", &token);
                term();
                //emit sub
                printf("token in expression is sub\n");
                emit(OPR, LexLevel, SUB);
            }    
        }

    }

    else{
        if(token == plussym){
            fscanf(in, "%d", &token);
            printf("token is plus\n");
        }

        term();

        while (token == plussym || token == minussym){
            fscanf(in, "%d", &token);
            printf("token is plus or minus\n");

            if (token == plussym){
                fscanf(in, "%d", &token);
                term();
                //emit add
                printf("token in expression is add\n");
                emit(OPR, LexLevel, SUB);
            }
            else{
                fscanf(in, "%d", &token);
                term();
                //emit sub
                printf("token in expression is sub\n");
                emit(OPR, LexLevel, SUB);
            }    
        }

    }

}

void term(){

    printf("in term\n");
    factor();

    while(token == multsym || token == slashsym){

        if(token == multsym){

            fscanf(in, "%d", &token);
            factor();
            //emit MUL
            emit(OPR, LexLevel, MUL);

        }

        else if(token == slashsym){

            fscanf(in, "%d", &token);
            factor();
            //emit DIV 
            printf("emit DIV in term\n");
            emit(OPR, LexLevel, DIV);

        }

    }

}

//still need to figure out error
void factor(){
    printf("in factor\n");
    if (token == identsym){

        char identifierStr[cmax];
        fscanf(in, "%s", identifierStr);
        int symIdx = symboltablecheck(identifierStr);
        if(symIdx == NOT_FOUND){
            emitError(UNDECLARED_IDENT_ERR, identifierStr);
        }

        if(table[symIdx].kind == CONST){
            //emit LIT(m = table[sym.idx].addr)
            printf("Emit LIT in factor\n");
            emit(LIT, LexLevel, table[symIdx].addr);
        }
        else if(table[symIdx].kind == VAR){
            //emit LOD (M = table[symIdx].addr)
            printf("Emit LOD in factor\n");
            emit(LOD, LexLevel, table[symIdx].addr);
        }
        fscanf(in, "%d", &token);
    }

    else if(token == numbersym){
        int actualNumber;
        fscanf(in, "%d", &actualNumber); //get the actual number after numbersym

        table[tableworkingIndex].val = actualNumber;

        //emit LIT
        printf("Emitting numbersym LIT %d from factor\n", actualNumber);
        emit(LIT, LexLevel, actualNumber);

        fscanf(in, "%d", &token);
    }

    else if(token == lparentsym){
        fscanf(in, "%d", &token);
        expression();
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

void printInstructions(){

    const char *op_code_names[] = { "LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP", "JPC", "SYS" };


    for (size_t i = 0; i < cx; i++)
    {
        char op_name[4];
        strcpy(op_name, op_code_names[code[i].op - 1]); //translate op number into name from above arr
        
        printf("%ld %s %d %d\n", i, op_name, code[i].L, code[i].M);
        // fprintf(out, "%d %d %d\n", code[i].op, code[i].L, code[i].M); //write op codes to file for VM to run
    
    }
    

}