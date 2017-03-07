/*

File:	 tiidlen.c
Purpose: Return the length of the text associated with an ID.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "textidp.h"


/* Return the total # of characters stored for id (which must have been 
 * previously returned by either getTextID() or addTextID()).
 */
Size 
idLen(textID, id)
  TextID textID;
  IDX id;
{
  TextIDP textIDP= (TextIDP) textID;
  assert(id < AX_NENTRIES(textIDP->tab));
  return TAB(id).len;
}
