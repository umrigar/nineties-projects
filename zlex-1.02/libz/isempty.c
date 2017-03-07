/*

File:	 isempty.c
Purpose: Check if a int-set is empty.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

Boolean 
FN_NAME(isEmptyNISet, isEmptyDISet)(set DEBUG_ARGS)
  CONST ISet set;
  DEBUG_DECS
{
  ISET_READ_OK(set);
  return BLK_SUCC((ISetBlk*)set) == NULL;
}

