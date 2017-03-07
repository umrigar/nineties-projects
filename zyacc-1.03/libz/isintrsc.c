/*

File:	 isintrsc.c
Purpose: Int-set intersection.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

ISet 
FN_NAME(intersectNISet, intersectDISet)(setA, setB DEBUG_ARGS)
  ISet setA;
  CONST ISet setB;
  DEBUG_DECS
{
  ISetBlk *hdrPA= (ISetBlk*)setA;
  CONST ISetBlk *hdrPB= (ISetBlk*)setB;
  ISetBlk *pA0= hdrPA;
  ISetBlk *pA1= BLK_SUCC(hdrPA);
  CONST ISetBlk *pB= BLK_SUCC(hdrPB);
  ISET_OK(setA);
  ISET_READ_OK(setB);
  while (pA1 != NULL && pB != NULL) {
    if (BLK_LO(pA1) == BLK_LO(pB)) {
      Boolean isZero= TRUE;
      Index i;
      for (i= 0; i < N_WORDS; i++) {
	BLK_WORDS(pA1)[i]&= BLK_WORDS(pB)[i];
	if (isZero) isZero= (BLK_WORDS(pA1)[i] == 0);
      }
      if (isZero) {
	BLK_SUCC(pA0)= BLK_SUCC(pA1); DEL_BLK(pA1); pA1= BLK_SUCC(pA0);
      }
      else {
	pA0= pA1; pA1= BLK_SUCC(pA1);
      }
      pB= BLK_SUCC(pB);
    }
    else if (BLK_LO(pA1) < BLK_LO(pB)) {
      BLK_SUCC(pA0)= BLK_SUCC(pA1); DEL_BLK(pA1); pA1= BLK_SUCC(pA0);
    }
    else {
      pB= BLK_SUCC(pB);
    }
  }
  if (pA1 != NULL) { /* Free remaining A blocks. */
    BLK_SUCC(pA0)= NULL;
    do {
      pA0= pA1; pA1= BLK_SUCC(pA1); DEL_BLK(pA0);
    } while (pA1 != NULL);
  }
  return setA;
}

