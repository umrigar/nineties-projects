/*

File:	 bsempty.c
Purpose: Check if a bit-set is empty.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

Boolean 
isEmptyBSet(set)
  CONST BSetX set;
{
  CONST UsedHdr *CONST hdrP= USED_HDR_P(set);
  Index i;
  CHECK_USED(set);
  for (i= 0; i < hdrP->nBitWords; i++) {
    if (hdrP->bitWords[i] != 0) return FALSE;
  }
  return TRUE;
}

