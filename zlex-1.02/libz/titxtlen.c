/*

File:	 titxtlen.c
Purpose: Return the text and length associated with an ID.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "textidp.h"


/* Return the text and the length in a single call. */
CONST Char * 
idTextLen(textID, id, lenP)
  TextID textID;
  IDX id;
  Size *lenP;
{
  TextIDP textIDP= (TextIDP) textID;
  assert(id < AX_NENTRIES(textIDP->tab));
  *lenP= TAB(id).len;
  return TAB(id).text;
}

