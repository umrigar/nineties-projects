/*

File:	 tiadd.c
Purpose: Add text to a TextID and return corresponding ID.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

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
  CONST int hashSum= _ZhashText(textIDP, text, len);
  CONST Index h= hashSum & (textIDP->hashSize - 1);
  CONST Index i= AX_NEXT(textIDP->tab);
  TAB(i).text= text; TAB(i).len= len;  TAB(i).hashSum= hashSum;
  TAB(i).succ= textIDP->hashTab[h]; textIDP->hashTab[h]= i;
  if (i > textIDP->hashSize * MAX_LOAD_FACTOR) {
    _ZresizeHashTab(textIDP);
  }
  return i;
}
