/*

File:	 isnext.c
Purpose: Routines for producing next element of an ISet.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

/* Return the next element in the ISetBlk pointed to by *blkPP which
 * is >= element.  Set *blkPP to NULL if there is none.  
 */
static ISetElement
nextElementInBlk(blkPP, element)
  ISetBlk **blkPP;
  ISetElement element;
{
  ISetBlk *CONST blkP= *blkPP;
  CONST ISetElement lo= BLK_LO(blkP);
  CONST BitWord *CONST wordP= BLK_WORDS(blkP);
  CONST Index offset= element - lo;
  CONST Index elemIndex= offset / WORD_BITS;
  CONST Index elemBitNum= offset % WORD_BITS;
  BitWord mask= WORD_WITH_MSB1 >> elemBitNum;
  ISetElement elemZ= element;
  Index i;
  assert(element >= lo);
  for (i= elemIndex; i < N_WORDS; i++) {
    if (wordP[i] & (mask|(mask - 1))) { /* Next element within this word. */
      while ((wordP[i] & mask) == 0) { 
	assert(mask != 0);
	elemZ++; mask>>= 1; 
      }
      break;
    }
    mask= WORD_WITH_MSB1;
    elemZ= lo + (ISetElement)((i + 1)*WORD_BITS);
  }
  if (i >= N_WORDS) *blkPP= NULL;
  return elemZ;
}

/* If *iterP == VOID_ISET_ITER, then produce return next set element >
 * element.  If *iterP == set, then return first element of set.
 * Otherwise if *iterP points to a block ranging over element, then
 * that helps searching for the next element; otherwise the action is
 * similar to *iterP == VOID_ISET_ITER.  In any case, if there is no
 * next element, then the return value is garbage, but *iterP is set
 * to VOID_ISET_ITER.
 */
ISetElement 
FN_NAME(nextElementNISet, nextElementDISet)(set, element, iterP DEBUG_ARGS)
  ISet set;
  ISetElement element;
  ISetIter *iterP;
  DEBUG_DECS
{
  ISetBlk *p= (ISetBlk*)*iterP;
  ISetElement e;
  ISET_READ_OK(set);
  if (p != NULL && p == (ISetBlk*)set) {
    p= BLK_SUCC(p); if (p) e= BLK_LO(p);
  }
  else if (p != NULL && BLK_LO(p) <= element && element < BLK_HI(p)) { 
    /* Iterating thru ISet: use p to guide search. */
    e= element + 1;
  }
  else { /* Set p to block containing elements > element. */
    e= element + 1;
    for (p= BLK_SUCC((ISetBlk*)set); p != NULL; p= BLK_SUCC(p)) {
      if (e < BLK_HI(p)) break;
    }
    if (p && BLK_LO(p) > e) e= BLK_LO(p);
  }
  if (p) {
    ISetBlk *p1= p;
    e= nextElementInBlk(&p1, e);
    if (!p1) { /* No element in current block. */
      if (BLK_SUCC(p)) { /* Get element from next block (there MUST be one). */
	p= BLK_SUCC(p); e= nextElementInBlk(&p, BLK_LO(p));
      }
      else {
	p= NULL;
      }
    }
  }
  *iterP= (ISetIter)p;
  return e;
}

/* Return first element in set.  set should not be empty. */
ISetElement
FN_NAME(firstElementNISet, firstElementDISet)(set DEBUG_ARGS)
  ISet set;
  DEBUG_DECS
{
  ISetIter iterP= set;
  ISetElement e= nextElementISet(set, 0, &iterP);
  ISET_READ_OK(set);
  assert(iterP);
  return e;
}
