/*

File:	 armem.c
Purpose: Return the memory associated with an area and reset the area.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "area.h"
#include "memalloc.h"

/* Return dynamically allocated (can be FREEd) current vector (with
 * A_NENTRIES elements).  Reset the area for a new vector.
 */
VOIDP
getMemFromArea(aP)
  Area *aP;
{
  VOIDP base= aP->base;
  Count nEntries= aP->next;
  aP->base= NULL; aP->next= 0;
  return REALLOC(base, nEntries * aP->typeSize);
}

