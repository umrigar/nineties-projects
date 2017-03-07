/*

File:	 isinit.c
Purpose: Initialize the integer-set module.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

enum {
  BLK_INC= 256		/* Allocate blocks in chunks of this. */
};

FreeList _ZISetFrees;

VOID 
initISet()
{
  newFreeList(sizeof(ISetBlk), 256, &_ZISetFrees);
  VOID_RET();
}

VOID 
terminateISet()
{
  destroyFreeList(&_ZISetFrees);
  VOID_RET();
}
