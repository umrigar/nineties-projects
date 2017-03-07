/*

File:	 issize.c
Purpose: Return the number of elements in a int-set.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

Size 
FN_NAME(sizeNISet, sizeDISet)(set DEBUG_ARGS)
  CONST ISet set;
  DEBUG_DECS
{
  CONST ISetBlk *p;
  Size s= 0;
  ISET_READ_OK(set);
  for (p= BLK_SUCC((ISetBlk*)set); p != NULL; p= BLK_SUCC(p)) {
    CONST BitWord *CONST wordP= BLK_WORDS(p);
    Index i;
    for (i= 0; i < N_WORDS; i++) {
      BitWord w= wordP[i];
      while (w != 0) { s++; w= w & (w - 1); }
    }
  }
  return s;
}

