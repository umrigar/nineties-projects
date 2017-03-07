/*

File:	 idsfail.c
Purpose: Get an ID with failure routine for non-reentrant ids module.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "idsp.h"

Index 
getIDFail(string, len)
  ConstString string;
  Size len;
{
  CONST Size len1= len + 1;
  ID id= getTextID(G.idTab, string, len1);
  return id;
}


