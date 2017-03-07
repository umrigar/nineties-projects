/*

File:	 isassign.c
Purpose: Assign one integer-set to another.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

/* Delete the contents of setA and assign to it a *copy* of the
 * contents of setB, returning the modified setA.
 */
ISet 
FN_NAME(assignNISet, assignDISet)(setA, setB DEBUG_ARGS)
  ISet setA;
  CONST ISet setB;
  DEBUG_DECS
{
  ISetBlk *p0, *p1;
  ISET_OK(setA); 
  ISET_READ_OK(setB);
  for (p0= BLK_SUCC((ISetBlk*)setA); p0; p0= p1) {
    p1= BLK_SUCC(p0); DEL_BLK(p0);
  }
  p0= (ISetBlk*)setA; BLK_SUCC(p0)= NULL;
  for (p1= BLK_SUCC((ISetBlk*)setB); p1 != NULL; p1= BLK_SUCC(p1)) {
    ISetBlk *newBlkP;
    Index i;
    LINK_NEW_BLK(p0, newBlkP);
    BLK_LO(newBlkP)= BLK_LO(p1);
    for (i= 0; i < N_WORDS; i++) BLK_WORDS(newBlkP)[i]= BLK_WORDS(p1)[i];
    p0= newBlkP;
  }
  return setA;
}

