/*

File:	 isetp.h
Purpose: Private interface for integer-set routines.

Last Update Time-stamp: <97/07/30 17:10:49 zdu>

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#ifndef _ISETP
#define _ISETP

/* An ISet is maintained as a linked list of fixed-sized blocks of
 * bitmaps with each block used for a specified range of elements.
 * Within each bit-word, more significant bits represents lower
 * numbered elements than less significant bits.  An invariant which
 * is maintained is that no block is empty --- i.e. each block
 * contains at least one element.
 */
 


#ifdef DEBUG_ISET
#undef NDEBUG		/* Ensure debugging library has assertions enabled. */
#endif

#include "ccport.h"
#include "freelist.h"
#include "iset.h"

struct ISetBlk;		/* Block used in list of blocks. */

typedef UInt BitWord;	/* Most convenient for bit manipulation. */

/* # of bits per BitWord. */
#define WORD_BITS	((Int)(sizeof(BitWord) * CHAR_BIT))

#define ALL_1S_BITWORD	((BitWord)(~0L))

/* Define a bit-word with the MSB 1 and all other bits 0. */
#define WORD_WITH_MSB1							\
  ((BitWord)(ALL_1S_BITWORD ^ (ALL_1S_BITWORD >> 1)))

enum {
  /* # of bitmap Words per block.  Try for about 50 % block overhead. 
   * We don't make this too large, as these words will definitely be
   * wasted in the header.  On the other hand, a smaller value increases
   * the proportion of block overhead.
   */
  N_WORDS= 
    (Int)((sizeof(ISetElement) + sizeof(struct ISetBlk *) + 3)/sizeof(BitWord)),

  /* # of set elements stored within a block. */
  N_ELEMENTS_PER_BLK= (Int)(N_WORDS * WORD_BITS)
};


/* These blocks are used for both the contents of an ISet as well as
 * for the header of the list of blocks representing the ISet (for
 * list manipulation convenience, an ISet is a pointer to a dummy
 * header which is a ISetBlk (with all fields except succ wasted)).
 */
typedef struct ISetBlk {	/* A block used in a ISet. */
  union {
    struct {
      BitWord words[N_WORDS];	/* Bit words for this blk. */
      ISetElement lo;		/* Smallest element possible in this block. */
    } content;
    struct {			/* Used in header when DEBUG_ISET. */
      ConstString modName;	/* Name of owning module if DEBUG_ISET. */
      ConstString fileName;	/* File where ownership was established. */
      UInt lineN;		/* Line # where ownership was established. */
    } debug;
  } u1;
  struct ISetBlk *succ;		/* Next block in this set. */
} ISetBlk;

/* Smallest element which can be stored in a block. */
#define BLK_LO(blkP)		((blkP)->u1.content.lo)

/* One greater than largest element which can be stored in a block. */
#define BLK_HI(blkP)		((blkP)->u1.content.lo + N_ELEMENTS_PER_BLK)

/* Bit words stored in a block. */
#define BLK_WORDS(blkP)		((blkP)->u1.content.words)

/* Next block on linked list. */
#define BLK_SUCC(blkP)		((blkP)->succ)

/* Access macros for DEBUG_ISET fields stored in ISet header. */
#define HDR_OWNER(hdrP)		((hdrP)->u1.debug.modName)
#define HDR_FILE(hdrP)		((hdrP)->u1.debug.fileName)
#define HDR_LINE(hdrP)		((hdrP)->u1.debug.lineN)

/* Free-list which tracks free blocks. Accessed only by macros NEW_BLK()
 * and DEL_BLK() except for initialization.
 */
extern FreeList _ZISetFrees;

/* If DEBUG_ISET is set, then validity checks are performed on ISet arguments
 * to all ISet functions.
 */
#ifdef DEBUG_ISET

/* Function which performs validity check. */
VOID _ZISetOk PROTO((CONST ISet set, ConstString modName,
		     ConstString fileName, UInt lineN));

#define BAD_ISET_SIG	"%!*@"	/* Signature for a block being freed. */

/* Allocate a new block. */
#define NEW_BLK()	((ISetBlk*)allocFreeList(_ZISetFrees))

/* Return a block to free storage. */
#define DEL_BLK(blkP)							\
  do {									\
    HDR_OWNER(blkP)= BAD_ISET_SIG;					\
    freeFreeList((blkP), _ZISetFrees);					\
  } while (0)

#define SET_OWNER(hdrP, owner)						\
  do {									\
    HDR_OWNER(hdrP)= (owner);						\
    HDR_FILE(hdrP)= fileName; HDR_LINE(hdrP)= lineN;			\
  } while (0)

/* Used for defining function headers for debug library functions. */
#define FN_NAME(noDebugName, debugName)	debugName
#define DEBUG_ARGS0	modName, fileName, lineN
#define DEBUG_ARGS	, modName, fileName, lineN
#define DEBUG_DECS							\
  ConstString modName; ConstString fileName; UInt lineN;

/* Perform validity check on ISet, with module name check. */
#define ISET_OK(p)	_ZISetOk(p, modName, fileName, lineN)

/* Perform validity check on ISet, without module name check. */
#define ISET_READ_OK(p)	_ZISetOk(p, NULL, fileName, lineN)

#else	/* No validity checks. */

/* Straight-forward allocation of a block by asking the free-list. */
#define NEW_BLK()	((ISetBlk*)allocFreeList(_ZISetFrees))

/* Straight-forward deleting of a block by adding to the free-list. */
#define DEL_BLK(blkP)	freeFreeList((blkP), _ZISetFrees)

/* Used for defining function headers for normal library functions. */
#define FN_NAME(noDebugName, debugName)	noDebugName
#define DEBUG_ARGS0
#define DEBUG_ARGS
#define DEBUG_DECS

/* Empty validity checks. */
#define ISET_OK(p)	
#define ISET_READ_OK(p)

#endif /* ifdef DEBUG_ISET */

/* Free an existing block. */

/* Create and link a new block p1 after existing block p0 (p0 and p1 
 * should be distinct). 
 */
#define LINK_NEW_BLK(p0, p1)						\
  do {									\
    (p1)= NEW_BLK(); BLK_SUCC(p1)= BLK_SUCC(p0); BLK_SUCC(p0)= (p1);	\
  } while (0)

#endif /* ifndef _ISETP */

