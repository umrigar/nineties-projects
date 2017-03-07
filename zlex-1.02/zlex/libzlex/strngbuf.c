/*

File:	 strngbuf.c
Purpose: Create a buffer initialized to a specified C-string.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"

/* Creates a buffer to scan a NUL-terminated string str. */
YYBufHandle
yyStringBuffer(d, str)
  YYDataHandle d;
  YY_CONST YYChar *str;
{
  YY_CONST YYChar *p;
  for (p= str; *p != 0; p++) ;
  return yyBytesBuffer(d, str, p - str);
}


