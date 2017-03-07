/*

File:	 memdebug.c
Purpose: Memory allocation routines to facilitate debugging memory problems.

Last Update Time-stamp: <97/07/30 22:59:43 zdu>

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


/* IMPERATIVE THAT NO OTHER LIBZ MODULES BE INCLUDED */
#include "memalloc.h"	

#include <stdio.h>

/*

This file can be used to ensure that the pointers returned to the
system via free and realloc were indeed obtained from the system using
calloc or malloc.  Achieved by rerouting all memory
allocation/deallocation request to the routines in this file.  This is
done by the macros MALLOC(), CALLOC(), REALLOC() and FREE() defined in
memAlloc.h (which add __FILE__ and __LINE__ information).  During
execution, memDebug.log will record all alloc/dealloc requests.

These routines also setup fences around each allocated block
initialized to a unique pattern and check opportunistically and
whenever a block is freed that the fences have not been corrupted.

To allow debugging libz library routines with MEM_DEBUG on, this file
does not use any libz routines (else there is a potential for an 
infinite loop).

*/


#define PRG_NAME	"memdebug"
#define LOGFILE_ENV	"MEM_DEBUG_LOG"

typedef struct MemBlk {	/* information saved for each user block */
  VOIDP uAddr;		/* address handed back to user */
  SIZE_T size;		/* # of user bytes in block */
  ConstString file;	/* file name where allocation request was made. */
  Index2 lineN;		/* line # of allocation request. */
  struct MemBlk *succ;	/* successor in hash-chain/freelist */
} MemBlk;

static struct {		/* Globals. */
  FILE *logFile;	/* file in which memory requests are logged. */
  MemBlk **hashTab;	/* hash-table */
  MemBlk *freeList;	/* free-list */
  Index hashMask;	/* current hash mask */
  Count nEntries;	/* # of entries currently in table */
  Boolean beenInit;	/* True if module has been initialized. */
} globals;
#define G globals

#define INIT() 								\
  do {									\
    if (!G.beenInit) doInit(); 						\
  } while (0)

enum {
  INIT_HASH_SIZE= 256,		/* initial hashsize (MUST be power of 2) */
  BLK_ALLOC= 256,		/* allocate this many MemBlks at a time */
  MAX_LOAD_FACTOR= 4		/* rehash if hashtable load factor > this */
};


static VOID doInit PROTO((VOID_ARGS));
static MemBlk *newBlks PROTO((VOID_ARGS));
static VOID rehash PROTO((VOID_ARGS));
static VOID putHashTab PROTO((VOIDP uAddr, ConstString file, Index lineN,
			      SIZE_T size));
static MemBlk *delHashTab PROTO((VOIDP uAddr));

static VOID setFence PROTO((UChar *CONST uAddr, SIZE_T size));
static VOID checkFence PROTO((CONST MemBlk *CONST memBlkP, 
			      ConstString file, Index lineN));
static VOID fenceError PROTO((CONST MemBlk *CONST memBlkP,
			      ConstString file, Index line,
			      Index fenceIndex, Index fenceValue, 
			      BooleanX isHi));
static VOIDP addMemBlk PROTO((ConstString file, Index lineN, SIZE_T s));
static VOID delMemBlk PROTO((ConstString file, Index lineN, VOIDP p));
static VOID currentAllocations PROTO((ConstString file, Index lineN));

static VOID 
doInit()
{
  ConstString envLogName= (ConstString)getenv(LOGFILE_ENV);
  G.hashMask= INIT_HASH_SIZE - 1;
  G.hashTab= callocCheck(INIT_HASH_SIZE, sizeof(MemBlk*));
  G.freeList= newBlks();
  if (envLogName) {
    G.logFile= fopen(envLogName, "w");
    if (G.logFile == NULL) {
      fprintf(stderr, "could not open %s for memory logging\n", envLogName);
      exit(1);
    }
    setvbuf(G.logFile, NULL, _IOLBF, 0);
  }
  G.beenInit= TRUE;
  VOID_RET();
}

static MemBlk *
newBlks()
{
  MemBlk *p= (MemBlk*)mallocCheck(sizeof(MemBlk) * BLK_ALLOC);
  Index i;
  for (i= 0; i < BLK_ALLOC - 1; i++) {
    p[i].succ= &p[i + 1];
  }
  p[i].succ= NULL;
  return &p[0];
}

#define HASH_CODE(p)		(((ULong)p) >> 3)


static VOID
rehash() 
{
  CONST Index oldSize= G.hashMask + 1;
  CONST Index newSize= (G.hashMask + 1)*2;
  CONST Index newMask= newSize - 1;
  MemBlk **newHashTab= callocCheck(newSize, sizeof(MemBlk*));
  MemBlk **oldHashTab= G.hashTab;
  Index i;
  for (i= 0; i < oldSize; i++) {
    MemBlk *p;
    MemBlk *pSucc;
    for (p= oldHashTab[i]; p != NULL; p= pSucc) {
      CONST Index h= HASH_CODE(p->uAddr) & newMask;
      pSucc= p->succ;
      p->succ= newHashTab[h]; newHashTab[h]= p;
    }
  }
  freeCheck(oldHashTab);
  G.hashMask= newMask; G.hashTab= newHashTab;
  VOID_RET();
}

/* Add block at user address uAddr to hashtable, allocating a new freelist
 * block for it.
 */
static VOID
putHashTab(uAddr, file, lineN, size)
  VOIDP uAddr;
  ConstString file;
  Index lineN;
  SIZE_T size;
{
  MemBlk *blkP= (G.freeList == NULL) ? newBlks() : G.freeList;
  Index h;
  if (G.nEntries >= MAX_LOAD_FACTOR * (G.hashMask + 1)) rehash();
  G.freeList= blkP->succ; 
  blkP->uAddr= uAddr; blkP->size= size; blkP->file= file; blkP->lineN= lineN;
  h= HASH_CODE(uAddr) & G.hashMask;
  blkP->succ= G.hashTab[h]; G.hashTab[h]= blkP;
  G.nEntries++;
  VOID_RET();
}

/* Delete address uAddr from hashtable.  If found return block, else NULL. */
static MemBlk *
delHashTab(uAddr)
  VOIDP uAddr;
{
  MemBlk **hashTab= G.hashTab;
  CONST Index h= HASH_CODE(uAddr) & G.hashMask;
  MemBlk *p;
  MemBlk *p0= NULL;
  for (p0= NULL, p= hashTab[h]; p != NULL; p0= p, p= p->succ) {
    if (p->uAddr == uAddr) {
      if (p0 == NULL) {
	hashTab[h]= p->succ;
      }
      else {
	p0->succ= p->succ;
      }
      G.nEntries--;
      return p;
    }
  }
  return NULL;
}



enum {			/* Fence parameters */
  FENCE_LO_SIZE= 256,	/* size of fence at lower addr than protected blk */
  FENCE_HI_SIZE= 256,	/* size of fence at higher addr than protected blk */
  FENCE_MIN= 0x3,	/* Inclusive lower bound on value written in fence.
			 * avoid 0, 1, 2 as programs have some preference
			 * for those values.
			 */
  FENCE_MAX= 0xFF,	/* Exclusive uppder bound on value written in fence.
			 * avoiding 0xFF as my programs have a preference
			 * for it.
			 */
  FENCE_START= 0x1B	/* start with a weird value; then wrap around */
};

/* Set up fences for user block at uAddr having size bytes.  Assumes fence
 * memory has been allocated around uAddr.
 */
static VOID
setFence(uAddr, size) 
  UChar *CONST uAddr;
  SIZE_T size;
{
  Index i;
  Index v= FENCE_START;
  UChar *p= uAddr - 1;
  for (i= 0; i < FENCE_LO_SIZE; i++) {
    p[-i]= v;
    v++;
    if (v >= FENCE_MAX) v= FENCE_MIN;
  }
  p= uAddr + size; v= FENCE_START;
  for (i= 0; i < FENCE_HI_SIZE; i++) {
    p[i]= v;
    v++;
    if (v >= FENCE_MAX) v= FENCE_MIN;
  }
}

static VOID
checkFence(memBlkP, file, line)
  CONST MemBlk *CONST memBlkP;
  ConstString file;
  Index line;
{
  CONST SIZE_T size= memBlkP->size;
  CONST UChar *CONST uAddr= memBlkP->uAddr;
  Index i;
  Index v= FENCE_START;
  CONST UChar *p= uAddr - 1;
  for (i= 0; i < FENCE_LO_SIZE; i++) {
    if (p[-i] != v) fenceError(memBlkP, file, line, i, p[-i], FALSE);
    v++;
    if (v >= FENCE_MAX) v= FENCE_MIN;
  }
  p= uAddr + size; v= FENCE_START;
  for (i= 0; i < FENCE_HI_SIZE; i++) {
    if (p[i] != v) fenceError(memBlkP, file, line, i, p[i], TRUE);
    v++;
    if (v >= FENCE_MAX) v= FENCE_MIN;
  }
}

static VOID
fenceError(memBlkP, file, line, fenceIndex, fenceValue,  isHi)
  CONST MemBlk *CONST memBlkP;
  ConstString file;
  Index line;
  Index fenceIndex;
  Index fenceValue;
  BooleanX isHi;
{
  fprintf(stderr,
    "%s %s:%d: *** memdebug fence error %d@[%d] %s block allocated at %s:%d\n",
	  PRG_NAME, 
	  file == NULL ? "" : file, line, (int)fenceValue, (int)fenceIndex,  
	  isHi ? "above" : "below", memBlkP->file, (int)memBlkP->lineN);
  if (G.logFile) {
    fprintf(G.logFile,
      "%s:%d: *** memdebug fence error %d@[%d] %s block allocated at %s:%d\n",
	    file == NULL ? "" : file, line, (int)fenceValue, (int)fenceIndex,  
	    isHi ? "above" : "below", memBlkP->file, (int)memBlkP->lineN);
  }
}

/* Add p to table (without checking if it is there). Allocate requested
 * memory block (including fences around it) and return user address of
 * block.
 */
static VOIDP
addMemBlk(file, lineN, size)
  ConstString file;
  Index lineN;
  SIZE_T size;
{
  CONST SIZE_T size0= FENCE_LO_SIZE + size + FENCE_HI_SIZE;
  UChar *p0= mallocCheck(size0);
  VOIDP p= p0 + FENCE_LO_SIZE;
  INIT();
  setFence(p, size);
  putHashTab(p, file, lineN, size);
  if (G.logFile) {
    fprintf(G.logFile, "%s:%d: allocated %d bytes\n", file, lineN, size);
  }
  return p;
}

static VOID 
delMemBlk(file, lineN, p)
  ConstString file;
  Index lineN;
  VOIDP p;
{
  MemBlk *blkP;
  INIT();
  if (p == NULL) return;
  blkP= delHashTab(p);
  if (blkP == NULL) {
    fprintf(stderr, "%s:%d: *** %s deallocation error\n",
	    file, lineN, PRG_NAME);
    if (G.logFile) {
      fprintf(G.logFile, "%s:%d *** deallocation error\n", 
	      file, lineN);
    }
  }
  else {
    checkFence(blkP, file, lineN);
    free((UChar*)p - FENCE_LO_SIZE);
    if (G.logFile) {
      fprintf(G.logFile, 
	      "%s:%d: freed %d bytes which were allocated at %s:%d\n", 
	      file, lineN, blkP->size, blkP->file, blkP->lineN);
    }
    blkP->succ= G.freeList; G.freeList= blkP;
  }
  VOID_RET();
}
      
static VOID
currentAllocations(file, lineN) 
  ConstString file;
  Index lineN;
{
  MemBlk **hashTab= G.hashTab;
  Index hashSize= G.hashMask + 1;
  Index i;
  INIT();
  for (i= 0; i < hashSize; i++) {
    CONST MemBlk *blkP;
    for (blkP= hashTab[i]; blkP != NULL; blkP= blkP->succ) {
      fprintf(G.logFile, "block allocated at %s:%d present\n", 
	      blkP->file, blkP->lineN);
      checkFence(blkP, file, lineN);
    }
  }
  VOID_RET();
}

VOID 
memAllocations(fName, lineN)
  ConstString fName;
  UInt lineN;
{
  if (G.logFile) {
    fprintf(G.logFile, "%s:%d:***** BEGIN ALLOCATIONS\n", fName, lineN);
    currentAllocations(fName, lineN);
    fprintf(G.logFile, "%s:%d:***** END ALLOCATIONS\n", fName, lineN);
  }
}

VOID 
memAtExit()
{
  if (G.logFile) {
    fprintf(G.logFile, "At Exit:***** BEGIN ALLOCATIONS\n");
    currentAllocations(NULL, 0);
    fprintf(G.logFile, "At Exit:***** END ALLOCATIONS\n");
    fclose(G.logFile);
  }
}

VOIDP 
mallocDebug(file, lineN, s)
  ConstString file;
  Index lineN;
  SIZE_T s;
{
  return addMemBlk(file, lineN, s);
}

VOIDP 
callocDebug(file, lineN, n, s)
  ConstString file;
  Index lineN;
  Count n;
  SIZE_T s;
{
  UChar *p= addMemBlk(file, lineN, n * s);
  memset(p, '\0', n * s);
  return p;
}

VOIDP 
reallocDebug(file, lineN, p, s)
  ConstString file;
  Index lineN;
  VOIDP p;
  SIZE_T s;
{
  VOIDP p1= addMemBlk(file, lineN, s); 
  if (p != NULL) {
    memcpy(p1, p, s);
    delMemBlk(file, lineN, p); 
  }
  return p1;
}

VOID 
freeDebug(file, lineN, p)
  ConstString file;
  Index lineN;
  VOIDP p;
{
  delMemBlk(file, lineN, p);
  VOID_RET();
}


