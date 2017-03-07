/*

File:	 memalloc.h
Purpose: Error-checking interface to malloc() and friends.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#ifndef _MEMALLOC
#define _MEMALLOC

#include "ccport.h"

/* Allocate vectors of types. */
#define MALLOC_VEC(n, type)		((type*)MALLOC((n) * sizeof(type)))
#define CALLOC_VEC(n, type)		((type*)CALLOC((n), sizeof(type)))
#define REALLOC_VEC(vec, n, type)					\
  ((type*)REALLOC((vec), (n) * sizeof(type)))

#if DEBUG_MEM		

/* Keep track of all allocated pointers and ensure nothing is freed or
 * reallocated unless it had been previously allocated.  Creates a
 * file memdebug.log in the current directory logging memory requests.
 * If MEM_ALLOCATIONS() is called the current allocations are appended
 * to memdebug.log.  This can be useful to call at program exit
 * (possibly using the stdlib function atexit() if it exists on the
 * system) to trace memory leaks.
 *
 * NOTE: The program being debugged should be linked with a version
 * of this library which has been compiled with -DDEBUG_MEM.
 */
#define MALLOC(s)	mallocDebug(__FILE__, __LINE__, (SIZE_T)s)
#define REALLOC(p, s)	reallocDebug(__FILE__, __LINE__, (VOIDP)(p), (SIZE_T)s)
#define CALLOC(n, s)	callocDebug(__FILE__, __LINE__, n, (SIZE_T)s)
#define FREE(p)		freeDebug(__FILE__, __LINE__, (VOIDP)(p))
#define MEM_ALLOCATIONS()	memAllocations(__FILE__, __LINE__)


#else /* !DEBUG_MEM */

/* Just check return values from the system routines. */
#define MALLOC(s)	mallocCheck(s)
#define REALLOC(p, s)	reallocCheck((VOIDP)p, s)
#define CALLOC(n, s)	callocCheck(n, s)
#define FREE(p)		freeCheck((VOIDP)p)

#endif /* if DEBUG_MEM */

VOIDP mallocDebug PROTO((ConstString f, Index line, SIZE_T s));
VOIDP reallocDebug PROTO((ConstString f, Index line, VOIDP p, SIZE_T s));
VOIDP callocDebug PROTO((ConstString f, Index line, Count n, SIZE_T s));
VOID freeDebug PROTO((ConstString f, Index line, VOIDP p));
VOID memAllocations PROTO((ConstString fName, UInt lineN));

VOIDP mallocCheck PROTO((SIZE_T s));
VOIDP reallocCheck PROTO((VOIDP p, SIZE_T s));
VOIDP callocCheck PROTO((Count n, SIZE_T s));
VOID freeCheck PROTO((VOIDP p));



#endif /* ifndef _MEMALLOC */
