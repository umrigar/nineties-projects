/*

File:	 debug.c
Purpose: Routines for debugging messages (--debug option).

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
YY_VOID 
yyDebugMsg(dP, srcFile, n, yyText, debugFlag)
  YY_CONST YYData *dP;
  char *srcFile;
  int n;
  YYChar *yyText;
  int debugFlag;
{
  if (debugFlag) {
    fprintf(*dP->yyerrP, "%s:%d: yytext= `", srcFile, n);
    /* Desperate attempt to output 16-bit yytext; should work for 8-bit. */
    fwrite(yyText, sizeof(YYChar), *dP->yylengP, *dP->yyerrP);
    fputs("'.\n", *dP->yyerrP);
  }
  VOID_RET();
}

YY_VOID 
yyEOFMsg(dP, debugFlag)
  YY_CONST YYData *dP;
  int debugFlag;
{
  if (debugFlag)
    fputs("--EOF.\n", *dP->yyerrP);
  VOID_RET();
}

YY_VOID 
yyDefaultMsg(dP, yyText, debugFlag)
  YY_CONST YYData *dP;
  YYChar *yyText;
  int debugFlag;
{
  if (debugFlag) {
    fprintf(*dP->yyerrP, "--default action (\"");
    /* Desperate attempt to output 16-bit yytext; should work for 8-bit. */
    fwrite(yyText, sizeof(YYChar), *dP->yylengP, *dP->yyerrP);
    fputs("\").\n", *dP->yyerrP);
  }
  VOID_RET();
}

