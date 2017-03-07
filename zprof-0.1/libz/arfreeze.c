/*

File:	 arfreeze.c
Purpose: Freeze an Area so that its size cannot be changed subsequently.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "area.h"
#include "memalloc.h"

VOID
freezeArea(a)
  Area *a;
{
/* No further calls to A_NEXT().  Return all memory not being used
 * back to system. 
 */
  if (a->next < a->size) {	/* a->size > 0 to succeed. */
    assert(a->base != 0);
    a->base= REALLOC(a->base, a->next * a->typeSize);
  }
  a->size= a->next;
  a->isFrozen= 1;
  VOID_RET();
}

VOID
unfreezeArea(a)
  Area *a;
{
  a->isFrozen= 0;
  VOID_RET();
}
