/*

File:	 htbasic.c
Purpose: Basic hashtable routines

Last Update Time-stamp: <97/07/30 14:31:56 zdu>

Copyright (C) 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "area.h"
#include "hashtabp.h"
#include "memalloc.h"
#include "zutil.h"

/* Create and return a new hashtable.  If optsP is NULL, then creates
 * a non-scoped hashtable with a default initSize and maxLoadFactor and
 * with codeFnP and equalFnP NULL.
 */
HashTab 
newHashTab(optsP)
  CONST HashOpts *optsP;
{
  enum { 
    DEFAULT_INIT_SIZE= 256, 
    DEFAULT_MAX_LOAD_FACTOR= 4
  };
  CONST Boolean isScoped= (optsP) ? optsP->isScoped : FALSE;
  CONST Boolean reorderOK= 
    isScoped ? FALSE : ((optsP) ? optsP->reorderOK : TRUE);
  CONST HashCodeFnP codeFnP= (optsP) ? optsP->codeFnP : NULL;
  CONST HashEqualFnP equalFnP= (optsP) ? optsP->equalFnP : NULL;
  CONST Size initSize= 
    (optsP) ? (1 << ceilingLg2(optsP->initSize)) : DEFAULT_INIT_SIZE;
  CONST Index maxLoadFactor= 
    (optsP) ? optsP->maxLoadFactor : DEFAULT_MAX_LOAD_FACTOR;
  HashTabP hashTabP= (HashTabP) MALLOC(sizeof(HashTabStruct));
  Index i;
  INIT_AREAX(hashTabP->entries, Entry, 2 + ceilingLg2(initSize), 0xFFFF);
  hashTabP->tab= MALLOC_VEC(initSize, HashIndex);
  for (i= 0; i < initSize; i++) hashTabP->tab[i]= NIL;
  hashTabP->codeFnP= codeFnP; hashTabP->equalFnP= equalFnP;
  hashTabP->nEntries= 0;
  hashTabP->mask= initSize - 1; hashTabP->maxLoadFactor= maxLoadFactor;
  hashTabP->isScoped= isScoped; hashTabP->reorderOK= reorderOK;
  { /* alway create a global scope */
    Index i= NEXT_ENTRY(hashTabP);
    ScopeEntry *CONST sP= &ENTRIES(hashTabP, i).u.scopeEntry;
    assert(i == 0);
    sP->succ= sP->pred= sP->frees= NIL; 
    ENTRIES(hashTabP, i).key= NULL_HASH_KEY;
    hashTabP->scope= i;
  }
  return hashTabP;
}

/* Delete previously created hashtable including all its entries. */
VOID 
delHashTab(hashTab)
  HashTab hashTab;
{
  HashTabP hashTabP= (HashTabP)hashTab;
  delAreaX(&hashTabP->entries);
  FREE(hashTabP->tab);
  FREE(hashTabP);
  VOID_RET();
}

/* Return value stored for key in hashTab.  NULL if none. If !isScoped, 
 * and key is found, then pull key to the front of its hash-chain.
 */
HashVal 
getHashTab(hashTab, key)
  HashTab hashTab;
  HashKey key;
{
  HashTabP hashTabP= (HashTabP)hashTab;
  CONST Boolean reorderOK= hashTabP->reorderOK;
  CONST HashCode c= 
    (hashTabP->codeFnP) ? hashTabP->codeFnP(key) : (HashCode)key;
  Index h= c & hashTabP->mask;
  HashEntry *p0= NULL; 	/* lag pointer */
  Index i, succ;
  for (i= hashTabP->tab[h]; i != NIL; i= succ) {
    HashEntry *p= &ENTRIES(hashTabP, i).u.hashEntry;
    HashKey key1= ENTRIES(hashTabP, i).key;
    if (p->code == c && 
	(hashTabP->equalFnP ? hashTabP->equalFnP(key1, key) : key1 == key)) {
      if (reorderOK && p0 != NULL) { /* move to front of hash chain */
	p0->succ= p->succ; 
	p->succ= hashTabP->tab[h]; hashTabP->tab[h]= i;
      }
      return p->val;
    }
    p0= p; succ= p->succ;
  }
  return NULL_HASH_VAL;
}

/* Rehash the hashtable, doubling its size.  Link entries into new 
 * hashchains. 
 */
static VOID
rehash(hashTabP)
  HashTabP hashTabP;
{
  CONST Size newSize= 2*(hashTabP->mask + 1);
  CONST Index mask= newSize - 1;
  CONST Count nEntries= NENTRIES(hashTabP);
  Index i;
  hashTabP->tab= REALLOC_VEC(hashTabP->tab, newSize, HashIndex);
  for (i= 0; i < newSize; i++) hashTabP->tab[i]= NIL;
  hashTabP->mask= mask;
  for (i= 0; i < nEntries; i++) {
    if (ENTRIES(hashTabP, i).key != NULL_HASH_KEY) {
      HashEntry *CONST p= &ENTRIES(hashTabP, i).u.hashEntry;
      CONST Index h= (p->code) & mask;
      p->succ= hashTabP->tab[h]; hashTabP->tab[h]= i;
    }
  }
  VOID_RET();
}

/* Enter key/val pair into hashTab. Neither should be "null". */
VOID 
putHashTab(hashTab, key, val)
  HashTab hashTab;
  HashKey key;
  HashVal val;
{
  HashTabP hashTabP= (HashTabP)hashTab;
  CONST HashCode c= 
    (hashTabP->codeFnP) ? hashTabP->codeFnP(key) : (HashCode)key;
  assert(key != NULL_HASH_KEY);
  assert(key != NULL_HASH_VAL);
  if (hashTabP->nEntries >= (hashTabP->mask + 1)*hashTabP->maxLoadFactor) {
    rehash(hashTabP);
  }
  { Index h= c & hashTabP->mask;
    ScopeEntry *CONST sP= &ENTRIES(hashTabP, hashTabP->scope).u.scopeEntry;
    Index i;
    if (sP->frees != NIL) {
      i= sP->frees; sP->frees= ENTRIES(hashTabP, i).u.scopeEntry.frees;
    }
    else {
      i= NEXT_ENTRY(hashTabP);
    }
    { HashEntry *CONST p= &ENTRIES(hashTabP, i).u.hashEntry;
      ENTRIES(hashTabP, i).key= key; p->val= val; p->code= c;
      p->succ= hashTabP->tab[h]; hashTabP->tab[h]= i;
    }
    hashTabP->nEntries++;
  }
  VOID_RET();
}

/* Return # of entries currently in hashTab. */
Count
nEntriesHashTab(hashTab)
  HashTab hashTab;
{
  return ((HashTabP)hashTab)->nEntries;
}


