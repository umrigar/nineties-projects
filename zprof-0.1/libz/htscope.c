/*

File:	 htscope.c
Purpose: Scope control routines for hashtable.

Last Update Time-stamp: <97/07/08 11:19:40 zdu>

Copyright (C) 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "area.h"
#include "hashtabp.h"

/* Begin a new scope in hashTab and return it. */
HashScope 
beginScopeHashTab(hashTab)
  HashTab hashTab;
{
  HashTabP hashTabP= (HashTabP)hashTab;
  Index i= NEXT_ENTRY(hashTabP);
  ScopeEntry *CONST p= &ENTRIES(hashTabP, i).u.scopeEntry;
  ENTRIES(hashTabP, i).key= NULL_HASH_KEY;
  p->pred= hashTabP->scope; p->frees= p->succ= NIL;
  hashTabP->scope= i;
  return i;
}

/* End current scope in hashTab forgetting all entries in the scope. Returns
 * previous scope. 
 */
HashScope 
endScopeHashTab(hashTab)
  HashTab hashTab;
{
  HashTabP hashTabP= (HashTabP)hashTab;
  CONST HashScope scope= hashTabP->scope;
  CONST HashScope prevScope= ENTRIES(hashTabP, scope).u.scopeEntry.pred;
  CONST Index mask= hashTabP->mask;
  Index i;
  assert(hashTabP->isScoped);
  assert(scope != NIL);
  for (i= NENTRIES(hashTabP) - 1; i > scope; i--) {
    if (ENTRIES(hashTabP, i).key != NULL_HASH_KEY) {
      HashEntry *CONST p= &ENTRIES(hashTabP, i).u.hashEntry;
      Index h= p->code & mask;
      assert(hashTabP->tab[h] == i);
      hashTabP->tab[h]= p->succ;
      hashTabP->nEntries--;
    }
  }
  AX_CUT(hashTabP->entries, scope);
  hashTabP->scope= prevScope;
  ENTRIES(hashTabP, prevScope).u.scopeEntry.succ= NIL;
  return prevScope;
}

/* Return current scope in hashTab. */
HashScope 
currentScopeHashTab(hashTab)
  HashTab hashTab;
{
  HashTabP hashTabP= (HashTabP)hashTab;
  assert(hashTabP->isScoped);
  return hashTabP->scope;
}

/* Return parent of scope in hashTab. */
HashScope 
parentScopeHashTab(hashTab, scope)
  HashTab hashTab;
  HashScope scope;
{
  HashTabP hashTabP= (HashTabP)hashTab;
  assert(hashTabP->isScoped);
  assert(scope != NIL);
  return ENTRIES(hashTabP, scope).u.scopeEntry.pred;
}



