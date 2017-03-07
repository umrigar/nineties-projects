/*

File:	 creatbuf.c
Purpose: Create a buffer for a file.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
/* Create a buffer with space for at least s chars (have at least
 * YY_TEXT_XPAND extra chars. to allow yytext to be retained. 
 */
YYBufHandle 
yyCreateBuffer(d, f, s)
  YYDataHandle d;
  FILE *f;
  unsigned long s; 
{
  enum {YY_TEXT_XPAND= 120};
  YYData *YY_CONST dP= (YYData *)d;
  YY_CONST yy_size_t size= s + YY_TEXT_XPAND;
  YYChar *buf= (YYChar *) malloc((size) * sizeof(*buf));
  if (buf == 0) 
    yyZlexFatal(*dP->yyerrP, "Out of memory for buffer.");
  return yyMakeBuf(dP, f, buf, size);
}


