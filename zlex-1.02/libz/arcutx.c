/*

File:	 arcutx.c
Purpose: Cut an AreaX.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "area.h"
#include "memalloc.h"


/* Forget all area entries after and including i. */
VOID 
cutAreaX(areaX, i)
  AreaX *areaX;
  Index i;
{
  CONST Count lg2Inc= areaX->lg2Inc;
  CONST Index i1= i - 1;
  CONST Index i1X= i1 >> lg2Inc;
  CONST Index i1Chunk= i1X + ((i1X<<lg2Inc) < i1);
  CONST Size size= areaX->size;
  CONST Index sizeX= size >> lg2Inc;
  CONST Count nChunks= sizeX + ((sizeX<<lg2Inc) < size);
  Index j;
  for (j= i1Chunk + 1; j < nChunks; j++) FREE(areaX->base[j]);
  areaX->size= i1Chunk << lg2Inc; areaX->next= i;
  VOID_RET();
}

