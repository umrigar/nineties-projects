/*

File:	 idslen.c
Purpose: Get length for an ID routine in non-reentrant ids module.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "idsp.h"

Size 
getIDLen(id)
  Index id;
{
  return idLen(G.idTab, id) - 1;
}

