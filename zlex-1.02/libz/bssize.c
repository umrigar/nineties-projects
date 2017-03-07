/*

File:	 bssize.c
Purpose: Return the number of elements in a bit-set.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

Size 
sizeBSet(set)
  CONST BSetX set;
{
  CONST UsedHdr *CONST hdrP= USED_HDR_P(set);
  Size s= 0;
  Index i;
  CHECK_USED(set);
  for (i= 0; i < hdrP->nBitWords; i++) {
    BitWord w= hdrP->bitWords[i];
    while (w != 0) { s++; w= w & (w - 1); }
  }
  return s;
}

