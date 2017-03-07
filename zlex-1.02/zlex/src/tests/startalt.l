%%
^"("  		|
[ \t]+"("	{ printf("BLANK_LPAREN\n"); }
.|\n		{ ECHO; }
