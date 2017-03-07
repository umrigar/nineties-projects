/* Infix notation calculator--calc */

%{
#include <math.h>
#include <stdio.h>

#define YYSTYPE double

int yylex(void);
void yyerror(const char *errMsg);
%}

/* zyacc declarations */
%token NUM_TOK
%left '-' '+'
%left '*' '/'
%left NEG	/* negation--unary minus */
%right '^'	/* exponentiation   */

/* Grammar follows */
%%
input
  : /* empty */
  | input line
  ;

line
  : '\n'
  | exp '\n'		{ printf ("\t%.10g\n", $1); }
  ;

exp
  : NUM_TOK
  | exp '+' exp   	{ $$= $1 + $3; }
  | exp '-' exp   	{ $$= $1 - $3; }
  | exp '*' exp   	{ $$= $1 * $3; }
  | exp '/' exp   	{ $$= $1 / $3; }
  | '-' exp  %prec NEG 	{ $$= -$2; }
  | exp '^' exp   	{ $$= pow ($1, $3); }
  | '(' exp ')'   	{ $$= $2; }
  ;
%%
/* !done! */
/* Lexical analyzer returns a double floating point
 * number in yylval and the token NUM_TOK, or the ASCII
 * character read if not a number.  Skips all blanks
 * and tabs, returns 0 for EOF. 
 */

#include <ctype.h>

int
yylex(void)
{
  int c;

  /* skip white space  */
  while ((c = getchar ()) == ' ' || c == '\t')
    ;
  /* process numbers   */
  if (c == '.' || isdigit (c)) {
     ungetc(c, stdin);
     scanf("%lf", &yylval);
     return NUM_TOK;
  }
  /* return end-of-file  */
  if (c == EOF)
    return 0;
  /* return single chars */
  return c;
}

#include <stdio.h>

/* Called by yyparse on error */
void
yyerror(const char *s)  
{
  printf("%s\n", s);
}

int 
main()
{
  return yyparse();
}
