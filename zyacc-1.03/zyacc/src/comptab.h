/*

File:	 comptab.h
Purpose: Table compression interface.

Last Update Time-stamp: "97/07/10 18:06:57 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/
#ifndef _COMPTAB_H
#define _COMPTAB_H

#include "port.h"

typedef struct {	/* Entry for a row being compressed. */
  Index2 input;		/* The column index. */
  Index2 value;		/* The value stored at that column in the row. */
} Entry;

typedef struct {	/* To compress a table tab[nRows][nCols]. */
  Index2 *next;		/* next[base[i] + j] == tab[i][j] if next[] valid. */
  Index2 *check;	/* next[] valid iff check[base[i] + j] == j. */
  Size2 checkNextSize;	/* Total # of allocated entries in check/next. */
  Index2 nCols;		/* Maximum # of columns of all tables stored. */
  Index2 lastNEntries;	/* # of significant entries in last row inserted. */
  Index2 lastBase;	/* Virtualized base for last row insertion. */
  Index2 maxUsed;	/* Virtualized index of last used location. */
} CompTab;
  
#if 0	/* Activate for simpler code; about 10% slower than complex code. */
typedef struct {	/* To compress a table tab[nRows][nCols]. */
  Index2 *next;		/* next[base[i] + j] == tab[i][j] if next[] valid. */
  Index2 *check;	/* next[] valid iff check[base[i] + j] == i. */
  Size2 checkNextSize;	/* Total # of allocated entries in check/next. */
  Index2 minFree;	/* Min. free entry in next[]/check[]. */
  Index2 nCols;		/* Maximum number of columns of all tables stored. */
  Index2 lastNEntries;	/* # of significant entries in last row inserted. */
  Index2 lastBase;	/* Base at which last row was inserted. */
} CompTab;
#endif

VOID initCompTab PROTO((CompTab *tabP, Count nCols));
Index getRowBaseCompTab PROTO((CompTab *tabP, CONST Entry entries[], 
			       Count nEntries));
Index trimCompTab PROTO((CompTab *tabP, Index2 **next, Index2 **check));

#endif /* ifndef _COMPTAB_H */
