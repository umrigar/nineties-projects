/*

File:	 area.h
Purpose: Dynamically sized memory areas. 

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#ifndef _AREA
#define _AREA

#include "ccport.h"

/* 

These macros are used to define memory areas which can be accessed as
vectors.  There are two main types: Area and AreaX.  Area allocates
all the memory used by an area contiguously: hence it suffers from
potentially large reallocs and requires large sections of contiguous memory 
but provided fast access to any vector element.  AreaX allocates the
memory allocated by an area in chunks (which need not be contiguous) but 
accessing an element is less efficient.

The main types defined are Area and AreaX.  Elements of these types
can be declared but they must be initialized with AREA_INIT() and
AREAX_INIT before they can be used.

Using Area:

The macro AREA(Area, Type, Lg2Inc, Max) can be used to declare and
initialize a static or automatic Area of base type Type whose size on
overflow is incremented by 2^Lg2Inc entries upto a maximum of Max entries.
A storage class specifier (static, extern, auto) can be specified
right before the call to AREA().

A_NENTRIES(Area) returns the number of elements currently allocated in
Area --- this is equal to the index of the next element which will be
allocated in Area.

A_NEXT(Area) allocates an entry in Area and returns its index.

A_ACCESS(Area, Type, i) returns the ith element of Area (a L-value).
Any reasonable optimizing compiler should do common subexpression
elimination (CSE) for multiple occurrences of A_ACCESS(Area, Type, i)
provided there are no intervening function calls or assignments thru
pointers.  If the situation is such that a compiler cannot reasonable
be expected to do CSE, then it is probably best to cache a access
pointer in a local variable and then use the pointer variable for the
accesses.

A_CUT(Area, Index) resets next area-index to Index.

NOTE:

1. Since A_NEXT() can cause reallocation of an area, the construct
A_BASE(Area, Type)[A_NEXT(Area)] is unsafe, since the results can be
unpredictable if A_BASE() is evaluated first, and A_NEXT() causes
reallocation.

2. Since an area can be dynamically reallocated on calls to A_NEXT(), 
pointers within an area should not be stored; only indices.

3. For convenience, after defining a area x of type T, it is best to
define a preprocessor macro X(i) to be A_ACCESS(x, T, i).  Typically, x can
be a lower-case symbol, and X all upper-case.  Then it is possible to
use expressions like X(i) to access the i'th element of area x.


Using AreaX:

The macro AREAX(Area, Type, Lg2Inc, Max) can be used to declare and
initialize a static or automatic Area of base type Type whose size on
overflow is incremented by 2^Lg2Inc entries upto a maximum of Max entries.
A storage class specifier (static, extern, auto) can be specified
right before the call to AREAX().

AX_INDEX(AreaX) returns the (read-only) index of the next element which
will be allocated in AreaX.

AX_NEXT(AreaX) allocates an entry in AreaX and returns its index.

AX_ACCESS(Area, Type, i) returns the ith element of AreaX (a L-value).
Any reasonable optimizing compiler should do common subexpression
elimination (CSE) for multiple occurrences of AX_ACCESS(Area, Type, i)
provided there are no intervening function calls or assignments thru
pointers.  If the situation is such that a compiler cannot reasonable
be expected to do CSE, then it is probably best to cache a access
pointer in a local variable and then use the pointer variable for the
accesses.

AX_CUT(AreaX, Index) resets next area-index to Index.

NOTE:

For convenience, after defining a AreaX x of type T, it is best to
define a preprocessor symbol X(i) to be AX_ACCESS(x, T, i).  Typically,
x can be a lower-case symbol, and X all upper-case.  Then it is
possible to use expressions like X(i) to access element i in AreaX x.

*/


/*			       AREA					*/

typedef struct {
  VOIDP base;		/* Base of area. */
  Index4 next;		/* Index of next entry to be allocated. */
  Size4 size;		/* Current # of entries obtained from memory. */
  Size4 max;		/* Maximum # of allowed entries. */ 
  Size2 typeSize;	/* sizeof(Type) where Type is the base-type. */
  Count1 lg2Inc;	/* lg2 of increment. */
  Boolean isFrozen;	/* Nonzero if frozen. */
} Area;

/* Declare and initialize static or auto areas. */
#define AREA(area, type, lg2Inc, max)					\
  Area area= { 0, 0, 0, max, sizeof(type), lg2Inc, 0 }

/* For areas which are components of structs or otherwise not declared
 * using AREA(). 
 */
#define INIT_AREA(area, type, lg2Incr, maxNEntries)			\
  do { 									\
    (area).base= 0;							\
    (area).next= (area).size= 0; (area).isFrozen= FALSE;		\
    (area).lg2Inc= lg2Incr; (area).typeSize= sizeof(type);		\
    (area).max= maxNEntries;						\
  } while (0)

#define A_NENTRIES(area)		((CONST Index)((area).next))

#define A_ACCESS(area, type, i)		(((type*)((area).base))[i])

#define A_NEXT(area)							\
  (((area).next >= (area).size) ? _ZarGrowArea(&(area)) : (area).next++)

/* Forget all area entries after and including i. */
#define A_CUT(area, i)							\
  do {									\
    (area).next= (i);							\
  } while (0)

#if 0
/* Change the growth increment associated with area to newInc. */
#define CHANGE_AREA_INC(area, newInc)					\
  do {									\
    (area).inc= newInc;							\
  } while (0)
#endif

/* Public routines. */
/* Free all memory occupied by area. */
VOID delArea PROTO((Area *area));	

/* Reset area a to newBase and nEntries. */
VOID resetArea PROTO((Area *a, VOIDP newBase, unsigned nEntries));

/* No further calls to A_NEXT().  
 * Return all memory not being used back to system. 
 */
VOID freezeArea PROTO((Area *a));

VOID unfreezeArea PROTO((Area *a));

/* Return dynamically allocated (can subsequently be FREEd) current
 * vector (with A_NENTRIES elements).  Reset the area to contain
 * no data (or allocated memory).
 */
VOIDP getMemFromArea PROTO((Area *a));

/* Private routine. */
unsigned _ZarGrowArea PROTO((Area *area));


/*			      AREAX					*/

typedef struct {
  VOIDP *base;		/* Base of chunk pointers. */
  Index4 next;		/* Index of next entry to be allocated. */
  Size4 size;		/* Current # of entries obtained from memory. */
  Size4 max;		/* Maximum # of allowed entries. */ 
  UInt4 mask;		/* Used to access elements (2^lg2Inc - 1). */
  Size2 typeSize;	/* sizeof(Type) where Type is the base-type. */ 
  Count1 lg2Inc;	/* Specified increment factor. */
} AreaX;

/* Declare and initialize static or auto areas. */
#define AREAX(areaX, type, lg2Inc, max)					\
  AreaX areaX= { 0, 0, 0, max, (1L << (lg2Inc))-1, sizeof(type), lg2Inc }

/* For areas which are components of structs or otherwise not declared
 * using AREA(). 
 */
#define INIT_AREAX(area, type, lg2Incr, maxNEntries)			\
  do { 									\
    (area).base= 0;							\
    (area).next= (area).size= 0;		 			\
    (area).lg2Inc= lg2Incr; (area).mask= (1 << lg2Incr) - 1;		\
    (area).typeSize= sizeof(type);					\
    (area).max= maxNEntries;						\
  } while (0)

#define AX_NENTRIES(areaX)		((CONST Index)((areaX).next))

#define AX_NEXT(areaX)							\
  (((areaX).next >= (areaX).size) ? _ZarGrowAreaX(&(areaX)) : (areaX).next++)

#define AX_ACCESS(areaX, type, i)					\
   (((type*)((areaX).base[(i) >> (areaX).lg2Inc]))[(i) & (areaX.mask)])

/* Forget all area entries after and including i. */
#define AX_CUT(area, i)	cutAreaX(&(area), i)

/* Public routines. */

/* Forget all area entries after and including i. */
VOID cutAreaX PROTO((AreaX *a, Index i));

/* Free all memory occupied by area. */
VOID delAreaX PROTO((AreaX *a));	

/* Private routine. */
unsigned _ZarGrowAreaX PROTO((AreaX *a));


#endif	/* ifndef _AREA */

