%%
[a-zA-Z_]+	
{ putchar(YY_GET()); putchar(YY_GET());
  YY_UNPUT('1'); YY_UNPUT('1'); YY_UNPUT('9'); 
  YY_ECHO; 
  if (YY_LENG > 3) YY_LESS(3); else YY_MORE; 
}
			     
