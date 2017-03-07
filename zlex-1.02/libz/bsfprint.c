/*

File:	 bsfprint.c
Purpose: Output elements of bit-set surrounded with braces.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

BSet 
fprintBSet(f, set)
  FILE *f;
  CONST BSetX set;
{
  BSetElement e= NIL_ELEMENT;
  fprintf(f, "{");
  e= nextElementBSet(set, e);
  if (e != NIL_ELEMENT) {
    fprintf(f, "%d", e);
    e= nextElementBSet(set, e);
    while (e != NIL_ELEMENT) {
      fprintf(f, ", %d", e); e= nextElementBSet(set, e);
    }
  }
  fprintf(f, "}");
  return set;
}

