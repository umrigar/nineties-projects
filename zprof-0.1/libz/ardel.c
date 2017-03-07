/*

File:	 ardel.c
Purpose: Delete an Area.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "area.h"
#include "memalloc.h"


/* Free all memory occupied by area. */
VOID 
delArea(area)
  Area *area;
{
  FREE((VOIDP)area->base); 
  area->size= area->next= 0;
  VOID_RET();
}

