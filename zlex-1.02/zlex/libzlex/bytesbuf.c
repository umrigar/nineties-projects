/*

File:	 bytesbuf.c
Purpose: Create a buffer initialized to specific byte values.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"

/* Creates a buffer to scan len bytes (including possibly sentinelChars) 
 * starting at location bytes.
 */
YYBufHandle
yyBytesBuffer(d, bytes, len)
  YYDataHandle d;
  YY_CONST YYChar *bytes;
  int len;
{
  YYData *YY_CONST dP= (YYData *)d;
  YYChar *buf= (YYChar *) malloc((len + 2) * sizeof(YYChar));
  if (buf == 0) yyZlexFatal(*dP->yyerrP, "Out of memory for buffer.");
  yyMoveBytes(buf, bytes, len); 
  buf[len]= buf[len + 1]= dP->sentinelChar;
  return yyMemBuffer(d, buf, len + 2);
}

