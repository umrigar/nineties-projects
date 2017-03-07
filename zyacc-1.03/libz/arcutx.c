/*

File:	 arcutx.c
Purpose: Cut an AreaX.

Last Update Time-stamp: <97/06/25 10:14:37 zdu>

Copyright (C) 1995, 1997 Zerksis D. Umrigar

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
  CONST Size size= areaX->size;
  CONST Count nChunks= size >> lg2Inc;
  Index j;
  assert(i <= areaX->next);
  assert(nChunks << lg2Inc == size);
  if (i == 0) {
    for (j= 0; j < nChunks; j++) FREE(areaX->base[j]);
    areaX->size= 0;
  }
  else {
    CONST Index i1= i - 1;
    CONST Index i1Chunk= i1 >> lg2Inc;
    for (j= i1Chunk + 1; j < nChunks; j++) FREE(areaX->base[j]);
    areaX->size= (i1Chunk + 1) << lg2Inc; 
  }
  areaX->next= i;
  VOID_RET();
}

