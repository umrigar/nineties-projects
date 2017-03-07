/*

File:	 bscopy.c
Purpose: Return a new copy of a bit-set using fresh storage.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

/* Return a copy of s which shares no storage with s.			*/
BSet 
copyBSet(s)
  CONST BSetX s;
{
  CONST UsedHdr *CONST hdrPs= USED_HDR_P(s);
  CONST BSet setZ= HDR_ALLOC();
#ifdef DEBUG_BSET
  Hdr *CONST hP= HDR_ELEMENT(setZ);
  assert(hP->isFree == TRUE); hP->isFree= FALSE;
#endif
  CHECK_USED(s);
  {
    UsedHdr *CONST hdrPZ= USED_HDR_P(setZ);
    CHECK_USED(setZ);
    hdrPZ->nBitWords= hdrPs->nBitWords;
    hdrPZ->bitWords= (BitWord *) MALLOC(hdrPZ->nBitWords * sizeof(BitWord));
    memcpy(hdrPZ->bitWords, hdrPs->bitWords, 
	   (SIZE_T)(hdrPZ->nBitWords * sizeof(BitWord)));
  }
  return setZ;
}

