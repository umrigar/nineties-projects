/*

File:	 isdebug.c
Purpose: Debugging support for ISet module.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/* This is used only when DEBUG_ISET. */
#define DEBUG_ISET

#include "isetp.h"

/* Calls abort if any block in set does not have its lo divisible by
 * N_ELEMENTS_PER_BLK or not in ascending order.  Also checks
 * ownership.
 */
VOID
_ZISetOk(set, modName, fileName, lineN)
  CONST ISet set;
  ConstString modName;
  ConstString fileName;
  UInt lineN;
{
  CONST ISetBlk *p= (ISetBlk*)set;
  ISetElement lastLo= INT_MIN;
  if (p == BAD_ISET != 0) {
    fprintf(stderr, "%s:%d: bad iset header.\n", fileName, lineN); 
    abort();
  }
  if (modName && HDR_OWNER(p) && strcmp(HDR_OWNER(p), modName) != 0) {
    fprintf(stderr, 
	    "%s:%d: DEBUG_ISET ownership violation on set %s:%d owned by %s\n",
	    fileName, lineN, HDR_FILE(p), HDR_LINE(p), HDR_OWNER(p));
  }
  for (p= BLK_SUCC((ISetBlk*)set); p != NULL; p= BLK_SUCC(p)) {
    if (BLK_LO(p) % N_ELEMENTS_PER_BLK != 0 || lastLo >= BLK_LO(p)) {
      fprintf(stderr, "%s:%d: bad iset block.\n", fileName, lineN); 
      abort();
    }
    lastLo= BLK_LO(p);
  }
  VOID_RET();
}


