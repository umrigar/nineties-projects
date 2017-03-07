/*

File:	 bsinit.c
Purpose: Initialize the bit-set module.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

BSetData _ZbsData;

VOID 
initBSet()
{
  INIT_AREA(hdrs, Hdr, 10, UINT2_MAX - 1);
  frees= NIL;
  dummyBSet= BAD_BSET;
  VOID_RET();
}

