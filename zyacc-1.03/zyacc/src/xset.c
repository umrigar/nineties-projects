/*

File:	 xset.c
Purpose: Space intensive "bit" sets.

Last Update Time-stamp: "97/07/10 18:15:36 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "xset.h"

/* System includes via port.h. */

int 
xsetElementCompare(p1, p2)
  CONST VOIDP p1;
  CONST VOIDP p2;
{
  Index e1= *((Index2 *) p1);
  Index e2= *((Index2 *) p2);
  assert(e1 != e2);
  return (e1 < e2) ? -1 : 1;
}

