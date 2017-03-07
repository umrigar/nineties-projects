/*

File:	 htcursor.c
Purpose: Iteration routines for hashtable.

Last Update Time-stamp: <97/07/30 14:33:45 zdu>

Copyright (C) 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "area.h"
#include "hashtabp.h"

/* hash-table iterator:  If cursor is NULL_HASH_CURSOR, then the
 * iterator is initialized.  The type of iterator depends on the 
 * values of key and scope:
 * 	key	scope	Type of iterator
 *	null	null	Iterate over all entries in hashTab.
 *	null   !null	Iterate over all entries in scope.
 *     !null	null	Iterate over all entries for key.
 *     !null   !null	Iterate over all entries for key in scope.
 *
 * The "null" values should be specified using NULL_HASH_KEY and
 * NULL_HASH_SCOPE respectively.
 * For a particular iteration, the caller is responsible for passing in
 * the same values of key and scope to each call to this routine.
 * The only operations on the hashtable while a cursor is active 
 * should be getCursorKeyHashTab() and getCursorValHashTab().
 * Returns NULL_HASH_CURSOR if the iterator exhausted.
 */
HashCursor 
stepCursorHashTab(hashTab, cursor, key, scope)
  HashTab hashTab;
  HashCursor cursor;
  HashKey key;
  HashScope scope;
{
  HashTabP hashTabP= (HashTabP)hashTab;
  CONST ScopeEntry *CONST p= 
    (scope != NULL_HASH_SCOPE) ? &ENTRIES(hashTabP, scope).u.scopeEntry : NULL;
  CONST Index lo= (p == NULL) ? ((hashTabP->isScoped) ? 1 : 0) : scope + 1;
  CONST Index hi= (p == NULL || p->succ == NIL) ? NENTRIES(hashTabP)
                                                : p->succ;
  Index cursorZ= NULL_HASH_CURSOR;
  assert(scope == NULL_HASH_SCOPE || scope < NENTRIES(hashTabP));
  if (cursor == NULL_HASH_CURSOR) { /* 1st iteration */
    if (key == NULL_HASH_KEY) { /* 1st iteration; fixed key */
      Index i;
      for (i= hi; i > lo; i--) {
	Index j= i - 1;
	if (ENTRIES(hashTabP, j).key != NULL_HASH_KEY) {
	  cursorZ= j; break;
	}
      } /* for (i= hi; i > lo; i--) */
    } /* if (key == NULL_HASH_KEY) 1st iteration; successive keys */
    else { /* 1st iteration; fixed key */
      CONST HashCode c= 
	(hashTabP->codeFnP) ? hashTabP->codeFnP(key) : (HashCode)key;
      Index h= c & hashTabP->mask;
      Index i, succ;
      for (i= hashTabP->tab[h]; i != NIL; i= succ) {
	HashEntry *p= &ENTRIES(hashTabP, i).u.hashEntry;
	CONST HashKey key1= ENTRIES(hashTabP, i).key;
	/* don't bale out of loop early when i < lo, as chains may be 
	 * reordered when !isScoped.
	 */
	if (p->code == c && lo <= i && i < hi &&
	    (hashTabP->equalFnP ? hashTabP->equalFnP(key1, key) : key1==key)) {
	  cursorZ= i;
	  break;
	}
	succ= p->succ;
      }
    } /* else 1st iteration; fixed key */
  } /* if (cursor == NULL_HASH_CURSOR) 1st iteration */
  else { /* not 1st iteration */
    if (key == NULL_HASH_KEY) { /* not 1st iteration; successive keys */
      Index i;
      for (i= cursor; i > lo; i--) {
	Index j= i - 1;
	if (ENTRIES(hashTabP, j).key != NULL_HASH_KEY) {
	  cursorZ= j; break;
	}
      }
    }
    else { /* not 1st iteration; fixed key */
      Index i, succ;
      CONST HashCode c= ENTRIES(hashTabP, cursor).u.hashEntry.code;
      assert(ENTRIES(hashTabP, cursor).key == key);
      for (i= ENTRIES(hashTabP, cursor).u.hashEntry.succ; i != NIL; i= succ) {
	HashEntry *p= &ENTRIES(hashTabP, i).u.hashEntry;
	CONST HashKey key1= ENTRIES(hashTabP, i).key;
	/* don't bale out of loop early when i < lo, as chains may be 
	 * reordered when !isScoped.
	 */
	if (p->code == c && lo <= i && i < hi &&
	    (hashTabP->equalFnP ? hashTabP->equalFnP(key1, key) : key1==key)) {
	  cursorZ= i;
	  break;
	}
	succ= p->succ;
      }
    } /* else not 1st iteration; fixed key */
  } /* else not 1st iteration */
  return cursorZ;
}

/* Return the key associated with cursor in hashTab. */
HashKey 
getCursorKeyHashTab(hashTab, cursor)
  HashTab hashTab; 
  HashCursor cursor;
{
  HashTabP hashTabP= (HashTabP)hashTab;
  assert(cursor != NULL_HASH_CURSOR);
  assert(cursor < NENTRIES(hashTabP));
  return ENTRIES(hashTabP, cursor).key;
}

/* Return the value associated with cursor in hashTab. */
HashVal getCursorValHashTab(hashTab, cursor)
  HashTab hashTab; 
  HashCursor cursor;
{
  HashTabP hashTabP= (HashTabP)hashTab;
  assert(cursor != NULL_HASH_CURSOR);
  assert(cursor < NENTRIES(hashTabP));
  return ENTRIES(hashTabP, cursor).u.hashEntry.val;
}

/* Set the value at a particular cursor. */
VOID 
setCursorValHashTab(hashTab, cursor, val)
  HashTab hashTab; 
  HashCursor cursor;
  HashVal val;
{
  HashTabP hashTabP= (HashTabP)hashTab;
  assert(cursor != NULL_HASH_CURSOR);
  assert(cursor < NENTRIES(hashTabP));
  assert(val != NULL_HASH_VAL);
  ENTRIES(hashTabP, cursor).u.hashEntry.val= val;
  VOID_RET();
}
