/*

File:	 issubset.c
Purpose: Check if a int-set is a subset of another.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

Boolean 
FN_NAME(isSubsetNISet, isSubsetDISet)(subset, superset DEBUG_ARGS)
  CONST ISet subset;
  CONST ISet superset;
  DEBUG_DECS
{
  CONST ISetBlk *subP= BLK_SUCC((ISetBlk*)subset);
  CONST ISetBlk *supP= BLK_SUCC((ISetBlk*)superset);
  ISET_READ_OK(subset);
  ISET_READ_OK(superset);
  while (subP != NULL && supP != NULL) {
    if (BLK_LO(subP) == BLK_LO(supP)) {
      CONST BitWord *CONST subWordP= BLK_WORDS(subP);
      CONST BitWord *CONST supWordP= BLK_WORDS(supP);
      Index i;
      for (i= 0; i < N_WORDS; i++) {
	BitWord subWord= subWordP[i];
	if ((subWord & supWordP[i]) != subWord) return FALSE;
      }
      subP= BLK_SUCC(subP); supP= BLK_SUCC(supP);
    }
    else if (BLK_LO(subP) < BLK_LO(supP)) {
      return FALSE;
    }
    else {
      supP= BLK_SUCC(supP);
    }
  }
  return (subP == NULL);
}

