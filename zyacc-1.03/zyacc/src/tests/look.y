/* Test for %look and %nolook directives. */

%%
S : A B M I J N
  ;
A : X %look(0)
  ;
X : Y
  ;
Y : Z
  ;
Z : 'z'
  ;
B : C %look(1)
  ;
C : 'c'
  | C 'c'
  ;
M : 'm' %look(0)
  ;
I : P %look(0)
  ;
P : Q
  ;
Q : R
  ;
R : 'r'
  | /* empty */
  ;
J : S %look(1)
  ;
S : 's'
  ;
N : 'n'
  | /* empty */ %look(0)
  ;
