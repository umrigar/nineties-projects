/*

File:	 isdel.c
Purpose: Delete a int-set and recover all its storage.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

/* Deletes int-set set. */
VOID 
FN_NAME(delNISet, delDISet)(set DEBUG_ARGS)
  ISet set;
  DEBUG_DECS
{
  ISetBlk *p0, *p1;
  ISET_OK(set);
  for (p0= (ISetBlk*)set; p0 != NULL; p0= p1) {
    p1= BLK_SUCC(p0); DEL_BLK(p0);
  }
  VOID_RET();
}

