/*

File:	 bsnew.c
Purpose: Return a new empty bit-set.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

BSet 
newBSet()
{
  /* Returns a new empty set. */
  CONST BSet set= HDR_ALLOC();
#ifdef DEBUG_BSET
  Hdr *CONST hP= HDR_ELEMENT(set);
  assert(hP->isFree == TRUE); hP->isFree= FALSE;
#endif
  {
    UsedHdr *CONST hdrP= USED_HDR_P(set);
    CHECK_USED(set);
    hdrP->nBitWords= N_BIT_WORDS(DEFAULT_MAX_ELEMENT);
    hdrP->bitWords= 
      (BitWord *) CALLOC(N_BIT_WORDS(DEFAULT_MAX_ELEMENT), sizeof(BitWord));
  }
  return set;
}

