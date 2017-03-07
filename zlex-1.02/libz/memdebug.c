/*

File:	 memdebug.c
Purpose: Memory allocation routines to facilitate debugging memory problems.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "area.h"
#include "errorz.h"
#include "memalloc.h"


/*

This file can be used to ensure that the pointers returned to the
system via free and realloc were indeed obtained from the system using
calloc or malloc.  Achieved by rerouting all memory
allocation/deallocation request to the routines in this file.  This is
done by the macros MALLOC(), CALLOC(), REALLOC() and FREE() defined in
memAlloc.h (which add __FILE__ and __LINE__ information).  During
execution, memDebug.log will record all alloc/dealloc requests.

*/

#define PRG_NAME	"memDebug"
#define LOGFILE		"memdebug.log"

typedef struct {
  VOIDP memPtr;		/* Allocated memory pointer. */
  ConstString file;	/* File name where request made. */
  Index2 lineN;		/* Line # of allocation request. */
  Index2 succ;		/* Next entry on hash-chain. */
} MemPtr;

static struct {
  MemPtr *area;		/* Holds MemPtr's. */
  Index next;		/* Next free index in area. */
  Size size;		/* Current # of allocated entries in area. */
} memPtrs;

enum {
  MEM_PTRS_INC= 1024
};
#define MEM_PTRS(i) (memPtrs.area[i])

#define HASH_SIZE 809	/* Some prime #. */
static Index2 hashTab[HASH_SIZE];

#define HASH_PTR(p)	((((ULong) p) >> 3) % HASH_SIZE)

static struct {		/* Global scalars. */
  FILE *logFile;	/* File in which memory requests are logged. */
  Index2 freeList;	/* List of MemPtrswhich have been freed. */
  Count4 nAlloc;	/* # of allocations which haven't been freed. */
  Boolean beenInit;	/* True if module has been initialized. */
} globals;
#define G globals

#define INIT() 								\
  do {									\
    if (!G.beenInit) doInit(); 						\
  } while (0)

static VOID doInit(VOID_ARGS);
static VOIDP addMemPtr(ConstString file, Index lineN, VOIDP p);
static VOID delMemPtr(ConstString file, Index lineN, VOIDP p);

static VOID 
doInit()
{
  Index i;
  for (i= 0; i < HASH_SIZE; i++) hashTab[i]= NIL;
  G.logFile= fopen(LOGFILE, "w");
  G.freeList= NIL;
  G.beenInit= TRUE;
  G.nAlloc= 0;
  VOID_RET();
}

static VOIDP 
addMemPtr(file, lineN, p)
  ConstString file;
  Index lineN;
  VOIDP p;
{
/* Add p to table (without checking if it is there). */
  CONST Index h= HASH_PTR(p);
  Index i;
  INIT();
  if (!p) libzError(ER_ERROR|ER_FATAL|ER_NL|ER_SYS, "Out of memory.");
  fprintf(G.logFile, "%s:%d `%p' allocated.\n", file, lineN, p);
  if (G.freeList != NIL) {
    i= G.freeList; G.freeList= MEM_PTRS(G.freeList).succ;
  }
  else {
    if (memPtrs.next >= memPtrs.size) {
      memPtrs.area= (MemPtr*)
	reallocCheck(memPtrs.area, 
		     (memPtrs.size+= MEM_PTRS_INC)*sizeof(MemPtr));
    }
    i= memPtrs.next++;
  }
  MEM_PTRS(i).memPtr= p; MEM_PTRS(i).file= file; MEM_PTRS(i).lineN= lineN;
  MEM_PTRS(i).succ= hashTab[h]; hashTab[h]= i;
  G.nAlloc++;
  return p;
}

static VOID 
delMemPtr(file, lineN, p)
  ConstString file;
  Index lineN;
  VOIDP p;
{
  CONST Index h= HASH_PTR(p);
  Index i;
  Index last= NIL;
  INIT();
  if (!p) VOID_RET();
  fprintf(G.logFile, "%s:%d `%p' deallocated.\n", file, lineN, p);
  for (i= hashTab[h]; i != NIL; i= MEM_PTRS(i).succ) {
    if (MEM_PTRS(i).memPtr == p) {
      if (last == NIL) {
	hashTab[h]= MEM_PTRS(i).succ;
      }
      else {
	MEM_PTRS(last).succ= MEM_PTRS(i).succ;
      }
      MEM_PTRS(i).succ= G.freeList; G.freeList= i;
      G.nAlloc--;
      VOID_RET();
    }
    last= i;
  }
  /* If we get here we have an error. */
  fprintf(G.logFile, "%s:%d *** Memory deallocation error for `%p'.\n", 
	  file, lineN, p);
  fclose(G.logFile);
  libzError(0, "%s:%d %s: Memory deallocation error for `%p'.\n",
	    file, lineN, PRG_NAME, p);
  libzError(ER_FATAL|ER_NL, "Check %s for details.", LOGFILE);
  VOID_RET();
}

VOID 
memAllocations(fName, lineN)
  ConstString fName;
  UInt lineN;
{
  Index i;
  INIT();
  fprintf(G.logFile, "%s:%d:***** BEGIN ALLOCATIONS\n", fName, lineN);
  for (i= 0; i < HASH_SIZE; i++) {
    Index j;
    for (j= hashTab[i]; j != NIL; j= MEM_PTRS(j).succ) {
      CONST MemPtr *mP= &MEM_PTRS(j);
      fprintf(G.logFile, "%s:%d `%p' present.\n", 
	      mP->file, mP->lineN, mP->memPtr);
    }
  }
  fprintf(G.logFile, "%s:%d:***** END ALLOCATIONS\n", fName, lineN);
}

VOIDP 
mallocDebug(file, lineN, s)
  ConstString file;
  Index lineN;
  Size s;
{
  return addMemPtr(file, lineN, (VOIDP)malloc((SIZE_T)s));
}

VOIDP 
callocDebug(file, lineN, n, s)
  ConstString file;
  Index lineN;
  Count n;
  Size s;
{
  return addMemPtr(file, lineN, (VOIDP)calloc((SIZE_T)n, (SIZE_T)s));
}

VOIDP 
reallocDebug(file, lineN, p, s)
  ConstString file;
  Index lineN;
  VOIDP p;
  Size s;
{
  delMemPtr(file, lineN, p); 
  return addMemPtr(file, lineN, (VOIDP)realloc(p, (SIZE_T)s)); 
}

VOID 
freeDebug(file, lineN, p)
  ConstString file;
  Index lineN;
  VOIDP p;
{
  delMemPtr(file, lineN, p); free(p);
  VOID_RET();
}


