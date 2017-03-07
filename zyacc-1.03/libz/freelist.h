/*

File:	 freelist.h
Purpose: Allocate memory in chunks.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _FREELIST
#define _FREELIST

#include "ccport.h"

typedef VOIDP FreeList;

/* Allocate a new free list for the  allocation of elements of size
 * elementSize in chunks of nAlloc elements at a time.
 */
FreeList newFreeList PROTO((SIZE_T elementSize, Count nAlloc));

/* Return a pointer to a new element allocated from the free-list 
 * freeList.
 */
VOIDP allocFreeList PROTO((FreeList freeList));

/* Make the storage pointed to by elementP available for future allocation
 * from the free-list freeList.
 */
VOID freeFreeList PROTO((VOIDP elementP, FreeList freeList));

/* Destroy all allocated and unallocated storage addressed by the
 * free-list freeList.
 */
VOID destroyFreeList PROTO((FreeList freeList));

#endif /* ifndef _FREELIST */
