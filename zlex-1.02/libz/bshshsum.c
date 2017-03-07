/*

File:	 bshshsum.c
Purpose: Return "sum" of the elements of a bit-set (suitable for hashing).

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

ULong 
hashSumBSet(set)
  CONST BSetX set;
{
  CONST UsedHdr *CONST hdrP= USED_HDR_P(set);
  ULong sum= 0;
  Index i;
  CHECK_USED(set);
  for (i= 0; i < hdrP->nBitWords; i++) sum+= hdrP->bitWords[i];
  return sum;
}

