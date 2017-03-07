/*

File:	 isrange.c
Purpose: Insert a range of elements into an existing int-set.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

/* Insert elements lo..hi (both inclusive) into set. 
 * Simple-minded implementation.
 */
ISet 
FN_NAME(rangeNISet, rangeDISet)(set, lo, hi DEBUG_ARGS)
  ISet set;
  CONST ISetElement lo; 
  CONST ISetElement hi;
  DEBUG_DECS
{
  ISetElement i;
  ISET_OK(set);
  for (i= lo; i <= hi; i++) {
    FN_NAME(addNISet, addDISet)(set, i DEBUG_ARGS);
  }
  return set;
}

