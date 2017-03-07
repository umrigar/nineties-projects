/*

File:	 isnew.c
Purpose: Return a new empty integer-set.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

ISet 
FN_NAME(newNISet, newDISet)(DEBUG_ARGS0)
  DEBUG_DECS
{
  ISetBlk *hdrP= NEW_BLK();
#ifdef DEBUG_ISET
  SET_OWNER(hdrP, modName);
#else
  HDR_OWNER(hdrP)= NULL;
#endif
  BLK_SUCC(hdrP)= NULL;
  return (ISet)hdrP;
}

