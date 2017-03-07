/* Echo which ensures that sentinel char (NUL) handled ok. */
%%
[^\n\0]+\0 fputs("(", yyout); ECHO; fputs(")", yyout);
\0\0+      fputs("[", yyout); ECHO; fputs("]", yyout);

