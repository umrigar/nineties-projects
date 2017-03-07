/*

File:	 ssdel.c
Purpose: Delete a string-space.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "sspacep.h"

/* Delete string-space strSpace, releasing all memory occupied by it. */
VOID 
delStrSpace(strSpace)
  StrSpace strSpace;
{
  popStrSpace(strSpace, NULL);
  FREE(strSpace);
  VOID_RET();
}

