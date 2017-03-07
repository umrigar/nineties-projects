/* Test yywrap(), YY_NEW_FILE and YY_RESTART() by producing two copies of
 * the input file.  We use --stdio for input as Linux doesn't take kindly
 * to rewinding FILEs and then read'ing the corresponding fileno().
 */
%option --stdio
%%
%%
int yywrap() {
  static int nCopies= 0;
  if (nCopies > 0) {
    return 1;
  }
  else {
    nCopies++;
    rewind(yyin);
    YY_NEW_FILE;
    return 0;
  }
}
