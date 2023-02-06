/*
    pm/0 Scanner
    COP3402 Euripedes Montagne
    Romsev Charles
    Brandon Sheridan
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
    nulsym = 1, idsym, numbersym, plussym, minussym,
    multsym,  slashsym, oodsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym, 
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym
}token_type;

/* list of reserved word names */
char  *word [ ] = {  "null", "begin", "call", "const", "do", "else", "end", "if", "odd", "procedure", "read", "then", "var", "while", "write"}; 
                         
/* internal representation  of reserved words */
// int  wsym [ ] =  { nulsym, beginsym, callsym, constsym, dosym, elsesym, endsym, ifsym, oddsym, procsym, readsym, thensym, varsym, whilesym, writesym};


/* list of special symbols */
int ssym[256];
// ssym['+']=plus; ssym['-']=minus; ssym['*']=mult; 
// ssym['/']=slash;  ssym['(']=lparen;  ssym[')']=rparen; 
// ssym['=']=eql;      ssym[',']=comma;  ssym['.']=period; 
// ssym['#']=neq;  ssym['<']=lss;  ssym['>']=gtr; 
// ssym['$']=leq;  ssym['%']=geq;  ssym[';']=semicolon;

FILE *f;

int main(int argc, char const *argv[])
{

    f = fopen(argv[1], "r");





    fclose(f);

    return 0;
}
