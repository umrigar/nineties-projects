/*

File:	 hashtab.h
Purpose: Hashtable routines

Last Update Time-stamp: <97/07/30 14:34:34 zdu>

Copyright (C) 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _HASHTAB_H
#define _HASHTAB_H

#include "ccport.h"

typedef VOIDP HashTab;		/* a hashtable: opaque to client */
typedef VOIDP HashKey;		/* hashtable key: opaque to this module */
typedef VOIDP HashVal;		/* value for key: opaque to this module */
typedef Index HashCursor;	/* cursor for iterator: opaque to client */
typedef Index HashScope;	/* scope: opaque to client */
typedef ULong HashCode;		/* hashcode: used by both client & this mod */

/* Distinguished "null" values for some of the above types. */
#define NULL_HASH_KEY		NULL
#define NULL_HASH_VAL		NULL
#define NULL_HASH_SCOPE 	NIL
#define NULL_HASH_CURSOR	NIL

/* Type of pointer to function used to compute hashCode for a
 * specified key.  If specified as NULL, then the HashCode for a key
 * is computed by simply casting the key to HashCode.
 */
typedef HashCode (*HashCodeFnP) PROTO((CONST HashKey key));

/* Type of pointer to function used to compare two keys for equality.
 * If specified as NULL, then the keys are compared by comparing 
 * the actual key values.
 */
typedef Boolean (*HashEqualFnP) PROTO((CONST HashKey key1, 
				       CONST HashKey key2));

typedef struct {	/* hashtable options used in constructing table */
  HashCodeFnP codeFnP;	/* function used to compute hashcode */
  HashEqualFnP equalFnP;/* function used to compute keys for equality */
  Size2 initSize;	/* initial size of hashtable */
  Index2 maxLoadFactor;	/* grow table if #entries/hashtable size > this */
  Boolean isScoped;	/* TRUE if hashtable should be scoped */
  Boolean reorderOK;	/* FALSE==> identical key entries accessed LIFO;
			 * TRUE==> identical keys entries accessed in 
			 * arbitrary order;
			 * if isScoped, then this option is always FALSE.
			 */
} HashOpts;

/* Create and return a new hashtable.  If optsP is NULL, then creates
 * a non-scoped hashtable, with hash-chain reordering permitted with a
 * default initSize and maxLoadFactor and with codeFnP and equalFnP
 * NULL.  The structure pointed to by optsP can be forgotten once
 * this routine returns.
 */
HashTab newHashTab PROTO((CONST HashOpts *CONST optsP));

/* Delete previously created hashtable including all its entries. */
VOID delHashTab PROTO((HashTab hashTab));

/* Return value stored for key in hashTab.  NULL_HASH_VAL if none. */
HashVal getHashTab PROTO((HashTab hashTab, HashKey key));

/* Enter key/val pair into hashTab.  Neither should be "null". */
VOID putHashTab PROTO((HashTab hashTab, HashKey key, HashVal val));

/* Return # of entries currently in hashTab. */
Count nEntriesHashTab PROTO((HashTab hashTab));

/* Begin a new scope in hashTab and return it. */
HashScope beginScopeHashTab PROTO((HashTab hashTab));

/* End current scope in hashTab forgetting all entries in the scope. Returns
 * previous scope. 
 */
HashScope endScopeHashTab PROTO((HashTab hashTab));

/* Return current scope in hashTab. */
HashScope currentScopeHashTab PROTO((HashTab hashTab));

/* Return parent of scope in hashTab. */
HashScope parentScopeHashTab PROTO((HashTab hashTab, HashScope scope));

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
 * The only operations allowed on the hashtable while a cursor is active 
 * are getCursorKeyHashTab() and getCursorValHashTab().
 * Returns NULL_HASH_CURSOR if iterator exhausted.
 */
HashCursor stepCursorHashTab PROTO((HashTab hashTab, HashCursor cursor,
			            HashKey key, HashScope scope));

/* Return the key associated with cursor in hashTab. */
HashKey getCursorKeyHashTab PROTO((HashTab hashTab, HashCursor cursor));

/* Return the value associated with cursor in hashTab. */
HashVal getCursorValHashTab PROTO((HashTab hashTab, HashCursor cursor));

/* Set the value at a particular cursor. */
VOID setCursorValHashTab PROTO((HashTab hashTab, HashCursor cursor,
				HashVal val));


/* Delete entry at a particular cursor.  Not particularly time efficient 
 */
VOID delCursorHashTab PROTO((HashTab hashTab, HashCursor cursor));


/* Print out hashtable statistics on stdout.  Useful for checking hashtable
 * performance.
 */
VOID statsHashTab PROTO((HashTab hashTab));

#endif /* ifndef _HASHTAB_H */
