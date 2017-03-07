/* Test for %tests.  Resolve operator priorities using %tests. */

%token <val>(int $v) DIGIT

%{
#include <ctype.h>
#include <stdio.h>

static unsigned char pri[128];	
void yyerror();
int yylex();

enum { ADD_P= 1, MULT_P, UMINUS_P };

%}

%%
Lines
  : Lines Exp($v) '\n' { printf("%d\n", $v); }
  | error '\n'
  | /* empty */
  ;
/* !opBegin! */
Exp(int $v)
  : Exp($v1) '+' Exp($v2) %test(pri['+'] >= pri[yychar])
      { $v= $v1 + $v2; }
  | Exp($v1) '-' Exp($v2) %test(pri['-'] >= pri[yychar])
      { $v= $v1 - $v2; }
  | Exp($v1) '*' Exp($v2) %test(pri['*'] >= pri[yychar])
      { $v= $v1 * $v2; }
  | Exp($v1) '/' Exp($v2) %test(pri['/'] >= pri[yychar])
      { $v= $v1 / $v2; }
  | '-' Exp($v1) %test(UMINUS_P >= pri[yychar])
      { $v= -$v1; }
  | '+' Exp($v1) %test(UMINUS_P >= pri[yychar])
      { $v= $v1; }
/* !opEnd! */
  | DIGIT($v)
  | '(' Exp($v) ')'
  ;

%%

int yylex() {
  int c= getchar();
  while (isspace(c) && c != '\n') c= getchar();
  if (c == EOF) return 0;
  else if isdigit(c) { yylval.val.v= c - '0'; return DIGIT; }
  else return c;
}

void yyerror(const char *s) {
  printf("%s\n", s);
}

int main() {
  pri['+']= pri['-']= ADD_P;
  pri['*']= pri['/']= MULT_P;
  return yyparse();
}
