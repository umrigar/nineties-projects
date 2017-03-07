/*

File:	 idsstrng.c
Purpose: Get string for an ID routine in non-reentrant ids module.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "idsp.h"

ConstString 
getIDString(id)
  Index id;
{
  return idText(G.idTab, id);
}


