/*

File:	 arreset.c
Purpose: Reset an Area to a specified memory area.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "area.h"
#include "memalloc.h"

/* Reset area a to newBase and nEntries. */
VOID
resetArea(a, newBase, newSize)
  Area *a;
  VOIDP newBase;
  unsigned newSize;
{
  FREE(a->base); 
  a->base= newBase; a->size= a->next= newSize;
  VOID_RET();
}

