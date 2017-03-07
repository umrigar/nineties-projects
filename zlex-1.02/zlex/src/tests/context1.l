  /* Test for right-context. */
%option --back=1

digit [0-9]
alpha [a-zA-Z]
%%
{digit}+$  fprintf(yyout, " %s [RULE 1] ", yytext);
{alpha}+/{digit}+ fprintf(yyout, " %s [RULE 2] ", yytext);
{alpha}+/#! fprintf(yyout, " %s [RULE 3] ", yytext);
{digit}{digit}/{alpha}+ fprintf(yyout, " %s [RULE 4] ", yytext);
{alpha}({alpha}|{digit})*/{digit}+ fprintf(yyout, " %s [RULE 5] ", yytext);
