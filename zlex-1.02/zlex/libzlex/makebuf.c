/*

File:	 makebuf.c
Purpose: Allocate and initialize a buffer for a file or in-memory buffer.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
/* Create and initialize a buffer.  If yyin == 0, then it is an in-memory
 * buffer.  Otherwise it is a file buffer.  The memory area for the buffer
 * is a total of size bytes pointed to by mem.
 */
YYBufHandle
yyMakeBuf(dP, yyin, mem, size)
  YYData *dP;
  FILE *yyin;
  YYChar *mem;
  yy_size_t size;
{
  YYBufP bP= (YYBufP) malloc(sizeof(*bP));
  if (bP == 0) {
    yyZlexFatal(*dP->yyerrP, "Out of memory for buffer.");
  }
  bP->yyin= yyin;
  bP->buf= mem;
  bP->size= size;
  bP->nRead= 0;
  bP->intraPos= bP->lastLinePos= bP->firstNLPos= 0;
  bP->lineno= 1;
  bP->hasNL= 0;
  bP->lastCh= '\n';		/* Pretend we're at start of line. */
  bP->eofState= 0; 
  bP->cp= bP->buf; 		/* Point to buffer start. */
  bP->end= (yyin) ? bP->buf : &mem[size - 2];
  return (YYBufHandle)bP;
}
