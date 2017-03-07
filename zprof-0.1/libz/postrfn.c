/*

File:	 postrfn.c
Purpose: Option function for a string-valued option with default value "".

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "optp.h"

int 
stringOptFn(id, checkP, stringP, argP)
  int id; 
  VOIDP checkP;
  VOIDP stringP;
  ConstString argP;
{
  *((CONST char **) stringP)= (argP) ? argP : "";
  return NO_OPT_ERR;
}

