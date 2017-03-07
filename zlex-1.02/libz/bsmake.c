/*

File:	 bsmake.c
Purpose: Create a new bit-set having specified elements.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

#if CC_STDARG

BSet makeBSet(Count nElements, ...) 
{
  BSet set= newBSet();
  va_list ap;
  Index i;
  va_start(ap, nElements);
  for (i= 0; i < nElements; i++) {
    BSetElement element= (BSetElement) va_arg(ap, Int);
    addBSet(set, element);
  }
  return set;
}

#else /* !CC_STDARG */

BSet makeBSet (va_alist) 
  va_dcl
{
  BSet set= newBSet();
  va_list ap;
  Count nElements;
  Index i;
  va_start(ap);
  nElements= (Count) va_arg(ap, Count);
  for (i= 0; i < nElements; i++) {
    BSetElement element= (BSetElement) va_arg(ap, Int);
    addBSet(set, element);
  }
  return set;
}

#endif /* else !CC_STDARG */


