/* Simple arith. expr. grammar (single-digit #s) with precedence via rules. */
%{
#include <stdio.h>

void yyerror(const char *);
int yylex(void);

typedef int YYSTYPE;

%}

%%
S	
  : S E { printf("%d\n", $2); } '\n'
  | error '\n'
  | /* empty */
  ;
E	
  : E '+' T { $$= $1 + $3; }
  | E '-' T { $$= $1 - $3; }
  | T
  ;
T : T '*' F { $$= $1 * $3; }
  | T '/' F { $$= $1 / $3; }
  | F
  ;
F : '(' E ')' { $$= $2; }
  | '-' F { $$= -$2; }
  | '+' F { $$= $2; }
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

