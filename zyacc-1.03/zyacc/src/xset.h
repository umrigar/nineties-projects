/*

File:	 xset.h
Purpose: "Bit" set interface.

Last Update Time-stamp: "97/07/10 18:15:42 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _XSET_H
#define _XSET_H

#include "port.h"
#include "area.h"
#include "error.h"

#include <assert.h>
#include <stdlib.h>

/*

An XSet is meant to be used for sets which do not have many occurrences as
it is rather memory intensive.  The basic set operations supported are
a set-membership test, adding a single element to the set, reporting the 
number of elements currently in the set and iterating thru the members of 
the set.

Its use involves typedefs and macros as follows:

XSet xset:  Declare.

INIT_XSET(xset, n): Initialize xset to allow elements from 0...(n-1).

Once these operations have been done, the following operations can occur
in any order.

IN_XSET(element, xset):	Returns TRUE iff element is in xset.

ADD_XSET(xset, element):  The element is added to xset.

SIZE_XSET(xset):  Returns the number of elements currently in xset.

SORT_XSET(xset):  Ensure that a subsequent enumeration (FOR_EACH_IN_XSET)
returns the elements in order.

FOR_EACH_IN_XSET(index, element, xset): index and element should be
previously declared integral types.  This introduces a loop header for
the following statement in which index gets the successive values 0,
1, ..., SIZE_XSET(xset) - 1 and element gets the values of elements
present in xset.  This will work correctly even if *NEW* elements are
added to the xset in the body of the loop.

EMPTY_XSET(xset): Empty out xset.

Finally:

DEL_XSET(xset): frees all storage used by xset.

*/

typedef struct {
  Size nSet;			/* Max. # of elements in set. */
  Boolean *inSet;		/* Array indicating set membership. */
  Area enumSet;			/* Vector of elements in set. */
#define XSET_ENUM_SET(s, i)	A_ACCESS(s.enumSet, Index2, i)
} XSet;

#define INIT_XSET(xSet, size)						\
  do {									\
    (xSet).nSet= size; 							\
    (xSet).inSet= (Boolean *)CALLOC(size, sizeof(Boolean));		\
    INIT_AREA((xSet).enumSet, Index2, 4, UINT2_MAX);			\
  } while (0)

#define IN_XSET(element, xset)	((xset).inSet[element])
#define ADD_XSET(xset, element)						\
  do {									\
    assert(element < (xset).nSet);					\
    if (!IN_XSET(element, xset)) {					\
      Index i= A_NEXT((xset).enumSet);					\
      XSET_ENUM_SET(xset, i)= element; (xset).inSet[element]= TRUE;	\
    }									\
  } while(0)

#define SIZE_XSET(xset)		(A_NENTRIES((xset).enumSet))

int xsetElementCompare PROTO((CONST VOIDP p1, CONST VOIDP p2));

#define SORT_XSET(xset) 						\
  qsort((VOIDP)&XSET_ENUM_SET(xset, 0), A_NENTRIES((xset).enumSet),	\
	sizeof(Index2), xsetElementCompare)

/* The following iterator will work even if elements are added to the xset
 * in the body of the loop.  On each iteration, index is set to 0, 1, ...,
 * and element is a corresponding element.
 */
#define FOR_EACH_IN_XSET(index, element, xset)				\
  for (									\
    index= 0,								\
    element= 								\
      SIZE_XSET(xset) > 0 ? XSET_ENUM_SET(xset, 0) : 0;			\
    index < A_NENTRIES((xset).enumSet);					\
    ++index , element= XSET_ENUM_SET(xset, index))			

#define EMPTY_XSET(xset)						\
  do { 									\
    Index i, e;	 							\
    FOR_EACH_IN_XSET(i, e, (xset)) { (xset).inSet[e]= FALSE; }		\
    A_CUT((xset).enumSet, 0);						\
  } while (0)

#define DEL_XSET(xset)							\
  do { 									\
    free((xset).inSet); (xset).inSet= 0; delArea(&(xset).enumSet); 	\
  } while (0)

#endif /* ifndef _XSET_H */
