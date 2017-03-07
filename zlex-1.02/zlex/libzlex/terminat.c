/*

File:	 terminat.c
Purpose: Scanner termination routine.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
YY_VOID
yyTerminate(dP)
  YYData *dP;
{
  YYBufP bP= dP->bufP;
  assert(bP != NULL);
  *dP->yylengP= 0;
  *dP->yytextP= dP->saveCP= bP->cp= bP->end= bP->buf;
  dP->saveCh= *bP->cp= dP->sentinelChar;
  dP->sentinelP= bP->cp;
  bP->eofState= 2;
  *dP->yyinP= bP->yyin= NULL;
#if 0
  if (*dP->yyCurrentBufP) {
    yyDeleteBuffer(dP, *dP->yyCurrentBufP); *dP->yyCurrentBufP= 0;
  }
#endif
  VOID_RET();
}

