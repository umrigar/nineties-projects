/* Test for yyloc.  Grammar accepts parenthesized lists, printing out
 * the beginning and end coordinates of each complete list.
 */

%{

#include <stdio.h>

static void yyerror();
int yylex();


%}
/* !begin! */
%%
s
  : s list
        { printf("(%d, %d) -- (%d, %d)\n", @2.first_line, @2.first_column,
		 @2.last_line, @2.last_column);
	}
  | /* empty */
  | error
  ;
list
  : '(' listSeq ')'
  | '(' ')'
  ;
listSeq
  : listSeq list
  | list
  ;

%%

static void yyerror(s)
  const char *s;
{
  printf("%s\n", s);
}

static int lineN= 1;
static int colN= 0;

int 
yylex() {
  int c;
  while (isspace(c= getchar())) { 
    if (c == '\n') { 
      lineN++; colN= 0;
    }
    else {
      colN++;
    }
  }
  if (c == EOF) return 0;
  yylloc.first_line= yylloc.last_line= lineN;
  yylloc.first_column= colN; yylloc.last_column= ++colN;
  return c;
}



int 
main() {
  return yyparse();
}

