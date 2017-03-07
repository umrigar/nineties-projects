/*

File:	 utstrlo.c
Purpose: Return a dynamically allocated lower-cased version of a  string.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "memalloc.h"
#include "zutil.h"

/* Return a dynamically allocated copy of string s with all chars. changed 
 * to lower-case.  Returned value can subsequently be FREE'd by the caller.
 */
ConstString 
toLowerString(s)
  ConstString s;
{
  Size len1= strlen(s) + 1;
  Char *z= (Char*)MALLOC(len1);
  ConstString src= s;
  Char *dest= z;
  do { *dest++= tolower(*src); } while (*src++ != '\0');
  assert(dest == z + len1);
  assert(src == s + len1);
  return z;
}


