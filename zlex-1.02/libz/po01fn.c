/*

File:	 po01fn.c
Purpose: Yes/No, 0/1 option function.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "optp.h"

static ConstString words[]= { /* Allow unambiguous prefixes. */
  "0", "no", "off", "n", "of", 
  "1", "yes", "on", "ye", "" };

/* The following function stores a 0 or 1 (1 if argP is NULL) in the
 * character pointed to by charP if the argP points to "0", "no", "off",
 * or "1", "yes", "on" or an appropriate unambiguous prefix.
 */
int 
yesNoOptFn(id, checkP, charP, argP)
  int id;
  VOIDP checkP;
  VOIDP charP;
  ConstString argP;
{
  char *cP= (char *)charP;
  if (!argP) {
    *cP= 1;
  }
  else {
    CONST Count nWords= N_ELEMENTS(words);
    Count argLen= strlen(argP);
    Count nMatch= 0;
    Index matchIndex;
    Index i;
    for (i= 0; i < nWords; i++) {
      if (strncmp(words[i], argP, argLen) == 0) { 
	matchIndex= i; nMatch++; 
	if (nMatch > 1) return AMBIGUOUS_OPT_ERR;
      } 
    }
    if (nMatch != 1) return CHECK_OPT_ERR;
    *cP= (matchIndex >= nWords/2);
  }
  return NO_OPT_ERR;
}
