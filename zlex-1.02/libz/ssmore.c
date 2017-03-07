/*

File:	 ssmore.c
Purpose: Private routine to grow a string-space.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "sspacep.h"

char *
_ZssMoreSpace(ssP, size)
  StrSpaceP ssP;
  unsigned size;
{
  CONST Size blockSize= (size > SS_BLOCK_SIZE) ? size : SS_BLOCK_SIZE;
  SSBlock *CONST bP= (SSBlock *)MALLOC(sizeof(SSBlock));
  Char *CONST cP= (Char *)MALLOC(blockSize);
  assert(ssP->nFree < size);
  bP->pred= ssP->blockP; bP->chars= cP;
  ssP->blockP= bP; ssP->next= cP; ssP->nFree= blockSize;
  return cP;
}
