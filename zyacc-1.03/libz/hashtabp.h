/*

File:	 hashtabp.h
Purpose: Private interface to hashtable routines

Last Update Time-stamp: <97/07/08 11:11:47 zdu>

Copyright (C) 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _HASHTABP_H
#define _HASHTABP_H

#include "hashtab.h"
#include "area.h"

typedef Index2 HashIndex;

typedef struct {
  HashVal val;		/* key value */
  HashCode code;	/* hashcode for key */
  HashIndex succ;	/* index of next entry in hashchain. */
} HashEntry;

typedef struct {
  HashIndex pred;	/* previous scope in chain */
  HashIndex succ;	/* next scope in chain */
  HashIndex frees;	/* list of entries deleted in this scope */
} ScopeEntry;

typedef struct {
  HashKey key;		/* entry key: NULL_HASH_KEY==> not a key/val entry. */
  union {
    HashEntry hashEntry;
    ScopeEntry scopeEntry;
  } u;
} Entry;

typedef struct {
  AreaX entries;	/* contains all the entries */
  HashIndex *tab;	/* currently allocated table */
  HashCodeFnP codeFnP;	/* hash-code function */
  HashEqualFnP equalFnP;/* equality function */
  Index maxLoadFactor;	/* max. load factor before grow */
  Count nEntries;	/* # of entries currently in table */
  HashScope scope;	/* current scope */
  Index2 mask;		/* 1 less than current size of tab. */
  Boolean isScoped;	/* TRUE if table is scoped */
  Boolean reorderOK;	/* TRUE if reordering of hash-chains permitted */
} HashTabStruct;

typedef HashTabStruct *HashTabP;

typedef struct {
  HashKey key;		/* key being iterated over */
  HashIndex current;	/* index of current entry in iteration */
  HashIndex limit;	/* no more iteration if current <= limit */
} *HashIterP;


#define ENTRIES(hashTabP, i)	AX_ACCESS((hashTabP)->entries, Entry, (i))
#define NENTRIES(hashTabP)	AX_NENTRIES((hashTabP)->entries)
#define NEXT_ENTRY(hashTabP)	AX_NEXT((hashTabP)->entries)

#endif /* #ifndef _HASHTABP_H */
