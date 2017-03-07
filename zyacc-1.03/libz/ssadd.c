/*

File:	 ssadd.c
Purpose: Add a string to a string-space.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "sspacep.h"

/* Save text in string-space, adding '\0' if appendNUL requested. 
 * If text is NULL, then merely reserve space without giving it
 * any contents.
 */
Char *
addStrSpace(strSpace, text, textLen)
  StrSpace strSpace;
  CONST Char *text;
  unsigned textLen;
{
  StrSpaceP ssP= (StrSpaceP) strSpace;
  CONST unsigned len=  textLen + ssP->appendNUL;
  char *CONST cP= 
    (len > ssP->nFree) ? _ZssMoreSpace(ssP, len) : ssP->next; 
  if (text) memcpy(cP, text, (SIZE_T)textLen);
  if (ssP->appendNUL) cP[textLen]= '\0';
  ssP->next+= len; ssP->nFree-= len; ssP->lastLen= len;
  return cP;
}
