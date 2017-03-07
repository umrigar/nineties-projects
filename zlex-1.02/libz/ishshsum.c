/*

File:	 ishshsum.c
Purpose: Return "sum" of the elements of a int-set (suitable for hashing).

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

ULong 
FN_NAME(hashSumNISet, hashSumDISet)(set DEBUG_ARGS)
  CONST ISet set;
  DEBUG_DECS
{
  ULong sum= 0;
  CONST ISetBlk *p;
  ISET_READ_OK(set);
  for (p= BLK_SUCC((ISetBlk*)set); p != NULL; p= BLK_SUCC(p)) {
    CONST BitWord *CONST wordP= BLK_WORDS(p);
    Index i;
    for (i= 0; i < N_WORDS; i++) sum+= wordP[i];
  }
  return sum;
}

