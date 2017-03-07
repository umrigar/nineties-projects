%option --yylineno
%%
[[:alnum:]]+	printf("%s@(%d, %d)\n", yytext, yylineno, YY_COL_NUM);
[[:space:]]+
