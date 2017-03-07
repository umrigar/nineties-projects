/*

File:	 xassert.h
Purpose: Multiple-level assertion facility using <assert.h>.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


/* Can be included multiple times. */


/*

If when this file is included, the macro ASSERTn(X) for n in {0, 1, 2,
3} is defined to be assert(X) if ASSERT_LEVEL >= n; else (VOID) 0.

The intent of this file is to allow selective turning off of assertions.
The different levels reflect the cost of the assertion.  A rough guideline
is as follows:

ASSERT0:  Simple sanity tests for function arguments, stuff which is
          cheap to test and not executed too often.

ASSERT1:  Simple tests which may be within loops or less simple tests
          outside loops.

ASSERT2:  Less simple tests within loops.

ASSERT3:  A assertion which requires a complex function call.

All assertion checking can always be turned off by defining NDEBUG as
usual.

Appears to be of limited utility.  It seems that the granularity provided
by NDEBUG (assertion checking is either on or off) is sufficient.

*/

#ifndef ASSERT_LEVEL
#define ASSERT_LEVEL 3	/* Turn on all assertions by default. */
#endif

#undef ASSERT0
#if ASSERT_LEVEL >= 0
#define ASSERT0(test)	assert(test)
#else
#define ASSERT0(test)	((VOID) 0)
#endif

#undef ASSERT1
#if ASSERT_LEVEL >= 1
#define ASSERT1(test)	assert(test)
#else
#define ASSERT1(test)	((VOID) 0)
#endif

#undef ASSERT2
#if ASSERT_LEVEL >= 2
#define ASSERT2(test)	assert(test)
#else
#define ASSERT2(test)	((VOID) 0)
#endif

#undef ASSERT3
#if ASSERT_LEVEL >= 3
#define ASSERT3(test)	assert(test)
#else
#define ASSERT3(test)	((VOID) 0)
#endif





