/*

File:	 isprint.c
Purpose: Print a int-set on <stdout>.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "isetp.h"

Int
FN_NAME(printNISet, printDISet)(set DEBUG_ARGS)
  CONST ISet set;
  DEBUG_DECS
{
  ISET_READ_OK(set);
  return fprintISet(stdout, set);
}

