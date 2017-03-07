/* Test for YY_TERMINATE. */
%%
^#	YY_TERMINATE();

%%
int main() {
  int i;
  for (i= 0; i < 5; i++) {
   /* Bang away at yylex() a few times.  All returned values should be 0. */
   printf("%d\n", yylex());
  }
  return 0;
}
