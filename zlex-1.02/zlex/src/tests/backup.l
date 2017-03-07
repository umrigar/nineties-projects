/* Test for YY_BACKUP action. Echo words, replacing space sequence by a
 * a single space, and removing escaped newlines.
 */
%%
+\\\n       	{ YY_BACKUP(2, ""); }
\n		|
[^ \t\n]+ 	{ ECHO; }
[ \t]+ 		{ fprintf(yyout, " "); }
