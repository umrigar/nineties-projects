/* Test for YY_MORE and YY_LESS(n). */

%%
[a-zA-Z_]+	
{ YY_ECHO; fputs(" ", yyout); 
  if (yyleng > 3) YY_LESS(yyleng - 3); else YY_MORE; 
}
[0-9]+		YY_ECHO;
