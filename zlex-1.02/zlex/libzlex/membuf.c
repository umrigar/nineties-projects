/*

File:	 membuf.c
Purpose: Create a buffer pointing to a specified memory block.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"

/* Returns a buffer to scan in place the buffer starting at buf, 
 * consisting of len bytes, the last two bytes of which *must* be
 * sentinel-characters.  The last two bytes will not be scanned; 
 * thus, the scanning is of of `buf[0]' through `buf[len-3]', 
 * inclusive.
 */
YYBufHandle
yyMemBuffer(d, buf, len)
  YYDataHandle d;
  YYChar *buf;
  yy_size_t len;
{
  YYData *YY_CONST dP= (YYData *)d;
  return (buf[len - 2] != dP->sentinelChar || buf[len - 1] != dP->sentinelChar)
         ? NULL
         : yyMakeBuf(dP, NULL, buf, len);
}

