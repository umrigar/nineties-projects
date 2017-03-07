/*

File:	 iset.h
Purpose: Arbitrary-sized integer-sets.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _ISETP_H
#define _ISETP_H

#include "ccport.h"

/*

Integer sets of arbitrary size: realized as lists of bit-sets.  

If DEBUG_ISET is defined *BEFORE* this file is included, then a
debugging version of the ISet module will be linked in.  The
debugging library does the following:

1.  It checks all ISet arguments to ISet functions for validity.
Invalid ISet arguments cause the program to abort after printing the
__FILE__ and __LINE__ of the offending call on stderr.

2.  It also assigns each ISet an owner module.  All modules are
permitted to have read-access to an ISet but only owners are allowed
to modify/delete an ISet. The owner of an ISet defaults to the module
which created it, but any owner can relinquish ownership and make the
set unowned (the owner is set to NULL) or transfer ownership to
another module.  The name of a module can be any string: it defaults
to the __FILE__ name but if another name is desired, the preprocessor
macro MODULE_ID_STRING should be defined to a string giving the
desired name of the module *BEFORE* this file is included.  To turn
ownership checking completely off, define MODULE_ID_STRING to be NULL.
When there is an ownership violation, the __FILE__ and __LINE__ of the
offending call is printed on stderr and execution continues.

Since this file is protected against multiple inclusion, DEBUG_ISET
and MODULE_ID_STRING (if desired) should be defined before any files
are included, in case this file is included via a nested include.

*/

/*  Types */
typedef VOIDP ISet;		/* The type of an ISet. */
typedef Int ISetElement;	/* The type of an ISet element. */
typedef VOIDP ISetIter;		/* Type used for iterating thru a ISet. */

#define BAD_ISET NULL

/* Value to be used to start and end an iteration thru an ISet. */
#define VOID_ISET_ITER NULL

#ifndef MODULE_ID_STRING
#define MODULE_ID_STRING	__FILE__
#endif

/* Module initialization. */
VOID initISet PROTO((VOID_ARGS));

/* Module termination. */
VOID terminateISet PROTO((VOID_ARGS));

/* Use the ISetIterator ip to set ISetElement e to subsequent elements
 * of ISet s on subsequent iterations; basically a for-each loop.
 */
#define FOR_EACH_IN_ISET(e, s, ip)					\
  for (ip= s, e= nextElementISet(s, e, &ip);				\
       ip != VOID_ISET_ITER;						\
       e= nextElementISet(s, e, &ip))

#if 0 
COMMENT:  Document other facilities.

The following are macros which expand to a call to an appropriate
function depending on the value of preprocessor symbol DEBUG_ISET.

/* Add element to set. */
ISet addISet PROTO((ISet set, ISetElement element));

/* Assign setB to setA, returning modified setA.  */
ISet assignISet PROTO((ISet setA, CONST ISet setB));

/* Change ownership of an ISet to module.  This only has effect when
 * DEBUG_ISET is defined, but serves as useful documentation even when
 * it is not.  ISet set should be owned by the calling module or have
 * NULL ownership.  If newModule is NULL, then the new ownership of set
 * is up for grabs: any other module can establish its ownership over
 * it.  This function should be used when an ISet is being permanently
 * tranferred over to another module.
 */
ISet chownISet PROTO((ISet set, ConstString newModule));

/* Compare setA, setB returning -1, 0, 1 if setA < setB, setA == setB
 * setA > setB.  setA < setB if the smallest element not in both setA
 * and setB is in setB; setA > setB if the smallest element not in both
 * setA and setB is in setA.
 */
Int compareISet PROTO((CONST ISet setA, CONST ISet setB));

/* Return a brand new copy of ISet s which does not share any storage
 * with s. 
 */
ISet copyISet PROTO((CONST ISet s));

/* Delete ISet, making its storage available to subsequent ISets. */
VOID delISet PROTO((ISet set));

/* Return setA= setA - setB. */
ISet diffISet PROTO((ISet setA, CONST ISet setB));

/* Return first element in set, where set should not be empty. */
ISetElement firstElementISet PROTO((ISet set));

/* Print elements of ISet set on FILE f, where the elements are separated
 * by ', ' and surrounded within braces.  Returns the # of characters
 * output, < 0 on error.
 */
Int fprintISet PROTO((FILE *f, CONST ISet set));

/* Return a hash-sum of ISet set. */
ULong hashSumISet PROTO((CONST ISet set));

/* Return TRUE iff ISetElement i is in set. */
Boolean inISet PROTO((ISetElement i, CONST ISet set));

/* Return setA= setA intersect setB. */
ISet intersectISet PROTO((ISet setA, CONST ISet setB));

/* Return TRUE iff set is empty. */
Boolean isEmptyISet PROTO((CONST ISet set));

/* Return TRUE iff setA is contained (properly or improperly) within setB. */
Boolean isSubsetISet PROTO((CONST ISet setA, CONST ISet setB));

/* Return a new ISet containing the nElements in the arguments. 
 * When DEBUG_ISET is defined, this routine does not assign the ownership
 * of the returned set to the calling module (due to C preprocessor 
 * limitations).  The user should immediately follow this call with
 * a call to chownISet() to assign the ownership.
 */
ISet makeISet PROTO((Count nElements, ...));

/* Return a new empty ISet. */
ISet newISet PROTO((VOID_ARGS));

/* If *iterP == VOID_ISET_ITER, then produce return next set element >
 * element.  If *iterP == set, then return first element of set.
 * Otherwise if *iterP points to a block ranging over element, then
 * that helps searching for the next element; otherwise the action is
 * similar to *iterP == VOID_ISET_ITER.  In any case, if there is no
 * next element, then the return value is garbage, but *iterP is set
 * to VOID_ISET_ITER.  The main use of this function to iterate over
 * all the elements of an ISet is encapsulated within the
 * FOR_EACH_IN_ISET macro which was defined earlier.
 */
ISetElement nextElementISet PROTO((CONST ISet setA, ISetElement e, 
				   ISetIter *iterP));

/* Equivalent to fprintf(stdout, set). */
Int printISet PROTO((CONST ISet set));

/* Add elements in range lo...hi (both inclusive to set. */
ISet rangeISet PROTO((ISet set, ISetElement lo, ISetElement hi));

/* Remove element e from ISet s, returning modified s. */
ISet rmISet PROTO((ISet s, ISetElement e));

/* Return the # of elements in ISet. */
Size sizeISet PROTO((CONST ISet set));

/* Return a new ISet containing the single element e. */
ISet unaryISet PROTO((ISetElement e));

/* Return setA= setA U setB. */
ISet unionISet PROTO((ISet setA, CONST ISet setB));

#endif /* if 0 COMMENT: */

#define	GRAB_ISET(set)		chownISet(set, MODULE_ID_STRING)
#define RELEASE_ISET(set)	chownISet(set, NULL)

#ifdef DEBUG_ISET

/* Define macros to call Debug routines. */
#define addISet(set, element)						\
  addDISet(set, element, MODULE_ID_STRING, __FILE__, __LINE__)
#define assignISet(setA, setB)						\
  assignDISet(setA, setB, MODULE_ID_STRING, __FILE__, __LINE__)
#define chownISet(set, newModName)					\
  chownDISet(set, newModName, MODULE_ID_STRING, __FILE__, __LINE__)
#define compareISet(setA, setB)						\
  compareDISet(setA, setB, MODULE_ID_STRING, __FILE__, __LINE__)
#define copyISet(s)							\
  copyDISet(s, MODULE_ID_STRING, __FILE__, __LINE__)
#define delISet(set)							\
  delDISet(set, MODULE_ID_STRING, __FILE__, __LINE__)
#define diffISet(setA, setB)						\
  diffDISet(setA, setB, MODULE_ID_STRING, __FILE__, __LINE__)
#define firstElementISet(set)						\
  firstElementDISet(set, MODULE_ID_STRING, __FILE__, __LINE__)
#define fprintISet(f, set)						\
  fprintDISet(f, set, MODULE_ID_STRING, __FILE__, __LINE__)
#define hashSumISet(set)						\
  hashSumDISet(set, MODULE_ID_STRING, __FILE__, __LINE__)
#define inISet(i, set)							\
  inDISet(i, set, MODULE_ID_STRING, __FILE__, __LINE__)
#define intersectISet(setA, setB)					\
  intersectDISet(setA, setB, MODULE_ID_STRING, __FILE__, __LINE__)
#define isEmptyISet(set)						\
  isEmptyDISet(set, MODULE_ID_STRING, __FILE__, __LINE__)
#define isSubsetISet(setA, setB)					\
  isSubsetDISet(setA, setB, MODULE_ID_STRING, __FILE__, __LINE__)
#define makeISet							\
  makeDISet
#define newISet()							\
  newDISet(MODULE_ID_STRING, __FILE__, __LINE__)
#define nextElementISet(setA, e, iterP)					\
  nextElementDISet(setA, e, iterP, MODULE_ID_STRING, __FILE__, __LINE__)
#define printISet(set)							\
  printDISet(set, MODULE_ID_STRING, __FILE__, __LINE__)
#define rangeISet(set, lo, hi)						\
  rangeDISet(set, lo, hi, MODULE_ID_STRING, __FILE__, __LINE__)
#define rmISet(s, e)							\
  rmDISet(s, e, MODULE_ID_STRING, __FILE__, __LINE__)
#define sizeISet(set)							\
  sizeDISet(set, MODULE_ID_STRING, __FILE__, __LINE__)
#define unaryISet(e)							\
  unaryDISet(e, MODULE_ID_STRING, __FILE__, __LINE__)
#define unionISet(setA, setB)						\
  unionDISet(setA, setB, MODULE_ID_STRING, __FILE__, __LINE__)

/* Prototypes for DEBUG functions: */
ISet addDISet PROTO((ISet set, ISetElement element,
		     ConstString modName, ConstString fName, UInt lineN));
ISet assignDISet PROTO((ISet setA, CONST ISet setB,
			ConstString modName, ConstString fName, UInt lineN));
ISet chownDISet PROTO((ISet set, ConstString newModule, ConstString modName,
		       ConstString fName, UInt lineN));
Int compareDISet PROTO((CONST ISet setA, CONST ISet setB,
			ConstString modName, ConstString fName, UInt lineN));
ISet copyDISet PROTO((CONST ISet s,
		      ConstString modName, ConstString fName, UInt lineN));
VOID delDISet PROTO((ISet set,
		     ConstString modName, ConstString fName, UInt lineN));
ISet diffDISet PROTO((ISet setA, CONST ISet setB,
		      ConstString modName, ConstString fName, UInt lineN));
ISetElement firstElementDISet 
  PROTO((ISet set, ConstString modName, ConstString fName, UInt lineN));
Int fprintDISet PROTO((FILE *f, CONST ISet set,
		       ConstString modName, ConstString fName, UInt lineN));
ULong hashSumDISet PROTO((CONST ISet set,
			  ConstString modName, ConstString fName, UInt lineN));
Boolean inDISet PROTO((ISetElement i, CONST ISet set,
		       ConstString modName, ConstString fName, UInt lineN));
ISet intersectDISet PROTO((ISet setA, CONST ISet setB,
			   ConstString modName, ConstString fName, UInt lineN));
Boolean isEmptyDISet 
  PROTO((CONST ISet set, ConstString modName, ConstString fName, UInt lineN));
Boolean isSubsetDISet 
  PROTO((CONST ISet setA, CONST ISet setB,
	 ConstString modName, ConstString fName, UInt lineN));
ISet makeDISet PROTO((Count nElements, ...));
ISet newDISet PROTO((ConstString modName, ConstString fName, UInt lineN));
ISetElement nextElementDISet 
  PROTO((CONST ISet setA, ISetElement e, ISetIter *iterP,
	 ConstString modName, ConstString fName, UInt lineN));
Int printDISet PROTO((CONST ISet set,
		      ConstString modName, ConstString fName, UInt lineN));
ISet rangeDISet PROTO((ISet set, ISetElement lo, ISetElement hi,
		       ConstString modName, ConstString fName, UInt lineN));
ISet rmDISet PROTO((ISet s, ISetElement e,
		    ConstString modName, ConstString fName, UInt lineN));
Size sizeDISet PROTO((CONST ISet set,
		      ConstString modName, ConstString fName, UInt lineN));
ISet unaryDISet PROTO((ISetElement e,
		       ConstString modName, ConstString fName, UInt lineN));
ISet unionDISet PROTO((ISet setA, CONST ISet setB,
		       ConstString modName, ConstString fName, UInt lineN));

#else

/* Define macros to call No-debug routines. */

#define addISet(set, element)						\
  addNISet(set, element)
#define assignISet(setA, setB)						\
  assignNISet(setA, setB)
#define chownISet(set, newModule)	(set)
#define compareISet(setA, setB)						\
  compareNISet(setA, setB)
#define copyISet(s)							\
  copyNISet(s)
#define delISet(set)							\
  delNISet(set)
#define diffISet(setA, setB)						\
  diffNISet(setA, setB)
#define firstElementISet(set)						\
  firstElementNISet(set)
#define fprintISet(f, set)						\
  fprintNISet(f, set)
#define hashSumISet(set)						\
  hashSumNISet(set)
#define inISet(i, set)							\
  inNISet(i, set)
#define intersectISet(setA, setB)					\
  intersectNISet(setA, setB)
#define isEmptyISet(set)						\
  isEmptyNISet(set)
#define isSubsetISet(setA, setB)					\
  isSubsetNISet(setA, setB)
#define makeISet							\
  makeNISet
#define newISet()							\
  newNISet()
#define nextElementISet(setA, e, iterP)					\
  nextElementNISet(setA, e, iterP)
#define printISet(set)							\
  printNISet(set)
#define rangeISet(set, lo, hi)						\
  rangeNISet(set, lo, hi)
#define rmISet(s, e)							\
  rmNISet(s, e)
#define sizeISet(set)							\
  sizeNISet(set)
#define unaryISet(e)							\
  unaryNISet(e)
#define unionISet(setA, setB)						\
  unionNISet(setA, setB)

/* Prototypes for No-debug functions: */
ISet addNISet PROTO((ISet set, ISetElement element));
ISet assignNISet PROTO((ISet setA, CONST ISet setB));
Int compareNISet PROTO((CONST ISet setA, CONST ISet setB));
ISet copyNISet PROTO((CONST ISet s));
VOID delNISet PROTO((ISet set));
ISet diffNISet PROTO((ISet setA, CONST ISet setB));
ISetElement firstElementNISet PROTO((ISet set));
Int fprintNISet PROTO((FILE *f, CONST ISet set));
ULong hashSumNISet PROTO((CONST ISet set));
Boolean inNISet PROTO((ISetElement i, CONST ISet set));
ISet intersectNISet PROTO((ISet setA, CONST ISet setB));
Boolean isEmptyNISet PROTO((CONST ISet set));
Boolean isSubsetNISet PROTO((CONST ISet setA, CONST ISet setB));
ISet makeNISet PROTO((Count nElements, ...));
ISet newNISet PROTO((VOID_ARGS));
ISetElement nextElementNISet PROTO((CONST ISet setA, ISetElement e, 
				    ISetIter *iterP));
Int printNISet PROTO((CONST ISet set));
ISet rangeNISet PROTO((ISet set, ISetElement lo, ISetElement hi));
ISet rmNISet PROTO((ISet s, ISetElement e));
Size sizeNISet PROTO((CONST ISet set));
ISet unaryNISet PROTO((ISetElement e));
ISet unionNISet PROTO((ISet setA, CONST ISet setB));

#endif /* if DEBUG_ISET */


#endif /* ifndef _ISETP_H */
