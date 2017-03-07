/*

File:	 pochrfn.c
Purpose: Basic option function for a character-valued option.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "optp.h"

int 
charOptFn(id, checkP, charP, argP)
  int id;
  VOIDP checkP;
  VOIDP charP;
  ConstString argP;
{
  char c;
  if (argP) {
    OptErr err= (argP[1] != '\0') ? CHECK_OPT_ERR 
                                  : _ZcheckOptLong((long)argP[0], checkP);
    if (err) return err;
    c= argP[0];
  }
  else {
    c= '\0';
  }
  *((char *) charP)= c;
  return NO_OPT_ERR;
}


