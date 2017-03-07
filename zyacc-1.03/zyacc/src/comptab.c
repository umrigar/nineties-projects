/*

File:	 comptab.c
Purpose: Table compression routines.

Last Update Time-stamp: "97/07/10 18:06:49 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/
/*			  TABLE COMPRESSION.				*/

/*

We chain free entries together in a doubly-linked circular list
without using any additional storage.  We use next[] for
forward-links, check[] for backward-links.  To distinguish links from
data stored in check[], we ensure that all links are greater than the
max. value ever stored in check[].  To facilitate addressing using
these coded links we often use a virtual origin for the check/next
arrays so that all indices are greater than the data value.  We also
use the last entry of the check/next array as the header for this
circular list, making it possible to unlink entries from the list
without any boundary checks.  Hence the last entry in check/next is
always free.

*/

#include "comptab.h"

#include "memalloc.h"
#include "error.h"


/*			    STATIC PROTOTYPES.				*/

static VOID growCompTab PROTO((CompTab *tabP, Index2 **nextP, Index2 **checkP, 
			Index *lastP));




/* The following macros assume a auto var called nCols which mirrors
 * the fields in the CompTab.  Also assumes a variable check which is the
 * virtualized start of the check[] array and variable last which is the
 * virtualized index of the last entry in check[]/next[].
 */
#define IS_FREE(i)	(check[i] >= nCols)
#define FREE_VAL	(nCols)
#define UNFREE_VAL	(0)
#define LINK_INVARIANT(i)						\
  (next[check[i]] == (i) && check[next[i]] == (i))

#define UNLINK(i) /* Unlink cell with virtual index i from frees. */	\
  do {									\
    CONST Index2 succ= next[i];						\
    CONST Index2 pred= check[i];					\
    assert(i != last);		/* Never unlink last cell. */		\
    assert(next[pred] == i);	/* Circular list invariant. */		\
    assert(check[succ] == i);	/* Circular list invariant. */		\
    next[pred]= succ; check[succ]= pred;				\
  } while (0)

static VOID 
growCompTab(tabP, nextP, checkP, lastP)
  CompTab *tabP;
  Index2 **nextP;
  Index2 **checkP;
  Index *lastP;
{
  enum { INC_FACTOR= 4 };
  CONST Index oldSize= tabP->checkNextSize;
  CONST Index newSize= oldSize + INC_FACTOR * tabP->nCols;
  CONST Index nCols= tabP->nCols;
  Index i;
  if (newSize >= UINT2_MAX - nCols - 1)
    fatal("compressed table too large at %d", newSize);
  tabP->check= 
    (Index2 *) REALLOC((VOIDP) tabP->check, sizeof(Index2) * newSize);
  tabP->next= 
    (Index2 *) REALLOC((VOIDP) tabP->next, sizeof(Index2) * newSize);
  tabP->checkNextSize= newSize;
  { /* Link new cells onto free-list. We use virtual indexes. */
    Index2 *CONST next= tabP->next - nCols;	/* Virtualized next. */
    Index2 *CONST check= tabP->check - nCols;	/* Virtualized check. */
    CONST Size oldVSize= oldSize + nCols;	/* Virtualized old size. */
    CONST Size newVSize= newSize + nCols;	/* Virtualized new size. */
    CONST Index last= newVSize - 1;		/* Virt. index of last entry. */
    for (i= oldVSize; i < newVSize; i++) { 
      next[i]= i + 1; check[i]= i - 1;
    }

    /* last is virtual index of new list header. */
    if (oldSize == 0) { /* Initialization of table. */
      next[last]= nCols; check[nCols]= last;
    }
    else { /* oldVSize - 1 is virtual index of old header. */
      /* New 1st element is old 1st element. */
      next[last]= next[oldVSize - 1]; 
      check[next[last]]= last;
      next[oldVSize - 1]= oldVSize;   	
    }
    assert(LINK_INVARIANT(last));
    *nextP= next; *checkP= check; *lastP= last;
  }
  VOID_RET();
}

VOID initCompTab(tabP, nCols)
  CompTab *tabP;
  Count nCols;
{
  tabP->next= tabP->check= NULL; tabP->checkNextSize= 0;
  tabP->nCols= nCols;
  tabP->lastBase= NIL; tabP->lastNEntries= 0; tabP->maxUsed= 0;
  { 
    Index2 *dummyNext, *dummyCheck; 
    Index dummyLast;
    growCompTab(tabP, &dummyNext, &dummyCheck, &dummyLast);
  }
  VOID_RET();
}


/* Insert row containing nEntries entries into tabP.  Assumes that the
 * entries[] are sorted by input.  Specifically, entries[0].input must
 * be the minimum of all inputs & entries[nEntries - 1].input must be
 * the max. of all inputs.  Returns unvirtualized base index where
 * inserted.
 */
Index getRowBaseCompTab(tabP, entries, nEntries)
  CompTab *tabP;
  CONST Entry entries[];
  Count nEntries;
{
  CONST Index nCols= 	/* Caches tabP->nCols. */
    tabP->nCols;	
  Index2 *next= 	/* Caches & virtualizes tabP->next. */
    tabP->next - nCols;	
  Index2 *check= 	/* Caches & virtualizes tabP->check. */
    tabP->check - nCols;	
  Index last= tabP->checkNextSize + nCols - 1;
  CONST Index minInput= entries[0].input;
  Index base= tabP->lastBase;	/* Virtualized base where inserted. */
                                /* Initialization assumes same as last row. */
  Index i= NIL;
  assert(nEntries > 0);
  assert(minInput < tabP->nCols);
  if (nEntries == tabP->lastNEntries) {	/* Possibly same as previous. */
    Index2 *nextBase= &next[tabP->lastBase];
    Index2 *checkBase= &check[tabP->lastBase];
    for (i= 0; i < nEntries; i++) {
      CONST Index in= entries[i].input;
      assert(minInput <= in);	/* minInput is minimum of all inputs. */
      if (nextBase[in] != entries[i].value || checkBase[in] != in) break;
    }
  }
  if (i != nEntries) { /* Did not find previous row to be identical. */
    Index free1;	/* Virtualized index where we try to insert entry[0]. */
    assert(IS_FREE(last));	/* We have at least 1 free entry. */
    assert(LINK_INVARIANT(last));
    assert(nCols < tabP->checkNextSize); /* For foll. loop termination. */
    for (free1= last; next[free1] < minInput + nCols; free1= next[free1]) 
      ;
    do { /* Main search loop. */
      Index savedCheck;	  /* Value saved when check[] used for sentinel. */
      free1= next[free1];
      assert(free1 >= minInput + nCols);	/* By above for-loop. */

      /* Grow to ensure that all entries can definitely be inserted */
      if (free1 - minInput + 1 >= tabP->checkNextSize) { 
	/* Since free1 is virtual, & checkNextSize is not, test succeeds if 
	 * free1 closer than (nCols - minInput) to end of check/next. 
	 */
	growCompTab(tabP, &next, &check, &last); 
      }

      assert(IS_FREE(free1));
      assert(LINK_INVARIANT(free1));
      assert(free1 != last);	/* Else we'd have just grown check/next. */

      base= free1 - minInput;		/* Slide over for row origin. */


      /* We now try to insert all entries at base. */
      assert(IS_FREE(base + minInput));
      assert(LINK_INVARIANT(base + minInput));
      savedCheck= check[base + minInput];
      check[base + minInput]= UNFREE_VAL;   /* Sentinel for loop. */
      for (i= nEntries - 1; IS_FREE(base + entries[i].input); i--) {
	assert(entries[i].input < nCols);
	assert(minInput < entries[i].input); /* minInput is a minimum. */
	assert(LINK_INVARIANT(base + entries[i].input));
      }
      check[base + minInput]= savedCheck;     /* Remove above sentinel. */

      assert(i < nCols);

      if (i == 0) { /* Successful so far. */
	for (i= 0; i < nCols; i++) { /* Ensure no other row uses same base. */
	  if (check[base + i] == i) break;
	}
      }
      /* If (i == nCols) we have succeeded in finding a base. */
    } while (i < nCols); /* Try next free base. */

    tabP->lastBase= base;  tabP->lastNEntries= nEntries;

    /* Insert entries into next[] with col-index into check[] entries. */
    for (i= 0; i < nEntries; i++) { 
      CONST Index vIndex= base + entries[i].input;
      assert(vIndex < tabP->checkNextSize + nCols);
      assert(IS_FREE(vIndex));
      UNLINK(vIndex);
      next[vIndex]= entries[i].value; check[vIndex]= entries[i].input;
    }
    if (base + entries[nEntries - 1].input > tabP->maxUsed) {
      tabP->maxUsed= base + entries[nEntries - 1].input;
    }
  } /* if (i != nEntries) */
  return base - nCols;  /* Return unvirtualized base. */
}

Index trimCompTab(tabP, nextP, checkP)
  CompTab *tabP;
  Index2 **nextP;
  Index2 **checkP;
{
  CONST Index nCols= tabP->nCols;	/* Caches tabP->nCols. */
  Index trimSize= tabP->maxUsed - nCols + 1;
  Index i;
  tabP->next= 
    (Index2 *) REALLOC((VOIDP) tabP->next, trimSize * sizeof(Index2));
  tabP->check= 
    (Index2 *) REALLOC((VOIDP) tabP->check, trimSize * sizeof(Index2));
  tabP->checkNextSize= trimSize;
  for (i= 0; i < trimSize; i++) {
    if (tabP->check[i] >= nCols) {
      tabP->check[i]= nCols; tabP->next[i]= 0;
    }
  }
  *nextP= tabP->next; *checkP= tabP->check;
  return trimSize;
}

#if 0

/* 
   This code does not use the chained free-list and is consequently simpler.
   If ever reactivated, it may be a good idea to remember a maxUsed index
   so that when the compression-table is trimmed, we don't have to search
   for the largest used value.

   This code is about 10% slower than the more complex code above.
   That makes a typical difference of about 1% in the speed of the
   parser (probably less if the -v option is used).  Is the complexity
   worth it?  Probably not.  Discard the complex code if it has
   problems.

   This code uses full prototypes: will need to be K&Rd if reactivated.

*/


static VOID growCompTab(CompTab *tabP)
{
  enum { INC_FACTOR= 16 };
  CONST Index oldSize= tabP->checkNextSize;
  CONST Index newSize= oldSize + INC_FACTOR * tabP->nCols;
  Index i;
  if (newSize >= UINT2_MAX)
    fatal("compressed table too large at %d", newSize);
  tabP->check= 
    (Index2 *) REALLOC((VOIDP) tabP->check, sizeof(Index2) * newSize);
  tabP->next= 
    (Index2 *) REALLOC((VOIDP) tabP->next, sizeof(Index2) * newSize);
  tabP->checkNextSize= newSize;
  for (i= oldSize; i < newSize; i++) {
    tabP->check[i]= tabP->nCols;
  }
  VOID_RET();
}

VOID initCompTab(CompTab *tabP, Index nCols)
{
  tabP->next= tabP->check= NULL; tabP->checkNextSize= tabP->minFree= 0;
  tabP->nCols= nCols;
  tabP->lastBase= NIL; tabP->lastNEntries= 0;
  growCompTab(tabP);
  VOID_RET();
}


Index getRowBaseCompTab(CompTab *tabP, CONST Entry entries[], 
			Count nEntries)
/* Insert row with index rowN containing nEntries entries into tabP.
 * Assumes that the entries[] are sorted by input.  Specifically,
 * entries[0].input must be the minimum of all inputs.  Returns base
 * index where inserted.
 */
{
  Index2 *next= tabP->next;		/* Caches tabP->next. */
  Index2 *check= tabP->check;		/* Caches tabP->check. */
  CONST Index nCols= tabP->nCols;	/* Caches tabP->nCols. */
  CONST Index minInput= entries[0].input;
  Index base;
  Index i= NIL;
  assert(nEntries > 0);
  assert(minInput < tabP->nCols);
  if (nEntries == tabP->lastNEntries) {	/* Possible duplicate. */
    Index2 *nextBase= &next[tabP->lastBase];
    Index2 *checkBase= &check[tabP->lastBase];
    base= tabP->lastBase;  
    for (i= 0; i < nEntries; i++) {
      CONST Index in= entries[i].input;
      assert(minInput <= in);	/* minInput is minimum of all inputs. */
      if (nextBase[in] != entries[i].value || checkBase[in] != in) break;
    }
  }
  if (i != nEntries) {
    base= (tabP->minFree > minInput) ? tabP->minFree - minInput : 0;
    do { /* Main search loop. */
      /* We have at least 1 free entry. */
      assert(IS_FREE(tabP->checkNextSize - 1)); 

      /* Search for base so that at least minInput entry can be inserted. */
      while (!IS_FREE(base + minInput)) base++;

      /* Grow to ensure that all entries can definitely be inserted */
      if (base + nCols + 1 >= tabP->checkNextSize) {
	growCompTab(tabP); 
	next= tabP->next; check= tabP->check;	/* Recache. */
      }

      /* We now try to insert all entries at base. */
      assert(IS_FREE(base + minInput));
      check[base + minInput]= UNFREE_VAL;   /* Sentinel for loop. */
      for (i= nEntries - 1; IS_FREE(base + entries[i].input); i--) {
	assert(entries[i].input < tabP->nCols);
	assert(minInput < entries[i].input); /* minInput is a minimum. */
      }
      check[base + minInput]= FREE_VAL;     /* Remove above sentinel. */

      assert(i < nCols);

      if (i == 0) { /* Successful so far. */
	for (i= 0; i < nCols; i++) { /* Ensure base unused. */
	  if (check[base + i] == i) break;
	}
      }
      base++; /* Pessimistically assume we try next base. */

    } while (i < nCols); /* Try next free base. */

    tabP->lastBase= --base; 	/* Undo pessimistic increment; record base. */
    tabP->lastNEntries= nEntries;

    /* Insert entries into next[] with col-index into check[] entries. */
    for (i= 0; i < nEntries; i++) { 
      assert(base + entries[i].input < tabP->checkNextSize);
      assert(IS_FREE(base + entries[i].input));
      next[base + entries[i].input]= entries[i].value; 
      check[base + entries[i].input]= entries[i].input;
    }

    if (base + minInput == tabP->minFree) { /* Update minFree. */
      assert(!IS_FREE(tabP->minFree));
      for (i= tabP->minFree + 1; !IS_FREE(i); i++) {
	/* Problems if there are no free entries.  Unlikely. */
	assert(i < tabP->checkNextSize);
      }
      tabP->minFree= i;
    }
  } /* if (i != nEntries) */
  return base;  
}

Index trimCompTab(CompTab *tabP, Index2 **nextP, Index2 **checkP)
{
  Index2 *check= tabP->check;		/* Caches tabP->check. */
  CONST Index nCols= tabP->nCols;	/* Caches tabP->nCols. */
  Index i;
  for (i= tabP->checkNextSize; IS_FREE(i - 1); i--) ;
  tabP->checkNextSize= i;
  tabP->next= 
    (Index2 *) REALLOC((VOIDP) tabP->next, i * sizeof(Index2));
  tabP->check= 
    (Index2 *) REALLOC((VOIDP) tabP->check, i * sizeof(Index2));
  *nextP= tabP->next; *checkP= tabP->check;
  return i;
}

#endif
