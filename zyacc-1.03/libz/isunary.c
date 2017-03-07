/*

File:	 isunary.c
Purpose: Create a new int-set containing a single specified element.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

ISet 
FN_NAME(unaryNISet, unaryDISet)(e DEBUG_ARGS)
  ISetElement e;
  DEBUG_DECS
{
  CONST ISetElement lo1= (e / N_ELEMENTS_PER_BLK) * N_ELEMENTS_PER_BLK;
  CONST ISetElement lo= lo1 > e ? lo1 - N_ELEMENTS_PER_BLK : lo1;
  CONST Index offset= e - lo;
  ISetBlk *hdrP= NEW_BLK();
  ISetBlk *p1;
  Index i;
  assert(lo <= e && e < lo + N_ELEMENTS_PER_BLK);
#ifdef DEBUG_ISET
  SET_OWNER(hdrP, modName);
#else
  HDR_OWNER(hdrP)= NULL;
#endif
  BLK_SUCC(hdrP)= NULL;
  LINK_NEW_BLK(hdrP, p1);
  BLK_LO(p1)= lo;
  for (i= 0; i < N_WORDS; i++) BLK_WORDS(p1)[i]= 0;
  BLK_WORDS(p1)[offset/WORD_BITS]|= WORD_WITH_MSB1 >> (offset%WORD_BITS);
  return (ISet)hdrP;
}

