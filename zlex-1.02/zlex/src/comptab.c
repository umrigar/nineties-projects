/*

File:	 comptab.c
Purpose: Table compression routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/*			  TABLE COMPRESSION.				*/

/*

We chain free entries together in a doubly-linked circular list
without using any additional storage.  We use next[] for
forward-links, check[] for backward-links.  To easily distinguish
which entries are free, we offset the free-list backward-link pointers
in the check[] array by nCols (the # of columns in the table being
compressed).  Unfree check[] entries will contain data which will
always be less than nCols.  We also use the last entry of the
check/next array as the header for this circular list, making it
possible to unlink entries from the list without any boundary checks.
Hence the last entry in check/next is always free.

*/

#include "comptab.h"

#include "memalloc.h"
#include "error.h"

typedef struct {	/* Used locally for compressed table info. */
  CompTabVec *next;	/* Current next[] vector. */
  CompTabVec *check;	/* Current check[] vector. */
  CompTabVec *base;	/* Current base vector. */
  Count nRows;		/* # of rows. */
  Count nCols;		/* # of columns. */
  Size checkNextSize;	/* Total # of allocated entries in check/next. */
  Count nTransitions;	/* Total # of non-default transitions stored. */
  Index lastNEntries;	/* # of significant entries in last row inserted. */
  Index lastBase;	/* Base for last row insertion. */
  Index maxUsed;	/* Index of last used location. */
} TabX;

/*			    STATIC PROTOTYPES				*/

static VOID initTabX PROTO((Count nRows, Count nCols, TabX *tabXP));
static VOID growCompTab PROTO((TabX *tabXP));
static Index getRowBaseCompTab PROTO((TabX *tabXP, CONST CompTabEntry entries[],
				      Count nEntries));
static VOID completeCompTab PROTO((CONST CompTabEntry *rowNEntries,
				   Count nDone, CONST TabX *CONST tabXP,
				   CompTab *CONST compTabP));
static int compTabVecCmp PROTO((CONST VOIDP vP1, CONST VOIDP vP2));
static int orderCmp PROTO((CONST VOIDP vP1, CONST VOIDP vP2));




/*			MACRO DEFINITIONS				*/

/* The following macros assume a auto var called nCols which mirrors
 * the field in the TabX.  Also assumes a variable check which is the
 * start of the check[] array and variable last which is the
 * index of the last entry in check[]/next[].
 */

/* We refer to the free-list links actually stored in the check[]
 * array as LINKs, whereas we use INDEX to refer to the index of a 
 * check[]/next[] entry.  The following macros offset LINKs by nCols
 * from INDEXes.
 */
#define LINK_TO_INDEX(link)	((link) - nCols)
#define INDEX_TO_LINK(index)	((index) + nCols)

#define FREE_VAL	(nCols)
#define UNFREE_VAL	(0)

#define IS_FREE(index)	(check[index] >= nCols)

/* Usual invariant for a doubly-linked list. */
#define LINK_INVARIANT(index)						\
  (next[LINK_TO_INDEX(check[index])] == (index) &&			\
   LINK_TO_INDEX(check[next[index]]) == (index))

/* Unlink list entry at index from doubly-linked circular list. */
#define UNLINK(index) 							\
  do {									\
    CONST Index succ= next[index];					\
    CONST Index pred= LINK_TO_INDEX(check[index]);			\
    assert(index != last);	/* Never unlink last cell. */		\
    assert(next[pred] == index);/* Circular list inv. */		\
    assert(LINK_TO_INDEX(check[succ]) == index);/* Circular list inv. */\
    next[pred]= succ;							\
    check[succ]= INDEX_TO_LINK(pred);					\
  } while (0)

/* The max. size of a next/check vector.  We subtract (nCols + 1) to
 * because of the offsetting used in setting up the LINKs.
 */
#define MAX_VEC_SIZE	((ULong)((CompTabVec)(~0L)) - (nCols + 1))


/*			   TABLE INITIALIZATION				*/

static VOID 
initTabX(nRows, nCols, tabXP)
  Count nRows;
  Count nCols;
  TabX *tabXP;
{
  tabXP->nRows= nRows; tabXP->nCols= nCols;
  tabXP->next= tabXP->check= NULL; tabXP->checkNextSize= 0;
  tabXP->lastBase= NIL; tabXP->lastNEntries= 0; tabXP->maxUsed= 0;
  tabXP->base= MALLOC_VEC(nRows, CompTabVec);
  tabXP->nTransitions= 0;
  growCompTab(tabXP);
  VOID_RET();
}


/*			  GROW TABLES					*/

/* Grow the next/check vectors, linking the new entries into the free-list. */
static VOID 
growCompTab(tabXP)
  TabX *tabXP;
{
  enum { INC_FACTOR= 4 };
  CONST Size oldSize= tabXP->checkNextSize;
  CONST Count nCols= tabXP->nCols;
  CONST Size newSize= oldSize + INC_FACTOR * nCols;
  if (newSize >= MAX_VEC_SIZE) {
    fatal("compressed table too large at %d", newSize);
  }
  else {
    CompTabVec *CONST check= REALLOC_VEC(tabXP->check, newSize, CompTabVec);
    CompTabVec *CONST next=  REALLOC_VEC(tabXP->next, newSize, CompTabVec);
    CONST Index last= newSize - 1;		/* Index of last entry. */
    Index i;

    tabXP->check= check; tabXP->next= next; 
    tabXP->checkNextSize= newSize;

    for (i= oldSize; i < newSize; i++) { /* Link new cells. */
      next[i]= i + 1; 
      check[i]= INDEX_TO_LINK(i - 1);
    }

    /* last is index of new list header. */
    if (oldSize == 0) { /* Initialization of table. */
      next[last]= 0; check[0]= INDEX_TO_LINK(last);
    }
    else { /* oldSize - 1 is index of old header. */
      next[last]= next[oldSize - 1]; /* New 1st element is old 1st element. */
      check[next[last]]= INDEX_TO_LINK(last);
      next[oldSize - 1]= oldSize;
    }
    assert(LINK_INVARIANT(last));
  }
  VOID_RET();
}

/*			     INSERT ROW					*/	

/* Insert row containing nEntries entries into tabP.  entries[0].input
 * must be the minimum of all inputs & entries[nEntries - 1].input
 * must be the max. of all inputs.  Returns base index where inserted.
 */
static Index 
getRowBaseCompTab(tabXP, entries, nEntries)
  TabX *tabXP;
  CONST CompTabEntry entries[];
  Count nEntries;
{
  CONST Index nCols= tabXP->nCols;	/* Caches tabXP->nCols. */	
  Index2 *next= tabXP->next;		/* Caches tabXP->next. */
  Index2 *check= tabXP->check;		/* Caches tabXP->check. */	
  Index last= tabXP->checkNextSize - 1;
  CONST Index minInput= entries[0].input;
  Index base= tabXP->lastBase;	/* base where inserted. */
                                /* Initialization assumes same as last row. */
  Index i= NIL;
  assert(nEntries > 0);
  assert(minInput < tabXP->nCols);
  if (nEntries == tabXP->lastNEntries) {	/* Possibly same as previous. */
    Index2 *nextBase= &next[base]; 
    if (nextBase[entries[nEntries - 1].input] == entries[nEntries - 1].value) {
      /* Last entry may match: check all of them now. */
      Index2 *checkBase= &check[base];
      for (i= 0; i < nEntries; i++) {
	CONST Index in= entries[i].input;
	assert(minInput <= in);	/* minInput is minimum of all inputs. */
	if (nextBase[in] != entries[i].value || checkBase[in] != in) break;
      }
    }
  }
  if (i != nEntries) { /* Did not find previous row to be identical. */
    Index free1;	/* Virtualized index where we try to insert entry[0]. */
    assert(IS_FREE(last));	/* We have at least 1 free entry. */
    assert(LINK_INVARIANT(last));
    assert(nCols < tabXP->checkNextSize); /* For foll. loop termination. */
    for (free1= last; next[free1] < minInput; free1= next[free1]) {
      /* Searches for a free entry with index >= minInput. */
    }
    do { /* Main search loop. */
      Index savedCheck;	  /* Value saved when check[] used for sentinel. */
      free1= next[free1];
      assert(free1 >= minInput);	/* By above for-loop. */

      /* Grow to ensure that all entries can definitely be inserted */
      if (free1 - minInput + nCols + 1 >= tabXP->checkNextSize) { 
	/* + 1 ensures that last stays free. */
	growCompTab(tabXP);
	next= tabXP->next; check= tabXP->check; 
	last= tabXP->checkNextSize - 1; 
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

      assert(i < nCols); /* Important for termination of search loop. */

      if (i == 0) { /* Successful so far. */
	for (i= 0; i < nCols; i++) { /* Ensure no other row uses same base. */
	  if (check[base + i] == i) break;
	}
      }
      /* If (i == nCols) we have succeeded in finding a base. */
    } while (i < nCols); /* Try next free base. */

    tabXP->lastBase= base;  tabXP->lastNEntries= nEntries;

    /* Insert entries into next[] with col-index into check[] entries. */
    for (i= 0; i < nEntries; i++) { 
      CONST Index ii= base + entries[i].input;
      assert(ii < tabXP->checkNextSize - 1);
      assert(IS_FREE(ii));
      UNLINK(ii);
      next[ii]= entries[i].value; check[ii]= entries[i].input;
    }
    if (base + entries[nEntries - 1].input > tabXP->maxUsed) {
      tabXP->maxUsed= base + entries[nEntries - 1].input;
    }
  } /* if (i != nEntries) previous row not identical. */
  return base; 
}


/*			COMPLETE TABLE					*/

/* Complete the compression table.  On entry, nDone contains the # of
 * rows from rowNEntries which have been processed (the remaining
 * rows should have zero non-default entries).  It sets up the base
 * values for the remaining rows to the smallest unused base values.
 * It trims and fills in unused entries in check/next.  Finally it
 * puts the final values into *compTabP.
 */
static VOID
completeCompTab(rowNEntries, nDone, tabXP, compTabP)
  CONST CompTabEntry *rowNEntries;
  Count nDone;
  CONST TabX *CONST tabXP;
  CompTab *CONST compTabP;
{
  CONST Index nRows= tabXP->nRows;
  CONST Index nCols= tabXP->nCols;
  CompTabVec *CONST base= tabXP->base;
  CompTabVec *check= tabXP->check;
  CompTabVec *next= tabXP->next;
  CompTabVec *CONST sortBase= MALLOC_VEC(nDone, CompTabVec);
  CONST Size nextSize= tabXP->maxUsed + 1;
  Size checkSize;
  CompTabVec fillVal;
  CompTabVec maxBase;
  Index i;
  for (i= 0; i < nDone; i++) {	
    assert(rowNEntries[i].value > 0); /* Rows with non-default entries. */
    sortBase[i]= tabXP->base[rowNEntries[i].input];
  }
  qsort(sortBase, nDone, sizeof(CompTabVec), compTabVecCmp);
  
  for (i= 1; i < nDone; i++) { /* Search for smallest unused base value. */
    if (sortBase[i] - sortBase[i - 1] > 1) {
      fillVal= sortBase[i - 1] + 1; break;
    }
  }
  if (i >= nDone) { /* Have not found a fillVal so far. */
    maxBase= fillVal= sortBase[nDone - 1] + 1; /* 1 more than largest base. */
  }
  else {
    maxBase= sortBase[nDone - 1];
  }
  FREE(sortBase);
  checkSize= maxBase + nCols;
  for (i= nDone; i < nRows; i++) { /* Fill in base for empty rows. */
    assert(rowNEntries[i].value == 0);
    base[rowNEntries[i].input]= fillVal; 
  }
  next= REALLOC_VEC(next, nextSize, CompTabVec);
  check= REALLOC_VEC(check, checkSize, CompTabVec);
  
  for (i= 0; i < nextSize; i++) { /* Fill in blank check/next entries. */
    if (check[i] >= nCols) {
      check[i]= nCols; next[i]= 0;
    }
  }
  for (; i < checkSize; i++) { /* Fill in tail check entries. */
    check[i]= nCols;
  }

  compTabP->base= base; compTabP->next= next; compTabP->check= check;
  compTabP->nextSize= nextSize; compTabP->checkSize= checkSize;
  compTabP->maxBase= maxBase; compTabP->nTransitions= tabXP->nTransitions;
  VOID_RET();
}

/* Compare two (CompTabVec*) values for qsort().  If two entries
 * compare equal (returning 0), result portability should not be
 * affected, since the sorted base values resulting from this
 * comparison is merely used to find an unused base value.
 */
static int 
compTabVecCmp(vP1, vP2)
  CONST VOIDP vP1;
  CONST VOIDP vP2;
{
  CompTabVec i1= *((CompTabVec*)vP1);
  CompTabVec i2= *((CompTabVec*)vP2);
  return (i1 < i2) ? -1 : (i1 > i2) ? 1 : 0;
}

/*			TOP-LEVEL COMPRESSION FUNCTION			*/

VOID
compressTab(nRows, nCols, nEntriesFnP, entriesFnP, compTabP)
  Count nRows;
  Count nCols;
  NEntriesFnP nEntriesFnP;
  EntriesFnP entriesFnP;
  CompTab *compTabP;
{
  CompTabEntry *CONST order= MALLOC_VEC(nRows, CompTabEntry);
  CompTabEntry *CONST entries= MALLOC_VEC(nCols, CompTabEntry);
  TabX tabX;
  Index i;

  initTabX(nRows, nCols, &tabX);
  nEntriesFnP(order);	/* Get # of entries for each row. */
  qsort(order, nRows, sizeof(CompTabEntry), orderCmp);
  for (i= 0; i < nRows && order[i].value > 0; i++) {
    Index rowN= order[i].input;
    Count nEntries= entriesFnP(rowN, entries);
    assert(nEntries == order[i].value);
    tabX.base[rowN]= getRowBaseCompTab(&tabX, entries, nEntries);
    tabX.nTransitions+= nEntries;
  }
  FREE(entries);
  completeCompTab(order, i, &tabX, compTabP);
  FREE(order);
}

/* Comparison function for qsort() to sort rows in order of
 * non-increasing # of entries.  If two rows have the same # of
 * entries, then instead of returning 0, to ensure portability we
 * return 1 or -1 depending on the row #.
 */
static int
orderCmp(vP1, vP2)
  CONST VOIDP vP1;
  CONST VOIDP vP2;
{
  typedef CompTabEntry Order;
  CONST Order *CONST oP1= (Order*)vP1;
  CONST Order *CONST oP2= (Order*)vP2;
  CONST Count n1= oP1->value;
  CONST Count n2= oP2->value;
  return n1 < n2 ? 1 : n1 > n2 ? -1 : (oP1->input < oP2->input) ? -1 : 1;
}


/* Free all dynamically allocated storage used by *compTabP. */
VOID 
delCompTab(compTabP)
  CompTab *compTabP;
{
  FREE(compTabP->base); compTabP->base= NULL;
  FREE(compTabP->next); compTabP->next= NULL; compTabP->nextSize= 0;
  FREE(compTabP->check); compTabP->check= NULL; compTabP->checkSize= 0;
  VOID_RET();
}

