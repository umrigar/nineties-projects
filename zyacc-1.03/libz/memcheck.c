/*

File:	 memcheck.c
Purpose: Error-checking wrappers around malloc() and friends.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/* Purposely doesn't use any other libz routines, to prevent infinite loops */
#include "memalloc.h"

#include <stdio.h>

#define NO_MEM()						\
  do { 								\
    fputs("out of memory; aborting execution\n", stderr); 	\
    exit(1); 							\
  } while (0)

VOIDP 
callocCheck(n, s)
  Count n;
  SIZE_T s;
{
  VOIDP p= (VOIDP)calloc((SIZE_T)n, (SIZE_T)s);
  if (!p) NO_MEM();
  return p;
}

VOID
freeCheck(p)
  VOIDP p;
{
#if REALLOC_NUL
  free(p);
#else
  if (p) free(p);
#endif
  VOID_RET();
}

VOIDP 
mallocCheck(s)
  SIZE_T s;
{
  VOIDP p= (VOIDP)malloc((SIZE_T)s);
  if (!p) NO_MEM();
  return p;
}

VOIDP 
reallocCheck(p, s)
  VOIDP p; 
  SIZE_T s;
{
#if REALLOC_NUL
  VOIDP p1= (VOIDP)realloc(p, (SIZE_T)s);
#else
  VOIDP p1= (p) ? (VOIDP)realloc(p, (SIZE_T)s) : (VOIDP)malloc((SIZE_T)s);
#endif
  if (!p1) NO_MEM();
  return p1;
}
