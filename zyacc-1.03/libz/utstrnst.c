/*

File:	 utstrnst.c
Purpose: Find a string of a given length in another string.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "zutil.h"

/* Return a pointer to the first occurrence of the first n characters of
 * string ct in cs.  NULL if none.  Simple-minded O(n*m) implementation.
 */
ConstString 
strNStr(cs, ct, n)
  ConstString cs;
  ConstString ct; Size n;
{
  ConstString csLimit= cs + strlen(cs) - n;
  ConstString p;
  for (p= cs; p < csLimit; p++) {
    if (strncmp(p, ct, n) == 0) return p;
  }
  return NULL;
}
