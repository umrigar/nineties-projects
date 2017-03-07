/*

File:	 tigetid.c
Purpose: Search a TextID for some text and return its ID (NIL if not found).

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "textidp.h"
#include "strspace.h"
static Boolean 
ignoreCaseTextEq(CONST char *text1, CONST char *text2, Size len);


static Boolean 
ignoreCaseTextEq(text1, text2, len)
  CONST char *text1;
  CONST char *text2;
  Size len;
{
  CONST char *p1;
  CONST char *p2;
  CONST char *limit1= text1 + len;
  for (p1= text1, p2= text2; p1 < limit1; p1++, p2++) {
    if (tolower(*p1) != tolower(*p2)) return FALSE;
  }
  return (p1 == limit1);
}

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
  CONST int hashSum= _ZhashText(textIDP, text, len);
  CONST Index h= hashSum & (textIDP->hashSize - 1);
  Index i0= NIL;	/* i0 lags i. */
  Index i= textIDP->hashTab[h];
  if (textIDP->ignoreCase) {
    while (i != NIL) {
      TabEntry *teP= &TAB(i);
      if (len == teP->len && hashSum == teP->hashSum && 
	  ignoreCaseTextEq(text, teP->text, len)) {
	break;
      }
      i0= i; i= teP->succ;
    }
  }
  else {
    while (i != NIL) {
      TabEntry *teP= &TAB(i);
      if (len == teP->len && hashSum == teP->hashSum &&
	  memcmp(text, teP->text, len) == 0) {
	break;
      }
      i0= i; i= teP->succ;
    }
  }
  if (i != NIL) { /* Found it: move entry to front of hash-chain. */
    if (i0 == NIL) {
      /* Entry is already in front of hash-chain. */
    }
    else { /* i0 is predecessor of i. */
      TAB(i0).succ= TAB(i).succ; TAB(i).succ= textIDP->hashTab[h];
      textIDP->hashTab[h]= i;
    }
  }
  return i;
}
