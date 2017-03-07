/*

File:	 isdiff.c
Purpose: Int-set difference.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

/* Remove from setA any elements which are also present in setB.  Return the
 * modified setA.
 */
ISet 
FN_NAME(diffNISet, diffDISet)(setA, setB DEBUG_ARGS)
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
      Boolean isZero= TRUE; /* To ensure deletion of any empty block. */
      Index i;
      for (i= 0; i < N_WORDS; i++) {
	BLK_WORDS(pA1)[i]&= ~BLK_WORDS(pB)[i];
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
      pA0= pA1; pA1= BLK_SUCC(pA1);
    }
    else {
      pB= BLK_SUCC(pB);
    }
  }
  return setA;
}

