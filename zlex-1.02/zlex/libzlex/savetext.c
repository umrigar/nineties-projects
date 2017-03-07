/*

File:	 savetext.c
Purpose: Squirrel away yytext when it is in danger of getting clobbered.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"

/* Save the current yytext from within the buffer, to a save area.  Will
 * be called when a buffer is deleted.
 */
YY_VOID
yySaveText(dP)
  YYData *dP;
{
  YYChar *YY_CONST text= *dP->yytextP;
  YY_CONST int leng= *dP->yylengP;
  YY_CONST int leng1= leng + 1;
#if 0
  YYBufP bP= dP->bufP;
  assert(bP->buf <= text && text < bP->cp);
#endif
  if (dP->yyTextSaveSize < leng1) {
    enum { SAVE_SIZE_MULT= 128 };
    dP->yyTextSaveSize= 
      ((leng + (SAVE_SIZE_MULT - 1))/SAVE_SIZE_MULT) * SAVE_SIZE_MULT;
    if (!(dP->yyTextSave= (YYChar *)
	( (dP->yyTextSave)
	  ? realloc(dP->yyTextSave, dP->yyTextSaveSize)
	  : malloc(dP->yyTextSaveSize)))) {
      yyZlexFatal(*dP->yyerrP, "Could not allocate save area.");
    }
  }
  *dP->yytextP= yyMoveBytes(dP->yyTextSave, text, leng1);
  if (dP->doMore) dP->moreText= *dP->yytextP;
}

