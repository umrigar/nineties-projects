/*

File:	 comptab.h
Purpose: Table compression interface.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/
#ifndef _COMPTAB_H
#define _COMPTAB_H

#include "port.h"

/* 

Comb-vector compression of a 2-dimensional table into 3 vectors
base[], check[] and next[].  The lookup algorithm for tab[i][j] is:

tab[i][j]= (check[base[i]+j] == j) ? next[base[i]+j] : default[i];

The default[] vector is not calculated by this module, but by the caller
who is responsible for providing the non-default entries for a row to the
caller.

*/

/* This type limits the size of the tables. */
typedef Index2 CompTabVec;

typedef struct {	/* Entry for a row being compressed. */
  Index2 input;		/* The column index. */
  Index2 value;		/* The value stored at that column in the row. */
} CompTabEntry;

typedef struct {	/* To compress a table tab[nRows][nCols]. */
  CompTabVec *base;	/* nRow base entries. */
  CompTabVec *next;	/* next[base[i] + j] == tab[i][j] if next[] valid. */
  CompTabVec *check;	/* next[] valid iff check[base[i] + j] == j. */
  Size nextSize;	/* # of entries in next[]. */
  Size checkSize;	/* # of entries in check[]. */
  Index maxBase;	/* Max. value inserted into base[]. */
  Count nTransitions;	/* Total # of non-default transitions stored. */
} CompTab;

/* Function which fills in the # of non-default entries for each row.
 * Specifically for an entry in rowNEntries[], set its input component
 * to the row number and the value component to the number of non-default
 * entries for that row.  rowNEntries[] can be filled in in any order
 * with the only constraint being is that there be exactly 1 entry for
 * each row.  rowNEntries[] is guaranteed to be large enough.
 */
typedef VOID (*NEntriesFnP) PROTO((CompTabEntry rowNEntries[]));  

/* Function which returns the number of significant entries and fills
 * in the significant entries for rowN (with the smallest input first
 * and the largest input last).
 */
typedef Count (*EntriesFnP) PROTO((Index rowN, CompTabEntry entries[]));

/* Return compress-table vectors in *compTabP for a table having nRows
 * rows and nCols columns.  nEntriesFnP should point to a function
 * which fills in the number of non-default entries in each row and
 * entriesFnP should fill in the significant entries for a row (with
 * the smallest input first and the largest input last), while
 * returning the number of significant entries.
 */
VOID compressTab PROTO((Count nRows, Count nCols, NEntriesFnP nEntriesFnP,
			EntriesFnP entriesFnP, CompTab *compTabP));

/* Free all dynamically allocated storage used by *compTabP. */
VOID delCompTab PROTO((CompTab *compTabP));

#endif /* ifndef _COMPTAB_H */
