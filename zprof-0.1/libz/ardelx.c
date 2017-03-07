/*

File:	 ardelx.c
Purpose: Delete an AreaX.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "area.h"
#include "memalloc.h"


/* Free all memory occupied by area. */
VOID 
delAreaX(areaX)
  AreaX *areaX;
{
  CONST Size size= areaX->size;
  CONST Count lg2Inc= areaX->lg2Inc;
  CONST Count nChunks= 
    (size >> lg2Inc) + (((size >> lg2Inc) << lg2Inc) < size);
  Index i;
  for (i= 0; i < nChunks; i++) FREE(areaX->base[i]);
  FREE(areaX->base); 
  areaX->size= areaX->next= 0;
  VOID_RET();
}

