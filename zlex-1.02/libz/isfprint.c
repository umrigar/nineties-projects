/*

File:	 isfprint.c
Purpose: Output elements of int-set surrounded with braces.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

/* Checked fprintf().  If fmt is const, use "%s" for fmt and pass
 * in original fmt as val.
 */
#define CP(file, fmt, val, retVal)					\
  do {									\
    Int pVal= fprintf(file, fmt, val);					\
    if (pVal < 0) return pVal; else retVal+= pVal;			\
  } while (0)

Int
FN_NAME(fprintNISet, fprintDISet)(f, set DEBUG_ARGS)
  FILE *f;
  CONST ISet set;
  DEBUG_DECS
{
  ISetIter iterP= set;
  ISetElement e= nextElementISet(set, e, &iterP);
  Int retVal= 0;	/* Accumulate # of characters output. */
  ISET_READ_OK(set);
  CP(f, "%s", "{", retVal);
  if (iterP != VOID_ISET_ITER) {	/* Set nonempty. */
    /* Print first element. */
    CP(f, " %d", e, retVal);		
    for (e= nextElementISet(set, e, &iterP); 
	 iterP != VOID_ISET_ITER; 
	 e= nextElementISet(set, e, &iterP)) {
      /* Print subseq. element, preceeded by ','. */
      CP(f, ", %d", e, retVal);	
    }
  }
  CP(f, "%s", " }", retVal);
  return retVal;
}

