/*

File:	 bssubset.c
Purpose: Check if a bit-set is a subset of another.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

Boolean 
isSubsetBSet(subset, superset)
  CONST BSetX subset;
  CONST BSetX superset;
{
  CONST UsedHdr *CONST hdrPSub= USED_HDR_P(subset);
  CONST UsedHdr *CONST hdrPSuper= USED_HDR_P(superset);
  CONST Size min= 
    hdrPSub->nBitWords < hdrPSuper->nBitWords 
  ? hdrPSub->nBitWords 
  : hdrPSuper->nBitWords;
  Index i;
  CHECK_USED(subset); CHECK_USED(superset);
  for (i= 0; i < min; i++) {
#if 0
    BitWord mask= 1L;
    while (mask != 0) {
      if ((hdrPSub->bitWords[i] & mask) && !(hdrPSuper->bitWords[i] & mask))
	return FALSE;
      mask<<= 1;
    }
#endif
    BitWord subWord= hdrPSub->bitWords[i];
    if ((subWord & hdrPSuper->bitWords[i]) != subWord) return FALSE;
  }
  for (; i < hdrPSub->nBitWords; i++) {
    if (hdrPSub->bitWords[i] != 0) return FALSE;
  }
  return TRUE;
}

