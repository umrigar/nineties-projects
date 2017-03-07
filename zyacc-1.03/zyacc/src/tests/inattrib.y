/* Test for input attributes within closure. */
/* Reads a letter c (either 'a', 'A', 'b', 'B', 'c' or 'C') followed
 * by a number n.  Prints out n * code(c) + c, where code('a') == 1,
 * code('A') == 2, ..., code('C') == 6.
 */
%{
#include <ctype.h>
#include <stdio.h>  

int yylex();
void yyerror();

%}
%%
S
  : S 
    { printf("Enter number: "); fflush(stdout); scanf("%d", &$i); }
    E($i, $n) { printf("%d\n", $n); }
  | /* empty */
  ;
E(%in int $m, int $z)
  : A($m, $z)
  | B($m, $z)
  | C($m, $z)
  ;
A(%in int $m, int $z)
  : A1($m, $z1) 'a' { $z= $z1 + 1; }
  | A2($m, $z1) 'A' { $z= $z1 + 2; }
  ;
B(%in int $m, int $z)
  : B1($m, $z1) 'b' { $z= $z1 + 3; }
  | B2($m, $z1) 'B' { $z= $z1 + 4; }
  ;
C(%in int $m, int $z)
  : C1($m, $z1) 'c' { $z= $z1 + 5; }
  | C2($m, $z1) 'C' { $z= $z1 + 6; }
  ;
A1(%in int $m, int $z)
  : { $z= $m*1; }
  ;
A2(%in int $m, int $z)
  : { $z= $m*2; }
  ;
B1(%in int $m, int $z)
  : { $z= $m*3; }
  ;
B2(%in int $m, int $z)
  : { $z= $m*4; }
  ;
C1(%in int $m, int $z)
  : { $z= $m*5; }
  ;
C2(%in int $m, int $z)
  : { $z= $m*6; }
  ;
%%
int yylex() {
  int c;
  while (isspace(c= getchar())) ;
  if (c == EOF) return 0;
  return c;
}

void yyerror(char *s)
{
  printf("%s\n", s);
}

int main()
{
  return yyparse();
}
