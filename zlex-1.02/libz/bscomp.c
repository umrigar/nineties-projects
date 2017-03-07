/*

File:	 bscomp.c
Purpose: Compare two bit-sets.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

Int 
compareBSet(setA, setB)
  CONST BSetX setA;
  CONST BSetX setB;
{
  CONST UsedHdr *CONST hdrPA= USED_HDR_P(setA);
  CONST UsedHdr *CONST hdrPB= USED_HDR_P(setB);
  CONST Boolean isABigger= hdrPA->nBitWords > hdrPB->nBitWords;
  CONST UsedHdr *CONST hdrBigP= isABigger ? hdrPA : hdrPB;
  CONST Size min= isABigger ? hdrPB->nBitWords : hdrPA->nBitWords;
  Index i;
  CHECK_USED(setA); CHECK_USED(setB);
  for (i= 0; i < min; i++) {
    if (hdrPA->bitWords[i] < hdrPB->bitWords[i]) return -1;
    else if (hdrPA->bitWords[i] > hdrPB->bitWords[i]) return 1;
  } 
  /* Check that remaining BitWords are 0. */
  for (; i < hdrBigP->nBitWords; i++) {
    if (hdrBigP->bitWords[i] != 0) 
      return (hdrBigP == hdrPA) ? 1 : -1;
  }
  return 0;
}

