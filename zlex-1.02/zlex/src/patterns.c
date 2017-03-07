/*

File:	 patterns.c
Purpose: Maintain patterns.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "chrclass.h"
#include "dfa.h"
#include "files.h"
#include "main.h"
#include "options.h"
#include "outvec.h"
#include "patterns.h"
#include "scan.h"
#include "sklnames.h"

#include "area.h"
#include "error.h"
#include "ids.h"
#include "iset.h"
#include "memalloc.h"

/*

The inputs to this module are a sequence of calls from the parser.  It
maintains start-states and position sets for start states.  As each
section 2 pattern is read in, all macro calls within it are expanded.
After a section 2 pattern is read in, its pattern sets are computed
using the algorithm given in section 3.9 of ASU86.  After the pattern
sets are computed, the pattern is deleted.

At the end of section 2, when the module know exactly how many
patterns there are, it computes mapped action #s for each pattern.  It
also deletes all the storage used for macros and patterns.

The main output of this module are the character classes at each leaf
position of a section 2 pattern and the follow sets for each leaf
position.  It also maintains a mapping from output action #s to source
action #s.

*/

/*			    STATIC DECLARATIONS				*/

/* Type used for storing each word of a macro definition or sec. 2 pattern. */
typedef Index DefX;
typedef UInt2 Def;

static VOID initGlobals PROTO((VOID_ARGS));
static VOID initSS PROTO((VOID_ARGS));
static VOID terminateSS PROTO((VOID_ARGS));
static Index getSSNum PROTO((Index textID));
static VOID initInfo1 PROTO((VOID_ARGS));
static VOID terminateInfo1 PROTO((VOID_ARGS));
static Index getMacroNum PROTO((Index textID));
static Index getMacro PROTO((Index textID));
static VOID delMacroClasses PROTO((VOID_ARGS));
static Def encodeRator PROTO((UInt rator, Index disp));
static Index addToDefs PROTO((DefX d));
static Pattern emptyPat PROTO((VOID_ARGS));
static Pattern expandMacroDef PROTO((Index def));
static Index patEnd PROTO((PatternX p));
static Pattern copyPatLo PROTO((PatternX p));
static Pattern copyPat PROTO((PatternX p, Count n));
static VOID delPatClasses PROTO((PatternX pat));
static Index patLength PROTO((PatternX p));
static VOID initLeaves PROTO((VOID_ARGS));
static VOID exportLeaves PROTO((VOID_ARGS));
static Index newLeaf PROTO((ClassX class));
static VOID computePatSets PROTO((PatternX pat, ISet *firstsP,
				  ISet *lastsP, Boolean *isNullableP));
static VOID initTopPats PROTO((VOID_ARGS));
static VOID terminateTopPats PROTO((VOID_ARGS));
static Index addDefaultPat PROTO((VOID_ARGS));
static VOID initIntra PROTO((VOID_ARGS));
static VOID terminateIntra PROTO((VOID_ARGS));
static VOID initActMaps PROTO((VOID_ARGS));
static VOID terminateActMaps PROTO((VOID_ARGS));
static Index mapAct PROTO((Index patN, BooleanX isIntraTok));
static Index addYYLineNoPat PROTO((VOID_ARGS));
static VOID initContexts PROTO((VOID_ARGS));
static VOID terminateContexts PROTO((VOID_ARGS));
static Index newContext PROTO((Index topPatN, PatternX pat, PatternX context));
static VOID completePats PROTO((VOID_ARGS));
static Index getYYLineNoActN PROTO((VOID_ARGS));

#if DO_TRACE
static VOID dumpMacros PROTO((FILE *dumpFile));
static VOID dumpPositions PROTO((FILE *dumpFile));
#endif


/*		DATA STRUCTURES FOR GLOBAL CONTEXT			*/
	
#define SS_HASH_SIZE 8

typedef struct {
  Index2 hashTab[SS_HASH_SIZE];
  Area startStates;	/* Hash-chains of StartState s. */
  ISet inclusiveSS;	/* Set of all inclusive start-states. */
} SSInfo;

struct Info1;

typedef struct {		/* Global module context. */
  Area leaves;			/* Storage for leaves in patterns. */
  Area follows;			/* Follow sets for leaf positions. */
  ISet intraFirsts;		/* First-sets for all intra-token patterns. */
  ISet intraPos;		/* All positions in intra-token patterns. */
  SSInfo ssInfo;		/* Start-state information. */
  Area topPats;			/* Track top-pattern actions. */
  Area contexts;		/* Context pattern information. */
  struct Info1 *info1P;		/* Information which is deleted after sec 2. */
  Index2 *actMap;		/* Intra-token action mapping. */
  Index2 defaultPatN;		/* Pattern for default action. */
  Count nPatLeaves;		/* # of leaves. */
  Count2 nExtraPats;		/* # of extra patterns (beyond user pats). */
  Count2 nIntraTokPats;		/* Number of intra-token patterns. */
  Boolean hasStartLinePat;	/* TRUE if any pattern contains '^'. */
  Boolean hasVarContextPat;	/* TRUE if there is a variable-context pat. */
} GlobalContext;

static GlobalContext globals;

#define G globals

/* Initialize global components to 0, in case it is heap allocated.
 * No termination necessary, as this is used merely for scalar
 * initialization.
 */
static VOID
initGlobals()
{
  G.defaultPatN= G.nPatLeaves= G.nExtraPats= G.nIntraTokPats= 0;
  G.hasStartLinePat= G.hasVarContextPat= FALSE;
  VOID_RET();
}


/*				PATTERN OPERATORS			*/

/* 

Patterns and macros are stored in a postfix notation so that they can
be constructed during left-to-right parsing in a linear data structure.

*/
 

#define PAT_NAMES							\
  /* ENUM    PRIORITY   	   PICTURE OF POSTFIX 		*/ 	\
  T(ALT_PAT,	0),		/* leftRand rightRand ALT_PAT 	*/	\
  T(CAT_PAT,    1),		/* leftRand rightRand CAT_PAT 	*/	\
  T(CLASS_PAT,  3),		/* class# CLASS_PAT		*/	\
  T(CLOSE_PAT,	2),		/* operand CLOSE_PAT		*/	\
  T(EMPTY_PAT,  3),		/* EMPTY_PAT			*/	\
  T(MACRO_PAT,	3)		/* macro# MACRO_PAT		*/

#define T(enumName, priority)	enumName
typedef enum {	/* enum giving pattern operators. */
  PAT_NAMES
} Rator;
#undef T


/*				START-STATES.				*/

typedef struct {	/* Information collected for each start-state. */
  Index2 textID;	/* Name of start-state. */
  Index2 defLineN;	/* Line on which it was defined. */
  ISet firsts[2];	/* First sets; 0: non-'^' ; 1: all patterns. */
  Index2 eofPatActN;	/* EOF patAct for this state (NIL if none). */
  Index2 succ;		/* Next element in hash-chain. */
  Boolean isExclusive;	/* TRUE if its a exclusive start-state. */
} StartState;

#if 0 /* COMMENT: SSInfo Data-structure declarations at beginning of file. */

#define SS_HASH_SIZE 8

typedef struct {
  Index2 hashTab[SS_HASH_SIZE];
  Area startStates;	/* Hash-chains of StartState s. */
  ISet inclusiveSS;	/* Set of all inclusive start-states. */
} SSInfo;

#endif 

#define SS (&G.ssInfo)

/* Macro which accesses each element of the START_STATES() vector. */
#define START_STATES(i) A_ACCESS(SS->startStates, StartState, i)

/* Initialize all start-states data structures. */
static VOID
initSS()
{
  Index2 *hashTab= SS->hashTab;
  Index i;
  for (i= 0; i < SS_HASH_SIZE; i++) hashTab[i]= NIL;
  INIT_AREA(SS->startStates, StartState, 4, UINT2_MAX);
  SS->inclusiveSS= newISet();
  defStartState(getID("INITIAL", 7), FALSE, 0);
  VOID_RET();
}

/* Destroy start-state data structures. */
static VOID
terminateSS()
{
  /* We do not need to delete SS firsts[] sets as they would have been
   * incorporated into the DFA or deleted if redundant.
   */
  delArea(&SS->startStates);
  delISet(SS->inclusiveSS);
  VOID_RET();
}


/*			GET / DEFINE A START STATE.			*/

#define HASH_SS_ID(id)	(id % SS_HASH_SIZE)

/* Return the start-state number for textID.  If not found, create
 * a (inclusive) entry.
 */
static Index
getSSNum(textID)
  Index textID;
{
  Index2 *CONST hashTab= SS->hashTab;
  CONST Index h= HASH_SS_ID(textID);
  Index i;
  for (i= hashTab[h]; i != NIL; i= START_STATES(i).succ) {
    if (START_STATES(i).textID == textID) return i;
  }
  i= A_NEXT(SS->startStates);
  START_STATES(i).textID= textID; START_STATES(i).isExclusive= FALSE; 
  START_STATES(i).defLineN= NIL;
  START_STATES(i).firsts[0]= newISet(); START_STATES(i).firsts[1]= newISet();
  START_STATES(i).eofPatActN= NIL;
  START_STATES(i).succ= hashTab[h]; hashTab[h]= i;
  return i;
}

/* Define start-state textID on line defLineN.  If already defined 
 * signal a error.  Make state exclusive if isExclusive.
 */
VOID
defStartState(textID, isExclusive, defLineN)
  Index textID;
  BooleanX isExclusive;
  Index defLineN;
{
  Index ssNum= getSSNum(textID);
  StartState *ssP= &START_STATES(ssNum);
  if (ssP->defLineN != NIL) {
    error("Multiple definition of start-state %s.",getIDString(textID));
    errorAtLine("%d: Previous definition.",  ssP->defLineN);
  }
  else {
    ssP->defLineN= defLineN; ssP->isExclusive= isExclusive;
    if (!isExclusive) addISet(SS->inclusiveSS, ssNum);
  }
  VOID_RET();
}

/* Return the number of start-state with name textID.  If not defined,
 * signal an error (after creating a dummy one).
 */
Index getStartState(textID)
  Index textID;
{
  Index ssNum= getSSNum(textID);
  StartState *ssP= &START_STATES(ssNum);
  if (ssP->defLineN == NIL) {
    error("Undefined start-state %s.  Only first occurrence reported.", 
	  getIDString(textID));
    ssP->defLineN= 0;
  }
  return ssNum;
}

/* Return the first positions for start state with number ssNum.  The
 * return value is a pointer to a 2-element vector firsts[] such that
 * firsts[0] is the set of first positions for those patterns which do not
 * include the '^' beginning-of-line anchor, whereas firsts[1] is a set
 * which the first positions for all patterns irrespective of whether
 * or not they contain '^'.
 */
ISet *
getStartStateFirsts(ssNum)
  Index ssNum;
{
  ISet *firsts= START_STATES(ssNum).firsts;
  assert(ssNum < A_NENTRIES(SS->startStates));
  (VOID) RELEASE_ISET(firsts[0]); (VOID) RELEASE_ISET(firsts[1]);
  return firsts;
}

/* Return the # of start states. */
Count
nStartStates()
{
  return A_NENTRIES(SS->startStates);
}

/* Output start state #defines to outFile, returning the # of lines
 * output. 
 */
Count
outSS(outFile)
  FILE *outFile;
{
  CONST Count nSS= A_NENTRIES(SS->startStates);
  Count nLines= 0;
  Index s;
  for (s= 0; s < nSS; s++) {
    fprintf(outFile, "#define %s %d\n", getIDString(START_STATES(s).textID), 
	    G.hasStartLinePat ? 2 * s : s);
    nLines++;
  }
  return nLines;
}


/*			        MACROS					*/

/* A macro has been defined when its def entry below is non-NIL.  If a 
 * macro is referred to (in another macro) before it is defined, then
 * its def entry will be NIL until it is defined.
 */
typedef struct {
  Index2 textID;	/* ID for this macro identifier. */
  Index2 defLineN;	/* Line # where macro first defined. */
  Pattern def;		/* If defined, macro definition (NIL == undef). */
  Index2 succ;		/* Next entry in hash-chain. */
  Boolean isExpanding;	/* TRUE if currently expanding macro in section 2. */
} Macro;

#define MACROS_HASH_SIZE 16
#define HASH_MACRO_ID(id)	((id) % MACROS_HASH_SIZE)

typedef struct Info1 {
  Index2 
    hashTab[MACROS_HASH_SIZE];	/* Hash-table for macros. */
  Area macros;			/* Hash-chains. */
  Area defs;			/* Macro definitions. */
  Area lasts;			/* Last sets for patterns. */
  Index2 lastNLeaves;		/* Total # of leaves after last toppat. */
  Index2 sec1DefsTop;		/* Top of DEFS after sec. 1. */
  Boolean inSec2;		/* TRUE if in section 2. */
} Info1;

/* MacrosInfo struct dynamically allocated and pointed to by global context. */
#define INFO1_P		(G.info1P)

#define MACROS(i) 	A_ACCESS(INFO1_P->macros, Macro, i)


/* Type used for storing each word of a macro definition or sec. 2 pattern. */
#if 0 /* COMMENT: Defined before static function protos. */
typedef Index DefX;
typedef UInt2 Def;
#endif

#define DEFS(i) A_ACCESS(INFO1_P->defs, Def, i)

#define LASTS(i) A_ACCESS(INFO1_P->lasts, ISet, i)

/* Initialize data structures used by macros. */
static VOID
initInfo1()
{
  Index2 *macrosHashTab;
  Index i;
  INFO1_P= MALLOC_VEC(1, Info1);
  macrosHashTab= INFO1_P->hashTab;
  for (i= 0; i < MACROS_HASH_SIZE; i++) macrosHashTab[i]= NIL;
  INIT_AREA(INFO1_P->macros, Macro, 5, UINT2_MAX);
  INIT_AREA(INFO1_P->defs, Def, 9, UINT2_MAX);
  INIT_AREA(INFO1_P->lasts, ISet, 8, UINT2_MAX);
  INFO1_P->lastNLeaves= 0;
  INFO1_P->inSec2= FALSE;
  VOID_RET();
}

/* Destroy all data structures used by macros. */
static VOID
terminateInfo1()
{
  delMacroClasses();
  delArea(&INFO1_P->macros);
  delArea(&INFO1_P->defs);
  delArea(&INFO1_P->lasts);
  FREE(INFO1_P); INFO1_P= NULL;
  VOID_RET();
}

/* Return number for macro with name textID.  Create an entry for it if
 * it isn't there.
 */
static Index getMacroNum(textID)
  Index textID;
{
  Index2 *CONST macrosHashTab= INFO1_P->hashTab;
  CONST Index h= HASH_MACRO_ID(textID);
  Index i;
  for (i= macrosHashTab[h]; i != NIL; i= MACROS(i).succ) {
    if (MACROS(i).textID == textID) return i;
  }
  i= A_NEXT(INFO1_P->macros);
  MACROS(i).textID= textID; 
  MACROS(i).defLineN= MACROS(i).def= NIL;
  MACROS(i).isExpanding= FALSE;
  MACROS(i).succ= macrosHashTab[h]; macrosHashTab[h]= i;
  return i;
}

/* Define a macro with name textID on line defLineN.  If pat is NIL,
 * then make the macro empty.  Signal a error if it has already been
 * defined.
 */
Index
defMacro(textID, pat, defLineN)
  Index textID;
  PatternX pat;
  Index defLineN;
{
  Index macroID= getMacroNum(textID);
  Macro *mP= &MACROS(macroID);
  if (mP->def != NIL) {
    error("Multiple definition of macro %s.",getIDString(textID));
    assert(mP->defLineN != NIL);
    errorAtLine("%d: Previous definition.",  mP->defLineN);
  }
  else {
    mP->defLineN= defLineN; 
    mP->def= (pat == NIL) ? emptyPat() : pat;
  }
  return macroID;
}

/* Return the macro number for name textID.  Signal an error if not defined. */
static Index 
getMacro(textID)
  Index textID;
{
  Index macroID= getMacroNum(textID);
  if (INFO1_P->inSec2) {
    Macro *mP= &MACROS(macroID);
    if (mP->def == NIL) {
      error("Undefined macro %s.  Only first occurrence reported.", 
	    getIDString(textID));
      mP->defLineN= 0; mP->def= emptyPat();
    }
  }
  return macroID;
}

/* When the macro definitions are deleted, the chrclasses used in the 
 * definitions should be deleted so that they can be GC'd.
 */
static VOID
delMacroClasses()
{
  CONST Count nMacros= A_NENTRIES(INFO1_P->macros);
  Index i;
  for (i= 0; i < nMacros; i++) {
    delPatClasses(MACROS(i).def);
  }
  VOID_RET();
}


/*				PATTERN STORAGE.			*/

/* 

The pattern storage area is used to store macro definitions as well as
a single pattern at a time (with expanded macros) when section 2 of
the source file is being processed.  A pattern or definition is
specified by the index of the top-level operator in the pattern: when
the RPN representation is written left-to-right, this will be the
rightmost part of the representation.  If an operator is at index i
its rightmost operand will be found at index i - 1.  To facilitate
finding the start of the leftmost operand (if any) its relative
displacement is encoded in the definition word which contains the
operator.

*/

/* # of bits needed to contain a pattern opeRator. */
enum { RATOR_BIT= 3 };	

/* Constructor for a word which contains a rator and its left rand disp. */
#define ENCODE_RATOR_WITH_DISP(rator, disp)	((rator)|((disp)<<RATOR_BIT))

/* Selectors for operator and left-operand displacement. */
#define OP_DISP(w)	((w) >> RATOR_BIT)
#define RATOR(w)	((Rator)((w) & ((1<<RATOR_BIT) - 1)))

/* Max. displacement possible. */
#define MAX_OP_DISP	((1L<<(sizeof(Def)*CHAR_BIT - RATOR_BIT)) - 1)

/* Encode a rator with its displacement, checking the displacement. */
static Def
encodeRator(rator, disp)
  UInt rator;
  Index disp;
{
  if (disp > MAX_OP_DISP) {
    error("Pattern too big.");
    return ENCODE_RATOR_WITH_DISP(EMPTY_PAT, 0);
  }
  else {
    return ENCODE_RATOR_WITH_DISP(rator, disp);
  }
}

/* Add d to end of DEFS[]. */
static Index 
addToDefs(d)
  DefX d;
{
  Index i= A_NEXT(INFO1_P->defs);
  DEFS(i)= d;
  return i;
}


/*			    PATTERN CONSTRUCTOR FUNCTIONS		*/
/*

Pattern constructor functions assume that their operands are at the
"end" of the DEFS() vector.  It is also assumed that the operands
are contiguous.

*/


/*			   	ALT PATTERNS				*/

/* Build an alternation of patterns left & right, ensuring that
 * character classes are merged whenever possible (this is important,
 * as this can reduce the number of eq-classes which can reduce the
 * size of the generated scanner). If a CLASS_PAT is returned as a
 * subtree, then it is always returned as the right subtree.  ALT_PATs
 * are maintained in left associative subtrees.
 *
 * This routine works using the following rewrite rules:
 *	
 *  alt([A], [B])	==>	[AB]
 *  alt([A], B)		==>	alt(B, [A])
 *  alt(A|[B], [C])	==>	A | [BC]
 *  alt(A|[B], C)	==>	alt(alt(A, C), [B])
 *  alt(A, B|C)		==>	alt(alt(A, B), C)
 *  alt(A, B)		==>	A|B
 *
 * where the alt(left, right) denotes altPat(left, right), the unadorned
 * A, B, C denotes an unrestricted regular expression, [A] and [B] denotes
 * character classes and [AB] denotes their union.  The rule selected
 * in the above list is the first one which matches the operands. alt()
 * on a rule RHS denotes a recursive call, whereas the '|' denotes an
 * ALT_PAT constructor.
 *
 * Termination is because the quantity 
 *
 *   # of top-level CLASS_PATs in left + # of top-level ALT_PATs in right
 *
 * is strictly smaller for any recursive call where top-level refers
 * to the root or a child of an ALT_PAT root.
 *
 * When the second-last rule is used, the current implementation saves
 * C in a save area before making the inner recursive call.  It may be
 * possible to do this without saving C in a separate save area and
 * merely moving it out of harm's way in the DEFS() area.  This would
 * be tricky.  The following invariant may prove useful if this is done.
 *
 *	 |alt(A, B)| <= |A| + |B| + 1
 *
 * where |x| denotes the number of words of pattern storage units
 * occupied by regular expression x.  The invariant can be established
 * by induction on the above rewrite rules.
 */
Pattern 
altPat(left, right)
  PatternX left;
  PatternX right;
{
  Rator leftOp= RATOR(DEFS(left));
  Rator rightOp= RATOR(DEFS(right));

  assert(right == A_NENTRIES(INFO1_P->defs) - 1);
  assert(left < right);
  assert(0 < left);
  if (leftOp == CLASS_PAT && rightOp == CLASS_PAT) { 
    /* alt([A], [B]) ==> [AB] 
     *           left		 right
     * CLASS#A CLASS_PAT CLASS#B CLASS_PAT 
     */
    Class cA= DEFS(left - 1);
    Class cB= DEFS(right - 1);
    Class unionC= unionClass(cA, cB);
    delClass(cA); delClass(cB);
    DEFS(left - 1)= unionC;
    A_CUT(INFO1_P->defs, left + 1);
    return left;
  }
  else if (leftOp == CLASS_PAT) { 
    /* alt([A], B) ==> alt(B, [A])
     *                         left			      right
     * ....           CLASS#A CLASS_PAT  .... 			B
     */
    Class cA= DEFS(left - 1);
    Index src, dest;
    PatternX leftX, rightX;
    for (src= left + 1, dest= left - 1; src <= right; src++, dest++) {
      DEFS(dest)= DEFS(src);
    }
    leftX= dest - 1; rightX= dest + 1;
    DEFS(rightX - 1)= cA; DEFS(rightX)= encodeRator(CLASS_PAT, 0);
    assert(A_NENTRIES(INFO1_P->defs) == rightX + 1);
    return altPat(leftX, rightX);
  }
  else if (leftOp == ALT_PAT && RATOR(DEFS(left - 1)) == CLASS_PAT &&
	   rightOp == CLASS_PAT) { 
    /* alt(A|[B], [C]) ==> A | [BC]
     *                         left		right
     * ...  CLASS#A CLASS_PAT ALT_PAT CLASS#B CLASS_PAT
     */
    Class cA= DEFS(left - 2);
    Class cB= DEFS(right - 1);
    Class unionC= unionClass(cA, cB);
    delClass(cA); delClass(cB);
    DEFS(left - 2)= unionC;
    A_CUT(INFO1_P->defs, left + 1);
    return left;
  }
  else if (leftOp == ALT_PAT && RATOR(DEFS(left - 1)) == CLASS_PAT) {
    /* alt(A|[B], C) ==> alt(alt(A, C), [B])
     *                         left		right
     * ...A  CLASS#B CLASS_PAT ALT_PAT ...         C
     */
    Class cB= DEFS(left - 2);
    Index src, dest;
    Pattern aAltc;
    for (src= left + 1, dest= left - 2; src <= right; src++, dest++) {
      DEFS(dest)= DEFS(src);
    }
    A_CUT(INFO1_P->defs, dest);
    aAltc= altPat(left - 3, dest - 1);
    addToDefs(cB); addToDefs(encodeRator(CLASS_PAT, 0));
    assert(A_NENTRIES(INFO1_P->defs) == aAltc + 3);
    return altPat(aAltc, aAltc + 2);
  }
  else if (rightOp == ALT_PAT) { 
    /* alt(A, B|C) ==> alt(alt(A, B), C)
     *           left          right
     * 		   A  B ... C ALT_PAT
     */
    Area save;	/* Save area for C. */
#define SAVE(i)	A_ACCESS(save, Def, i)
    Pattern a= left;
    Pattern b= right - OP_DISP(DEFS(right));
    Pattern c= right - 1;
    Pattern cEnd= b + 1;
    CONST Size cLen= c - cEnd + 1;
    Pattern aAltb;
    Index src;
    INIT_AREA(save, Def, 8, UINT2_MAX);
    for (src= cEnd; src <= c; src++) { /* Save C. */
      Index i= A_NEXT(save);
      SAVE(i)= DEFS(src);
    }
    assert(A_NENTRIES(save) == cLen);
    A_CUT(INFO1_P->defs, b + 1);
    aAltb= altPat(a, b);
    for (src= 0; src < cLen; src++) addToDefs(SAVE(src));
    delArea(&save);
#undef SAVE
    return altPat(aAltb, A_NENTRIES(INFO1_P->defs) - 1);
  }
  else { /* alt(A, B) ==> A | B */
    return addToDefs(encodeRator(ALT_PAT, A_NENTRIES(INFO1_P->defs) - left));
  }
}

/*			  CATENATION PATTERNS				*/

/* Return a pattern which consists of the catenation of left and right. 
 * Minor simplification of getting rid of a CAT with an EMPTY_PAT.
 */
Pattern 
catPat(left, right)
  PatternX left;
  PatternX right;
{
  Def leftOp= RATOR(DEFS(left));
  Def rightOp= RATOR(DEFS(right));

  assert(right == A_NENTRIES(INFO1_P->defs) - 1);
  assert(left < right);
  assert(0 < left);
  if (rightOp == EMPTY_PAT) {
    assert(right == left + 1);
    A_CUT(INFO1_P->defs, right);
    return left;
  }
  else if (leftOp == EMPTY_PAT) {
    Index src, dest;
    for (src= left + 1, dest= left; src <= right; src++, dest++) {
      DEFS(dest)= DEFS(src);
    }
    assert(dest == right);
    A_CUT(INFO1_P->defs, right);
    return right - 1;
  }
  else {
    return addToDefs(encodeRator(CAT_PAT, A_NENTRIES(INFO1_P->defs) - left));
  }
}


/*			CHARACTER CLASS PATTERNS.			*/	

/* Return a pattern containing all the characters in set charClass. 
 * Note that we do not copy the charClass as we assume we have a 
 * fresh copy in transit to DEFS().
 */
Pattern 
classPat(charClass)
  Index charClass;
{
  if (sizeClass(charClass) == 0) {
    return addToDefs(encodeRator(EMPTY_PAT, 0));
  }
  else {
    addToDefs(charClass);
    return addToDefs(encodeRator(CLASS_PAT, 0));
  }
}


/*			      EMPTY PATTERN				*/

static Pattern
emptyPat()
{
  return addToDefs(encodeRator(EMPTY_PAT, 0));
}


/*			     MACRO PATTERNS.				*/

/* Create a new copy of pattern def with all macros expanded.  Note
 * that we call the constructor functions like altPat() so as to get
 * the benefit of the simplifications done by them.
 */
static Pattern
expandMacroDef(def)
  PatternX def;
{
  CONST Def d= DEFS(def);
  assert(def < A_NENTRIES(INFO1_P->defs));
  switch (RATOR(d)) {
    case ALT_PAT: {
      Pattern leftX= expandMacroDef(def - OP_DISP(d));
      Pattern rightX= expandMacroDef(def - 1);
      return altPat(leftX, rightX);
    }
    case CAT_PAT: {
      Pattern leftX= expandMacroDef(def - OP_DISP(d));
      Pattern rightX= expandMacroDef(def - 1);
      return catPat(leftX, rightX);
    }
    case CLASS_PAT:
      return classPat(copyClass(DEFS(def - 1)));
    case CLOSE_PAT:
      return repeatPat(expandMacroDef(def - 1), 0, NIL);
    case EMPTY_PAT:
      return emptyPat();
    case MACRO_PAT: {
      CONST Index macroNum= DEFS(def - 1);
      Macro *CONST mP= &MACROS(macroNum);
      if (mP->isExpanding) {
	error("Attempt to expand circularly defined macro %s.",
	      getIDString(MACROS(macroNum).textID));
	return emptyPat();
      }
      else {
	Pattern expand;
	mP->isExpanding= TRUE;
        expand= expandMacroDef(mP->def);
	mP->isExpanding= FALSE;
	return expand;
      }
    } /* case MACRO_PAT: */
    default:
      INTERNAL_ERROR();
  } /* switch (RATOR(d) */
  return 0;
}

/* Return a pattern corresponding to a call to macro textID. */
Pattern 
macroPat(textID)
  Index textID;
{
  Index macroNum= getMacro(textID);
  if (!INFO1_P->inSec2) { /* Simply store a MACRO_PAT constructor. */
    addToDefs(macroNum);
    return addToDefs(encodeRator(MACRO_PAT, 0));
  }
  else { /* expand the macro. */
    Macro *CONST mP= &MACROS(macroNum);
    Pattern expand;
    assert(mP->isExpanding == FALSE);
    mP->isExpanding= TRUE;
    expand= expandMacroDef(MACROS(macroNum).def);
    mP->isExpanding= FALSE;
    return expand;
  }
}


/*			      REPETITION PATTERNS.			*/

/* Given a pattern p, return the index of the "end" of the pattern:
 * (since patterns are stored in postfix and the "pattern" indexes its
 * topmost operator the "end" of the pattern refers to the smallest
 * index in DEFS[] which contains the pattern.
 */
static Index
patEnd(p)
  PatternX p;
{
  Index op= RATOR(DEFS(p));
  switch (op) {
    case ALT_PAT:
    case CAT_PAT:
      return patEnd(p - OP_DISP(DEFS(p)));
    case CLASS_PAT:
    case MACRO_PAT:
      return p - 1;
    case CLOSE_PAT:
      return patEnd(p - 1);
    case EMPTY_PAT:
      return p;
    default:
      INTERNAL_ERROR();
  }
  return 0;
}

/* Make a new copy of pattern p, taking care to ensure that character
 * classes are copied using copyClass().
 */
static Pattern
copyPatLo(p)
  PatternX p;
{
  Def d= DEFS(p);
  Rator rator= RATOR(d);
  assert(p < A_NENTRIES(INFO1_P->defs));
  switch (rator) {
    case EMPTY_PAT:
      break;
    case CAT_PAT:
    case ALT_PAT: 
      copyPatLo(p - OP_DISP(d));
      /* No break */
    case CLOSE_PAT:
      copyPatLo(p - 1); 
      break;
    case MACRO_PAT:
      addToDefs(DEFS(p - 1));
      break;
    case CLASS_PAT:
      addToDefs(copyClass(DEFS(p - 1)));
      break;
    default:
      INTERNAL_ERROR();
  } /* switch (rator) */
  return addToDefs(d);	/* Copy operator (including operand displacement). */
}

/* Return CAT of n new copies of pattern p.  Returns EMPTY_PAT when n
 * == 0.
 */
static Pattern
copyPat(p, n)
  PatternX p;
  Count n;
{
  if (n == 0) {
    return emptyPat();
  }
  else {
    Index i;
    for (i= 0; i < n; i++) {
      CONST Index leftCatRand= A_NENTRIES(INFO1_P->defs) - 1;
      copyPatLo(p);
      if (i > 0) {
	addToDefs(encodeRator(CAT_PAT, 
			      A_NENTRIES(INFO1_P->defs) - leftCatRand));
      }
    }
    return A_NENTRIES(INFO1_P->defs) - 1;
  }
}

/* Create ALT of lo thru hi CATs of pattern p.  No part of the result
 * is shared, tho' the returned pattern has a single occurrence of p
 * (unless the result is the EMPTY_PAT when lo == hi == 0).
 * Implements Kleene-closure '*' (lo == 0, hi == NIL), one-or-more '+'
 * (lo == 1, hi == NIL) and optional '?'  (lo == 0, hi == 1).
 */
Pattern 
repeatPat(pat, lo, hi)
  PatternX pat;
  Count lo;
  Count hi;
{
  assert(pat == A_NENTRIES(INFO1_P->defs) - 1);
  if (lo > hi) {
    error("bad repetition bounds: lower bound %d greater than upper bound %d", 
	  lo, hi);
    return pat;
  }
  else if (hi == NIL) { /* Kleene closure involved. */
    /* pat{lo,} compiled into pat* pat{lo,lo}. */
    addToDefs(encodeRator(CLOSE_PAT, 0));	/* pat*. */
    if (lo > 0) { 
      Index leftCatRand= A_NENTRIES(INFO1_P->defs) - 1;
      copyPat(pat, lo);  /* Make lo copies of pat with intervening CATs. */
      addToDefs(encodeRator(CAT_PAT, /* Cat lo copies with pat*. */
			    A_NENTRIES(INFO1_P->defs) - leftCatRand)); 
    }
    return A_NENTRIES(INFO1_P->defs) - 1;
  }
  else if (hi == 0) { /* Replace pattern by EMPTY_PAT. */
    Index e= patEnd(pat);
    DEFS(e)= encodeRator(EMPTY_PAT, 0);
    A_CUT(INFO1_P->defs, e + 1);
    return e;
  }
  else { /* lo, hi both finite bounds with lo <= hi and hi > 0. */
    /* pat{1,hi} compiled to pat{hi,hi}|pat{hi-1,hi-1}|...|pat{lo,lo}. */
    Index i;
    if (hi > 1) { /* Using original pat, make pat{hi,hi}. */
      CONST Index leftCatRand= A_NENTRIES(INFO1_P->defs) - 1; /* Original */
      copyPat(pat, hi - 1); /* hi - 1 copies with intervening CATs. */
      /* Now CAT original pat with above hi - 1 copies. */
      addToDefs(encodeRator(CAT_PAT, A_NENTRIES(INFO1_P->defs) - leftCatRand));
    }
    for (i= hi; i > lo; i--) { /* pat{hi-1,hi-1}|...|pat{lo,lo}. */
      CONST Index leftAltRand= A_NENTRIES(INFO1_P->defs) - 1;
      copyPat(pat, i - 1);  /* i - 1 copies of pat with intervening CATs. */
      addToDefs(encodeRator(ALT_PAT, A_NENTRIES(INFO1_P->defs) - leftAltRand));
    }
    return A_NENTRIES(INFO1_P->defs) - 1;
  }
}


/*			     DELETE PATTERN CLASSES.			*/

/* Delete all character classes in pattern pat, prior to freeing up storage
 * used by pat.  This is called when freeing up macro definitions.  It is
 * not called when deleting a pattern read in from section 2, as the 
 * char classes in those patterns are transferred to the LEAVES[] vector.
 */
static VOID
delPatClasses(pat)
  PatternX pat;
{
  Def d= DEFS(pat);
  switch (RATOR(d)) {
    case ALT_PAT:
    case CAT_PAT:
      delPatClasses(pat - OP_DISP(d)); 
      /* No break */
    case CLOSE_PAT:
      delPatClasses(pat - 1);
      break;
    case CLASS_PAT:
      delClass(DEFS(pat - 1));
      break;
    case MACRO_PAT:
    case EMPTY_PAT:
      break;
    default:
      INTERNAL_ERROR();
  }
  VOID_RET();
}


/*				DUMP MACROS.				*/

#ifdef DO_TRACE

static VOID dumpPat PROTO((FILE *dumpFile, PatternX pat, UInt priority));
static VOID dumpMacroDef PROTO((FILE *dumpFile, Index macroNum));
static VOID dumpMacros PROTO((FILE *dumpFile));

#define T(enumName, priority) 	priority
static UInt1 patPriorities[]= { PAT_NAMES };
#undef T

/* Top-down traversal of pattern, printing it out in infix. */
static VOID
dumpPat(dumpFile, pat, priority)
  FILE *dumpFile;
  PatternX pat;
  UInt priority;
{
  CONST Def d= DEFS(pat);
  CONST Rator rator= RATOR(d);
  CONST UInt ratorPriority= patPriorities[rator];
  if (ratorPriority < priority) fputs("(", dumpFile);
  switch (rator) {
    case ALT_PAT:
    case CAT_PAT:
      dumpPat(dumpFile, pat - OP_DISP(d), ratorPriority); 
      fprintf(dumpFile, "%s", rator == ALT_PAT ? "|" : "");
      dumpPat(dumpFile, pat - 1, ratorPriority + 1);
      break;
    case CLASS_PAT:
      dumpClass(dumpFile, DEFS(pat - 1));
      break;
    case CLOSE_PAT:
      dumpPat(dumpFile, pat - 1, ratorPriority);
      fprintf(dumpFile, "*");
      break;
    case EMPTY_PAT:
      fprintf(dumpFile, "[]");
      break;
    case MACRO_PAT:
      fprintf(dumpFile, "{%s}", getIDString(MACROS(DEFS(pat - 1)).textID));
      break;
  }
  if (ratorPriority < priority) fputs(")", dumpFile);
  VOID_RET();
}

static VOID
dumpMacroDef(dumpFile, macroNum)
  FILE *dumpFile;
  Index macroNum;
{
  fprintf(dumpFile, "%s: ", getIDString(MACROS(macroNum).textID));
  dumpPat(dumpFile, MACROS(macroNum).def, 0); fputs("\n", dumpFile);
  VOID_RET();
}

static VOID 
dumpMacros(dumpFile)
  FILE *dumpFile;
{
  CONST Count nMacros= A_NENTRIES(INFO1_P->macros);
  Index i;
  fputs("MACROS:\n", dumpFile);
  for (i= 0; i < nMacros; i++) {
    dumpMacroDef(dumpFile, i);
  }
  fputs("\n", dumpFile);
  VOID_RET();
}

#endif /* ifdef DO_TRACE */

/*				TEST MACROS				*/

#ifdef TEST_MACROS

/*

Standalone interactive test program to test regular expression
construction.  At one point in development, it could be compiled and
linked using the following commands:

gmake -k CFLAGS="-g -Wall" DFLAGS="-DDO_TRACE -DHAVE_CONFIG_H -DTEST_MACROS" \
      patterns.o; \
gcc patterns.o chrclass.o comptab.o outvec.o -L../libz -lz -o patterns

Will need to #if 0 out the rest of the file following the test program.

Accepts lines specified by the following grammar:

defLine
  : LETTER '=' regExp		#define macro with name LETTER to regExp.
  | #empty
  ;
regExp
  : catExp '|' regExp
  | catExp
  ;
catExp
  : suffixExp catExp
  | suffixExp
  ;
suffixExp
  : suffixExp '*'
  | suffixExp range
  | primExp
  ;
primExp
  : LETTER		#Class consisting of single LETTER.
  | '(' regExp ')'
  | '{' LETTER '}'	#Call to macro with name LETTER
  ;
range
  : '{' DIGIT ',' DIGIT '}'
  | '{' DIGIT ',' '}'
  | '{' DIGIT '}'
  ;

Dumps macro after definition.  Dumps all macros at EOF.

Uses a rec-descent parser.

*/

#include <setjmp.h>

static int tok;			/* Current lookahead token. */
static int yylineno= 1;
static jmp_buf errJmp;

static VOID
nextTok()
{
  int c= getchar();
  while (c == ' ' || c == '\t') c= getchar();
  tok= c;
  if (c == '\n') yylineno++;
  VOID_RET();
}

/* Return the 2nd lookahead (the 1 beyond tok) without affecting tok or
 * the non-blank portion of the input stream. 
 */
static int
look2()
{
  int t= tok;		/* Save current lookahead. */
  int tok2;		/* Next token. */
  nextTok();
  tok2= tok;
  ungetc(tok2, stdin);	/* Push it back. */
  tok= t;		/* Restore current lookahead. */
  return tok2;
}


static VOID
expectTok(c)
  int c;
{
  if (tok != c) {
    error("Expecting `%c'.", c);
    longjmp(errJmp, 1);
  }
  nextTok();
}

static Index defLine();
static Pattern regExp();
static Pattern catExp();
static Pattern suffixExp();
static Pattern primExp();
static VOID range(Index *loP, Index *hiP);

static Index
defLine()
{
  Index macroNum= NIL;
  if (tok != '\n') { /* Line nonempty. */
    if (!isalpha(tok)) {
      error("Macro name should be a letter.");
      longjmp(errJmp, 1);
    }
    else {
      char name[2];
      Pattern p;
      name[0]= tok; name[1]= '\0';
      nextTok(); expectTok('=');
      p= regExp();
      macroNum= defMacro(getID(name, 1), p, yylineno);
    }
  }
  return macroNum;
}

static Pattern 
regExp()
{
  Pattern p= catExp();
  while (tok == '|') {
    Pattern p1;
    expectTok('|'); p1= regExp();
    p= altPat(p, p1);
  }
  return p;
}

static Pattern 
catExp()
{
  Pattern p= suffixExp();
  while (isalpha(tok) || tok == '(' || (tok == '{' && isalpha(look2()))) { 
    /* tok in first(catExp) */
    Pattern p1= suffixExp();
    p= catPat(p, p1);
  }
  return p;
}

static Pattern 
suffixExp()
{
  Pattern p= primExp();
  while (tok == '*' || (tok == '{' && isdigit(look2()))) {
    if (tok == '*') {
      expectTok('*');
      p= repeatPat(p, 0, NIL);
    }
    else if (tok == '{') {
      Index lo, hi;
      assert(isdigit(look2()));
      range(&lo, &hi);
      p= repeatPat(p, lo, hi);
    }
  } 
  return p;
}

static Pattern 
primExp()
{
  Pattern p;
  if (isalpha(tok)) {
    p= classPat(rawClassToClass(addRawClass(newRawClass(), tok, tok)));
    nextTok();
  }
  else if (tok == '(') {
    expectTok('(');
    p= regExp();
    expectTok(')');
  }
  else if (tok == '{' && isalpha(look2())) { /* Macro call. */
    char name[2];
    expectTok('{');
    name[0]= tok; name[1]= '\0';
    nextTok(); 
    p= macroPat(getID(name, 1));
    expectTok('}');
  }
  else {
    error("Primitive regular expression expected."); longjmp(errJmp, 1);
  }
  return p;
}

static VOID 
range(Index *loP, Index *hiP)
{
  expectTok('{');
  if (!isdigit(tok)) {
    error("Expecting a digit."); longjmp(errJmp, 1);
  }
  *loP= tok - '0'; nextTok();
  if (tok == '}') {
    *hiP= *loP;
  }
  else if (tok == ',') {
    expectTok(',');
    if (isdigit(tok)) {
      *hiP= tok - '0'; nextTok();
    }
    else {
      *hiP= NIL;
    }
  }
  expectTok('}');
  VOID_RET();
}

/* Define global variables needed by other files. */
static Options options;
CONST Options *CONST optionsP= &options;
Files files;

#define PROMPT()	printf("> ");

int main() {
  initError(NULL, NULL, NULL, NULL);
  initID(FALSE);
  options.charSetSize= 256;
  initChrClass(); initInfo1();
  files.dumpFile= stdout;
  INFO1_P->inSec2= TRUE;		/* Force expansion of macros. */
  if (setjmp(errJmp) == 0) {
    PROMPT(); nextTok();
  }
  else {
    while (tok != '\n') nextTok();
    PROMPT(); nextTok();
  }
  while (tok != EOF) {
    Index d= defLine();
    if (d != NIL) dumpMacroDef(stdout, d);
    if (tok != '\n') {
      error("Garbage at end of line ignored.");
      longjmp(errJmp, 1);
    }
    PROMPT(); nextTok();
  }
  dumpMacros(stdout);
  terminateInfo1();
  return 0;
}

#endif /* ifdef TEST_MACROS */


/*			      PATTERN LENGTH.				*/

/* Return the length of the string matched by pattern p; NIL if
 * indeterminate. 
 */
static Index
patLength(p)
  PatternX p;
{
  assert(p < A_NENTRIES(INFO1_P->defs));
  switch (RATOR(DEFS(p))) {
    case EMPTY_PAT:
      return 0;
    case CLASS_PAT:
      return 1;
    case ALT_PAT: {
      Size rightLen= patLength(p - 1);
      return (rightLen == NIL || patLength(p - OP_DISP(DEFS(p))) == rightLen)
	     ? rightLen
	     : NIL;
    }
    case CAT_PAT: {
      Size rightLen= patLength(p - 1);
      if (rightLen == NIL)
	return NIL;
      else {
        Size leftLen= patLength(p - OP_DISP(DEFS(p)));
        return (leftLen == NIL) ? NIL : leftLen + rightLen;
      }
    }
    case CLOSE_PAT:
      return NIL;
    default:
      INTERNAL_ERROR();
      return NIL;
  }
}


/*			        LEAVES.					*/

/* 

The algorithm for NFA construction requires the leaves of the patterns
to be indexed by "positions".  By storing all the pattern leaves in an
array LEAVES[], the "positions" are simply indexes for this array.

The leaves of a pattern are of two types: CLASS_PATs (with associated
character-class) and ACT_PATs (with associated pattern #).  An initial
segment of LEAVES[] contains only CLASS_PATs.  This is followed by
only ACT_PATs.  Since the (output) action # associated with an action
is its LEAVES[] index minus the number of leaves for CLASS_PATs, there
is no need to actually reserve storage in LEAVES[] for the ACT_PATs.
Hence the ACT_PATs can be regarded as stored virtually in LEAVES[].

*/

/* LEAVES[] and FOLLOWS[] are parallel vectors.  We did not have a single 
 * vector (using a struct to pack each LEAVES[] and FOLLOWS[] component 
 * together), because on common architectures, there are likely to be
 * holes in the struct, and the size of these vectors could be one of
 * the limiting aspects of this implementation.
 */

/* LEAVES(t) is the character class labelling position t. */
#define LEAVES(t) A_ACCESS(G.leaves, Class, t)

/* FOLLOWS(t) is the set of positions which can follow position t. */
#define FOLLOWS(t) A_ACCESS(G.follows, ISet, t)

/* Globally readable variables to allow efficient access to leaves by
 * other modules.
 */
CONST Class *leafClasses= 0;
CONST ISet *leafFollows= 0;

/* Initialize leaves data structures. */
static VOID
initLeaves()
{
  INIT_AREA(G.leaves, Class, 10, UINT2_MAX);
  INIT_AREA(G.follows, ISet, 10, UINT2_MAX);
  VOID_RET();
}

/* Terminate leaves data structures (by the time this is called, the
 * memory in the leaves should be available via the global pointers
 * leafClasses and leafFollows.  We don't bother with deleting the
 * classes in the leafClasses, because by the time this routine
 * is called all the classes have already been deleted.
 */
VOID
terminateLeaves()
{
  delArea(&G.leaves); delArea(&G.follows); /* Not strictly necessary. */
  FREE(leafClasses); FREE(leafFollows);
  leafClasses= NULL; leafFollows= NULL;
  VOID_RET();
}

/* Export the active memory from the leaves areas to global pointers
 * which can be accessed read-only by other modules.
 */
static VOID 
exportLeaves()
{
  G.nPatLeaves= A_NENTRIES(G.leaves);
  leafClasses= getMemFromArea(&G.leaves);
  leafFollows= getMemFromArea(&G.follows);
}

/* Create a new leaf for CLASS_PAT with character-class class. 
 * Note that since the class is part of a pattern which will be
 * deleted (after the entire pattern has been processed), we do not
 * use copyClass().  Basically the class is being moved from the
 * pattern storage area to the LEAVES[] storage area.
 */
static Index
newLeaf(class)
  ClassX class;
{
  CONST Index t= A_NEXT(G.leaves);
  CONST Index f= A_NEXT(G.follows);
  assert(t == f);
  LEAVES(t)= class; 
  FOLLOWS(f)= newISet();
  return t;
}

/* Return the # of leaves in the patterns. */
Count 
nPatLeaves(VOID_ARGS)
{
  return G.nPatLeaves;
}


/*			   PATTERN SETS COMPUTATION			*/

/* Bottom-up traversal of pattern pat, computing firsts, lasts and
 * nullability in *firstsP, *lastsP and *isNullableP.  Create leaves
 * and update follow-sets for leaves during the computation.
 */
static VOID
computePatSets(pat, firstsP, lastsP, isNullableP)
  PatternX pat;
  ISet *firstsP;
  ISet *lastsP;
  Boolean *isNullableP;
{
  CONST Def d= DEFS(pat);
  switch (RATOR(d)) {
    case ALT_PAT: {
      ISet leftFirsts, leftLasts, rightFirsts, rightLasts;
      Boolean leftIsNullable, rightIsNullable;
      computePatSets(pat - OP_DISP(d), &leftFirsts, &leftLasts, 
		     &leftIsNullable);
      computePatSets(pat - 1, &rightFirsts, &rightLasts, &rightIsNullable);
      *firstsP= unionISet(leftFirsts, rightFirsts);
      *lastsP= unionISet(leftLasts, rightLasts);
      *isNullableP= leftIsNullable || rightIsNullable;
      delISet(rightFirsts);  delISet(rightLasts);
      break;
    }
    case CAT_PAT: {
      ISet leftFirsts, leftLasts, rightFirsts, rightLasts;
      Boolean leftIsNullable, rightIsNullable;
      ISetIter leftLastsIter;
      ISetElement e;
      computePatSets(pat - OP_DISP(d), &leftFirsts, &leftLasts, 
		     &leftIsNullable);
      computePatSets(pat - 1, &rightFirsts, &rightLasts, &rightIsNullable);
      FOR_EACH_IN_ISET(e, leftLasts, leftLastsIter) {
	assert(e < A_NENTRIES(G.follows));
	unionISet(FOLLOWS(e), rightFirsts);
      }
      *firstsP= leftIsNullable 
                ? unionISet(leftFirsts, rightFirsts)
	        : leftFirsts;
      *lastsP= rightIsNullable
	       ? unionISet(rightLasts, leftLasts)
	       : rightLasts;
      *isNullableP= leftIsNullable && rightIsNullable;
      delISet(rightFirsts);  delISet(leftLasts);
      break;
    }
    case CLASS_PAT: {
      Index t= newLeaf(DEFS(pat - 1));
      *firstsP= unaryISet(t);  *lastsP= unaryISet(t);
      *isNullableP= FALSE;
      break;
    }
    case CLOSE_PAT: {
      ISet firsts, lasts;
      ISetElement e;
      ISetIter lastsIter;
      computePatSets(pat - 1, &firsts, &lasts, isNullableP);
      *firstsP= firsts; *lastsP= lasts; *isNullableP= TRUE;
      FOR_EACH_IN_ISET(e, lasts, lastsIter) {
	assert(e < A_NENTRIES(G.follows));
	unionISet(FOLLOWS(e), firsts);
      }
      break;
    }
    case EMPTY_PAT: {
      *firstsP= newISet(); *lastsP= newISet(); *isNullableP= TRUE;
      break;
    }
    default:
      INTERNAL_ERROR();
      break;
  } /* switch (RATOR(d)) */
  VOID_RET();
}
  



/*			    TOP-LEVEL PATTERNS.				*/

typedef enum {
  EOF_TOP,		/* Patterns involving <<EOF>>. */
  CONTEXT_TOP,		/* Patterns with right context. */
  INTRA_TOP,		/* Intra-token pattern. */
  REGULAR_TOP		/* Patterns with optional '^' without right context. */
} TopPatType;

typedef struct {
  Index2 auxInfo;		/* Additional information. */
  Index2 defLineN;		/* Line # where pattern terminated. */
  Index2 srcActN;		/* Source action #. */
  UInt1 topPatType;		/* TopPatType. */
  Boolean isStartLinePat;	/* True if active only at start-of-line. */
} TopPat;

#define TOP_PATS(p) A_ACCESS(G.topPats, TopPat, p)

static VOID
initTopPats()
{
  INIT_AREA(G.topPats, TopPat, 8, UINT2_MAX);
  VOID_RET();
}

static VOID
terminateTopPats()
{
  delArea(&G.topPats);
  VOID_RET();
}


/* Return # of top-level user patterns: output action #s for user actions 
 * are in [1, nUserPats()]. Numbers for pseudo-actions are > nUserPats().
 */
Count 
nUserPats()
{
  return A_NENTRIES(G.topPats) - G.nExtraPats;
}

/* Add a regular pattern with components given by the arguments to
 * TOP_PATS().  The pattern is deleted after its pattern sets
 * have been computed and its leaves moved over into the LEAVES[]
 * area.  The start states set ss is deleted if it is not BAD_ISET.
 */
Index 
addRegularPat(isStartLine, ss, defLineN, pat, context, srcActN)
  BooleanX isStartLine;
  ISet ss;
  Index defLineN;
  PatternX pat;
  PatternX context;
  Index srcActN;
{
  Index i= A_NEXT(G.topPats);
  Index j= A_NEXT(INFO1_P->lasts);
  TopPat *CONST p= &TOP_PATS(i);
  ISet starts= (ss == BAD_ISET) ? SS->inclusiveSS : GRAB_ISET(ss);
  Pattern cPat= 
    (context == NIL) ? pat : ((pat == NIL) ? context : catPat(pat, context));
  ISetIter ssIter;
  ISetElement s;
  ISet firsts, lasts;
  Boolean isNullable;
  assert(i == j);
  assert(!(pat == NIL && context == NIL));
  computePatSets(cPat, &firsts, &lasts, &isNullable);
  FOR_EACH_IN_ISET(s, starts, ssIter) {
    if (!isStartLine) unionISet(START_STATES(s).firsts[0], firsts);
    unionISet(START_STATES(s).firsts[1], firsts);
  }
  LASTS(j)= lasts;
  delISet(firsts);
  if (ss != BAD_ISET) delISet(ss);
  p->isStartLinePat= isStartLine; 
  if (isStartLine) G.hasStartLinePat= TRUE; 
  p->defLineN= defLineN; 
  p->auxInfo= (context == NIL) ? NIL : newContext(i, pat, context);
  p->topPatType= (context == NIL) ? REGULAR_TOP : CONTEXT_TOP;
  p->srcActN= srcActN; 
  INFO1_P->lastNLeaves= A_NENTRIES(G.leaves);
  A_CUT(INFO1_P->defs, INFO1_P->sec1DefsTop); /* Forget pattern. */
  return i;
}

/* Add a <<EOF>> pattern with components given by the arguments to
 * TOP_PATS().  The start states set ss is deleted if it is not
 * BAD_ISET.
 */
Index 
addEOFPat(ss, defLineN, srcActN)
  ISet ss;
  Index defLineN;
  Index srcActN;
{

  Index i= A_NEXT(G.topPats);
  TopPat *CONST p= &TOP_PATS(i);
  Index j= A_NEXT(INFO1_P->lasts);
  ISetElement s;
  ISetIter startsIter;
  ISet starts= (ss == BAD_ISET) ? SS->inclusiveSS : GRAB_ISET(ss); 
  assert(i == j);
  p->isStartLinePat= FALSE; 
  FOR_EACH_IN_ISET(s, starts, startsIter) {
    StartState *ssP= &START_STATES(s);
    if (ssP->eofPatActN != NIL) {
      error("Multiple <<EOF>> acts in start-state %s.",
	    getIDString(ssP->textID));
      errorAtLine("%d: Previous <<EOF>> act definition.", 
	      TOP_PATS(ssP->eofPatActN - 1).defLineN);
    }
    else {
      ssP->eofPatActN= (i + 1);
    }
  }
  p->defLineN= defLineN;
  p->auxInfo= NIL;
  p->srcActN= srcActN; p->topPatType= EOF_TOP;
  if (ss != BAD_ISET) delISet(ss);
  return i;
}

/* Add pattern .|\n for all start-states. */
static Index 
addDefaultPat()
{
  Pattern univPat= classPat(univClass());
  ISet ss= rangeISet(newISet(), 0, nStartStates() - 1);
  Index i= addRegularPat(FALSE, ss, 0, univPat, NIL, 0);
  return i;
}

/* Return the source action number corresponding to the output action
 * number patActN.  Should only be called for a user pattern.
 */
Index 
patActToSrcAct(patActN)
  Index patActN;
{
  Index srcActN= TOP_PATS(patActN - 1).srcActN;
  assert(1 <= patActN && patActN <= nUserPats());
  assert(1 < srcActN && srcActN < nSrcActs());
  return srcActN;
}


/*			        INTRA-TOKEN SUPPORT.			*/

/*

Normally action numbers 1...n correspond to patterns 1...n.  However
with intra-token patterns, we need to have intra-token actions take
precedence over normal actions.  This is conveniently achieved by
remapping the action numbers so that the mapped intra-token action
numbers are less than the numbers of the normal actions.  If there are
m intra-token actions, then the intra-token actions are mapped in
sequence to 1...m, and the regular actions are mapped to (m + 1)
... n.

*/

/* Initialize action patterns.  Should only be called *AFTER* section
 * 2 has been fully seen, so that the # of patterns is known. 
 */
static VOID
initIntra()
{
  G.intraFirsts= newISet();
  G.intraPos= newISet();
  VOID_RET();
}

static VOID
terminateIntra()
{
  delISet(G.intraFirsts);   delISet(G.intraPos);
}
    

static VOID
initActMaps()
{
  if (G.nIntraTokPats > 0) {
    G.actMap= MALLOC_VEC(A_NENTRIES(G.topPats) + 1, Index2);
    G.actMap[0]= 0; /* Map sentinel action to itself. */
  }
  else {
    G.actMap= NULL;
  }
}

static VOID
terminateActMaps()
{
  if (G.nIntraTokPats > 0) FREE(G.actMap);
}

/* Map intra-token pattern with pattern number patN (1-origin) to an
 * action # such that all intra-token actions are before non
 * intra-token actions.
 */
static Index 
mapAct(patN, isIntraTok)
  Index patN;
  BooleanX isIntraTok;
{
  static Count nIntra= 0;
  CONST Index actZ= isIntraTok ? ++nIntra : patN + G.nIntraTokPats - nIntra;
  assert(0 < patN);
  assert(patN <= A_NENTRIES(G.topPats));
  G.actMap[actZ]= patN;
  return actZ;
}

/* Given a mapped action # return the unmapped output action #. */
Index 
unmapAct(mappedAct)
  Index mappedAct;
{
  assert(mappedAct <= A_NENTRIES(G.topPats));
  return (G.nIntraTokPats > 0) ? G.actMap[mappedAct] : mappedAct;
}

/* Add the intra-token pattern pat.  It is deleted after its pattern
 * sets are computed.  All positions in pat are added to G.intraPos.
 */
Index 
addIntraTokPat(defLineN, pat, srcActN)
  Index defLineN;
  PatternX pat;
  Index srcActN;
{
  CONST Index patLen= patLength(pat);
  if (patLen == NIL) {
    error("Intra-token patterns can only match strings of determinate length.");
    return 0;
  }
  else {
    CONST Index t= A_NEXT(G.topPats);
    CONST Index i= A_NEXT(INFO1_P->lasts);
    ISet firsts, lasts;
    Boolean isNullable;
    assert(i == t);
    computePatSets(pat, &firsts, &lasts, &isNullable);
    unionISet(G.intraFirsts, firsts); LASTS(i)= lasts;
    delISet(firsts);
    A_CUT(INFO1_P->defs, INFO1_P->sec1DefsTop); /* Forget pattern. */
    TOP_PATS(t).defLineN= defLineN;
    TOP_PATS(t).srcActN= srcActN;
    TOP_PATS(t).auxInfo= patLen;
    TOP_PATS(t).topPatType= INTRA_TOP;
    TOP_PATS(t).isStartLinePat= FALSE;
    assert(INFO1_P->lastNLeaves < A_NENTRIES(G.leaves));
    rangeISet(G.intraPos, INFO1_P->lastNLeaves, A_NENTRIES(G.leaves));
    INFO1_P->lastNLeaves= A_NENTRIES(G.leaves);
    G.nIntraTokPats++;
    return t;
  }
}

/* Add an intra-token pattern `+\n' to support yylineno. */
static Index
addYYLineNoPat()
{
  return 
    addIntraTokPat(0, classPat(rawClassToClass(addRawClass(newRawClass(), 
							   '\n', '\n'))), 
		   0);
}

/* Return the set containing the first positions of all intra-token
 * patterns. 
 */
ISet 
getIntraFirsts()
{
  return G.intraFirsts;
}

/* Return the # of intra-token patterns. */
Count 
nIntraTokPats()
{
  return G.nIntraTokPats;
}
/* Return *ALL* the positions (including action pseudo-positions) occupied
 * by intra-token patterns.
 */
ISet
getIntraPos()
{
  return G.intraPos;
}


/*			 CONTEXT PATTERN INFORMATION.			*/

typedef struct {	/* Extra information for right context patterns. */
  ISet firsts[2];	/* First sets for var-context. 0: pat; 1: context. */
  ISet lasts[2];	/* Last sets for var-context. 0: pat; 1: context. */
  Int2 len;		/* >=0: patlen; <0: context len. */
  Index2 states[2];	/* States to start rescan for pat and context. */
  Boolean isVar;	/* TRUE if variable context pattern. */
} Context;

#define CONTEXTS(c) A_ACCESS(G.contexts, Context, c)

static VOID
initContexts()
{
  INIT_AREA(G.contexts, Context, 4, UINT2_MAX);
  VOID_RET();
}

static VOID
terminateContexts()
{
  delArea(&G.contexts);
}


/* Build context-information for context pattern topPatN.  Check if
 * length of either pattern or context is determinate.  If so, record
 * (>0: pattern length; <0: context length).  If not determinate, then
 * it is a var-context pattern; record the length as 0 and create a
 * var-context record with first[2] initialized to empty sets;
 * first[0] will be the first-positions set of the pattern; first[1]
 * will be the first-positions set of the context.
 */
static Index 
newContext(topPatN, mainPat, contextPat)
  Index topPatN;
  PatternX mainPat;
  PatternX contextPat;
{
  Index c= A_NEXT(G.contexts);
  assert(contextPat != NIL);
  { Size patLen= (mainPat == NIL) ? 0 : patLength(mainPat);
    Size contextLen= patLength(contextPat);
    CONTEXTS(c).isVar= (patLen == NIL && contextLen == NIL);
    if (CONTEXTS(c).isVar) {
      Pattern mainPatCopy= copyPatLo(mainPat);
      Pattern contextPatCopy= copyPatLo(contextPat);
      Boolean isNullable;
      assert(mainPat != NIL && contextPat != NIL);
      computePatSets(mainPatCopy, &CONTEXTS(c).firsts[0], 
		     &CONTEXTS(c).lasts[0], &isNullable);
      computePatSets(contextPatCopy, &CONTEXTS(c).firsts[1], 
		     &CONTEXTS(c).lasts[1], &isNullable);
      G.hasVarContextPat= TRUE;
      if (optionsP->doCode) {
	error("Variable context patterns not supported for code-scanners.");
      }
    }
    else {
      CONTEXTS(c).len= (patLen == NIL) ? -(Int)contextLen : patLen;
    }
  }
  return c;
}

/* Return the index of the next-var context in CONTEXTS() after cursor.
 * If cursor is NIL, then return the index of the first var-context
 * in CONTEXTS().  If there is no next var-context, then return NIL.
 */
Index 
nextVarContext(cursor)
  Index cursor;
{
  CONST Count nContexts= A_NENTRIES(G.contexts);
  assert(cursor == NIL || cursor < nContexts);
  for (cursor= (cursor == NIL) ? 0 : cursor + 1; 
       cursor < nContexts; cursor++) {
    if (CONTEXTS(cursor).isVar) return cursor;
  }
  return NIL;
}

/* Return a pointer to the first[2] sets for the var-context at
 * CONTEXTS(cursor). 
 */
ISet *
varContextFirsts(cursor)
  Index cursor;
{
  ISet *firsts= CONTEXTS(cursor).firsts;
  assert(cursor < A_NENTRIES(G.contexts) && CONTEXTS(cursor).isVar);
  (VOID) RELEASE_ISET(firsts[0]); (VOID) RELEASE_ISET(firsts[1]);
  return firsts;
}

/* Return a pointer to the states[2] sets for the var-context at
 * CONTEXTS(cursor). 
 */
Index2 *
varContextStates(cursor)
  Index cursor;
{
  assert(cursor < A_NENTRIES(G.contexts) && CONTEXTS(cursor).isVar);
  return CONTEXTS(cursor).states;
}


/*			   COMPLETE PATTERNS.				*/

/* Complete all non-<<EOF>> top-level patterns.  Now since we know the 
 * # of patterns, we can compute mapped action #s.  Add these mapped
 * action #s to the FOLLOWS sets of the last positions of all non-<<EOF>>
 * patterns and delete the last sets.  For intra-token patterns add the
 * mapped action # to the set of intra-token pattern positions.  For
 * variable context patterns add the mapped action # to the last sets
 * for the copy of the pattern and context merely so that the dfa
 * will accept them.
 */
static VOID
completePats()
{
  CONST Index actOffset=	/* Offset for actions in LEAVES(). */
    A_NENTRIES(G.leaves);
  CONST Count nPats= 
    A_NENTRIES(G.topPats);
  Index i;
  for (i= 0; i < nPats; i++) {
    if (TOP_PATS(i).topPatType != EOF_TOP) {
      CONST Index mappedPatN= 
	(G.nIntraTokPats) ? mapAct(i + 1, TOP_PATS(i).topPatType == INTRA_TOP)
	                  : i + 1;
      CONST Index mappedActN= mappedPatN + actOffset;
      ISet lasts= LASTS(i);
      ISetIter lastsIter;
      ISetElement pos;
      assert(lasts != BAD_ISET);
      FOR_EACH_IN_ISET(pos, lasts, lastsIter) {
	addISet(FOLLOWS(pos), mappedActN);
      }
      delISet(lasts); LASTS(i)= BAD_ISET;
      if (TOP_PATS(i).topPatType == INTRA_TOP) {
	addISet(G.intraPos, mappedActN);
      }
      else if (TOP_PATS(i).topPatType == CONTEXT_TOP) {
	Index c= TOP_PATS(i).auxInfo;
	assert(c != NIL);
	if (CONTEXTS(c).isVar) {
	  Index j;
	  for (j= 0; j < 2; j++) {
	    lasts= CONTEXTS(c).lasts[j];
	    assert(lasts != BAD_ISET);
	    FOR_EACH_IN_ISET(pos, lasts, lastsIter) {
	      addISet(FOLLOWS(pos), mappedActN);
	    }
	    delISet(lasts); CONTEXTS(c).lasts[j]= BAD_ISET;
	  } /* for (i= 0; i < 2; i++) */
	} /* if (CONTEXTS(c).isVar) */
      } /* else if (TOP_PATS(i).topPatType == CONTEXT_TOP) */
    } /* if (TOP_PATS(i).topPatType != EOF_TOP) */
  } /* for (i= 0; i < nPats; i++) */
}



/*		      SECTION 2 ENTRY/EXIT ROUTINES			*/

/* Record the fact that we are in section 2. */
VOID
beginSec2Patterns()
{
  INFO1_P->inSec2= TRUE;
  INFO1_P->sec1DefsTop= A_NENTRIES(INFO1_P->defs);
  VOID_RET();
}

/* This routine is called when the parser hits the end of section 2.
 * We ensure that we have a leaf for a sentinel char.  We create
 * a default pattern for the default action and a intra-token pattern 
 * to support yylineno (if requested).  We set up action mapping 
 * (to map action #s when intra-token patterns are used) and complete
 * the patterns by adding the mapped actions to them.  Finally we
 * export the leaves used by the patterns to global variables and
 * delete all the storage occupied by patterns and macros.
 */
VOID
endSec2Patterns()
{
  { /* Allocate a sentinel-char class to ensure that its class is not GC'd. */
    Class sentinelCharClass;
    sentinelCharClass= 
      rawClassToClass(addRawClass(newRawClass(), 
				  optionsP->sentinelChar, 
				  optionsP->sentinelChar));
    newLeaf(sentinelCharClass);
  }
  G.defaultPatN= addDefaultPat(); G.nExtraPats++;
  if (optionsP->doYYLineNo) { addYYLineNoPat(); G.nExtraPats++; }
  initActMaps();
  completePats();
  exportLeaves();
#if DO_TRACE
  if (files.dumpFile) {
    dumpMacros(files.dumpFile);
    dumpPositions(files.dumpFile);
  }
#endif
  terminateInfo1();
  VOID_RET();
}


/*			SPECIAL PSEUDO-ACTIONS.				*/

/* Return the output action for the yylineno action.  0 if yylineno 
 * not requested.
 */
static Index 
getYYLineNoActN()
{
  return optionsP->doYYLineNo ? A_NENTRIES(G.topPats) : 0;
}


/*			     EOF ACTIONS.				*/

/* Return the output EOF action for startState (NIL if none). */
Index
getEOFAct(startState)
  Index startState;
{
  return START_STATES(startState).eofPatActN;
}


/*			OUTPUT PATTERN PARAMETERS.			*/

/* Output parameters determined by this module to outFile.  Return the
 * number of lines output.
 */
Count 
outPatternParams(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  fprintf(outFile, "#define YY_DEFAULT_ACTN %d\n", G.defaultPatN + 1); 
  nLines++;
  fprintf(outFile, "#define YY_HAS_VAR_CONTEXTS %d\n", G.hasVarContextPat); 
  nLines++;
  fprintf(outFile, "#define YY_HAS_INTRA_TOK_PAT %d\n", G.nIntraTokPats > 0); 
  nLines++;
  fprintf(outFile, "#define YY_LINENO_ACTN %d\n", getYYLineNoActN());
  nLines++;
  fprintf(outFile, "#define YY_N_START_STATES %d\n", nStartStates()); 
  nLines++;
  fprintf(outFile, "#define YY_NUM_RULES %d\n", A_NENTRIES(G.topPats) + 2); 
  nLines++;
  fprintf(outFile, "#define YY_START_OF_LINE %d\n", G.hasStartLinePat); 
  nLines++;
  return nLines;
}


/*			     OUTPUT ACTION CASES.			*/

/* Output patAction patActN to outFile.  Return # of lines output.
 * Take care of following situations: If a context pattern then add
 * pre-actions to adjust the context.  If a intra pattern then add
 * pre-actions and post actions.  If the source action # for the next
 * pattern is the same as the current source action # then output a
 * goto to the next action.  Else output the corresponding source
 * action preceeded by a debugging macro and a reject macro (if
 * requested).
 */
Count 
outPatAct(outFile, patActN)
  FILE *outFile;
  Index patActN;
{
  CONST Count nPats= A_NENTRIES(G.topPats) - G.nExtraPats;
  CONST Index patN= patActN - 1;		/* 0-origin pattern #. */
  Count nLines= 0;
  CONST Index srcActN= TOP_PATS(patN).srcActN;
  assert(0 < patActN && patActN <= nPats);
  if (TOP_PATS(patN).topPatType == CONTEXT_TOP) {
    Index cIndex= TOP_PATS(patN).auxInfo;
    if (CONTEXTS(cIndex).isVar) { /* variable context. */
      fprintf(outFile, "\t  YY_VAR_CONTEXT_RESCAN(%d, %d);\n", 
	      CONTEXTS(cIndex).states[0], CONTEXTS(cIndex).states[1]);
      nLines++;
    }
    else if (CONTEXTS(cIndex).len >= 0) { 
      fprintf(outFile, "\t  YY_SET_LENG(%d);\n", CONTEXTS(cIndex).len);
      nLines++;
    }
    else {
      fprintf(outFile, "\t  YY_DEC_LENG(%d);\n", -CONTEXTS(cIndex).len);
      nLines++;
    }
  }
  else if (TOP_PATS(patN).topPatType == INTRA_TOP) {
    fprintf(outFile, "\t  YY_INTRA_PRE(%d);\n", TOP_PATS(patN).auxInfo);
    nLines++;
  }
  fprintf(outFile, "\tYY_DEBUG_MSG(%d);\n", TOP_PATS(patN).defLineN); nLines++;
  if (patN == nPats - 1 || 
      TOP_PATS(patN).srcActN != TOP_PATS(patN + 1).srcActN) {
    if (patN > 0 && TOP_PATS(patN - 1).srcActN == TOP_PATS(patN).srcActN) {
      fprintf(outFile, "  yyLab%d:\n", srcActN); nLines++;
    }
    if (optionsP->doReject) {
      fprintf(outFile, "#undef %s\n", REJECT_MACRO); nLines++;
      fprintf(outFile, "#define %s \\\n", REJECT_MACRO); nLines++;
      nLines+= outRejectMacroBody(outFile, srcActN);
    }
    nLines+= outSrcAction(outFile, srcActN);
    if (TOP_PATS(patN).topPatType == INTRA_TOP) {
      fprintf(outFile, "\t  YY_INTRA_POST();\n"); nLines++;
    }
  }
  else {
    fprintf(outFile, "\tgoto yyLab%d;\n", srcActN); nLines++;
  }
  return nLines;
}



/*				INITIALIZATION.				*/

VOID 
initPatterns()
{
  /* actmaps[] initialized in endSec2Patterns(). */
  initGlobals();
  initInfo1();
  initSS();
  initLeaves();
  initTopPats();
  initContexts();
  initIntra();
  VOID_RET();
}

VOID
terminatePatterns()
{
  /* Info1 terminated in endSec2Patterns(). */
  /* No termination corresponding to initGlobals(). */
  terminateActMaps();
  terminateSS();
  terminateLeaves();
  terminateTopPats();
  terminateIntra();
  terminateContexts();
  VOID_RET();
}

Boolean 
hasStartLinePat()
{
  return G.hasStartLinePat;
}


/*				DUMP POSITIONS				*/
#ifdef DO_TRACE

static VOID
dumpPositions(dumpFile)
  FILE *dumpFile;
{
  CONST Count nSS= nStartStates();
  CONST Count nLeaves= nPatLeaves();
  Index i;
  fprintf(dumpFile, "START-STATE FIRSTS:\n");
  for (i= 0; i < nSS; i++) {
    Index j;
    fprintf(dumpFile, "%2d) %s:\n", i, getIDString(START_STATES(i).textID));
    for (j= 0; j < 2; j++) {
      fprintf(dumpFile, "    firsts[%d]= ", j); 
      fprintISet(dumpFile, START_STATES(i).firsts[j]);
      fputs("\n", dumpFile);
    }
  }
  fprintf(dumpFile, "\nTERMINAL CHARACTER SETS AND FOLLOWS\n");
  fprintf(dumpFile, "# of terminal character sets= %d\n", nLeaves);
  for (i= 0; i < nLeaves; i++) {
    fprintf(dumpFile, "%4d ", i); dumpClass(dumpFile, LEAF_CLASSES(i)); 
    fputs("\n    ", dumpFile);
    fprintISet(dumpFile, LEAF_FOLLOWS(i));
    fputs("\n", dumpFile);
  }
  VOID_RET();
}

#endif /* ifdef DO_TRACE */

/*			      TEST ROUTINE				*/

#if TEST_PATTERNS

/*
Test program for patterns, chrclass, parse and scan.  At one point in 
development, the test program could be compiled and linked using:

gmake -k CFLAGS="-g -Wall" DFLAGS="-DDO_TRACE -DHAVE_CONFIG_H -DTEST_PATTERNS" \
  patterns.o chrclass.o parse.o scan.o comptab.o outvec.o;
gcc patterns.o chrclass.o parse.o scan.o comptab.o outvec.o \
  -L../libzlex -L../libz -lz -lzlex -o patterns

Run as `patterns <LEX_FILE_NAME>'.

*/


#include "zparse.h"

Options options;
CONST Options *CONST optionsP= &options;

Files files;

VOID setOption(Index optionID) { }
Count outRejectMacroBody(FILE *outFile, Index srcActN) { return 0; }


int main(int argc, ConstString argv[])
{
  options.charSetSize= 256;
  files.dumpFile= stdout;
  initError(NULL, errorSrc, lineN, NULL);
  initISet();
  initScan(argc, 1, argv); initChrClass(); initPatterns(); initParse();
  yyparse();
  return 0;
}

#endif /* if TEST_PATTERNS */

