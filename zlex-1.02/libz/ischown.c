/*

File:	 ischown.c
Purpose: Change the ownership of a ISet.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/* This module is used only for DEBUG_ISET */
#define DEBUG_ISET

#include "isetp.h"

/* Change ownership of ISet set iff it isn't a BAD_ISET.  */
ISet 
chownDISet(set, newMod, modName, fileName, lineN)
  ISet set;
  ConstString newMod;
  ConstString modName;
  ConstString fileName;
  UInt lineN;
{
  if (set != BAD_ISET) {
    ISetBlk *CONST hdrP= (ISetBlk*)set;
    ISET_OK(set);
    SET_OWNER(hdrP, newMod);
  }
  return set;
}


