/*

File:	 bsassign.c
Purpose: Assign one bit-set to another.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

BSet 
assignBSet(setA, setB)
  BSetX setA;
  CONST BSetX setB;
{
  UsedHdr *CONST hdrPA= USED_HDR_P(setA);
  CONST UsedHdr *CONST hdrPB= USED_HDR_P(setB);
  CONST Size max= 
    hdrPA->nBitWords > hdrPB->nBitWords 
    ? hdrPA->nBitWords 
    : hdrPB->nBitWords;
  Index i;
  CHECK_USED(setA); CHECK_USED(setB);
  if (hdrPA->nBitWords < hdrPB->nBitWords) {
    hdrPA->bitWords= (BitWord *)
      REALLOC(hdrPA->bitWords, hdrPB->nBitWords * sizeof(BitWord));
    hdrPA->nBitWords= hdrPB->nBitWords;
  }
  for (i= 0; i < hdrPB->nBitWords; i++) {
    hdrPA->bitWords[i]= hdrPB->bitWords[i];
  }
  for(; i < max; i++) hdrPA->bitWords[i]= 0;
  return setA;
}

