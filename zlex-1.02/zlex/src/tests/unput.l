 /* Test for start-states and unput(). */
%s A B C
%%
<INITIAL>^## { BEGIN A; fputs("BEGUN A.\n", yyout); }
<A>^##      { BEGIN B; fputs("BEGUN B.\n", yyout); }
<B>^##      { BEGIN C; fputs("BEGUN C.\n", yyout); }
<C>^##      { BEGIN INITIAL; fputs("BEGUN INITIAL.\n", yyout); }
<A>[0-9]+   { 
  int i;
  unput(')');
  for (i= yyleng - 1;i >= 0; i--) {
    unput(yytext[i]);
  }
  unput('(');
}
<A>\([0-9]+\) ECHO;
<B>[0-9]+   { 
  int i;
  unput(']');
  for (i= yyleng - 1;i >= 0; i--) {
    unput(yytext[i]);
  }
  unput('[');
}
<B>\[[0-9]+\] ECHO;
<C>[0-9]+   { 
  int i;
  unput('}');
  for (i= yyleng - 1;i >= 0; i--) {
    unput(yytext[i]);
  }
  unput('{');
}
<C>\{[0-9]+\} ECHO;
[0-9]+ ECHO;
.
%%
int main() {
  return yylex();
}


