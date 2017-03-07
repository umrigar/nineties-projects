%%
[a-z0-9]+	|
\n		|
[ \t]+		ECHO;
.		printf("Bad char `%c'\n", yytext[0]); YY_FLUSH_BUFFER;
