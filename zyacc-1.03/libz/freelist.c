/*

File:	 freelist.c
Purpose: Allocate memory in chunks.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "freelist.h"
#include "memalloc.h"

typedef struct {
  SIZE_T elementSize;		/* Size of each element. */
  Count nAlloc;			/* # of cells to allocate per chunk. */
  char *freeP;			/* Pointer to free-list. */
  VOIDP blocks;			/* Keep track of allocated blocks. */
} FreeListX;


typedef struct Block {
  struct Block *succ; 		/* Next allocated block. */
  char mem[1];			/* Allocated memory. */
} Block;

/* Allocate a new free list for the  allocation of elements of size
 * elementSize in chunks of nAlloc elements at a time.
 */
VOIDP
newFreeList(elementSize, nAlloc)
  SIZE_T elementSize;
  Count nAlloc;
{
  FreeListX *CONST freeListP= MALLOC(sizeof(FreeListX));
  freeListP->elementSize=
    (elementSize > sizeof(char*)) ? elementSize : sizeof(char*);
  freeListP->nAlloc= nAlloc;
  freeListP->freeP= NULL; freeListP->blocks= NULL;
  return freeListP;
}

/* Return a pointer to a new element allocated from the free-list 
 * freeList.
 */
VOIDP 
allocFreeList(freeList)
  FreeList freeList;
{
  FreeListX *CONST freeListP= freeList;
  CONST SIZE_T elementSize= freeListP->elementSize;
  CONST Count nAlloc= freeListP->nAlloc;
  VOIDP p;
  if (freeListP->freeP == NULL) {
    char *p;
    Block *blockP= (Block*)MALLOC(sizeof(Block) + nAlloc * elementSize - 1);
    char *CONST freeP= &blockP->mem[0];
    CONST char *CONST endP= freeP + (nAlloc - 1)*elementSize;
    blockP->succ= (Block*)freeListP->blocks; 
    freeListP->blocks= (VOIDP)blockP;
    freeListP->freeP= freeP;
    for (p= freeP; p != endP; p+= elementSize) {
      *(char**)p= p + elementSize;
    }
    *(char**)p= NULL;
  }
  p= freeListP->freeP;
  freeListP->freeP= *(char**)p;
  return p;
}

/* Make the storage pointed to by elementP available for future allocation
 * from the free-list freeList.
 */
VOID 
freeFreeList(elementP, freeList)
  VOIDP elementP;
  FreeList freeList;
{
  FreeListX *CONST freeListP= freeList;
  *(char**)elementP= freeListP->freeP;
  freeListP->freeP= elementP;
  VOID_RET();
}

/* Destroy all allocated and unallocated storage addressed by the
 * free-list freeList.
 */
VOID 
destroyFreeList(freeList)
  FreeList freeList;
{
  FreeListX *CONST freeListP= freeList;
  Block *p, *succ;
  for (p= (Block*)freeListP->blocks; p != NULL; p= succ) {
    succ= p->succ; FREE(p);
  }
  freeListP->freeP= NULL; freeListP->blocks= NULL;
  FREE(freeListP);
  VOID_RET();
}
