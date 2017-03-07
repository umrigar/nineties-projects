/*

File:	 iscopy.c
Purpose: Return a new copy of a int-set using fresh storage.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

/* Return a copy of s which shares no storage with s. */
ISet 
FN_NAME(copyNISet, copyDISet)(s DEBUG_ARGS)
  CONST ISet s;
  DEBUG_DECS
{
  ISetBlk *newISetP= NEW_BLK();
  ISetBlk *pZ= newISetP;
  ISetBlk *pA;
  ISET_READ_OK(s);
  BLK_LO(pZ)= 0;
#ifdef DEBUG_ISET
  SET_OWNER(pZ, modName);
#else
  HDR_OWNER(pZ)= NULL;
#endif
  BLK_SUCC(pZ)= NULL;
  for (pA= BLK_SUCC((ISetBlk*)s); pA != NULL; pA= BLK_SUCC(pA)) {
    ISetBlk *newBlkP;
    Index i;
    LINK_NEW_BLK(pZ, newBlkP);
    BLK_LO(newBlkP)= BLK_LO(pA);
    for (i= 0; i < N_WORDS; i++) BLK_WORDS(newBlkP)[i]= BLK_WORDS(pA)[i];
    pZ= newBlkP;
  }
  return (ISet)newISetP;
}

