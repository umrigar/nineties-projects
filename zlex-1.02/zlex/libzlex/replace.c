/*

File:	 replace.c
Purpose: Replace routine used within intra-token actions.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"

/* This is only meant to be called during intra-token actions.  It
 * replaces the last len characters of *intraTextP by string, moving
 * intraTextP if strlen(string) != len.  Assumes that len <=
 * yyleng. Returns the updated cp pointing just past the end of the
 * unmodified portion of yytext.
 */
YYChar *
yyReplace(dP, len, string, intraTextP)
  YYData *dP;
  yy_size_t len;
  YY_CONST YYChar *string;
  YYChar **intraTextP;
{
  yy_size_t sLen;
  YY_CONST YYChar *p;
  YYChar *text= *intraTextP;		/* Current token + intra-tok yytext. */
  YYBuf *bP= dP->bufP;
  YY_CONST int textLen= bP->cp - text;	/* Length of text. */
  assert(len <= *dP->yylengP);
  assert(bP->cp == dP->saveCP);
  *dP->saveCP= dP->saveCh;		/* Restore terminating char. */
  for (p= string; *p != 0; p++) ;	/* Compute length */
  sLen= p - string;			/* of string in sLen. */
  if (sLen != len) { 			/* Need to move intraText. */
    int diff= (int)sLen - (int)len;	/* Move amount: + -> lo; - -> hi. */
    YYChar *dest= text - diff;
    if (dest < bP->buf) {		/* Need to shift buffer. */
      yy_size_t yyTextDisp= *dP->yytextP - text;
      bP->cp= text;			/* Preserve complete intra-text. */
      yyShiftBuffer(dP, diff);		/* Ensure diff chars before cp. */
      text= bP->cp; bP->cp+= textLen;	/* Restore intraText. */	
      *dP->yytextP= text + yyTextDisp; 	/* Restore yytext. */
      dest= bP->buf; 			/* Will move to beginning of buffer. */
    }
    yyMoveBytes(dest, text, textLen - len);/* Move good part of text to dest. */
    *intraTextP= text= dest;		/* Update global intraText. */
  }
  yyMoveBytes(text + textLen - len, string, sLen);	/* Move string. */
  bP->cp= text + textLen - len;
  /* Set up terminating NUL. */
  dP->saveCP= bP->cp; dP->saveCh= *dP->saveCP; *dP->saveCP= '\0';
  return bP->cp;
}

