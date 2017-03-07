/*

File:	 tiidtext.c
Purpose: Return the text associated with an ID.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "textidp.h"


/* Return the pointer to the stored text for id (which must have been 
 * previously returned by either getTextID() or addTextID()).
 */
CONST char *
idText(textID, id)
  TextID textID;
  IDX id;
{
  TextIDP textIDP= (TextIDP) textID;
  assert(id < AX_NENTRIES(textIDP->tab));
  return TAB(id).text; 
}
