/*

File:	 ssinit.c
Purpose: Create and initialize a string-space. 

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "sspacep.h"

StrSpace 
initStrSpace(appendNUL)
  BooleanX appendNUL;
{
  StrSpaceP ssP= (SSStruct *)MALLOC(sizeof(SSStruct));
  ssP->blockP= NULL; ssP->next= NULL; ssP->nFree= ssP->lastLen= 0;
  ssP->appendNUL= appendNUL;
  return (StrSpace) ssP;
}

