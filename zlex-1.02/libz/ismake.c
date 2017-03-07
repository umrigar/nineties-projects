/*

File:	 ismake.c
Purpose: Create a new int-set having specified elements.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

#if CC_STDARG

ISet 
FN_NAME(makeNISet, makeDISet)(Count nElements, ...) 
{
#ifdef DEBUG_ISET
  ISet set= newDISet(NULL, NULL, 0);
#else
  ISet set= newNISet();
#endif
  va_list ap;
  Index i;
  va_start(ap, nElements);
  for (i= 0; i < nElements; i++) {
    ISetElement element= (ISetElement) va_arg(ap, ISetElement);
    addISet(set, element);
  }
  return set;
}

#else /* !CC_STDARG */

ISet 
FN_NAME(makeNISet, makeDISet) (va_alist) 
  va_dcl
{
  ISet set= newISet();
  va_list ap;
  Count nElements;
  Index i;
  va_start(ap);
  nElements= (Count) va_arg(ap, Count);
  for (i= 0; i < nElements; i++) {
    ISetElement element= (ISetElement) va_arg(ap, ISetElement);
    addISet(set, element);
  }
  return set;
}

#endif /* else !CC_STDARG */


