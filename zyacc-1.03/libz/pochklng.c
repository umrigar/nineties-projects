/*

File:	 pochklng.c
Purpose: Check if long option value is ok.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "optp.h"

/* If checkP is non-NULL, then if *checkP is 0, then checkP should point
 * to a 3-element long array such that check[1] and check[2] specify the
 * inclusive lower and upper bounds on val.  If *checkP is non-zero, then
 * it contains the number of elements in the int array checkP[].  In that
 * case val must be 1 of the elements checkP[1]...checkP[checkP[0] - 1].
 * If the value is ok, then NO_OPT_ERR is returned, otherwise OPT_ARG_ERR.
 */
OptErr 
_ZcheckOptLong(val, checkP)
  long val;
  CONST long *checkP;
{
  if (checkP) {
    if (*checkP == 0) {
      if (val < checkP[1] || val > checkP[2]) return RANGE_VAL_OPT_ERR;
    }
    else {
      CONST Index nCheckP= *checkP;
      Index i;
      for (i= 1; i < nCheckP; i++) {
	if (val == checkP[i]) break;
      }
      if (i == nCheckP) return CHECK_OPT_ERR;
    }
  }
  return NO_OPT_ERR;
}
