/*

File:	 tiadd.c
Purpose: Add text to a TextID and return corresponding ID.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "hashtab.h"
#include "textidp.h"

/* Add the text (having a total of len chars) to textID and return the
 * corresponding ID.
 */
ID 
addTextID(textID, text, len)
  TextID textID;
  CONST char *text;
  Size len;
{
  TextIDP textIDP= (TextIDP) textID;
  CONST Index i= AX_NEXT(textIDP->tab);
  TAB(i).text= text; TAB(i).len= len;
  putHashTab(textIDP->hashTab, (HashKey)&TAB(i), (HashVal)(i + 1));
  return i;
}
