/*

File:	 idslen.c
Purpose: Intern text without identifying it in non-reentrant ids module.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "idsp.h"

ConstString
internText(text, len)
  CONST Char *text;
  Size len;
{
  return addStrSpace(G.ss, text, len + 1);
}

