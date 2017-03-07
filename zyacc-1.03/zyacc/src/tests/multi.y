/* Test program for multiple start non-terminals. 
 * 3 separate grammars for infix, postfix and suffix arith expressions.
 * No unary minus.  No error checking.
 */

%{
#include <stdio.h>

int yylex();
void yyerror(char *s);

%}
/* !gramBegin! */
%start infix, prefix, suffix

%%
infix
  : iExp '\n' %look(0) { return $1; }
  ;
iExp
  : iExp '+' iTerm { $$= $1 + $3; }
  | iTerm
  ;
iTerm
  : iTerm '*' iFactor { $$= $1 * $3; }
  | iFactor
  ;
iFactor
  : digit 
  | '(' iExp ')' { $$= $2; }
  ;
digit
  : '0'	{ $$= 0; }
  | '1'	{ $$= 1; }
  | '2'	{ $$= 2; }
  | '3'	{ $$= 3; }
  | '4'	{ $$= 4; }
  | '5'	{ $$= 5; }
  | '6'	{ $$= 6; }
  | '7'	{ $$= 7; }
  | '8'	{ $$= 8; }
  | '9'	{ $$= 9; }
  ;

prefix
  : pExp '\n' %look(0) { return $1; }
  ;
pExp
  : '+' pExp pExp { $$= $2 + $3; }
  | '*' pExp pExp { $$= $2 * $3; }
  | digit
  ;

suffix
  : sExp '\n' %look(0) { return $1; }
  ;
sExp
  : sExp sExp '+' { $$= $1 + $2; }
  | sExp sExp '*' { $$= $1 * $2; }
  | digit
  ;
/* !gramEnd! */
%%
int yylex()
{
  int c= getchar();
  while (c == ' ' && c == '\t') c= getchar();
  if (c == EOF) c= 0;
  return c;
}

void yyerror(char *s) 
{
  printf(s);
}

/* !mainBegin! */
int main() 
/* Call infix grammar if line starts with 'i'; suffix grammar if line
 * starts with a 's'; prefix grammar if line starts with a 'p'.
 */
{
  int c;
  while ((c= getchar()) != EOF) {
    int z= yyparse(c == 'i' ? infix : (c == 's' ? suffix : prefix));
    printf("%d\n", z);
  }
  return 0;
}
/* !mainEnd! */
