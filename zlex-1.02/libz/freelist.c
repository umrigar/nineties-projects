/*

File:	 freelist.c
Purpose: Allocate memory in chunks.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "freelist.h"
#include "memalloc.h"

typedef struct Block {
  struct Block *succ; 		/* Next allocated block. */
  char mem[1];			/* Allocated memory. */
} Block;

/* Initialize free-list freListP for an allocation of elements of size
 * elementSize in chunks of nAlloc elements at a time (use when
 * FREE_LIST_INITIALIZER() cannot be used).
 */
VOID 
newFreeList(elementSize, nAlloc, freeListP)
  size_t elementSize;
  Count nAlloc;
  FreeList *CONST freeListP;
{
  freeListP->elementSize=
    (elementSize > sizeof(char*)) ? elementSize : sizeof(char*);
  freeListP->nAlloc= nAlloc;
  freeListP->freeP= NULL; freeListP->blocks= NULL;
}

/* Return a pointer to a new element allocated from the free-list pointed
 * to by freeListP.
 */
VOIDP allocFreeList(freeListP)
  FreeList *CONST freeListP;
{
  CONST size_t elementSize= freeListP->elementSize;
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
 * from the free-list pointed to by freeListP.
 */
VOID 
freeFreeList(elementP, freeListP)
  VOIDP elementP;
  FreeList *CONST freeListP;
{
  *(char**)elementP= freeListP->freeP;
  freeListP->freeP= elementP;
  VOID_RET();
}

/* Destroy all allocated and unallocated storage addressed by the
 * free-list pointed to by freeListP.
 */
VOID 
destroyFreeList(freeListP)
  FreeList *CONST freeListP;
{
  Block *p, *succ;
  for (p= (Block*)freeListP->blocks; p != NULL; p= succ) {
    succ= p->succ; FREE(p);
  }
  freeListP->freeP= NULL; freeListP->blocks= NULL;
  VOID_RET();
}
