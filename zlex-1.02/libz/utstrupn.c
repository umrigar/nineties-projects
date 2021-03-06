/*

File:	 utstrupn.c
Purpose: Return a dynamically allocated upper-cased version of a length-string.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "memalloc.h"
#include "zutil.h"

/* Return a dynamically allocated copy of the first len chars of
 * string s with all chars. changed to upper-case, terminated by
 * '\0'.  Returned value can subsequently be FREE'd by the caller.
 */
ConstString 
toUpperStringN(s, len)
  ConstString s;
  Size len;
{
  Char *z= (Char*)MALLOC(len + 1);
  ConstString src= s;
  Char *dest= z;
  Char *destEnd= z + len;
  do { *dest++= toupper(*src++); } while (dest != destEnd);
  *destEnd= '\0';
  return z;
}


