%option --yylineno
%%
.+	printf("%03d: %s\n", yylineno, yytext);
\n
