%{
#define YY_USER_ACTION		\
  printf("(%d)%s matched act # %d.\n", yyleng, yytext, yy_act);
%}
%%
[a-z]+	 REJECT;
.|\n



