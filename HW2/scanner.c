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

#define  norw      15         /* number of reserved words */
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
char  *word [ ] = {  "null", "begin", "call", "const", "do", "else", "end", "if", "odd", "procedure", "read", "then", "var", "while", "write"}; 
                         
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


void cpytilspace(char bufferArr[], char charArr[]);


int main(int argc, char const *argv[])
{

    f = fopen(argv[1], "r");


    char *charArr = (char *) malloc(1 * sizeof(char));
    int arrSize;
    for (int i = 0; fscanf(f, "%c", &charArr[i]) > 0; ){    //incrementor in statement


        printf("at index %d adding + 1\n",i);

        i++;    //added in statement so it didnt mess up realloc math for some reason
        charArr = realloc(charArr, sizeof(char) * (i + 1));
        if(charArr == NULL){
            free(charArr);
            return -1; //exit program with error
        }
        
        // printf("%c\n", charArr[i - 1]);
        
        arrSize = i;

    }
 
    char *bufferArr = malloc(arrSize * sizeof(char));
    cpytilspace(bufferArr, charArr);

    //clean up
    free(bufferArr);
    free(charArr);
    fclose(f);

    return 0;
}

//copys array until it reaches a "space" or bar "|"
void cpytilspace(char bufferArr[], char charArr[]){

    int i;
    // while()


}
