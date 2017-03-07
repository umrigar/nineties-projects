/*

File:	 iscomp.c
Purpose: Compare two int-sets.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

/* Return -1, 0, 1 if the relationship between setA and setB is < == or >.
 * The set which has the smallest element not present in the other is
 * defined to be > than the other.
 */
Int 
FN_NAME(compareNISet, compareDISet)(setA, setB DEBUG_ARGS)
  CONST ISet setA;
  CONST ISet setB;
  DEBUG_DECS
{
  CONST ISetBlk *pA= BLK_SUCC((ISetBlk*)setA);
  CONST ISetBlk *pB= BLK_SUCC((ISetBlk*)setB);
  Index i;
  ISET_READ_OK(setA); 
  ISET_READ_OK(setB);
  while (pA != NULL && pB != NULL) {
    if (BLK_LO(pA) == BLK_LO(pB)) {
      for (i= 0; i < N_WORDS; i++) {
	if (BLK_WORDS(pA)[i] < BLK_WORDS(pB)[i]) return -1;
	else if (BLK_WORDS(pA)[i] > BLK_WORDS(pB)[i]) return 1;
      }
      pA= BLK_SUCC(pA); pB= BLK_SUCC(pB);
    }
    else {
      return (BLK_LO(pA) < BLK_LO(pB)) ? 1 : -1;
    }
  }
  return (pA == NULL && pB == NULL) ? 0 : ((pA == NULL) ? -1 : 1);
}

