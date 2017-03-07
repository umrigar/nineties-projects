/* Test for action mapping: intra-token actions + multi-source actions +
 * reject actions.
 */
%%
123	|
abc	|
def 	{ printf("%d: %s MULTI REJECTING\n", __LINE__, yytext); REJECT; }
123	{ printf("%d: %s REJECTING\n", __LINE__, yytext); REJECT; }
abc	{ printf("%d: %s REJECTING\n", __LINE__, yytext); REJECT; }
def	{ printf("%d: %s REJECTING\n", __LINE__, yytext); REJECT; }
12	{ printf("%d: %s REJECTING\n", __LINE__, yytext); REJECT; }
ab	{ printf("%d: %s REJECTING\n", __LINE__, yytext); REJECT; }
de	{ printf("%d: %s REJECTING\n", __LINE__, yytext); REJECT; }
+ABC	{ printf("%d: %s INTRA ERASED\n", __LINE__, yytext); 
          YY_BACKUP(yyleng, ""); 
        }
+DEF	{ printf("%d: %s INTRA ERASED\n", __LINE__, yytext); 
          YY_BACKUP(yyleng, ""); 
        }
+GHI	{ printf("%d: %s INTRA ERASED\n", __LINE__, yytext); 
          YY_BACKUP(yyleng, ""); 
        }
+qabef	{ printf("%d: %s INTRA ERASED\n", __LINE__, yytext); 
          YY_BACKUP(2, "c123"); 
        }
.|\n    { printf("%d: `%s' echoed.\n", __LINE__, yytext); }
