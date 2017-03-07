/*

File:	 isinbset.c
Purpose: Check if an element is a member of a int-set.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

Boolean 
FN_NAME(inNISet, inDISet)(i, set DEBUG_ARGS)
  ISetElement i;
  CONST ISet set;
  DEBUG_DECS
{
  CONST ISetBlk *p;
  ISET_READ_OK(set);
  for (p= BLK_SUCC((ISetBlk*)set); p != NULL; p= BLK_SUCC(p)) {
    if (i < BLK_HI(p)) break;
  }
  if (p && BLK_LO(p) <= i) {
    CONST Index offset= i - BLK_LO(p);
    CONST Index iIndex= offset / WORD_BITS;
    CONST Index iBitNum= offset % WORD_BITS;
    return (BLK_WORDS(p)[iIndex] & (WORD_WITH_MSB1 >> iBitNum)) != 0;
  }
  return FALSE;
}

