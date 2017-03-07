/*

File:	 tipop.c
Purpose: Pop a TextID until and including a specified ID.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "textidp.h"


/* Forget all IDs added to textID after and including id (which
 * must have been previously returned by either getTextID() or
 * addTextID()). 
 */
VOID 
popTextID(textID, id)
  TextID textID;
  IDX id;
{
  TextIDP textIDP= (TextIDP) textID;
  CONST Count nIDs= AX_NENTRIES(textIDP->tab);
  Index i;
  assert(id < nIDs);
  for (i= id; i < nIDs; i++) { /* Unlink ids from hash-chains. */
    Index h= TAB(i).hashSum & (textIDP->hashSize - 1);
    TabIndex *predP= &textIDP->hashTab[h];
    Index j;
    for (j= *predP; j != NIL; j= *predP) {
      if (j == i) break;
      predP= &TAB(j).succ;
    }
    assert(j != NIL); /* Must occur on hash-chain h. */
    *predP= TAB(j).succ; /* Unlink from hash chain. */
  }
  AX_CUT(textIDP->tab, id);
  VOID_RET();
}
