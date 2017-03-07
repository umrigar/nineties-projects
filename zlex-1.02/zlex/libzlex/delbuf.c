/*

File:	 delbuf.c
Purpose: Delete a previously created buffer.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
/* Free storage occupied by buffer b. */
YY_VOID 
yyDeleteBuffer(d, b)
  YYDataHandle d;
  YYBufHandle b;
{
  YYData *dP= (YYData *)d;
  YYBufP bP= (YYBufP)b;
  if (bP->buf <= *dP->yytextP && *dP->yytextP < bP->end) {
    yySaveText(dP);
  }
  if (*(dP->yyCurrentBufP) == bP) { /* Cause error if we try to access it. */
    *(dP->yyCurrentBufP)= 0; dP->bufP->cp= 0;
  }
  free((YY_VOIDP)(bP->buf)); free((YY_VOIDP)bP);
  VOID_RET();
}


