/*

File:	 bsetp.h
Purpose: Private interface for bit-set routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#ifndef _BSETP
#define _BSETP

#ifdef DEBUG_BSET 	/* Define to check run-time assertions. */
#undef NDEBUG
#endif

#include "area.h"
#include "bset.h"
#include "memalloc.h"

typedef UInt BitWord;	/* Most convenient for bit manipulation. */

typedef struct {
  BitWord *bitWords;	/* Point to first BitWord. */
  Size2 nBitWords;	/* # of bit words allocated. */		
} UsedHdr;

typedef struct FreeHdr {
  Index succ;		/* Index of next free hdr. */
} FreeHdr;

typedef struct Hdr {
  union {
    UsedHdr used;	/* Point to Value of set. */
    FreeHdr free;	/* Retain information for free Hdr. */
  } u;
#ifdef DEBUG_BSET
  Boolean isFree;	/* TRUE if free. */
#endif
} Hdr;

enum {
  DEFAULT_MAX_ELEMENT= 127 /* Max. element when a set is first created. */
};
  
/* Macros for accessing nSet.hdrs. */

#ifdef DEBUG_BSET

#define HDR_ALLOC()							\
  ( frees == NIL							\
    ? ( dummyBSet= A_NEXT(hdrs) ,					\
        HDRS(dummyBSet).isFree= TRUE ,					\
        dummyBSet							\
      )									\
    : ( dummyBSet= frees ,						\
	frees= HDRS(frees).u.free.succ ,				\
	dummyBSet							\
      )									\
  )									\

#else

#define HDR_ALLOC()							\
  ( frees == NIL							\
    ? A_NEXT(hdrs)							\
    : ( dummyBSet= frees ,						\
	frees= HDRS(frees).u.free.succ ,				\
	dummyBSet							\
      )									\
  )									\

#endif

#define HDR_ELEMENT(i)	(&HDRS(i))

/* We assume that since BITWORD_BIT will be a power-of-2, the compiler	*/
/* will convert x/BITWORD_BIT and x%BITWORD_BIT into right-shift and 	*/
/* mask operations respectively.					*/
#define BITWORD_BIT	(sizeof(BitWord) * CHAR_BIT)
#define N_BIT_WORDS(maxElement)						\
  (((maxElement) + BITWORD_BIT)/BITWORD_BIT)

/* Originally, the assert() was built into the USED_HDR_P() and 
 * FREE_HDR_P() macros, but some screwed up <assert.h>s appear to
 * require assert() to be a statement.  Hence we have these separate
 * CHECK_USED() and CHECK_FREE() macros.
 */
#ifdef DEBUG_BSET
#define CHECK_USED(bSet)						\
  assert(bSet < A_INDEX(hdrs) && HDRS(bSet).isFree == FALSE)
#define CHECK_FREE(bSet)						\
  assert(bSet < A_INDEX(hdrs) && HDRS(bSet).isFree == TRUE)
#else
#define CHECK_USED(bSet)
#define CHECK_FREE(bSet)
#endif

#define USED_HDR_P(s) ( &HDR_ELEMENT(s)->u.used )
#define FREE_HDR_P(s) ( &HDR_ELEMENT(s)->u.free )

typedef struct {
  Area headers;		/* Area of headers used for all BSets. */
  Index2 freeList;	/* Head of list of free headers. */
  BSet dummy;		/* Dummy BSet used as temporary in macro. */
} BSetData;

extern BSetData _ZbsData;

#define hdrs (_ZbsData.headers)
#define HDRS(i) A_ACCESS(hdrs, Hdr, i)

#define frees (_ZbsData.freeList)

#define dummyBSet (_ZbsData.dummy)

#endif /* ifndef _BSETP */

