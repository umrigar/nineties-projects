/*

File:	 bsdiff.c
Purpose: Bit-set difference.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

BSet 
diffBSet(setA, setB)
  BSetX setA;
  CONST BSetX setB;
{
  CONST UsedHdr *CONST hdrPA= USED_HDR_P(setA);
  CONST UsedHdr *CONST hdrPB= USED_HDR_P(setB);
  CONST Size min= 
    hdrPA->nBitWords < hdrPB->nBitWords 
    ? hdrPA->nBitWords 
    : hdrPB->nBitWords;
  Index i;
  CHECK_USED(setA); CHECK_USED(setB);
  for (i= 0; i < min; i++) {
    hdrPA->bitWords[i]&= ~hdrPB->bitWords[i];
  }
  return setA;
}

