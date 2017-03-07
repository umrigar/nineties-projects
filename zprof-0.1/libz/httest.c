/*

File:	 httest.c
Purpose: Test routines for hashtable.

Last Update Time-stamp: <97/07/08 11:58:22 zdu>

Copyright (C) 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "hashtabp.h"
#include "memalloc.h"

#undef NDEBUG
#include <assert.h>

enum {
  N_PRIMES= 1023,		/* # of prime numbers to be generated */
  N_REPEAT= 3,			/* # of times each prime # is inserted */
  N_DATA= N_PRIMES*N_REPEAT	/* Total # of data elements */
};

static Int data[N_DATA];

static Boolean 
isPrime(n)
  Index n;
{
  Index i;
  for (i= 2; i*i < n; i++) {
    if (n % i == 0) return FALSE;
  }
  return TRUE;
}

/* generate data */
static VOID
genData()
{
  Index i;
  for (i= 0; i < N_REPEAT; i++) {
    Index lastPrime= 1;
    Index j;
    for (j= 0; j < N_PRIMES; j++) {
      do { lastPrime+= 2; } while (!isPrime(lastPrime));
      data[i*N_PRIMES + j]= lastPrime;
    }
  }
}

static VOID
chkUnscoped()
{
  /* using the prime #s as the hashcode is going to result in lousy
   * hashtable performance as all the buckets for even hashcodes will
   * be empty.
   */
  HashTab hashTab= newHashTab(NULL);
  HashCursor c;
  Index i;
  for (i= 0; i < N_DATA; i++) { /* enter inverted data into hashtable */
    putHashTab(hashTab, (HashKey)data[i], (HashVal)(i + 1));
  }
  for (i= 0; i < N_DATA; i++) { /* lookup data */
    CONST Index j= (Index)getHashTab(hashTab, (HashKey)data[i]) - 1;
    CONST Index diff= j > i ? j - i : i - j;
    assert(diff % N_PRIMES == 0);
  }
  for (i= 0; i < N_DATA; i++) { /* iterate over fixed keys */
    Count nFound= 0;
    for (c= stepCursorHashTab(hashTab, NULL_HASH_CURSOR, 
			      (HashKey)data[i], NULL_HASH_SCOPE);
	 c != NULL_HASH_CURSOR;
	 c= stepCursorHashTab(hashTab, c, (HashKey)data[i], 
			      NULL_HASH_SCOPE)) {
      CONST Index j= (Index)getCursorValHashTab(hashTab, c) - 1;
      CONST Index diff= j > i ? j - i : i - j;
      assert((Index)getCursorKeyHashTab(hashTab, c) == data[i]);
      assert(diff % N_PRIMES == 0);
      nFound++;
    }
    assert(nFound == N_REPEAT);
  }
  i= 0;
  for (c= stepCursorHashTab(hashTab, NULL_HASH_CURSOR, 
			    NULL_HASH_KEY, NULL_HASH_SCOPE);
       c != NULL_HASH_CURSOR;
       c= stepCursorHashTab(hashTab, c, NULL_HASH_KEY, NULL_HASH_SCOPE)) {
    Index key= (Index)getCursorKeyHashTab(hashTab, c);
    Index val= (Index)getCursorValHashTab(hashTab, c) - 1;
    assert(val < N_DATA);
    assert(data[val] == key);
    i++;
  }
  assert(i == N_DATA);
  statsHashTab(hashTab);
  delHashTab(hashTab);
  VOID_RET();
}

static HashCode
primeHashCode(key)
  CONST HashKey key;
{
  return ((Index)key)/2;
}


static Boolean
primeEqual(key1, key2)
  CONST HashKey key1;
  CONST HashKey key2;
{
  CONST Index prime1= (Index)key1;
  CONST Index prime2= (Index)key2;
  assert(isPrime(prime1));
  assert(isPrime(prime2));
  return prime1 == prime2;
}

static VOID
chkScoped()
{
  HashOpts opts;
  HashTab hashTab;
  HashCursor c;
  HashScope s;
  Index i;
  opts.codeFnP= primeHashCode;
  opts.equalFnP= primeEqual;
  opts.initSize= 53; opts.maxLoadFactor= 5;
  opts.isScoped= TRUE; opts.reorderOK= FALSE;
  hashTab= newHashTab(&opts);
  s= currentScopeHashTab(hashTab);
  for (i= 0; i < N_PRIMES; i++) { /* insert first N_PRIMES primes */
    putHashTab(hashTab, (HashKey)data[i], (HashVal)(i + 1));
    assert((Index)(getHashTab(hashTab, (HashKey)data[i])) == i + 1);
  }

  for (i= 3; i < N_PRIMES; i++) { /* delete entries with prime values */
    if (isPrime(i)) {
      c= stepCursorHashTab(hashTab, NULL_HASH_CURSOR, (HashKey)data[i], s);
      delCursorHashTab(hashTab, c);
    }
  }

  /* check that deletions were ok */
  for (c= stepCursorHashTab(hashTab, NULL_HASH_CURSOR, NULL_HASH_KEY, s);
       c != NULL_HASH_CURSOR;
       c= stepCursorHashTab(hashTab, c, NULL_HASH_KEY, s)) {
    Index k= (Index)getCursorKeyHashTab(hashTab, c);
    Index v= (Index)getCursorValHashTab(hashTab, c) - 1;
    assert(data[v] == k);
    assert(v < 3 || !isPrime(v));
  }

  /* insert data[N_PRIMES ... 2*N_PRIMES) in new scope */
  s= beginScopeHashTab(hashTab);
  for (i= N_PRIMES; i < 2*N_PRIMES; i++) {
    putHashTab(hashTab, (HashKey)data[i], (HashVal)(i + 1));
    assert((Index)(getHashTab(hashTab, (HashKey)data[i])) == i + 1);
  }

  /* iterate over fixed key in current scope */
  for (i= N_PRIMES; i < 2*N_PRIMES; i++) {
    c= stepCursorHashTab(hashTab, NULL_HASH_CURSOR, (HashKey)data[i], s);
    assert((Index)getCursorKeyHashTab(hashTab, c) == data[i]);
    assert((Index)getCursorValHashTab(hashTab, c) == i + 1);
    assert(stepCursorHashTab(hashTab, c, (HashKey)data[i], s) == 
	   NULL_HASH_CURSOR);
  }

  /* iterate over fixed key in entire table */
  for (i= 0; i < N_PRIMES; i++) {
    c= stepCursorHashTab(hashTab, NULL_HASH_CURSOR, (HashKey)data[i], 
			 NULL_HASH_SCOPE);
    assert((Index)getCursorKeyHashTab(hashTab, c) == data[i]);
    assert((Index)getCursorValHashTab(hashTab, c) == i + 1 + N_PRIMES);
    c= stepCursorHashTab(hashTab, c, (HashKey)data[i], NULL_HASH_SCOPE);
    if (c == NULL_HASH_CURSOR) {
      assert(isPrime(i));
    }
    else {
      assert((Index)getCursorKeyHashTab(hashTab, c) == data[i]);
      assert((Index)getCursorValHashTab(hashTab, c) == i + 1);
      c= stepCursorHashTab(hashTab, c, (HashKey)data[i], NULL_HASH_SCOPE);
      assert(c == NULL_HASH_CURSOR);
    }
  }

  /* end scope and confirm scope's entries disappeared */
  s= endScopeHashTab(hashTab);
  for (c= stepCursorHashTab(hashTab, NULL_HASH_CURSOR, NULL_HASH_KEY, s);
       c != NULL_HASH_CURSOR;
       c= stepCursorHashTab(hashTab, c, NULL_HASH_KEY, s)) {
    Index k= (Index)getCursorKeyHashTab(hashTab, c);
    Index v= (Index)getCursorValHashTab(hashTab, c) - 1;
    assert(data[v] == k);
    assert(v < 3 || !isPrime(v));
  }

  /* reinsert deleted entries */
  for (i= 3; i < N_PRIMES; i++) { /* delete entries with prime values */
    if (isPrime(i)) {
      putHashTab(hashTab, (HashKey)data[i], (HashVal)(i + 1));
    }
  }

  for (i= 0; i < N_PRIMES; i++) {
    c= stepCursorHashTab(hashTab, NULL_HASH_CURSOR, (HashKey)data[i], s);
    assert((Index)getCursorKeyHashTab(hashTab, c) == data[i]);
    assert((Index)getCursorValHashTab(hashTab, c) == i + 1);
    assert(stepCursorHashTab(hashTab, c, (HashKey)data[i], s) == 
	   NULL_HASH_CURSOR);
  }
  
  statsHashTab(hashTab);
  delHashTab(hashTab);
  VOID_RET();
}


int 
main()
{
  genData();
  chkUnscoped();
  chkScoped();
  return 0;
}
