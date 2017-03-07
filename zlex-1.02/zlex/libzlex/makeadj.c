/*

File:	 makeadj.c
Purpose: Routine to make yytext adjacent to cp.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"


/* Make yytext adjacent to just before cp. */
YYChar *
yyMakeAdjText(dP)
  YYData *dP;
{
  YYBufP bP= dP->bufP;
  int yyLeng= *dP->yylengP;
  if (bP->cp - yyLeng < bP->buf) {
    bP->cp= yyShiftBuffer(dP, yyLeng);
  }
  return yyMoveText(dP, bP->cp - yyLeng);
}

