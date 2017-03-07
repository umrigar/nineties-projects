/*

File:	 memcheck.c
Purpose: Error-checking wrappers around malloc() and friends.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "errorz.h"
#include "memalloc.h"

VOIDP 
callocCheck(n, s)
  Count n;
  Size s;
{
  VOIDP p= (VOIDP)calloc((SIZE_T)n, (SIZE_T)s);
  if (!p) libzError(ER_ERROR|ER_FATAL|ER_NL|ER_SYS, "Out of memory.");
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
  Size s;
{
  VOIDP p= (VOIDP)malloc((SIZE_T)s);
  if (!p) libzError(ER_ERROR|ER_FATAL|ER_NL|ER_SYS, "Out of memory.");
  return p;
}

VOIDP 
reallocCheck(p, s)
  VOIDP p; 
  Size s;
{
#if REALLOC_NUL
  VOIDP p1= (VOIDP)realloc(p, (SIZE_T)s);
#else
  VOIDP p1= (p) ? (VOIDP)realloc(p, (SIZE_T)s) : (VOIDP)malloc((SIZE_T)s);
#endif
  if (!p1) libzError(ER_ERROR|ER_FATAL|ER_NL|ER_SYS, "Out of memory.");
  return p1;
}
