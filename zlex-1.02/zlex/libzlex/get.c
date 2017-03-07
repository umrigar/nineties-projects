/*

File:	 get.c
Purpose: Get a character from the current buffered input stream.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
/* Gets next character from buffer. Return -1 on EOF. */
int 
yyGet(d)
  YYDataHandle d;
{ 
  YYData *YY_CONST dP= (YYData *)d;
  YYBufP bP= dP->bufP;
  int c= (bP->cp == dP->saveCP) ? dP->saveCh : *bP->cp;
  bP->cp++;
  if (c == dP->sentinelChar && bP->cp == dP->sentinelP + 1) {
    bP->cp--; yyProcessSentinel(dP, 0); 
    c= (bP->eofState && bP->cp >= bP->end) ? -1 : *bP->cp++;
  }
  if (c == '\n' && (dP->options & YY_LINENO_FLAG) && !bP->hasNL) {
    yy_size_t nlPos=  bP->nRead + (bP->cp - bP->buf);
    if (nlPos > bP->lastLinePos) { /* Not previously counted. */
      bP->hasNL= 1; bP->firstNLPos= nlPos;
    }
  }
  bP->lastCh= c;
  return c;
}


