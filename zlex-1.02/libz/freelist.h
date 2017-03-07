/*

File:	 freelist.h
Purpose: Allocate memory in chunks.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _FREELIST
#define _FREELIST

#include "ccport.h"

typedef struct {
  size_t elementSize;		/* Size of each element. */
  Count nAlloc;			/* # of cells to allocate per chunk. */
  char *freeP;			/* Pointer to free-list. */
  VOIDP blocks;			/* Keep track of allocated blocks. */
} FreeList;

/* Used as a FreeList initializer. */
#define FREE_LIST_INITIALIZER(elementSize, nAlloc)			\
  {									\
    (((elementSize) > sizeof(char*)) ? (elementSize) : sizeof(char*)),	\
    (nAlloc),								\
    NULL,								\
    NULL								\
  }

/* Initialize free-list freListP for an allocation of elements of size
 * elementSize in chunks of nAlloc elements at a time (use when
 * FREE_LIST_INITIALIZER() cannot be used).
 */
VOID newFreeList PROTO((size_t elementSize, Count nAlloc,
			FreeList *CONST freeListP));

/* Return a pointer to a new element allocated from the free-list pointed
 * to by freeListP.
 */
VOIDP allocFreeList PROTO((FreeList *CONST freeListP));

/* Make the storage pointed to by elementP available for future allocation
 * from the free-list pointed to by freeListP.
 */
VOID freeFreeList PROTO((VOIDP elementP, FreeList *CONST freeListP));

/* Destroy all allocated and unallocated storage addressed by the
 * free-list pointed to by freeListP.
 */
VOID destroyFreeList PROTO((FreeList *CONST freeListP));

#endif /* ifndef _FREELIST */
