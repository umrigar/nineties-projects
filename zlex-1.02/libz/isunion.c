/*

File:	 isunion.c
Purpose: Int-set union.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

ISet 
FN_NAME(unionNISet, unionDISet)(setA, setB DEBUG_ARGS)
  ISet setA;
  CONST ISet setB;
  DEBUG_DECS
{
  CONST ISetBlk *pB= BLK_SUCC((ISetBlk*)setB);
  ISetBlk *pA0= (ISetBlk*)setA;
  ISetBlk *pA1= BLK_SUCC(pA0);
  ISET_READ_OK(setA);
  ISET_READ_OK(setB);
  while (pA1 != NULL && pB != NULL) {
    if (BLK_LO(pA1) == BLK_LO(pB)) {
      BitWord *CONST wordPA= BLK_WORDS(pA1);
      CONST BitWord *CONST wordPB= BLK_WORDS(pB);
      Index i;
      for (i= 0; i < N_WORDS; i++) wordPA[i]|= wordPB[i];
      pA0= pA1; pA1= BLK_SUCC(pA1); pB= BLK_SUCC(pB);
    }
    else if (BLK_LO(pA1) > BLK_LO(pB)) { /* Copy B block. */
      ISetBlk *newP;
      CONST BitWord *CONST wordPB= BLK_WORDS(pB);
      BitWord *wordPA;
      Index i;
      LINK_NEW_BLK(pA0, newP);
      BLK_LO(newP)= BLK_LO(pB); wordPA= BLK_WORDS(newP);
      for (i= 0; i < N_WORDS; i++) wordPA[i]= wordPB[i];
      pA0= newP; pA1= BLK_SUCC(newP); pB= BLK_SUCC(pB);
    }
    else { /* Go past current A block. */
      pA0= pA1; pA1= BLK_SUCC(pA1);
    }
  }
  while (pB != NULL) {
    CONST BitWord *CONST wordPB= BLK_WORDS(pB);
    ISetBlk *newP;
    BitWord *wordPA;
    Index i;
    LINK_NEW_BLK(pA0, newP);
    BLK_LO(newP)= BLK_LO(pB); wordPA= BLK_WORDS(newP);
    for (i= 0; i < N_WORDS; i++) wordPA[i]= wordPB[i];
    pA0= newP; pA1= BLK_SUCC(newP); pB= BLK_SUCC(pB);
  }
  return setA;
}

