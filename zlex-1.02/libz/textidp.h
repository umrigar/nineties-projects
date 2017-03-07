/*

File:	 textidp.h
Purpose: Private interface for text-id routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _STRNGIDP_H
#define _STRNGIDP_H

#include "area.h"
#include "memalloc.h"
#include "textid.h"

typedef Index2 TabIndex;

typedef struct {
  CONST char *text;	/* The stored text. */
  int hashSum;		/* Its hash sum (before %). */
  Size2 len;		/* # of character in stored stored text. */
  TabIndex succ;	/* Next entry in hash-chain. */
} TabEntry;

typedef struct {
  AreaX tab;		/* Where TabEntry s are stored. */
  TabIndex *hashTab;	/* Hash table. */
  Size2 hashSize;	/* Current size of hash-table: will be power of 2. */
  Boolean ignoreCase;  	/* TRUE if case is to be ignored. */
} TextIDStruct;

typedef TextIDStruct *TextIDP;

#define TAB(i)	(AX_ACCESS(textIDP->tab, TabEntry, i))

#define MAX_LOAD_FACTOR  4 /* Resize if #entries/hashSize above this. */

VOID _ZresizeHashTab PROTO((TextIDStruct *textIDInfoP));
int _ZhashText PROTO((TextIDStruct *textIDInfoP, 
		      CONST char *text, Size len));

#endif /* ifndef _STRNGIDP_H */
