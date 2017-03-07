/*

File:	 move.c
Purpose: Move bytes routine.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
/* Move len bytes from src to dest.  Will work even for overlaps. */
YYChar *
yyMoveBytes(dest, src, len)
  YYChar *YY_CONST dest;
  YY_CONST YYChar *YY_CONST src;
  yy_size_t len;
{
  YY_CONST YYChar *p;
  YYChar *q;
  if (src < dest) { /* Move from hi addresses down to lo addresses. */
    for (p= src + len, q= dest + len; p > src; ) *--q= *--p;
  }
  else if (src > dest) { /* Move from lo addresses up to hi addresses. */
    YY_CONST YYChar *YY_CONST srcEnd= src + len;
    for (p= src, q= dest; p < srcEnd; ) *q++ = *p++;
  }
  return dest;
}

