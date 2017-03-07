/*

File:	 bsunary.c
Purpose: Create a new bit-set containing a single specified element.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

BSet 
unaryBSet(BSetElement e)
{
  BSet s= newBSet();
  return addBSet(s, e);
}

