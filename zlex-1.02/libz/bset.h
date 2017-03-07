/*

File:	 bset.h
Purpose: Arbitrary-sized bit-sets.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _BSET
#define _BSET

#include "ccport.h"

/*  Types */
typedef Index2 BSet;		/* When BSets are stored. */
typedef Index BSetX;		/* For passing between functions. */
typedef Index BSetElement;
#define NIL_ELEMENT UINT2_MAX
#define BAD_BSET NIL

/* Functions provided. */
BSet addBSet PROTO((BSetX set, BSetElement element));
BSet assignBSet PROTO((BSetX setA, CONST BSetX setB));
Int compareBSet PROTO((CONST BSetX setA, CONST BSetX setB));
BSet copyBSet PROTO((CONST BSetX s));
BSet delBSet PROTO((BSetX set));
BSet diffBSet PROTO((BSetX setA, CONST BSetX setB));
BSet fprintBSet PROTO((FILE *f, CONST BSetX set));
ULong hashSumBSet PROTO((CONST BSetX set));
VOID initBSet PROTO((VOID_ARGS));
Boolean inBSet PROTO((BSetElement i, CONST BSetX set));
BSet intersectBSet PROTO((BSetX setA, CONST BSetX setB));
Boolean isEmptyBSet PROTO((CONST BSetX set));
Boolean isSubsetBSet PROTO((CONST BSetX setA, CONST BSetX setB));
BSet makeBSet PROTO((Count nElements, ...));
BSet newBSet PROTO((VOID_ARGS));
BSetElement nextElementBSet PROTO((CONST BSetX set, BSetElement element));
BSet printBSet PROTO((CONST BSetX set));
BSet rangeBSet PROTO((BSetX set, BSetElement lo, BSetElement hi));
BSet rmBSet PROTO((BSetX s, BSetElement e));
VOID statusBSet PROTO((VOID_ARGS));
Size sizeBSet PROTO((CONST BSetX set));
BSet unaryBSet PROTO((BSetElement e));
BSet unionBSet PROTO((BSetX setA, CONST BSetX setB));

#define FOR_EACH_IN_BSET(e, s)						\
  for (e= nextElementBSet(s, NIL_ELEMENT);				\
       e != NIL_ELEMENT;						\
       e= nextElementBSet(s, e))


#endif /* ifndef _BSET */
