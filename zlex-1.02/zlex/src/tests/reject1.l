  /* Test for REJECT. */
%%
a    |
ab   |
abc  |
abcd ECHO; fputs("\n", yyout); REJECT;
b    |
bc   |
bcd  ECHO; fputs("\n", yyout); REJECT;
c    |
cd   ECHO; fputs("\n", yyout); REJECT;
.    ECHO; fputs("\n", yyout);



