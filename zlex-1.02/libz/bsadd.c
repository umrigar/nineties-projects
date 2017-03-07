/*

File:	 bsadd.c
Purpose: Add an element to a bit-set.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

BSet 
addBSet(set, element)
  BSetX set;
  BSetElement element;
{
  UsedHdr *CONST hdrP= USED_HDR_P(set);
  CONST Size nBitWords= N_BIT_WORDS(element);
  CHECK_USED(set);
  if (hdrP->nBitWords < nBitWords) {
    Index i;
    hdrP->bitWords= 
      (BitWord *) REALLOC(hdrP->bitWords, nBitWords * sizeof(BitWord));
    for (i= hdrP->nBitWords; i < nBitWords; i++) {
      hdrP->bitWords[i]= 0;
    }
    hdrP->nBitWords= nBitWords;
  }
  hdrP->bitWords[element/BITWORD_BIT]|= 
    1L << (element % BITWORD_BIT);
  return set;
}


