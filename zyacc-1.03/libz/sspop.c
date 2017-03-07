/*

File:	 sspop.c
Purpose: Pop a string-space until and including an intern'd string.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "sspacep.h"


VOID 
popStrSpace(strSpace, text)
  StrSpace strSpace;
  char *text;
/* Destroy space in strSpace allocated since (including) text. text == NULL
 * means destroy all space.
 */
{
  StrSpaceP ssP= (StrSpaceP) strSpace;
  while (ssP->blockP && 
	 (!text || text < ssP->blockP->chars || 
	  text >= ssP->blockP->chars + SS_BLOCK_SIZE)) {
    SSBlock *p= ssP->blockP->pred;
    FREE(ssP->blockP->chars); FREE(ssP->blockP); 
    ssP->blockP= p;
  }
  ssP->next= text; 
  ssP->nFree= 
    (ssP->blockP) ? (SS_BLOCK_SIZE - (text - ssP->blockP->chars)) : 0;
  ssP->lastLen= 0;
  VOID_RET();
}
