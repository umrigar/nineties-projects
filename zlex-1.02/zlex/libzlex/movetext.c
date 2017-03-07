/*

File:	 movetext.c
Purpose: Move yytext routine.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
/* Move yytext to dest.  Assumes enough space at dest.  Update yytext 
 * & saveCh.
 * Will work even if dest area overlaps current yytext. 
 */
YYChar *
yyMoveText(dP, dest)
  YYData *dP;
  YYChar *dest;
{
  YYChar *yyText= *dP->yytextP;
  int yyLeng= *dP->yylengP;
  *dP->saveCP= dP->saveCh;
  *dP->yytextP= yyText= yyMoveBytes(dest, yyText, yyLeng);
  dP->saveCP= &yyText[yyLeng]; dP->saveCh= yyText[yyLeng]; yyText[yyLeng]= '\0';
  return yyText;
}
