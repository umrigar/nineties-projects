/*

File:	 bsunion.c
Purpose: Bit-set union.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

BSet 
unionBSet(setA, setB)
  BSetX setA;
  CONST BSetX setB;
{
  UsedHdr *CONST hdrPA= USED_HDR_P(setA);
  CONST UsedHdr *CONST hdrPB= USED_HDR_P(setB);
  CHECK_USED(setA); CHECK_USED(setB);
  if (hdrPA->nBitWords < hdrPB->nBitWords) {
    Index i;
    BitWord *CONST bitWords= 
      (BitWord *)MALLOC(sizeof(BitWord)*hdrPB->nBitWords);
     for (i= 0; i < hdrPA->nBitWords; i++) {
      bitWords[i]= hdrPA->bitWords[i] | hdrPB->bitWords[i];
    }
    for (; i < hdrPB->nBitWords; i++) {
      bitWords[i]= hdrPB->bitWords[i];
    }
    FREE(hdrPA->bitWords);
    hdrPA->bitWords= bitWords; hdrPA->nBitWords= hdrPB->nBitWords;
  }
  else {
    Index i;
    for (i= 0; i < hdrPB->nBitWords; i++) {
      hdrPA->bitWords[i]|= hdrPB->bitWords[i];
    }
  }
  return setA;
}

