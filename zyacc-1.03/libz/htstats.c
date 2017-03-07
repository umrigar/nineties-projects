/*

File:	 htstats.c
Purpose: Output hashtable statistics for debugging.

Last Update Time-stamp: <97/07/28 13:23:58 zdu>

Copyright (C) 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "hashtabp.h"
#include "memalloc.h"

#include <stdlib.h>

static int sizeCmp PROTO((CONST VOIDP s1P, CONST VOIDP s2P));

static int sizeCmp(s1P, s2P)
  CONST VOIDP s1P;
  CONST VOIDP s2P;
{
  Size *size1P= (Size *) s1P;
  Size *size2P= (Size *) s2P;
  return (*size1P < *size2P) ? -1 : ((*size1P > *size2P) ? 1 : 0);
}

VOID
statsHashTab(hashTab) 
  HashTab hashTab;
{
  HashTabP hashTabP= (HashTabP)hashTabP;
  Size hashSize= hashTabP->mask + 1;
  Size *chainLengths= (Size *)CALLOC(hashSize, sizeof(Size));
  Count nEntries= nEntriesHashTab(hashTab);
  Index i;
  Count nScopes= 0;
  for (i= hashTabP->scope; i != NIL;
       i= ENTRIES(hashTabP, i).u.scopeEntry.pred) {
    nScopes++;
  }
  printf("# of entries= %d\n", nEntries);
  printf("Hash size= %d\n", hashSize);
  printf("Load factor= %g\n", (double)nEntries/hashSize);
  for (i= 0; i < hashSize; i++) {
    Index j;
    Index len= 0;
    for (j= hashTabP->tab[i]; j != NIL; 
	 j= ENTRIES(hashTabP, j).u.hashEntry.succ) {
      len++;
    }
    chainLengths[i]= len;
  }
  qsort(chainLengths, hashSize, sizeof(Size), sizeCmp);
  printf("HASH CHAIN LENGTH\t# OF CHAINS\n");
  for (i= 0; i < hashSize; ) {
    Size val= chainLengths[i];
    Count nVal= 0;
    Index j;
    for (j= i; j < hashSize && chainLengths[j] == val; j++) nVal++;
    printf("\t%d\t\t%d\n", val, nVal);
    i= j;
  }	
  FREE(chainLengths);
}
