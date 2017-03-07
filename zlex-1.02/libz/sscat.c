/*

File:	 sscat.c
Purpose: Catenate a string to the last added string in a string-space.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "sspacep.h"

Char *
catStrSpace(strSpace, text, textLen)
  StrSpace strSpace;
  CONST Char *text;
  unsigned textLen;
/* Destructively append text to the last string added to the
 * string-space.  Return a pointer to the appended string.  Previous
 * string-space pointers to the last string are no longer valid. 
 */
{
  StrSpaceP ssP= (StrSpaceP) strSpace;
  char *catText= ssP->next - ssP->lastLen;
  CONST Size len= textLen + ssP->appendNUL;
  if (ssP->nFree < textLen) { /* Copy old text sans NUL into a new block. */
    CONST char *CONST oldText= ssP->next - ssP->lastLen;
    CONST Size oldLen= ssP->lastLen - (ssP->appendNUL);
    CONST Size newLen= ssP->lastLen + textLen;
    catText= _ZssMoreSpace(ssP, newLen);
    memcpy(catText, oldText, (SIZE_T)oldLen); 
  }
  else if (ssP->appendNUL) { /* Forget previously appended NUL. */
    ssP->nFree++; ssP->next--;
  }
  /* ssP->next, ssP->nFree reflect end of old text without a NUL. */
  memcpy(ssP->next, text, (SIZE_T)textLen);
  if (ssP->appendNUL) ssP->next[textLen]= '\0';
  ssP->nFree-= len; ssP->next+= len; ssP->lastLen+= textLen;
  return catText;
}
