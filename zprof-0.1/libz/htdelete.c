/*

File:	 htdelete.c
Purpose: Hashtable deletion routine.

Last Update Time-stamp: <97/07/08 11:27:01 zdu>

Copyright (C) 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "hashtabp.h"

/* Delete entry at a particular cursor.  Not particularly time efficient 
 */
VOID
delCursorHashTab(hashTab, cursor)
  HashTab hashTab; 
  HashCursor cursor;
{
  HashTabP hashTabP= (HashTabP)hashTab;
  CONST HashEntry *CONST hP= &ENTRIES(hashTabP, cursor).u.hashEntry;
  CONST Index h= hP->code & hashTabP->mask;
  HashScope scope= hashTabP->scope;
  assert(cursor != NULL_HASH_CURSOR);
  assert(cursor < NENTRIES(hashTabP));
  if (hashTabP->tab[h] == cursor) { /* unlink from head of hash chain */
    hashTabP->tab[h]= hP->succ;
  }
  else { /* unlink from properly within hash chain */
    HashIndex i0= hashTabP->tab[h]; /* lag index */
    HashIndex i;
    assert(i0 != NIL);
    for (i= ENTRIES(hashTabP, i0).u.hashEntry.succ; 
	 i != cursor;
	 i0= i, i= ENTRIES(hashTabP, i0).u.hashEntry.succ) {
      assert(i != NIL);
    }
    ENTRIES(hashTabP, i0).u.hashEntry.succ= hP->succ;
  }
  /* link into frees list for proper scope */
  while (cursor < scope) scope= ENTRIES(hashTabP, scope).u.scopeEntry.pred;
  ENTRIES(hashTabP, cursor).key= NULL_HASH_KEY;
  ENTRIES(hashTabP, cursor).u.scopeEntry.frees= 
    ENTRIES(hashTabP, scope).u.scopeEntry.frees;
  ENTRIES(hashTabP, scope).u.scopeEntry.frees= cursor;
  hashTabP->nEntries--;
  VOID_RET();
}
