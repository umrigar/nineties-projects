/*

File:	 isinit.c
Purpose: Initialize the integer-set module.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

/* compile this module only if !DEBUG_ISET */
#ifdef DEBUG_ISET

char _ZisinitGarbage= 0;	/* just something to prevent empty compilation
				 * unit warnings
				 */

#else

enum {
  BLK_INC= 256		/* Allocate blocks in chunks of this. */
};

FreeList _ZISetFrees;

VOID 
initISet()
{
  _ZISetFrees= newFreeList(sizeof(ISetBlk), 256);
  VOID_RET();
}

VOID 
terminateISet()
{
  destroyFreeList(_ZISetFrees);
  VOID_RET();
}

#endif
