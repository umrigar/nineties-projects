/*

File:	 isrm.c
Purpose: Remove an element from a int-set.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

ISet 
FN_NAME(rmNISet, rmDISet)(set, element DEBUG_ARGS)
  ISet set;
  ISetElement element;
  DEBUG_DECS
{
  ISetBlk *p0, *p1;
  ISET_OK(set);
  for (p0= (ISetBlk*)set, p1= BLK_SUCC(p0); p1 &&  element >= BLK_HI(p1);
       p0= p1, p1= BLK_SUCC(p1)) {
  }
  if (p1 && BLK_LO(p1) <= element) { /* Element must be in this block. */
    Index offset= element - BLK_LO(p1);
    BitWord *wordP= BLK_WORDS(p1);
    Index i;
    assert(BLK_LO(p1) <= element && element < BLK_HI(p1));
    wordP[offset/WORD_BITS]&= ~(WORD_WITH_MSB1 >> (offset%WORD_BITS));
    for (i= 0; i < N_WORDS && wordP[i] == 0; i++) ;
    if (i == N_WORDS) { /* Unlink zero block. */
      BLK_SUCC(p0)= BLK_SUCC(p1); DEL_BLK(p1);
    }
  }
  return set;
}


