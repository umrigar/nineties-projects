/*

File:	 bsnext.c
Purpose: Given an element in a bit-set, return the next element.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

BSetElement 
nextElementBSet(set, element)
  CONST BSetX set;
  BSetElement element;
{
  CONST UsedHdr *CONST hdrP= USED_HDR_P(set);
  CONST BSetElement start= element == NIL_ELEMENT ? 0 : element + 1;
  CONST Index nWord= start/BITWORD_BIT;
  CHECK_USED(set);
  if (nWord >= hdrP->nBitWords) 
    return NIL_ELEMENT;
  else {
    BitWord w= hdrP->bitWords[nWord] >> (start % BITWORD_BIT);
    if (w != 0) {	/* We have an element in this word. */
      BSetElement e= start;
      while ((w & 1) == 0) { e++; w>>= 1; }
      return e;
    }
    else {
      Index i;
      for (i= nWord + 1; i < hdrP->nBitWords; i++) {
	if (hdrP->bitWords[i] != 0) { 	/* Element in here. */
	  BitWord wI= hdrP->bitWords[i];
	  BSetElement e= i * BITWORD_BIT;
	  while ((wI & 1) == 0) { wI>>= 1; e++; }
	  return e;
	}
      }
      return NIL_ELEMENT;  
    }
  }
}

