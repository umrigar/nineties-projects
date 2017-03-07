/*

File:	 chrclass.c
Purpose: Maintain character classes.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "chrclass.h"
#include "comptab.h"
#include "files.h"
#include "options.h"
#include "outprms.h"
#include "outvec.h"
#include "scan.h"

#include "area.h"
#include "iset.h"
#include "error.h"
#include "freelist.h"
#include "ids.h"
#include "memalloc.h"

/* 

The implementation of character classes is predicated on the fact that
typically a character class contains relatively few characters or
ranges of consecutive character.  Hence character classes are
implemented as ordered lists of maximal disjoint ranges of characters.
Though not as efficient as a bit-set implementation, this allows
efficient representation of character classes over large character
sets.

During the eq-class computation, character classes are represented as
sets of min-ranges (no range boundary can be within a min-range).
Since hopefully the number of min-ranges should be reasonable (a few
100) even for last character sets, the sets should be reasonable.

*/


#if TEST_RAW_CLASS || TEST_CHRCLASS
#define OPTIONS(name)	OPT_##name
enum {	/* TEST value for options. */
  OPT_charSetSize= 256,
  OPT_ignoreCase= 0,
  OPT_equivClasses= 1
};
#else
#define OPTIONS(name)	optionsP->name
#endif


/*			    STATIC FUNCTION PROTOTYPES				*/

struct Range;
struct ChrClass;

static VOID initRanges PROTO((VOID_ARGS));
static VOID terminateRanges PROTO((VOID_ARGS));
static RawClass newRawClassHdr PROTO((VOID_ARGS));
static VOID freeRawClassHdr PROTO((RawClass c));
static struct Range *newRange PROTO((VOID_ARGS));
static VOID freeRange PROTO((struct Range *rP));
static VOID delRawClass PROTO((RawClass class));
static RawClass addRawClassLo PROTO((RawClass rawClass, UInt lo, UInt hi));
static RawClass unionRawClass PROTO((RawClass c0, RawClass c1));
static VOID initClasses PROTO((VOID_ARGS));
static VOID terminateRanges PROTO((VOID_ARGS));
static VOID delClasses PROTO((VOID_ARGS));
static Count hashSumRawClass PROTO((RawClass class));
static Index newHashEntry PROTO((VOID_ARGS));
static VOID freeHashEntry PROTO((Index i));
static int isBlank PROTO((int c));
static VOID initNamedClasses PROTO((VOID_ARGS));
static Class ctypeClass PROTO((Index id));
static VOID rmRedundantCtypeClasses PROTO((VOID_ARGS));
static struct ChrClass *newChrClass PROTO((VOID_ARGS));
static VOID delChrClass PROTO((struct ChrClass *chrClassP));
static ISet *newClassSets PROTO((VOID_ARGS));
static int boundsCmp PROTO((CONST VOIDP b1P, CONST VOIDP b2P));
static VOID buildMinRanges PROTO((VOID_ARGS));
static ISet rawClassToMinRangeISet PROTO((RawClass rawClass, ISet s));
static VOID formEqClasses PROTO((VOID_ARGS));
static Count outEqClasses PROTO((FILE *outFile, Index indent));
static VOID segNEntries PROTO((CompTabEntry entries[]));
static Count segEntries PROTO((Index hiN, CompTabEntry entries[]));
static VOID compressEqClasses PROTO((VOID_ARGS));
static VOID outEqDefaultEntry PROTO((FILE *outFile, Index segN));

#if DO_TRACE
static VOID dumpEqClasses PROTO((FILE *dumpFile));
#endif


/*			GLOBAL DATA STRUCTURE.				*/

/* This global data structure is used during class construction. */
enum { HASH_SIZE= 128 };

typedef struct {
  Count nRanges;		/* # of ranges which are not list headers. */	
  FreeList freeRanges;		/* Keep track of free-ranges. */
  Count nClasses;		/* # of active unique classes. */
  Class hashTab[HASH_SIZE];	/* Hash-table which indexes foll. area. */
  Area hashEntries;		/* HashEntry area. */
  Index freeHashEntries;	/* List of free hash-entries. */
} Classes;

static Classes *classesP= NULL;


/*				RANGES					*/

/* Note that we use Index4 instead of UShrt for a Bound to ensure that
 * we can have an exclusive upper bound of 0x10000 which is necessary
 * for supporting 16-bit chars.
 */
typedef Index4 Bound;

typedef struct Range {
  struct Range *succ;		/* Next range in character class. */
  union {			/* In case we need more members. */
    Bound lo;			/* Inclusive lower bound. */
  } u1;
  union {			/* In case we need more elements. */
    Bound hi;			/* Exclusive upper bound. Must have lo < hi. */
  } u2;
} Range;

/* Invariant for ranges which are not used as list headers. */
#define RANGE_INVARIANT(rP)						\
  ( (rP)->u1.lo < (rP)->u2.hi && 					\
    ( (rP)->succ == NULL || (rP)->u2.hi < (rP)->succ->u1.lo )		\
  )

static VOID
initRanges()
{
  classesP->nRanges= 0;
  newFreeList(sizeof(Range), 512, &classesP->freeRanges);
}

static VOID
terminateRanges()
{
  destroyFreeList(&classesP->freeRanges);
}


/*			    RAW CLASS HEADERS				*/

/*

Raw classes are simply lists of Ranges (with a dummy Range list header).
They are constructed in the course of building a character class (which
may contain multiple ranges).  When the raw-class is completed it is
converted to a Class (which may lead to the deletion of the raw class
if an identical one was already previously converted to a class).

*/

/* Allocate a Range to act as the header of a RawClass. */
static RawClass
newRawClassHdr()
{
  return allocFreeList(&classesP->freeRanges);
}

/* Deallocate a Range used as a header. */
static VOID
freeRawClassHdr(c)
  RawClass c;
{
  freeFreeList(c, &classesP->freeRanges);
  VOID_RET();
}


/*			   INTRA-CLASS RANGES				*/

/* Allocate a range used within a class (not as a raw-class-header). */
static Range *
newRange()
{
  classesP->nRanges++;
  return (Range*)allocFreeList(&classesP->freeRanges);
}

/* Deallocate a range used within a class (not as a raw-class-header). */
static VOID
freeRange(rP)
  Range *rP;
{
  classesP->nRanges--;
  freeFreeList(rP, &classesP->freeRanges);
  VOID_RET();
}


/*		      DESTRUCTIVE RAW CLASS OPERATIONS.			*/

RawClass
newRawClass()
{
  Range *rP= newRawClassHdr();
  rP->succ= 0; rP->u1.lo= 0;
  return rP;
}

/* Free all ranges used by RawClass c (including its header). */
static VOID
delRawClass(class)
  RawClass class;
{
  Range *CONST hdrP= (Range*)class;
  Range *p, *succ;
  for (p= hdrP->succ; p != NULL; p= succ) {
    succ= p->succ; freeRange(p);
  }
  freeRawClassHdr(class);
  VOID_RET();
}

/* Add characters in range lo (inclusive) to hi (inclusive) to raw character
 * class rawClass returning the modified rawClass.  The addition is done
 * so as to minimize the number of ranges.
 */
static RawClass
addRawClassLo(rawClass, lo, hi)
  RawClass rawClass;
  UInt lo;
  UInt hi;
{
  Range *hdrP= (Range*) rawClass;
  Range *loP;		/* Will point to first range whose lo > lo. */
  Range *loP0;		/* Lag pointer for loP. */
  Range *hiP;		/* Will point to first range whose lo > hi. */
  Range *hiP0;		/* Lag pointer for hiP. */
  Bound newHi;		/* Exclusive upper bound for new range. */
  Range *rP;
  Boolean loP0Inc; 	/* TRUE iff range inserted by modifying loP0 range. */

  assert(lo <= hi);

  for (loP0= hdrP, loP= hdrP->succ; loP && loP->u1.lo <= lo; 
       loP0= loP, loP= loP->succ) {
  }
  /* lo is either "within" loP0 or between loP0 and loP. */
  loP0Inc= (loP0 != hdrP && lo <= loP0->u2.hi); 
  for (hiP0= loP0, hiP= loP; hiP && hiP->u1.lo <= hi;  
       hiP0= hiP, hiP= hiP->succ) {
  }
  /* hi is either within hiP0 or between hiP0 and hiP. */
  newHi= (hiP0 != hdrP && hi < hiP0->u2.hi) ? hiP0->u2.hi : hi + 1;
  /* It is possible that newHi == hiP->u1.lo, in which case we need to
   * include hiP in whatever new range is formed.
   */

  { /* Delete all ranges in [loP, hiP). */
    Range *succP;
    for (rP= loP; rP != hiP; rP= succP) { succP= rP->succ; freeRange(rP); }
  }
  if (loP0Inc) { /* Set new range in loP0. */
    assert(loP0->u1.lo <= lo);
    if (hiP && hiP->u1.lo == newHi) { /* Collapse hiP into loP. */
      loP0->succ= hiP->succ; loP0->u2.hi= hiP->u2.hi; freeRange(hiP);
    }
    else {
      loP0->succ= hiP; loP0->u2.hi= newHi;
    }
  }
  else if (hiP && hiP->u1.lo == newHi) { /* Put new range in hiP. */
    loP0->succ= hiP; hiP->u1.lo= lo;
  }
  else {/* Create new range between loP0 and hiP. */
    rP= newRange();
    rP->u1.lo= lo; rP->u2.hi= newHi; rP->succ= hiP;
    loP0->succ= rP;
  }
  return rawClass;
}

/* Add chars in lo...hi (inclusive) to raw-class s.  Essentially an
 * error-checking and ignore-case wrapper around addRawClassLo().
 */
RawClass 
addRawClass(class, lo, hi)
  RawClass class;
  UInt lo;
  UInt hi;
{
  if (hi >= OPTIONS(charSetSize)) {
    error("Character `0x%x' greater than maximum permissible character.", hi);
    lo= hi= ' ';
  }
  if (lo > hi) {
    warn("Empty character range.");
  }
  else {
    addRawClassLo(class, lo, hi);
    if (OPTIONS(ignoreCase)) {
      if (islower(lo)) {
	addRawClassLo(class, toupper(lo), islower(hi) ? toupper(hi) : 'Z');
      }
      else if (isupper(lo)) {
	addRawClassLo(class, tolower(lo), isupper(hi) ? tolower(hi) : 'z');
      }
    }
  } 
  return class;
}



/*		     NON-DESTRUCTIVE RAW CLASS OPERATIONS		*/

/* Return a new RawClass which is the union of two RawClasses. */
static RawClass
unionRawClass(c0, c1)
  RawClass c0;
  RawClass c1;
{
  RawClass cZ= newRawClass();
  Range *zP= (Range*) cZ;
  Range *cP[2];
  cP[0]= ((Range*) c0)->succ; cP[1]= ((Range*) c1)->succ;
  while (cP[0] != NULL || cP[1] != NULL) {
    Index choice= 
        cP[0] == NULL ? 1
      : cP[1] == NULL ? 0
      : (cP[0]->u1.lo < cP[1]->u1.lo) ? 0 : 1;
    Bound lo= cP[choice]->u1.lo;
    Bound hi= cP[choice]->u2.hi;
    cP[choice]= cP[choice]->succ;
    do {
      choice=   (cP[0] && cP[0]->u1.lo <= hi) ? 0
  	      : (cP[1] && cP[1]->u1.lo <= hi) ? 1
	      : NIL;
      if (choice == NIL) break;
      if (cP[choice]->u2.hi > hi) hi= cP[choice]->u2.hi; 
      cP[choice]= cP[choice]->succ;
    } while (1);
    zP->succ= newRange(); zP= zP->succ;
    zP->u1.lo= lo; zP->u2.hi= hi;
  }
  zP->succ= NULL;
  return cZ;
}


/* Return a new RawClass which is the complement of RawClass class.
 * Operates by sliding lo of a range into the hi slot and hi of a
 * range into lo of the *next* range, with special case fixups of 1st
 * and last range.
 */
RawClass
negRawClass(class)
  RawClass class;
{
  CONST Count charSetSize= OPTIONS(charSetSize);
  RawClass cZ= newRawClass();
  Range *zP= (Range*) cZ;
  Bound lo= 0;	/* Value being slid into lo of next range. */
  Range *rP;
  for (rP= ((Range*) class)->succ; rP != NULL; rP= rP->succ) {
    zP->succ= newRange(); zP= zP->succ;
    zP->u1.lo= lo; zP->u2.hi= rP->u1.lo;
    lo= rP->u2.hi;
  }
  if (lo != charSetSize) { /* Create a last range for [lo,charSetSize). */
    zP->succ= newRange(); zP= zP->succ;
    zP->u1.lo= lo; zP->u2.hi= charSetSize;
  }
  zP->succ= NULL;
  rP= ((Range*) cZ)->succ;	/* rP points to first result range. */
  if (rP != NULL && rP->u2.hi == 0) { /* First range is empty: remove it. */
    assert(rP->u1.lo == 0);
    ((Range*) cZ)->succ= rP->succ;
    freeRange(rP);
  }
  return cZ;
}


/*			      RAW CLASS TEST				*/

#ifdef TEST_RAW_CLASS

/*  Very crude test program for raw classes. 
Compile using command:

gcc -g -Wall -I../libz -I.. -DHAVE_CONFIG_H -DTEST_RAW_CLASS \
    chrclass.c -L../libz -lz -o chrclass

May need to deactivate rest of file.

Not tested after modifications made to file.
*/

static VOID dumpRawClass PROTO((RawClass c));

static VOID 
dumpRawClass(c)
  RawClass c;
{
  Range *hdr= (Range*) c;
  Range *p;
  for (p= hdr->succ; p; p= p->succ) {
    if (p->u1.lo == p->u2.hi) {
      printf("%d ", p->u1.lo);
    }
    else {
      printf("%d-%d ", p->u1.lo, p->u2.hi);
    }
  }
  printf("\n");
}

static char helpMsg[]=
  "COMMANDS (spaces significant):\n"
  "<number> <number>:	Add range specified by <number>s to current class.\n"
  "<upper-case-letter>:	Set variable <upper-case-letter> to current class;\n"
  "			Clear current class.\n"
  "-<upper-case-letter>:Return negation of class specified by variable.\n"
  "+<upper-case-letter><upper-case-letter>:\n"
  "			Return union of classes specified by two variables.\n";

int 
main()
{
  classesP= MALLOC_VEC(1, Classes); initRanges();
  RawClass class= newRawClass();
  RawClass vars[26];	/* vars with upper-case letter names. */
  printf(helpMsg);
  while (1) {		/* Break out with SIGINT. */
    int err= 0;
    char c;
    printf("> "); scanf("%c", &c);
    if (isdigit(c)) {
      int lo, hi;
      ungetc(c, stdin);
      scanf("%d %d", &lo, &hi);
      addRawClass(class, lo, hi);
      dumpRawClass(class); 
    }
    else if (isupper(c)) {
      vars[c - 'A']= class; 
      class= newRawClass();
      printf("CLEARED\n");
    }
    else if (c == '-') {
      char var1;
      scanf("%c", &var1);
      if (isupper(var1)) {
	dumpRawClass(negRawClass(vars[var1 - 'A']));
      }
      else {
	err= 1;
      }
    }
    else if (c == '+') {
      char var1, var2;
      scanf("%c%c", &var1, &var2);
      if (isupper(var1) && isupper(var2)) {
	dumpRawClass(unionRawClass(vars[var1-'A'], vars[var2-'A']));
      }
      else {
	err= 1;
      }
    }
    else {
      err= 1;
    }
    if (err) printf(helpMsg);
    while (c != '\n') scanf("%c", &c);
  }
  return 0;
}

#endif /* ifdef TEST_RAW_CLASS */


/*			CANONICAL CHARACTER CLASSES			*/

/*

Since it is likely that we have a lot of identical character classes, 
raw classes are converted to unique Classes.  A Class is merely an
index into the hash-chain area of a hash-table.  Note that since
classes can be destroyed, not all entries in the hash-chain area
may be currently active.

*/

typedef struct HashEntry {
  RawClass rawClass;		/* Character class. */
  Count hashSum;		/* Facilitate comparison and deletion. */
  Count2 refCount;		/* # of references to this class. */
  Index2 succ;			/* Next entry on hash-chain. */
} HashEntry;

#define HASH_ENTRIES(i) A_ACCESS(classesP->hashEntries, HashEntry, i)

/* Return a pointer to the Range which is the header for a class. */
#define CLASS_HDR_P(class)	((Range*)(HASH_ENTRIES(class).rawClass))

/* A class c is valid iff the following evaluates to TRUE. */
#define VALID_CLASS(c)							\
  ((c) < A_NENTRIES(classesP->hashEntries) 				\
   && HASH_ENTRIES(c).refCount > 0)

/* Initialize the global struct pointed to by classesP. */
static VOID
initClasses()
{
  Index i;
  Class *hashTab;
  classesP= MALLOC_VEC(1, Classes);
  initRanges();
  hashTab= classesP->hashTab;
  for (i= 0; i < HASH_SIZE; i++) hashTab[i]= NIL;
  INIT_AREA(classesP->hashEntries, HashEntry, 8, UINT2_MAX);
  classesP->freeHashEntries= NIL;
  classesP->nClasses= 0;
  VOID_RET();
}

/* This routine can be called after the sets in Classes have been 
 * computed and ranges are no longer needed (after dumping).
 */
static VOID
delClasses()
{
  delArea(&classesP->hashEntries);
  terminateRanges();
  FREE(classesP); classesP= NULL;
}


/*			CLASSES HASH TABLE MAINTENANCE.			*/

/* Return a hash-sum which can be used to hash character classes. */
static Count
hashSumRawClass(class)
  RawClass class;
{
  Range *p;
  Count sum= 0;
  for (p= ((Range*) class)->succ; p != NULL; p= p->succ) {
    sum+= p->u1.lo; sum+= p->u2.hi;
  }
  return sum;
}

/* Return a free hash-entry. */
static Index
newHashEntry()
{
  Index i;
  if (classesP->freeHashEntries == NIL) {
    i= A_NEXT(classesP->hashEntries);
  }
  else {
    i= classesP->freeHashEntries; 
    classesP->freeHashEntries= HASH_ENTRIES(i).succ;
  }
  classesP->nClasses++;
  return i;
}

/* Add the hash-entry with index i to the free-list of hash-entries, after
 * unlinking it from its hash-chain.
 */
static VOID
freeHashEntry(i)
  Index i;
{
  HashEntry *hePi= &HASH_ENTRIES(i);
  CONST Index h= hePi->hashSum % HASH_SIZE;
  Class *CONST hashTab= classesP->hashTab;
  Index j;
  Index j0;

  /* The following assertion is used to distinguish free entries. */
  assert(hePi->refCount == 0);  /* No references to it. */

  /* Search for predecessor of i on hash-chain and set j0 to it. */
  for (j0= NIL, j= hashTab[h]; j != i; j= HASH_ENTRIES(j).succ) ;

  /* Unlink entry i from hash-chain. */
  if (j0 == NIL) { /* i is first entry on hash-chain. */
    hashTab[h]= hePi->succ;
  }
  else { /* i is not first entry on hash-chain. */
    HASH_ENTRIES(j0).succ= hePi->succ;
  }

  /* Add entry i to free-list. */
  hePi->succ= classesP->freeHashEntries; classesP->freeHashEntries= i;

  classesP->nClasses--;
  VOID_RET();
}

/* Find the hash-entry for RawClass class.  Create one (with 0
 * refCount) if it is not found.
 */
Class
findHashEntry(rawClass)
  RawClass rawClass;
{
  Class *CONST hashTab= classesP->hashTab;
  CONST Count hashSum= hashSumRawClass(rawClass);
  CONST Index h= hashSum % HASH_SIZE;
  Range *classP= ((Range*) rawClass)->succ;
  Class c= hashTab[h]; 
  while (c != NIL) {
    CONST HashEntry *hePi= &HASH_ENTRIES(c);
    if (hashSum == hePi->hashSum) {
      Range *p0, *p1;
      for (p0= classP, p1= CLASS_HDR_P(c)->succ;
	   p0 != NULL && p1 != NULL;
	   p0= p0->succ, p1= p1->succ) {
	if (p0->u1.lo != p1->u1.lo || p0->u2.hi != p1->u2.hi) break;
      }
      if (p0 == NULL && p1 == NULL) break;
    }
    c= hePi->succ;
  }
  if (c == NIL) { /* Not found: create a new entry. */
    Index i= newHashEntry();
    HASH_ENTRIES(i).rawClass= rawClass; HASH_ENTRIES(i).refCount= 0;
    HASH_ENTRIES(i).hashSum= hashSum;
    HASH_ENTRIES(i).succ= hashTab[h]; hashTab[h]= i;
    c= i;
  }
  return c;
}

/* Given a RawClass class, return the unique Class which has the
 * same entries as class.
 */
Class
rawClassToClass(rawClass)
  RawClass rawClass;
{
  CONST Class c= findHashEntry(rawClass);
  HashEntry *hePc= &HASH_ENTRIES(c);
  if (hePc->refCount > 0) delRawClass(rawClass);
  hePc->refCount++;
  return c;
}


/*			  	CLASS OPERATIONS			*/

/* Effectively delete Class class.  Really deletes it if its refCount
 * becomes 0.
 */
VOID
delClass(c)
  ClassX c;
{
  HashEntry *heP= &HASH_ENTRIES(c);
  assert(VALID_CLASS(c));
  heP->refCount--;
  if (heP->refCount == 0) {
    delRawClass(heP->rawClass);
    freeHashEntry(c);
  } /* if (heP->refCount == 0) */
}

/* Return a copy of class c (actually identical to c). */
Class
copyClass(c)
  ClassX c;
{
  assert(VALID_CLASS(c));
  HASH_ENTRIES(c).refCount++;
  return c;
}


/* Non-destructive union Class c2 and Class c1. */
Class
unionClass(c1, c2)
  ClassX c1;
  ClassX c2;
{
  assert(VALID_CLASS(c1));
  assert(VALID_CLASS(c2));
  return rawClassToClass(unionRawClass(CLASS_HDR_P(c1), CLASS_HDR_P(c2)));
}

/* Non-destructive complement of Class c. */
Class
negClass(c)
  ClassX c;
{
  assert(VALID_CLASS(c));
  return rawClassToClass(negRawClass(CLASS_HDR_P(c)));
}

Count
sizeClass(class)
  ClassX class;
{
  Range *p;
  Count n= 0;
  assert(VALID_CLASS(class));
  for (p= CLASS_HDR_P(class)->succ; p != NULL; p= p->succ) {
    n+= p->u2.hi - p->u1.lo;
  }
  return n;
}


/*			PREDEFINED NAMED CHARACTER CLASSES		*/

/* Ensure that we have isType() functions rather than macros.  We assume 
 * that when the header file defined macros, functions are also provided.
 * In the following, we undef any macros which have been defined in
 * <ctype.h>.  We also declare the corresponding function (hopefully no
 * clash with prototypes).
 */
#ifdef isalnum
#undef isalnum
int isalnum();
#endif
#ifdef isalpha
#undef isalpha
int isalpha();
#endif
#ifdef iscntrl
#undef iscntrl
int iscntrl();
#endif
#ifdef isdigit
#undef isdigit
int isdigit();
#endif
#ifdef isgraph
#undef isgraph
int isgraph();
#endif
#ifdef islower
#undef islower
int islower();
#endif
#ifdef isprint
#undef isprint
int isprint();
#endif
#ifdef ispunct
#undef ispunct
int ispunct();
#endif
#ifdef isspace
#undef isspace
int isspace();
#endif
#ifdef isupper
#undef isupper
int isupper();
#endif
#ifdef isxdigit
#undef isxdigit
int isxdigit();
#endif

typedef int (*CTypeFnP) PROTO((int));

typedef struct {
  ConstString name;	/* Name of class. */
  CTypeFnP ctypeFnP;	/* Recognizer function. */
  Class class;		/* Associated character class. */
} NamedClass;

/* Note that we call this fn isBlank instead of isblank, in case there
 * is a isblank macro in the system header files.
 */
static int
isBlank(c) 
  int c;
{
  return c == ' ' || c == '\t';
}

static NamedClass namedClasses[]= {
  { "alnum", isalnum, NIL} ,
  { "alpha", isalpha, NIL } ,
  { "blank", isBlank, NIL } ,
  { "cntrl", iscntrl, NIL } ,
  { "digit", isdigit, NIL } ,
  { "graph", isgraph, NIL } ,
  { "lower", islower, NIL } ,
  { "print", isprint, NIL } ,
  { "punct", ispunct, NIL } ,
  { "space", isspace, NIL } ,
  { "upper", isupper, NIL } ,
  { "xdigit", isxdigit, NIL } 
};

/* ID of first ctype string from above table. */
static Index firstCTypeID;

/* Throw ctype-names into ID-space.  Assumes they are contiguous in ID-space.
 * Remembers index of first one in firstCTypeID.
 */
static VOID
initNamedClasses()
{
  CONST Count n= sizeof(namedClasses)/sizeof(namedClasses[0]);
  Index lastID= getID(namedClasses[0].name, strlen(namedClasses[0].name));
  Index i;
  firstCTypeID= lastID; 
  for (i= 1; i < n; i++) {
    lastID= getID(namedClasses[i].name, strlen(namedClasses[i].name));
  }
  assert(lastID == firstCTypeID + n - 1);
}

/* Return the character class for <ctype.h> type class whose name is
 * given by ctypeID.  The returned class is case-adjusted depending
 * on OPTIONS(ignoreCase).
 */
static Class
ctypeClass(id)
  Index id;
{
  Index i= id - firstCTypeID;
  assert(firstCTypeID <= id && id < firstCTypeID \
	 + sizeof(namedClasses)/sizeof(namedClasses[0]));
  if (namedClasses[i].class == NIL) {
    RawClass rawClass= newRawClass();
    Int c;
    CTypeFnP ctypeFnP= namedClasses[i].ctypeFnP;
    for (c= 0; c <= UCHAR_MAX; c++) {
      if (ctypeFnP(c)) {
	addRawClassLo(rawClass, c, c);
	if (OPTIONS(ignoreCase)) {
	  if (islower(c)) addRawClassLo(rawClass, toupper(c), toupper(c));
	  else if (isupper(c)) addRawClassLo(rawClass, 
					     tolower(c), tolower(c));
	}
      }
    }
    namedClasses[i].class= rawClassToClass(rawClass);
  }
  return copyClass(namedClasses[i].class);
}

/* Since the ctype class is cached in the namedClasses struct, it is possible
 * that the cached value is the only reference to the class (the copies
 * returned by ctypeClass() may all have been deleted).  This can lead to
 * finer eq-classes than necessary.  Hence the following routine deletes
 * all cached ctype classes which have a refCount of 1.
 */
static VOID
rmRedundantCtypeClasses()
{
  CONST Count n= sizeof(namedClasses)/sizeof(namedClasses[0]);
  Index i;
  for (i= 0; i < n; i++) {
    Index c= namedClasses[i].class;
    if (c != NIL && HASH_ENTRIES(c).refCount == 1) delClass(c);
  }
  VOID_RET();
}

RawClass 
ctypeAddToRawClass(rawClass, ctypeID)
  RawClass rawClass;
  Index ctypeID;
{
  if (ctypeID < firstCTypeID 
      || ctypeID >= firstCTypeID + sizeof(namedClasses)/sizeof(NamedClass)) {
    error("Invalid character class %s", getIDString(ctypeID));
  }
  else {
    Class cClass= ctypeClass(ctypeID);
    Range *hdrP= CLASS_HDR_P(cClass);
    Range *p;
    for (p= hdrP->succ; p != NULL; p= p->succ) {
      assert(p->u1.lo < p->u2.hi);
      rawClass= addRawClassLo(rawClass, p->u1.lo, p->u2.hi - 1);
    }
    delClass(cClass);
  }
  return rawClass;
}



/* Return a brand-new character-class corresponding to '.'. */
Class
dotClass()
{
  static Class class= NIL;
  if (class == NIL) {
    RawClass rawClass= newRawClass();
    addRawClassLo(rawClass, 0, '\n' - 1);
    addRawClassLo(rawClass, '\n' + 1, OPTIONS(charSetSize) - 1);
    class= rawClassToClass(rawClass);
  }
  return copyClass(class);
}

/* Return the character class containing all characters. */
Class
univClass()
{
  static Class class= NIL;
  if (class == NIL) {
    RawClass rawClass= newRawClass();
    addRawClassLo(rawClass, 0, OPTIONS(charSetSize) - 1);
    class= rawClassToClass(rawClass);
  }
  return copyClass(class);
}


/*			   EQ-CLASS INFO				*/

/* A min-range is the largest range which is not split across two
 * ranges in some character-classes.
 */

/* Type used to represent the number of a equiv-class. */
typedef Index2 EqClass;

typedef struct {	/* Information about segments. */
  Index2 segMinRange;	/* 1st min-range for 256-char segment. */
  Index2 defEqClass;	/* Default eq-class for segment. */
} SegInfo;

typedef struct ChrClass {
  Count maxClassNum1;	/* 1 greater than max. class number. */
  Count nEq;		/* # of eq-classes; == charSetSize if no eq-classes. */
  ISet *classSets;	/* [] set of char or eq-classes for each class. */
  Count nMinRanges;	/* # of min-ranges. */

  /* Bounds for minRanges.  Since N min-ranges represented require N +
   * 1 bounds, there is one more bound stored here than there are
   * min-ranges.  Additionally, after the last bound a sentinel entry
   * (all 1s) is stored to facilitate searching for the start of a
   * segment range.  The (inclusive) lo bound for min-range i is
   * represented by minRangeBounds[i] and the (exclusive) hi bound for
   * min-range i is  represented by minRangeBounds[i + 1].
   */
  Bound *minRangeBounds;

  /* eqClasses[i] specifies equiv-class for min-range i.  Hence a
   * total of nMinRanges active entries + 1 extra entry at end cause
   * some code may refer to it.
   */
  EqClass *eqClasses;

  SegInfo *segInfo;	/* Segment bounds (when 16-bit chars used). */
  CompTab *eqComp;	/* Compressed table structure for equiv-classes. */
} ChrClass;

/* Dynamically allocated main data-structure */
static ChrClass *chrClassP;


static ChrClass *
newChrClass()
{
  ChrClass *CONST chrClassP= MALLOC_VEC(1, ChrClass);
  chrClassP->maxClassNum1= A_NENTRIES(classesP->hashEntries);
  chrClassP->nEq= OPTIONS(charSetSize);
  chrClassP->classSets= newClassSets(); chrClassP->nMinRanges= 0;
  chrClassP->minRangeBounds= NULL;
  chrClassP->eqClasses= NULL;
  chrClassP->segInfo= NULL;
  chrClassP->eqComp= NULL;
  return chrClassP;
}

static VOID
delChrClass(chrClassP)
  ChrClass *chrClassP;
{
  if (chrClassP->classSets) { INTERNAL_ERROR(); }
  FREE(chrClassP->minRangeBounds);
  FREE(chrClassP->eqClasses);
  FREE(chrClassP->segInfo);
  if (chrClassP->eqComp) delCompTab(chrClassP->eqComp);
  FREE(chrClassP->eqComp);
  FREE(chrClassP);
}

static ISet *
newClassSets()
{
  CONST Count maxClassNum1= A_NENTRIES(classesP->hashEntries);
  ISet *CONST classSets= MALLOC_VEC(maxClassNum1, ISet);
  Index i;
  for (i= 0; i < maxClassNum1; i++) classSets[i]= BAD_ISET;
  return classSets;
}

/* Return a vector v of ISets such that if c is a valid class, then
 * v[c] returns the ISet of characters or eq-classes associated with
 * class c.
 */
ISet *
getClassISets()
{
  return chrClassP->classSets;
}

/* To be called after classSets component above no longer needed. */
VOID
delClassSets()
{
  CONST Count maxClassNum1= chrClassP->maxClassNum1;
  ISet *classSets= chrClassP->classSets;
  Index i;
  for (i= 0; i < maxClassNum1; i++) {
    if (classSets[i] != BAD_ISET) delISet(classSets[i]);
  }
  FREE(classSets); chrClassP->classSets= NULL;
  VOID_RET();
} 

/* Return # of eq-classes (charSetSize if no eq-classes used). */
Count 
nEqClasses()
{
  return chrClassP->nEq;
}

/*			MIN-RANGE CONSTRUCTION				*/


/* Comparison function for qsort(). */
static int
boundsCmp(b1P, b2P)
  CONST VOIDP b1P;
  CONST VOIDP b2P;
{
  CONST Bound b1= *(Bound*)b1P;
  CONST Bound b2= *(Bound*)b2P;
  return (b1 < b2) ? -1 : ( (b1 > b2) ? 1 : 0 );
}

/* Compute bounds for min-ranges into eqClassInfo field of data-structure
 * pointed to by chrClassP.
 */
static VOID
buildMinRanges()
{
  CONST Count charSetSize= OPTIONS(charSetSize);
  CONST Count nBounds= 2*classesP->nRanges;
  CONST Count nHashEntries= A_NENTRIES(classesP->hashEntries);
  Bound *bounds= MALLOC_VEC((nBounds + 1), Bound); /* +1 for sentinel. */
  Count boundsIndex= 0;	/* Indexes bounds[] during construction. */
  Index srcI= 0;	/* Source index while eliminating dups in bounds[]. */
  Index destI= 0;	/* Dest. index while eliminating dups in bounds[]. */
  Index i;
  for (i= 0; i < nHashEntries; i++) {
    CONST Class c= i;
    if (HASH_ENTRIES(i).refCount > 0) {
      Range *hdrP= CLASS_HDR_P(c);
      Range *p;
      for (p= hdrP->succ; p != NULL; p= p->succ) {
	assert(boundsIndex < nBounds);
	bounds[boundsIndex++]= p->u1.lo; bounds[boundsIndex++]= p->u2.hi;
      }
    }
  }
  assert(boundsIndex == nBounds);
  qsort(bounds, boundsIndex, sizeof(Bound), boundsCmp);

  /* Since the univClass() will have contributed range [0, charSetSize). */
  assert(bounds[0] == 0 && bounds[nBounds - 1] == charSetSize);

  while (1) { /* Eliminate duplicates. */
    Bound boundVal= bounds[srcI++];
    assert(srcI <= boundsIndex);
    bounds[destI++]= boundVal;
    if (boundVal == charSetSize) break;
    while (bounds[srcI] == boundVal) srcI++;
  }
  assert(destI < nBounds + 1);		/* We have space to insert sentinel. */
  bounds[destI]= ~0L;			/* Insert sentinel. */
  bounds= REALLOC_VEC(bounds, (destI + 1), Bound);
  chrClassP->nMinRanges= destI - 1;
  chrClassP->minRangeBounds= bounds;
  VOID_RET();
} 

/* Add the min-ranges constituting chr-class class to ISet s (which should
 * initially be empty). 
 */
static ISet
rawClassToMinRangeISet(rawClass, s)
  RawClass rawClass;
  ISet s;
{
  CONST Bound *minRangeBounds= chrClassP->minRangeBounds;
  Index i= 0;	/* Index within minRangeBounds[]. */
  Range *p;
  assert(isEmptyISet(s));
  for (p= ((Range*)rawClass)->succ; p != NULL; p= p->succ) {
    CONST Bound lo= p->u1.lo;
    CONST Bound hi= p->u2.hi;
    while (minRangeBounds[i] < lo) {
      assert(i < chrClassP->nMinRanges);
      i++;
    }
    assert(minRangeBounds[i] == lo);
    while (minRangeBounds[i] < hi) {
      addISet(s, i);
      assert(i < chrClassP->nMinRanges);
      i++;
    }
    assert(minRangeBounds[i] == hi);
  }
  return s;
}



/*			   EQUIVALENCE CLASS COMPUTATION		*/

/* For each character class computed by patterns module, compute a
 * corresponding set of equivalence classes in equivs[] representing
 * that character class. To form the equivalence class, partition the
 * characters into equivalence classes such that two chars c1 and c2
 * are in the same equivalence class iff when c1 being in some
 * character class implies that c2 is in the same character class.
 * eqClass[c] is set to the equivalence class computed for character
 * c. The iSet component in the header for each character class is set
 * to the set of equivalence classes which cover that character class.
 *
 * New bit-sets allocated in this routine eventually work their way
 * into tempClasses[] and the classSets components of *chrClassP.
 * Since the iSets components in the classSets component are exported,
 * those sets are retained.  The sets allocated within tempClasses[]
 * are deleted.
 */
static VOID 
formEqClasses()
{
  CONST Count nHashEntries= A_NENTRIES(classesP->hashEntries);
  CONST Count nRanges= chrClassP->nMinRanges;
  ISet *classSets= chrClassP->classSets;
  Count nTempClasses= 0;	/* # of entries used in tempClasses[]. */
  Count nEq= 0;			/* # of eq-classes. */
  typedef struct { 		/* Temporary classes. */		
    ISet val;			/* Set of min-ranges. in class. */
    EqClass eqClass;		/* Finally assigned eq-class #. */
  } TempClass;
  /* In the worst-case, each unique chr-class (of which there are 
   * nClasses) could split into an individual min-range: hence reserve
   * enough space for building these classes.
   */
  TempClass *tempClasses= MALLOC_VEC(classesP->nClasses + nRanges, TempClass);

  Index i;			/* General loop index. */

  EqClass *eqClasses= MALLOC_VEC(nRanges + 1, EqClass);
  eqClasses[nRanges]= 0;	/* Assign value to extra dummy entry. */

  /* Initialize eqClass[] entries to NIL.				*/
  for (i= 0; i < nRanges; i++) eqClasses[i]= NIL;

  /* Initialize tempClasses to contain same min-range bit-sets
   * corresponding to all the unique chr-classes.
   */
  for (i= 0; i < nHashEntries; i++) {
    CONST Class c= i;
    if (HASH_ENTRIES(i).refCount > 0) {
      VOIDP rP= CLASS_HDR_P(c);
      RawClass rawClass= rP;
      ISet s= newISet();
      tempClasses[nTempClasses].val= rawClassToMinRangeISet(rawClass, s);
      tempClasses[nTempClasses].eqClass= NIL;
      nTempClasses++;
    }
  }
  assert(nTempClasses == classesP->nClasses);
  
  /* Assign each min-range. to a SINGLE tempClass. Split classes if necessary*/
  for (i= 0; i < nTempClasses; i++) { /* nTempClasses moving target. */
    ISet setI= tempClasses[i].val;	/* Set of min-ranges. */
    ISetElement c;			/* Min-range from setI. */
    VOIDP iterP;
    FOR_EACH_IN_ISET(c, setI, iterP) {
      if (eqClasses[c] == NIL) {/* c not previously assigned to temp-class. */
	eqClasses[c]= i;	/* Assign c to temp-class i. */
      }
      else if (eqClasses[c] != i) { /* i not eq-class as c in other eq-class. */
	/* We have a char. c in more than one tempClass.  Split them. */
	Index j= eqClasses[c];	/* Other tempClass. */
	ISet setJ= tempClasses[j].val; /* Other min-range set. */
	ISet intIJ;		/* Intersection of two min-range sets.	*/
	Index intClass;		/* TempClass for intersection min-range. */
	ISetElement intElement; /* Element in intersection. */
	VOIDP iterP1;
	assert(j < i);		/* j must have been previously processed. */
	if (isSubsetISet(setI, setJ)) {
	  intClass= i; diffISet(setJ, setI); intIJ= setI;
	}
	else if (isSubsetISet(setJ, setI)) {
	  intClass= j; diffISet(setI, setJ); intIJ= setJ; 
	}
	else { /* We need to process the intersection later. */
	  Index k= nTempClasses++; 	/* New temporary class. */
	  assert(k > i);
	  assert(nTempClasses <= classesP->nClasses + nRanges);
	  intClass= NIL; /* Will process int. ranges with tempClasses[k]. */
	  intIJ= copyISet(setI); intersectISet(intIJ, setJ); 
	  diffISet(setI, intIJ); diffISet(setJ, intIJ);
	  tempClasses[k].val= intIJ; tempClasses[k].eqClass= NIL;
	}
	FOR_EACH_IN_ISET(intElement, intIJ, iterP1) { 
	  /* Assign each min-range in intersection to intClass. */
	  eqClasses[intElement]= intClass;
	}
      } /* else if (eqClasses[c] != i) */ 
    } /* FOR_EACH_IN_ISET(c, setI, iterP) */
  } /* for (i= 0; i < nTempClasses; i++) */

  /* At this point, each min-range. will be in some eqClass, as there
   * will have been one pattern which covers all chars. for the
   * default action.  Mark those classes in tempClasses which are
   * really eqClasses by setting their eqClass field to the final
   * equiv. class number.
   */
  for (i= 0; i < nRanges; i++) {
    assert(eqClasses[i] != NIL);
    if (tempClasses[eqClasses[i]].eqClass == NIL) {
      tempClasses[eqClasses[i]].eqClass= nEq++;
    }
    eqClasses[i]= tempClasses[eqClasses[i]].eqClass;
  }

  { /* Compute equivalence class bit-sets to cover each unique chr-class. */
    for (i= 0; i < nHashEntries; i++) {
      HashEntry *heP= &HASH_ENTRIES(i);
      if (heP->refCount > 0) {
	CONST Class class= i;
	ISet minRanges= newISet();
	VOIDP rP= CLASS_HDR_P(class);
	RawClass rawClass= rP;
	ISet eqC= classSets[i]= newISet();
	ISetElement c;
	VOIDP iterP;
	minRanges= rawClassToMinRangeISet(rawClass, minRanges);
	FOR_EACH_IN_ISET(c, minRanges, iterP) {
	  addISet(eqC, eqClasses[c]);
	}
	delISet(minRanges);
      }
    } /* for (i= 0; i < nHashEntries; i++) */
  }

  /* Clean up tempClasses (hi-to-lo to allow easier recycling of sets). */
  for (i= nTempClasses; i > 0; i--) {
    delISet(tempClasses[i - 1].val);
  }

  FREE(tempClasses);
  chrClassP->nEq= nEq; chrClassP->eqClasses= eqClasses;
  VOID_RET();
}

/* Get eq-class for character c.  Not an efficient implementation (a more
 * efficient one would use a binary search) but suffices for its use
 * in finding the eq-class for the sentinel-char.
 */
Index
getEqClass(c)
  Index c;
{
  assert(c < OPTIONS(charSetSize));
  if (chrClassP->nEq == OPTIONS(charSetSize)) {
    return c;
  }
  else { /* Linear search. */
    CONST Bound *CONST minRangeBounds= chrClassP->minRangeBounds;
    Index minRange1= 1;
    while (c >= minRangeBounds[minRange1]) minRange1++;
    return chrClassP->eqClasses[minRange1 - 1];
  }
}


/*			CLASS BIT-SETS COMPUTATION			*/

/* Compute bit-set representations of all classes.  To be called after
 * all classes have been constructed.  Will do eq-class construction
 * (if requested via options or 16-bit chars) and compression (if
 * 16-bit chars and compression is worthwhile).  Also retain return
 * value internally to be used as implicit parameter in following
 * routines.  This will delete all internal information except for
 * that contained in the return value.
 */
ClassContext
computeClasses() {
  ClassContext classContext;
  chrClassP= classContext= newChrClass();
  rmRedundantCtypeClasses();
  if (OPTIONS(equivClasses)) {
    buildMinRanges(); formEqClasses(); compressEqClasses();
  }
  else { /* Create sets containing raw chars in each class. */
    CONST Count maxClassNum1= chrClassP->maxClassNum1;
    ISet *CONST classSets= chrClassP->classSets;
    Index i;
    for (i= 0; i < maxClassNum1; i++) {
      HashEntry *heP= &HASH_ENTRIES(i);
      if (heP->refCount > 0) {
	Range *hdrP= (Range*) heP->rawClass;
	Range *p;
	ISet set= classSets[i]= newISet();
	for (p= hdrP->succ; p != NULL; p= p->succ) {
	  rangeISet(set, p->u1.lo, p->u2.hi - 1);
	}
      }
    }
  }
#if DO_TRACE
  if (files.dumpFile && chrClassP->nEq != OPTIONS(charSetSize)) {
    dumpEqClasses(files.dumpFile);
  }
#endif
  delClasses();
  return classContext;
}



/*			COMPRESS EQUIVALENCE CLASSES			*/

/* 

When 16 bit-characters are used, the char-space is divided into
segments  and a char-code is regarded as a <segment-#> field followed
by a <offset-within-segment> field.  The above <segment-#> can be
regarded as the row # and the <offset-within-segment> can be regarded
as the column # of a 2-dimensional table specifying the eq-class
associated with each character, and normal row-displacement
compression can be used to compress the table.

*/

enum {
  CHAR16_SIZE= 0x10000,	/* Size of a 16-bit char-set. */
  CLASS_SEG_BIT= 8,	/* The bit # (little-endian) for seg-division of a
			   16-bit char. */
  SEG_SIZE= 		/* <offset-within-segment> is in [0, SEG_SIZE). */
    1<<CLASS_SEG_BIT,	
  N_SEGS= 		/* <segment-#> is in [0, N_SEGS). */
    CHAR16_SIZE/SEG_SIZE
};

/* With row-displacement compression with the above segment parameters, 
 * to get eqClass(c), where cHi= c >> 8 and cLo= c & 0xFF:
 * eqClass(c)= 
 *   (check[base[cHi]+cLo] == cLo) ? next[base[cHi]+cLo] : default[cHi];
 */

/* Fill in # of non-default equivalence-classes for each segment entry.
 * While doing so, compute for each segment, the smallest minRange
 * which intersects it and the default eq-class for that segment;
 * save that information in segInfo.
 */
static VOID
segNEntries(entries)
  CompTabEntry entries[];
{
  CONST Bound *CONST minRangeBounds= chrClassP->minRangeBounds;
  CONST EqClass *CONST eqClasses= chrClassP->eqClasses;
  SegInfo *CONST segInfo= MALLOC_VEC(N_SEGS, SegInfo);
  Count *eqCounts= MALLOC_VEC(chrClassP->nEq, Count);
  Index minRange= 0;
  Index segN;
  for (segN= 0; segN < N_SEGS; segN++) { /* Loop thru segments. */
    CONST Index segLo= segN * SEG_SIZE;  /* Lower (inclusive) seg-limit. */
    CONST Index segHi= segLo + SEG_SIZE; /* Upper (exclusive) seg-limit. */
    CONST Index segMinRange= minRange;	 /* 1st minRange for segment. */
    CONST Index rangeLo= minRangeBounds[minRange];
    CONST Index rangeHi= minRangeBounds[minRange + 1];
    CONST Index hi= (rangeHi < segHi) ? rangeHi : segHi;
    CONST Index eqC= eqClasses[minRange];
    ISet eqs= newISet(); 	/* Set of eq-classes for segment segN. */
    Index defEqC= eqC;			/* Assume 1st eq-class is default. */
    Count maxCount= hi - segLo;		/* #chars which intersect 1st range. */
    assert(rangeLo <= segLo && segLo < rangeHi);
    assert(isEmptyISet(eqs));	
    eqCounts[eqC]= maxCount; addISet(eqs, eqC);
    while (minRangeBounds[minRange + 1] <= segHi) { 
      /* This loop does not run off the end of minRangeBounds[] because of
       * the sentinel at the end of it.
       */
      minRange++;
      {	CONST Index rangeLo= minRangeBounds[minRange];
	CONST Index rangeHi= minRangeBounds[minRange + 1];
	CONST Index hi= (rangeHi < segHi) ? rangeHi : segHi;
	CONST Index eqC= eqClasses[minRange];
	assert(segLo < rangeLo && rangeLo <= segHi);
	if (!inISet(eqC, eqs)) { 
	  addISet(eqs, eqC); eqCounts[eqC]= 0; 
	}
	eqCounts[eqC]+= hi - rangeLo;
	if (eqCounts[eqC] > maxCount) {
	  maxCount= eqCounts[eqC]; defEqC= eqC;
	}
      }
    } /* while (minRangeBounds[minRange + 1] <= segHi) */
    segInfo[segN].segMinRange= segMinRange; segInfo[segN].defEqClass= defEqC;
    entries[segN].input= segN; entries[segN].value= SEG_SIZE - maxCount;
    delISet(eqs);
  } /*   for (segN= 0; segN < N_SEGS; segN++) */
  assert(minRange == chrClassP->nMinRanges);
  assert(minRangeBounds[minRange] == segN * SEG_SIZE);
  FREE(eqCounts);
  chrClassP->segInfo= segInfo;
  VOID_RET();
}

/* Fill in non-default equivalence-classes for segment number segN. */
static Count
segEntries(segN, entries)
  Index segN;
  CompTabEntry entries[];
{
  CONST EqClass *CONST eqClasses= chrClassP->eqClasses;
  CONST SegInfo *CONST segInfo= chrClassP->segInfo;
  CONST Bound *CONST minRangeBounds= chrClassP->minRangeBounds;
  CONST Index entryLo= segN * SEG_SIZE;
  CONST Index entryHi= entryLo + SEG_SIZE;
  CONST Index defEqClass= segInfo[segN].defEqClass;
  Count nEntries= 0;
  Index minRange= segInfo[segN].segMinRange;
  while (minRangeBounds[minRange] < entryHi) {
    CONST Index minRangeEqClass= eqClasses[minRange];
    if (minRangeEqClass != defEqClass) {
      CONST Index rangeLo= minRangeBounds[minRange];
      CONST Index rangeHi= minRangeBounds[minRange + 1];
      CONST Index lo= (entryLo > rangeLo) ? entryLo : rangeLo;
      CONST Index hi= (entryHi < rangeHi) ? entryHi : rangeHi;
      Index j;
      assert(lo < hi);
      for (j= lo; j < hi; j++) {
	entries[nEntries].input= j % SEG_SIZE; 
	entries[nEntries].value= minRangeEqClass;
	nEntries++;
      }
    }
    minRange++;
  } /* while (minRangeBounds[minRange] < entryHi) */
  return nEntries;
}


static VOID
compressEqClasses()
{
  if (OPTIONS(charSetSize) > 256) {
    CompTab *eqComp= MALLOC_VEC(1, CompTab);
    compressTab(N_SEGS, SEG_SIZE, segNEntries, segEntries, eqComp);
    if (eqComp->nextSize > optionsP->charSetSize/2) {
      /* Compression probably not worthwhile. */
      delCompTab(eqComp); FREE(eqComp); eqComp= NULL;
    }
    chrClassP->eqComp= eqComp;
  }
}


/*			OUTPUT EQUIVALENCE CLASSES			*/

Count
outEqClassParams(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  fprintf(outFile, "#define YY_16_BIT %d\n", OPTIONS(charSetSize) > 256);
  nLines++;
  fprintf(outFile, "#define YY_CLASS_SEG_BIT %d\n", CLASS_SEG_BIT);
  nLines++;
  fprintf(outFile, "#define YY_N_EQ_CLASSES %d\n", chrClassP->nEq);
  nLines++;
  fprintf(outFile, "#define YY_MAX_EQ_BASE %d\n", 
	  chrClassP->eqComp ? chrClassP->eqComp->maxBase : 0);
  nLines++;
  fprintf(outFile, "#define YY_MAX_EQ_CHECK %d\n", 
	  chrClassP->eqComp ? SEG_SIZE : 0);
  nLines++;
  return nLines;
}

static Count
outEqClasses(outFile, indent)
  FILE *outFile;
  Index indent;
{
  enum { N_PER_LINE= NLINE1 };
  CONST Bound *CONST minRangeBounds= chrClassP->minRangeBounds;
  CONST EqClass *CONST eqClasses= chrClassP->eqClasses;
  CONST Count charSetSize= OPTIONS(charSetSize);
  Count nLines= 0;
  Count nInLine= 0;
  Index boundsIndex= 0;
  Index c;
  fprintf(outFile, "%*s", indent, ""); 
  for (c= 0; c < charSetSize; c++) {
    CONST Index minRange= 
      (c < minRangeBounds[boundsIndex + 1]) ? boundsIndex : ++boundsIndex;
    CONST Index eqC= eqClasses[minRange];
    assert(boundsIndex < chrClassP->nMinRanges);
    fprintf(outFile, "%5d%s", eqC, c == charSetSize - 1 ? "" : ",");
    nInLine++;
    if (nInLine == N_PER_LINE && c != charSetSize - 1) {
      fprintf(outFile, "\n%*s", indent, ""); nLines++; nInLine= 0;
    }
  }
  return nLines;
}

/* Equiv-spec has 3 possibilities:
 * 1. If no equiv-classes are being used then its empty.
 * 2. If equiv-classes are being used without compression, then
 *	YYClass eqClass[CHAR_SET_SIZE];
 * 3. If equiv-classes are being used with compression, then:
 *	YYEqBase eqBase[YY_N_EQ_BASE];
 *	YYClass eqNext[YY_N_EQ_NEXT];
 *	YYEqCheck eqCheck[YY_N_EQ_CHECK];
 */ 

static VOID
outEqDefaultEntry(outFile, segN)
  FILE *outFile;
  Index segN;
{
  fprintf(outFile, "%5d", chrClassP->segInfo[segN].defEqClass);
  VOID_RET();
}

Count 
equivInitFn(outFile, isDcl, indentP)
  FILE *outFile;
  BooleanX isDcl; 
  Index *indentP;
{
  CONST CompTab *CONST eqComp= chrClassP->eqComp;
  CONST Index indent= *indentP;
  Count nLines= 0;
  if (isDcl) {
    if (OPTIONS(equivClasses)) { /* Eq-classes are being used. */
      if (eqComp) { /* Compression being used. */
	assert(OPTIONS(charSetSize) == CHAR16_SIZE);
	fprintf(outFile, "%*sYYEqBase eqBase[%d];\n", indent, "", N_SEGS);
	nLines++;
	fprintf(outFile, "%*sYYClass eqDefault[%d];\n", indent, "", N_SEGS);
	nLines++;
	fprintf(outFile, "%*sYYClass eqNext[%d];\n", indent, "", 
		eqComp->nextSize);
	nLines++;
	fprintf(outFile, "%*sYYEqCheck eqCheck[%d];\n", indent, "", 
		eqComp->checkSize);
	nLines++;
      }
      else { /* No compression: either 7 or 8 bit char or compression failed. */
	fprintf(outFile, "%*sYYClass eqClass[%d];\n", indent, "", 
		OPTIONS(charSetSize));
	nLines++;
      }
    } /* if (OPTIONS(equivClasses)) */
  } /* if (isDcl) */
  else { 
    CONST Index indent1= indent + INDENT_INC;
    if (OPTIONS(equivClasses)) {
      if (eqComp) { /* Compression being used. */
	assert(OPTIONS(charSetSize) == CHAR16_SIZE);
	fprintf(outFile, "%*s{ /* eqBase[%d] */\n", indent, "", N_SEGS);
	nLines++;
	nLines+= outIndex2Vec(outFile, indent1, NLINE1, N_SEGS, 
			      eqComp->base);
	fprintf(outFile, "\n%*s}, /* eqBase[%d] */\n", indent, "", N_SEGS);
	nLines+= 2;
	fprintf(outFile, "%*s{ /* eqDefault[%d] */\n", indent, "", N_SEGS);
	nLines++;
	nLines+= outVec(outFile, indent1, NLINE1, N_SEGS, outEqDefaultEntry);
	fprintf(outFile, "\n%*s}, /* eqDefault[%d] */\n", indent, "", N_SEGS);
	nLines+= 2;
	fprintf(outFile, "%*s{ /* eqNext[%d] */\n", indent, "", 
		eqComp->nextSize);
	nLines++;
	nLines+= outIndex2Vec(outFile, indent1, NLINE1, eqComp->nextSize,
			      eqComp->next);
	fprintf(outFile, "\n%*s}, /* eqNext[%d] */\n", indent, "", 
		eqComp->nextSize);
	nLines+= 2;
	fprintf(outFile, "%*s{ /* eqCheck[%d] */\n", indent, "", 
		eqComp->checkSize);
	nLines++;
	nLines+= outIndex2Vec(outFile, indent1, NLINE1, eqComp->checkSize,
			      eqComp->check);
	fprintf(outFile, "\n%*s} /* eqCheck[%d] */\n", indent, "", 
		eqComp->checkSize);
	nLines+= 2;
      } /* if (eqComp) */
      else { /* Simple vector of eq-classes indexed by character. */
	fprintf(outFile, "%*s{ /* eqClass[%d] */\n", indent, "", 
		OPTIONS(charSetSize));
	nLines++;
	nLines+= outEqClasses(outFile, indent1);
	fprintf(outFile, "\n%*s} /* eqClass[%d] */\n", indent, "", 
		OPTIONS(charSetSize));
	nLines+= 2;
      }
    } /* if (OPTIONS(equivClasses)) */
    delChrClass(chrClassP);
  }
  return nLines;
}


/* 			INITIALIZATION & DESTRUCTION			*/

/* Module initialization. */
VOID 
initChrClass()
{
  initClasses();
  initNamedClasses();
  univClass();
}


/*				DUMP ROUTINES.				*/

/*			   DUMP CHARACTER CLASS.			*/

#ifdef DO_TRACE

static VOID dumpChar PROTO((FILE *dumpFile, Int c));
static VOID dumpRange PROTO((FILE *dumpFile, CONST Range *rP));


static VOID
dumpChar(dumpFile, c)
  FILE *dumpFile;
  Int c;
{
#if __STDC__
  enum { BEL= '\a' };
#else
  enum { BEL= 07 };
#endif
  if (isalnum(c)) {
    fprintf(dumpFile, "%c", c);
  }
  else if (c == BEL) {
    fputs("\\a", dumpFile);
  }
  else if (c == '\b') {
    fputs("\\b", dumpFile);
  }
  else if (c == '\f') {
    fputs("\\f", dumpFile);
  }
  else if (c == '\n') {
    fputs("\\n", dumpFile);
  }
  else if (c == '\r') {
    fputs("\\r", dumpFile);
  }
  else if (c == '\t') {
    fputs("\\t", dumpFile);
  }
  else if (c == '\v') {
    fputs("\\v", dumpFile);
  }
  else if (c <= ' ' || c >= 128) {
    fprintf(dumpFile, "\\x%x", c);
  }
  else {
    fprintf(dumpFile, "\\%c", c);
  }
  VOID_RET();
}

static VOID
dumpRange(dumpFile, rP)
  FILE *dumpFile;
  CONST Range *rP;
{
  CONST Bound lo= rP->u1.lo;
  CONST Bound hi= rP->u2.hi - 1; /* -1 makes into inclusive upper bound. */
  assert(lo <= hi);
  if (lo == hi) {
    dumpChar(dumpFile, lo);
  }
  else if (lo + 1 == hi) {
    dumpChar(dumpFile, lo); dumpChar(dumpFile, hi);
  }
  else {
    if (   (islower(lo) && islower(hi))
	|| (isupper(lo) && isupper(hi))
        || (isdigit(lo) && isdigit(hi))
	|| (lo >= 256 || hi >= 256)
       ) {
      fprintf(dumpFile, "%c-%c", lo, hi);
    }
    else {
      Index i;
      for (i= lo; i <= hi; i++) dumpChar(dumpFile, i);
    }
  }
  VOID_RET();
}

VOID
dumpClass(dumpFile, class)
  FILE *dumpFile; 
  ClassX class;
{
  CONST Count n= sizeClass(class);
  CONST Range *CONST hdrP= CLASS_HDR_P(class);
  if (n == 1) {
    assert(hdrP->succ);
    dumpChar(dumpFile, hdrP->succ->u1.lo);
  }
  else {
    CONST Boolean negate= n > OPTIONS(charSetSize)/2;
    Class class1= negate ? negClass(class) : copyClass(class);
    Range *hdrP= CLASS_HDR_P(class1);
    Range *p;
    fprintf(dumpFile, "[%s", negate ? "^" : ""); 
    for (p= hdrP->succ; p != NULL; p= p->succ) {
      dumpRange(dumpFile, p);
    }
    fputs("]", dumpFile);
    delClass(class1);
  }
  VOID_RET();
}

/* Dump out all equivalence classes.  Slow, but it is meant to be used
 * only for development.
 */
static VOID
dumpEqClasses(dumpFile)
  FILE *dumpFile;
{
  CONST Count nEq= chrClassP->nEq;
  CONST Count nMinRanges= chrClassP->nMinRanges;
  CONST EqClass *CONST eqClasses= chrClassP->eqClasses;
  CONST Bound *CONST minRangeBounds= chrClassP->minRangeBounds;
  EqClass eqClass;
  fputs("EQUIVALENCE CLASSES:\n", dumpFile);
  for (eqClass= 0; eqClass < nEq; eqClass++) {
    RawClass rawClass= newRawClass();
    Class class;
    Index i;
    for (i= 0; i < nMinRanges; i++) {
      if (eqClasses[i] == eqClass) {
	addRawClassLo(rawClass, minRangeBounds[i], minRangeBounds[i+1]-1);
      }
    }
    class= rawClassToClass(rawClass);
    fprintf(dumpFile, "%d: ", eqClass);
    dumpClass(dumpFile, class); fprintf(dumpFile, "\n");
    delClass(class);
  }
  fputs("\n", dumpFile);
  VOID_RET();
}

#endif /* ifdef DO_TRACE */



/*			TEST ROUTINES					*/

#ifdef TEST_CHRCLASS

/* Compile using command:

gcc -g -Wall -I../libz -I.. -DHAVE_CONFIG_H -DTEST_CHRCLASS -DDO_TRACE \
    chrclass.c -L../libz -lz -o chrclass

*/
static char helpMsg[]=
 "Input pairs of non-negative inclusive bounds to specify a class.\n"
 "Input -1 to end a class.  Input -2 to compute and print equivalence\n"
 "classes.\n";
int main()
{
  RawClass rawClass= newRawClass();
  int val;
  initID(FALSE); initISet();
  initChrClass();
  printf(helpMsg);
  do {
    scanf("%d", &val);
    if (val >= 0) {
      Bound lo= val;
      Bound hi;
      scanf("%d", &hi);
      addRawClass(rawClass, lo, hi);
    }
    else if (val == -1) {
      Class class= rawClassToClass(rawClass);
      fprintf(dumpFile, "%d: ", class); 
      dumpClass(stdout, class); printf("\n");
      rawClass= newRawClass();
    }
    else if (val == -2) {
      computeClassISets(); 
      dumpEqClasses(stdout);
      outEqClasses(stdout);
    }
    else {
      printf(helpMsg);
    }
  } while (val != -2);
  return 0;
}

#endif /* ifdef TEST_CHRCLASS */
