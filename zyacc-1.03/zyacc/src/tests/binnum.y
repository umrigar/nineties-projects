/* Attribute expression for binary numbers using inherited attributes. */
%{

#include <stdio.h>

void yyerror();
int yylex();

%}
%%
Line
  : Line BNum($v) '\n' { printf("%.4f\n", $v); }
  | error '\n'
  | /* empty */
  ;
BNum(double $v)
  : IntPart(0, $i) '.' FracPart(0.5, 0, $f)
	{ $v= $i + $f; }
  ;	      
IntPart(%in int $acc, int $v)
  : Bit($b) IntPart(2 * $acc + $b, $v)
  | /* empty */ { $v= $acc; }
  ;
FracPart(%in double $wt, %in double $acc, double $v)
  : Bit($b) FracPart($wt/2, $acc + $wt*$b, $v)
  | /* empty */ { $v= $acc; }
  ;
Bit(int $b)
  : '0' { $b= 0; }
  | '1' { $b= 1; }
  ;
%%

void yyerror(const char *s) {
  printf("%s\n", s);
}

int yylex()
{
  int c= getchar();
  return (c == EOF) ? 0 : c;
}

int main()
{
  return yyparse();
}
