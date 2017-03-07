/*

File:	 swtchbuf.c
Purpose: Switch current buffer to specified one.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"

YY_VOID
yySwitchToBuffer(d, b)
  YYDataHandle d;
  YYBufHandle b;
{
  YYData *YY_CONST dP= (YYData *)d;
  if (b == NULL) {
    yyZlexFatal(*dP->yyerrP, "Cannot switch to a NULL buffer.");
  }
  else {
    YYBuf *YY_CONST bP0= dP->bufP;		/* Old buffer. */
    YYBuf *YY_CONST  bP1= (YYBufP)b;		/* New buffer. */
    *dP->sentinelP= dP->sentinelSave;
    if (dP->options & YY_LINENO_FLAG) {
      bP0->lineno= *dP->yylinenoP;
      *dP->yylinenoP= bP1->lineno;
    }
    if (*dP->yyCurrentBufP) **dP->yyCurrentBufP= *bP0;
    *bP0= *bP1; *dP->yyCurrentBufP= bP1;
    *dP->yyinP= bP1->yyin;
    yyUpdateSentinel(dP, 0);	
  }
  VOID_RET();
}




