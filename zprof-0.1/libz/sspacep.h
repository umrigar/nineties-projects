/*

File:	 sspacep.h
Purpose: Private interface for string space routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _SSPACEP
#define _SSPACEP

#include "memalloc.h"
#include "strspace.h"

enum { 
  SS_BLOCK_SIZE= 4096	/* The nominal size of a string-space block. */
};

typedef struct SSBlock {
  struct SSBlock *pred;	/* Previous block in block chain. */
  char *chars;		/* Pointer to stored characters. */
} SSBlock;

typedef struct {
  SSBlock *blockP;		/* Ptr. to current block. */
  char *next;			/* Ptr. to next free char. */
  unsigned nFree;		/* # of free chars. in current block. */
  unsigned lastLen;		/* Length of last added string (including
				 * appended NUL if appendNUL. 
				 */
  Boolean appendNUL;		/* Whether NUL should be appended. */
} SSStruct;

typedef SSStruct *StrSpaceP;

char * 
_ZssMoreSpace PROTO((StrSpaceP ssP, unsigned size));

#endif /* ifndef _SSPACEP */

