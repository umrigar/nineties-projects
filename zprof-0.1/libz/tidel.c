/*

File:	 tidel.c
Purpose: Delete a TextID and free all its storage.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "area.h"
#include "textidp.h"


/* Delete the TextID structure denoted by textID and free all memory
 * occupied by it. 
 */
VOID 
delTextID(textID)
  TextID textID;
{
  TextIDP textIDP= (TextIDP) textID;
  delHashTab(textIDP->hashTab); delAreaX(&textIDP->tab);
  FREE(textIDP);
  VOID_RET();
}
