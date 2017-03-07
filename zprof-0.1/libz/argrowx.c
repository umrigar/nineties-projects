/*

File:	 argrowx.c
Purpose: Grow an AreaX, increasing its size.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "area.h"
#include "errorz.h"		/* For error routines. */
#include "memalloc.h"

unsigned 
_ZarGrowAreaX(areaX)
  AreaX *areaX;
{
  assert(areaX->next == areaX->size);
  assert(1 << areaX->lg2Inc < areaX->max);
  if (areaX->size >= areaX->max)
    libzError(ER_ERROR|ER_FATAL, "AreaX overflow.");
  else {
    enum {
      LG2_BASE_INC_DIFF= 4
    };
    CONST Count lg2Inc= areaX->lg2Inc;
    CONST Count inc= 1L << lg2Inc;
    CONST Count oldSize= areaX->size;
    CONST Count baseIndex= oldSize >> lg2Inc;
    CONST Count lg2BaseInc= 
      (lg2Inc > LG2_BASE_INC_DIFF) ? lg2Inc - LG2_BASE_INC_DIFF : 1;
    CONST Count baseInc= 1L << lg2BaseInc;
    CONST Size newSize= 
      (areaX->size + inc < areaX->max) ? areaX->size + inc : areaX->max;
    if ((baseIndex & (baseInc - 1)) == 0) {
      areaX->base= REALLOC_VEC(areaX->base, baseIndex + baseInc, VOIDP);
    }
    areaX->base[baseIndex]= MALLOC(inc * areaX->typeSize);
    areaX->size= newSize;
  }
  return areaX->next++;
}
