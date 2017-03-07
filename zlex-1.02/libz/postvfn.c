/*

File:	 postvfn.c
Purpose: Option function for a string-valued option with default value 0.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "optp.h"

/* The following function stores the index in checkP of the string
 * pointed to by argP in the location pointed to by intP when argP is
 * non-NULL.  If argP is NULL, the value -1 is stored.  checkP should
 * point to a NULL-terminated array of strings, which specify possible
 * values for the argument.  The comparison with the strings in checkP
 * require only that the string pointed to by argP be a unique prefix.
 */
int 
stringValOptFn(id, checkP, intP, argP)
  int id;
  VOIDP checkP;
  VOIDP intP;
  ConstString argP;
{
  int v= -1;
  if (argP) {
    ConstString *checkStrings= (ConstString *)checkP;
    Count argLen= strlen(argP);
    Count nMatch= 0;
    ConstString *p;
    assert(checkP != NULL);
    for (p= &checkStrings[0]; (*p != NULL); p++) {
      if (strncmp(*p, argP, argLen) == 0) { 
	v= p - checkStrings; nMatch++; 
	if (nMatch > 1) return AMBIGUOUS_VAL_OPT_ERR;
      } 
    }
    if (nMatch != 1) return CHECK_OPT_ERR;
  }
  *((int *)intP)= v;
  return NO_OPT_ERR;
}
