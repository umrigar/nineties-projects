/* Simple arith. expr. grammar (single-digit #s) with ambiguous 
 * grammar using assoc/prec info. 
 */
%{
#include <stdio.h>

void yyerror(const char *);
int yylex(void);

typedef int YYSTYPE;

%}

%left '+' '-'
%left '*' '/'
%right UMINUS UPLUS

%%
S	
  : S E { printf("%d\n", $2); } '\n'
  | error '\n'
  | /* empty */
        ;
E	
  : E '+' E { $$= $1 + $3; }
  | E '-' E { $$= $1 - $3; }
  | E '*' E { $$= $1 * $3; }
  | E '/' E { $$= $1 / $3; }
  | '+' E %prec UPLUS { $$= $2; }
  | '-' E %prec UMINUS { $$= -$2; }
  | '(' E ')' { $$= $2; }
  | '0' { $$= 0; }
  | '1' { $$= 1; }
  | '2' { $$= 2; }
  | '3' { $$= 3; }
  | '4' { $$= 4; }
  | '5' { $$= 5; }
  | '6' { $$= 6; }
  | '7' { $$= 7; }
  | '8' { $$= 8; }
  | '9' { $$= 9; }
  ;
%%

#include <stdio.h>
void yyerror(const char *s) {
  printf("%s\n", s);
}

int yylex(void) {
  int c; 
  do { c= getchar(); } while (c == ' ' || c == '\t');
  if (c == EOF) c= 0;
  return c;
}

int main() {
  return yyparse();
}

