/*

File:	 more.c
Purpose: Support for yymore().

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"

/* Restore char. replaced by current sentinel.  Set sentinel to fire at
 * next character and set moreFlag to indicate that yymore() was responsible
 * for the firing.
 */
YY_VOID 
yyMore(d)
  YYDataHandle d;
{
  YYData *YY_CONST dP= (YYData *)d;
  *dP->sentinelP= dP->sentinelSave;
  dP->moreText= *dP->yytextP; dP->moreLeng= *dP->yylengP;
  dP->doMore= 1;
  yyUpdateSentinel(dP, 0);
  VOID_RET();
}

