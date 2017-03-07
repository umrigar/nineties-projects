/*

File:	 shiftbuf.c
Purpose: Shift buffer routine.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
/* Shift the current buffer in *dP so as to ensure that there are 
 * nFree unused locations before cp.  Preserves contents of locations
 * in buffer before cp.
 */
YYChar *
yyShiftBuffer(dP, nFree)
  YYData *dP;
  YY_CONST yy_size_t nFree;
{
  YYBufP bP= dP->bufP;
  YYChar *cp= bP->cp;
  YYChar nUsed= bP->end - cp;
  yy_size_t neededSize= nFree + nUsed + 2; /* 2 extra for sentinel. */
  assert(cp - bP->buf < nFree); 	/* Which is why we are shifting. */
  *dP->sentinelP= dP->sentinelSave;
  if (neededSize > bP->size) { /* Grow buffer. */
    if (!(bP->buf= (YYChar *)
	  realloc(bP->buf, (bP->size= neededSize)*sizeof(YYChar)))) {
      yyZlexFatal(*dP->yyerrP, "Could not reallocate buffer.");
    }
  }
  cp= yyMoveBytes(bP->buf + nFree, cp, nUsed);
  bP->end= cp + nUsed;
  bP->cp= cp;
  yyUpdateSentinel(dP, 0);
  return bP->cp;
}

