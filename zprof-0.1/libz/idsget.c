/*

File:	 idsget.c
Purpose: Get an ID routine for non-reentrant ids module.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "idsp.h"

Index 
getID(string, len)
  ConstString string;
  Size len;
{
  CONST Size len1= len + 1;
  ID id= getTextID(G.idTab, string, len1);
  if (id == NIL) {
    id= addTextID(G.idTab, addStrSpace(G.ss, string, len1), len1);
  }
  return id;
}


