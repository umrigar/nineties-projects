/*

File:	 unput.c
Purpose: Support for unput().

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
/* Store ch in buf. so that it will be next char. to be read. May be
 * necessary to shift yytext and resize buffer.
 */
YY_VOID 
yyUnput(d, chX)
  YYDataHandle d; 
  YYCharX chX; 
{
  enum { SHIFT_SIZE= 128 };
  YYChar ch= (YYChar)chX;
  YYData *YY_CONST dP= (YYData *)d;
  YYBufP bP= dP->bufP;
  YYChar *cp= bP->cp;	/* cp is address to be read after ch. */
  YYChar *yyText= *dP->yytextP;
  YY_CONST int yyLeng= *dP->yylengP;
  YY_CONST int yyLeng1= yyLeng + 1;
  YY_CONST YYBool doMoveText= (cp <= yyText + yyLeng1 && yyText < cp);
  assert(!(yyText <= cp && cp < yyText + yyLeng)); /* cp not within yytext. */
  if (cp == bP->buf) {
    assert(!doMoveText);
    cp= yyShiftBuffer(dP, SHIFT_SIZE);
  }
  else if (doMoveText) {
    *dP->saveCP= dP->saveCh; 
    if (yyText == bP->buf) { 
      YY_CONST yy_size_t shiftSize= (SHIFT_SIZE + yyLeng1);
      cp= yyShiftBuffer(dP, shiftSize);
    }
    *dP->yytextP= yyText= yyMoveBytes(bP->buf, yyText, yyLeng);
    dP->saveCP= &yyText[yyLeng]; /* Not bothered about saving ch. */
    yyText[yyLeng]= '\0';
  }
  cp--;	/* cp points to location where ch is written. */
  assert(cp != &yyText[yyLeng]);
  *cp= ch;
  bP->cp= cp;
  VOID_RET();
}

