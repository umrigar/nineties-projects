/*

File:	 bsdel.c
Purpose: Delete a bit-set and recover all its storage.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

/* Returns set (which is invalid as a set). */
BSet 
delBSet(set)
  BSetX set;
{
  Hdr *hdrP= HDR_ELEMENT(set);
#ifdef DEBUG_BSET
  assert(hdrP->isFree == FALSE);
  hdrP->isFree= TRUE;
#endif
  FREE(hdrP->u.used.bitWords);
  hdrP->u.free.succ= frees; frees= set;
  return set;
}

