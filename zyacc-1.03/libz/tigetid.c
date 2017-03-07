/*

File:	 tigetid.c
Purpose: Search a TextID for some text and return its ID (NIL if not found).

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "textidp.h"
#include "strspace.h"

/* Return the ID corresponding to text (having a total of len chars).  
 * Return NIL if not found in textID.  If found move to front of hash-chain.
 */
ID 
getTextID(textID, text, len)
  TextID textID;
  CONST char *text;
  Size len;
{
  TextIDStruct *textIDP= (TextIDStruct *) textID;  
  HashVal v;
  TabEntry entry;
  entry.text= text; entry.len= len;
  v= getHashTab(textIDP->hashTab, (HashKey)&entry);
  return v == NULL_HASH_VAL ? NIL : (Index)v - 1;
}
