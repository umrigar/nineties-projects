/*

File:	 memalloc.h
Purpose: Error-checking interface to malloc() and friends.

Last Update Time-stamp: <97/07/30 19:04:54 zdu>

Copyright (C) 1995, 1997 Zerksis D. Umrigar

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

#if MEM_DEBUG		

/* Keep track of all allocated pointers and ensure nothing is freed or
 * reallocated unless it had been previously allocated.  
 * These routines also setup fences around each allocated block
 * initialized to a unique pattern and check opportunistically and
 * whenever a block is freed that the fences have not been corrupted.
 *
 * If the environmental variable DEBUG_MEM_LOG is defined, then
 * every memory request and error is logged in the file given
 * by the value of MEM_DEBUG_LOG.
 *
 * If MEM_ALLOCATIONS() is called, the integrity of the current
 * allocations is verified and the current allocations are appended to
 * the logfile.  memAtExit() does the same thing, but can be
 * registered to be called at program exit (possibly using the stdlib
 * function atexit() if it exists on the system) to trace memory
 * leaks.
 *
 * NOTE: If there seems to be a memory problem in the library, then
 * the program being debugged should be linked with a version of this
 * library which has been compiled with -DMEM_DEBUG.
 */
#define MALLOC(s)	mallocDebug(__FILE__, __LINE__, (SIZE_T)s)
#define REALLOC(p, s)	reallocDebug(__FILE__, __LINE__, (VOIDP)(p), (SIZE_T)s)
#define CALLOC(n, s)	callocDebug(__FILE__, __LINE__, n, (SIZE_T)s)
#define FREE(p)		freeDebug(__FILE__, __LINE__, (VOIDP)(p))
#define MEM_ALLOCATIONS()	memAllocations(__FILE__, __LINE__)


#else /* !MEM_DEBUG */

/* Just check return values from the system routines. */
#define MALLOC(s)	mallocCheck(s)
#define REALLOC(p, s)	reallocCheck((VOIDP)p, s)
#define CALLOC(n, s)	callocCheck(n, s)
#define FREE(p)		freeCheck((VOIDP)p)

#endif /* if MEM_DEBUG */

VOIDP mallocDebug PROTO((ConstString f, Index line, SIZE_T s));
VOIDP reallocDebug PROTO((ConstString f, Index line, VOIDP p, SIZE_T s));
VOIDP callocDebug PROTO((ConstString f, Index line, Count n, SIZE_T s));
VOID freeDebug PROTO((ConstString f, Index line, VOIDP p));
VOID memAllocations PROTO((ConstString fName, UInt lineN));
VOID memAtExit PROTO((VOID_ARGS));

VOIDP mallocCheck PROTO((SIZE_T s));
VOIDP reallocCheck PROTO((VOIDP p, SIZE_T s));
VOIDP callocCheck PROTO((Count n, SIZE_T s));
VOID freeCheck PROTO((VOIDP p));



#endif /* ifndef _MEMALLOC */
