/*

File:	 argrow.c
Purpose: Grow an Area, increasing its size.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "area.h"
#include "errorz.h"		/* For error routines. */
#include "memalloc.h"

unsigned 
_ZarGrowArea(area)
  Area *area;
{
  assert(area->next == area->size);
  assert(1 << area->lg2Inc < area->max);
  if (area->isFrozen) {
    libzError(ER_ERROR|ER_FATAL, 
	      "Internal error: attempt to grow frozen area.");
  }
  else if (area->size >= area->max)
    libzError(ER_ERROR|ER_FATAL, "Area overflow.");
  else {
    CONST Size inc= 1 << area->lg2Inc;
    CONST Size newSize= 
      (area->size + inc < area->max) ? area->size + inc : area->max;
    area->base= REALLOC(area->base, newSize * area->typeSize);
    area->size= newSize;
  }
  return area->next++;
}
