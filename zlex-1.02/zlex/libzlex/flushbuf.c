/*

File:	 flushbuf.c
Purpose: Flush a buffer.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"

YY_VOID
yyFlushBuffer(d, b)
  YYDataHandle d;
  YYBufHandle b;
{
  YYBufP bP= (YYBufP)b;
  YYData *YY_CONST dP= (YYData *)d;

  bP->nRead+= bP->end - bP->cp;
  bP->end= bP->cp;
  if (bP == *dP->yyCurrentBufP) {
    dP->sentinelP[0]= dP->sentinelSave;
    *dP->bufP= *bP;
    yyUpdateSentinel(dP, 0);
  }
}

