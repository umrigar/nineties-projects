%{
#include <stdio.h>
#include <stddef.h>

typedef struct LineStruct {
  struct LineStruct *last;
  char *text;
} LineStruct;

static LineStruct *pushLine(LineStruct *lines, const char *text, int textLen);

%}
%%
  /* Declare local variables. */
  int wc= 0;
  LineStruct *lines= NULL;
[ \t]+		yymore();
[^ \t\n]+	wc++; yymore();
\n              lines= pushLine(lines, yytext, yyleng);
<<EOF>>         { LineStruct *p;
                  for (p= lines; p; p= p->last) fputs(p->text, stdout);
		  printf("# of words= %d\n", wc);
                }
%%
static LineStruct *
pushLine(LineStruct *lines, const char *text, int textLen)
{
  char *const savedText= malloc(textLen + 1);
  LineStruct *const lineP= malloc(sizeof(LineStruct));
  if (!savedText || !lineP) {
    fprintf(stderr, "Out of memory.\n"); exit(1);
  }
  strcpy(savedText, text);
  lineP->text= savedText; lineP->last= lines;
  return lineP;
}
