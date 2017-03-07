/*

File:	 isadd.c
Purpose: Add an element to a int-set.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

/* Add element to existing ISet set.  Return modified set. */
ISet 
FN_NAME(addNISet, addDISet)(set, element DEBUG_ARGS)
  ISet set;
  ISetElement element;
  DEBUG_DECS
{
  ISetBlk *p0, *p1;
  Index offset;
  ISET_OK(set);
  for (p0= (ISetBlk*)set, p1= BLK_SUCC(p0); p1 &&  element >= BLK_HI(p1);
       p0= p1, p1= BLK_SUCC(p1)) {
  }
  if (!p1 || BLK_LO(p1) > element) { /* Create a new block. */
    CONST ISetElement lo1= (element / N_ELEMENTS_PER_BLK) * N_ELEMENTS_PER_BLK;
    CONST ISetElement lo= lo1 > element ? lo1 - N_ELEMENTS_PER_BLK : lo1;
    Index i;
    assert(lo <= element && element < lo + N_ELEMENTS_PER_BLK);
    LINK_NEW_BLK(p0, p1);
    BLK_LO(p1)= lo;
    for (i= 0; i < N_WORDS; i++) BLK_WORDS(p1)[i]= 0;
  }
  assert(BLK_LO(p1) <= element && element < BLK_HI(p1));
  offset= element - BLK_LO(p1);
  BLK_WORDS(p1)[offset/WORD_BITS]|= WORD_WITH_MSB1 >> (offset%WORD_BITS);
  return set;
}


