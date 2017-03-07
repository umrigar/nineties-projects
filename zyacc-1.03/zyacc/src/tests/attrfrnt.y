/* Test for attribute front-end. */

%token <type>(%out int $a, %out float $b) XX 
%token <type>(%in int $a) YY

%%

A(%in int $a): S($x +2, $a+3, $y);

S(int $a, %in float $b, int $c)
  : XX($a, $b) 
    { $d $a } 
    YY($d)
  ;

