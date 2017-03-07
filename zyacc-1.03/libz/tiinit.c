/*

File:	 tiinit.c
Purpose: Allocate and initialize a TextID.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "area.h"
#include "hashtab.h"
#include "textidp.h"

enum { CSIZE= 256 };

static int randHash[CSIZE];
static int randHashInit= 0;


static HashCode
hashCodeWithCase(key)
  CONST HashKey key;
{
  CONST TabEntry *textIDP= (CONST TabEntry*)key;
  CONST char *text= textIDP->text;
  Size len= textIDP->len;
  int h= 0;
  CONST char *p;
  CONST char *limit= text + len;
  for (p= text; p < limit; p++) {
    h+= randHash[(unsigned)*p];
  }
  return (h < 0) ? -h : h;
}

static Boolean 
textEqualWithCase(key1, key2)
  CONST HashKey key1;
  CONST HashKey key2;
{
  CONST TabEntry *eP1= (CONST TabEntry*)key1;
  CONST TabEntry *eP2= (CONST TabEntry*)key2;
  CONST Size len1= eP1->len;
  CONST Size len2= eP2->len;
  if (len1 != len2) {
    return FALSE;
  }
  else {
    CONST char *text1= eP1->text;
    CONST char *text2= eP2->text;
    CONST char *p1;
    CONST char *p2;
    CONST char *limit1= text1 + len1;
    for (p1= text1, p2= text2; p1 < limit1; p1++, p2++) {
      if (*p1 != *p2) return FALSE;
    }
    return (p1 == limit1);
  }
}

static HashCode
hashCodeIgnoreCase(key)
  CONST HashKey key;
{
  CONST TabEntry *textIDP= (CONST TabEntry*)key;
  CONST char *text= textIDP->text;
  Size len= textIDP->len;
  int h= 0;
  CONST char *p;
  CONST char *limit= text + len;
  for (p= text; p < limit; p++) {
    h+= randHash[tolower(*p)];
  }
  return (h < 0) ? -h : h;
}

static Boolean 
textEqualIgnoreCase(key1, key2)
  CONST HashKey key1;
  CONST HashKey key2;
{
  CONST TabEntry *eP1= (CONST TabEntry*)key1;
  CONST TabEntry *eP2= (CONST TabEntry*)key2;
  CONST Size len1= eP1->len;
  CONST Size len2= eP2->len;
  if (len1 != len2) {
    return FALSE;
  }
  else {
    CONST char *text1= eP1->text;
    CONST char *text2= eP2->text;
    CONST char *p1;
    CONST char *p2;
    CONST char *limit1= text1 + len1;
    for (p1= text1, p2= text2; p1 < limit1; p1++, p2++) {
      if (tolower(*p1) != tolower(*p2)) return FALSE;
    }
    return (p1 == limit1);
  }
}

/* Initialize and return a TextID.  If ignoreCase is TRUE, then case will
 * be ignored when comparing characters in subsequent getTextID()s. 
 */
TextID 
newTextID(ignoreCase)
  BooleanX ignoreCase;
{
  enum { 
    INIT_HASHTAB_SIZE= 128, 
    LG2_AREA_INC= 8,
    MAX_LOAD_FACTOR= 4
  };
  TextIDP textIDP= (TextIDP)MALLOC(sizeof(TextIDStruct));
  HashOpts opts;
  Index i;
  opts.codeFnP= ignoreCase ? hashCodeIgnoreCase : hashCodeWithCase;
  opts.equalFnP= ignoreCase ? textEqualIgnoreCase : textEqualWithCase;
  opts.initSize= INIT_HASHTAB_SIZE;
  opts.maxLoadFactor= MAX_LOAD_FACTOR;
  opts.isScoped= FALSE; opts.reorderOK= TRUE;
  INIT_AREAX(textIDP->tab, TabEntry, LG2_AREA_INC, UINT2_MAX);
  if (!randHashInit) {
    for (i= 0; i < CSIZE; i++) randHash[i]= rand();
    randHashInit= 1;
  }
  textIDP->hashTab= newHashTab(&opts);
  textIDP->ignoreCase= ignoreCase;
  return (TextID)textIDP;
}


#if 0
static Boolean isPrime PROTO((Index n));
static Index nextPrime PROTO((Index n));

/* Returns TRUE iff the number n (which must be odd and > 2) is prime. */
static Boolean 
isPrime(n)
  Index n;
{
  Index i;
  assert(n > 2 && n%2 == 1);
  for (i= 3; i * i < n; i+= 2) {
    if (n%i == 0) return FALSE;
  }
  return TRUE;
}

/* Return the next prime which is >= n. */
static Index 
nextPrime(n)
  Index n;
{ 
  Index i;
  assert(n > 2);
  for (i= (n%2) ? n : n + 1; !isPrime(i); i+= 2) ;
  return i;
}

/* Adapted from hashpjw, AUS pg. 436. */
TabIndex 
_ZhashText(textIDP, text, len)
  TextIDStruct *textIDP; 
  CONST char *text;
  Size len;
{
  Count h= 0, g;
  CONST char *p;
  CONST char *limit= text + len;
  if (textIDP->ignoreCase) {
    for (p= text; p < limit; p++) {
      h= (h << 4) + tolower(*p);
      if ((g= (h & MAKE_U_INT(0xf0000000)))) {
	h= h ^ (g >> 24); h= h ^ g;
      }
    }
  }
  else {
    for (p= text; p < limit; p++) {
      h= (h << 4) + *p;  
      if ((g= (h & MAKE_U_INT(0xf0000000)))) {
	h= h ^ (g >> 24); h= h ^ g;
      }
    }
  }
  return h%textIDP->hashSize;
}

/* Approximately double the size of the hash-table; rehash all strings in
 * the table.  Double the size of the increment for area TAB[]. 
 */
VOID 
_ZresizeHashTab(textIDP)
  TextIDP textIDP;
{
  CONST Count nEntries= AX_INDEX(textIDP->tab);
  CONST Size hashSize= nextPrime(2*textIDP->hashSize + 1);
  TabIndex *hashTab= (TabIndex *)
    REALLOC(textIDP->hashTab, hashSize * sizeof(TabIndex));
  Index i;
  textIDP->hashTab= hashTab; textIDP->hashSize= hashSize;
  assert(hashSize * MAX_LOAD_FACTOR > nEntries);
  for (i= 0; i < hashSize; i++) hashTab[i]= NIL;
  for (i= 0; i < nEntries; i++) {
    TabEntry *teP= &TAB(i);
    CONST Index h= _ZhashText(textIDP, teP->text, teP->len);
    assert(h < textIDP->hashSize);
    teP->succ= hashTab[h]; hashTab[h]= i;
  }
  VOID_RET();
}

#endif
