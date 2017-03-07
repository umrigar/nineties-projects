/*

File:	 utread.c
Purpose: Read file into dynamic memory.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "memalloc.h"
#include "zutil.h"

/* Read the rest of file into a dynamically allocated array (which can
 * subsequently be freed).
 */
Char *
readFile(file)
  FILE *file;
{
  Char *readBuf= NULL;
  Size readBufSize= 0;
  Count readBufIndex= 0;
  Int c;
  while ((c= getc(file)) != EOF) {
    if (readBufIndex >= readBufSize) {
      enum { READ_BUF_INC= 512 };
      readBuf= (Char*)REALLOC(readBuf, readBufSize+= READ_BUF_INC);
    }
    readBuf[readBufIndex++]= c;
  }
  return readBuf;
}
