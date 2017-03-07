/*

File:	 less.c
Purpose: Peform action required for yyless().

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
YY_VOID 
yyLess(d, n)
  YYDataHandle d;
  int n;
{
  YYData *YY_CONST dP= (YYData *)d;
  YY_CONST int yyLeng= *dP->yylengP;
  YYChar *yyText= *dP->yytextP;
  YY_CONST unsigned nChars= yyLeng - n;	/* # of chars. to be rescanned. */
  YYBuf *bP= dP->bufP;
  assert(0 <= n && n < yyLeng);
  if (yyText + yyLeng != bP->cp) { /* Must have done input or buf switch. */
    yyText= yyMakeAdjText(dP);
  }
  bP->cp-= nChars; 
  *dP->saveCP= dP->saveCh; 
  dP->saveCP= bP->cp; dP->saveCh= *bP->cp; *bP->cp= '\0';
  *dP->yylengP= n;
  if (dP->options & YY_ARRAY_FLAG) dP->textArray[n]= '\0';
  VOID_RET();
}

