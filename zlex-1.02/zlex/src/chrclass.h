/*

File:	 chrclass.h
Purpose: Character class interface.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _CHRCLASS_H
#define _CHRCLASS_H

#include "port.h"
#include "iset.h"

/* Type of a character class as it is being constructed. */
typedef VOIDP RawClass;

/* Type of a character class for storing in data-structures. */
typedef Index2 Class;

/* Type of a character class for passing as function arguments. */
typedef Index ClassX;

/*

We can think of this module going thru 3 phases:

1.  Character classes are constructed incrementally driven by the parser.
Information is remembered in a global data structure.

2.  Class sets and eq-classes are computed.  The global data structure
is deleted but information about the state of the module is returned to
the caller.

3.  Character class information is consumed (iterated thru or
printed).  Once information is no longer required, the module takes it
on itself to delete it (except in one situation where the caller needs
to explicitly call delClassSets()).  By the end of this phase, all
information is destroyed and all dynamically allocated memory is
deallocated.

A reference counting garbage collector is used for Classes (not
raw-classes).  Hence it is imperative that whenever a returned Class
is stored in a data-structure, the function copyClass() is called.
copyClass() should not be called when a Class returned by
rawClassToClass() is stored in a data-structure.

 */

/* Initialize and terminate character class module. */
VOID initChrClass PROTO((VOID_ARGS));
VOID terminateChrClass PROTO((VOID_ARGS));


/* 			CONSTRUCTOR ROUTINES				*/
/* Create a new empty raw class. */
RawClass newRawClass();

/* Add characters in range lo (inclusive) to hi (inclusive) to raw character
 * class cClass returning the modified cClass.
 */
RawClass addRawClass PROTO((RawClass rawClass, UInt lo, UInt hi));

/* Add characters specified by ctypeID to rawClass, returning the modified
 * rawClass.
 */
RawClass ctypeAddToRawClass PROTO((RawClass rawClass, Index ctypeID));

/* Convert a RawClass to a class. */
Class rawClassToClass PROTO((RawClass rawClass));

/* Non-destructive operations. */
Class unionClass PROTO((CONST ClassX class1, CONST ClassX class2));
Class negClass PROTO((ClassX class));

/* Return a copy of class c (actually identical to c, but necessary for GC). */
Class copyClass PROTO((ClassX c));

/* Effectively delete class c. Really deletes it if there are no references
 * left to it. 
 */
VOID delClass PROTO((ClassX c));

/* Return # of elements in class class. */
Count sizeClass PROTO((ClassX class));

/* Return unique class representing a specific constant class. */
Class dotClass PROTO((VOID_ARGS));
Class univClass PROTO((VOID_ARGS));


/*			MAIN COMPUTATIONAL ROUTINE			*/

/* Opaque type to package up context for module. */
typedef VOIDP ClassContext;

/* Compute bit-set representations of all classes.  To be called after
 * all classes have been constructed.  Will return context for future
 * calls to other routines.  Will do eq-class construction (if
 * requested via options or 16-bit chars) and compression (if 16-bit
 * chars and compression is worthwhile).  Also retain return value
 * internally to be used as implicit parameter in following routines.
 * This will delete all internal information except for that contained
 * in the return value.  No class constructor or destructors should
 * be called after this routine has been called.
 */
ClassContext computeClasses PROTO((VOID_ARGS));

/* Return eq-class for character c. */
Index getEqClass PROTO((Index c));

/* Return a vector v of ISets such that if c is a valid class, then
 * v[c] returns the ISet of characters or eq-classes associated with
 * class c.
 */
ISet *getClassISets PROTO((VOID_ARGS));

/* To be called after classSets component above no longer needed. */
VOID delClassSets PROTO((VOID_ARGS));

/* Return # of eq-classes (charSetSize if no eq-classes used). */
Count nEqClasses PROTO((VOID_ARGS));

/* Output eq-class parameters to outFile. Implicit parameter of
 * ClassContext. 
 */
Count outEqClassParams PROTO((FILE *outFile));

/* Output declaration (if isDcl) and initialization data (if !isDcl)
 * for eq-classes to outFile.  All internal information is deleted
 * after the !isDcl call.  Implicit parameter of ClassContext.
 */
Count equivInitFn PROTO((FILE *outFile, BooleanX isDcl, Index *indentP));

#ifdef DO_TRACE
VOID dumpClass PROTO((FILE *dumpFile, ClassX class));
#endif

#endif /* ifndef _CHRCLASS_H */
