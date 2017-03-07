/*

File:	 bsprint.c
Purpose: Print a bit-set on <stdout>.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "bsetp.h"

BSet 
printBSet(set)
  CONST BSetX set;
{
  return fprintBSet(stdout, set);
}

