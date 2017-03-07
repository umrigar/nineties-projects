/*

File:	 bsinbset.c
Purpose: Check if an element is a member of a bit-set.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

Boolean 
inBSet(i, set)
  BSetElement i;
  CONST BSetX set;
{
  CONST UsedHdr *CONST hdrP= USED_HDR_P(set);
  CONST Index4 nWord= i/BITWORD_BIT;
  CHECK_USED(set);
  return hdrP->nBitWords > nWord 
         && (hdrP->bitWords[nWord] & (1L << (i%BITWORD_BIT)));
}

