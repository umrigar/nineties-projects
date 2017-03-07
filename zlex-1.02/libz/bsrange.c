/*

File:	 bsrange.c
Purpose: Insert a range of elements into an existing bit-set.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

/* Insert elements lo..hi (both inclusive) into set. */
BSet 
rangeBSet(set, lo, hi)
  BSetX set;
  CONST BSetElement lo; 
  CONST BSetElement hi;
{
  if (lo <= hi) {
    UsedHdr *CONST hdrP= USED_HDR_P(set);
    CONST Index loWord= lo/BITWORD_BIT;
    CONST Index hiWord= hi/BITWORD_BIT;
    CONST Count loShift= lo%BITWORD_BIT;
    CONST Count hiShift= hi%BITWORD_BIT;
    CHECK_USED(set);
    if (hdrP->nBitWords <= hiWord) {
      Index i;
      hdrP->bitWords= (BitWord *)
	REALLOC(hdrP->bitWords, (hiWord + 1) * sizeof(BitWord));
      for (i= hdrP->nBitWords; i < hiWord + 1; i++) {
        hdrP->bitWords[i]= 0;
      }
      hdrP->nBitWords= hiWord + 1;
    }
    if (loWord < hiWord) {
      Index i;
      hdrP->bitWords[loWord]|= ~((1L << loShift) - 1);
      for (i= loWord + 1; i < hiWord; i++) {
	hdrP->bitWords[i]= (BitWord)~0x0;
      }
      hdrP->bitWords[hiWord]|= ((1L << hiShift) - 1) | (1L << hiShift);
    }
    else {	/* loWord == hiWord. */
      hdrP->bitWords[loWord]|= 
	(~((1L << loShift) - 1)) & (((1L << hiShift) - 1) | (1L << hiShift));
    }
  }
  return set;
}

