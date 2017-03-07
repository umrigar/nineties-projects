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

static FILE *in;

void yyerror(const char *s) {
  printf("%s\n", s);
}

int yylex(void) {
  int c; 
  do { c= getc(in); } while (c == ' ' || c == '\t');
  if (c == EOF) c= 0;
  return c;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: debugtst FNAME\n");
    exit(1);
  }
  if ((in= fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "could not open input file %s\n", argv[1]);
    exit(1);
  }
  return yyparse();
}

