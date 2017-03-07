/*

File:	 restart.c
Purpose: Support for yyrestart().

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
YY_VOID 
yyRestart(d, fp)
  YYDataHandle d;
  FILE *fp;
{
  YYData *YY_CONST dP= (YYData *)d;
  YYBuf *bP= dP->bufP;
  *dP->sentinelP= dP->sentinelSave;
  bP->yyin= fp;
  bP->eofState= 0; 
  bP->nRead+= bP->end - bP->cp; bP->end= bP->cp;
  yyUpdateSentinel(dP, 0);
  VOID_RET();
}

