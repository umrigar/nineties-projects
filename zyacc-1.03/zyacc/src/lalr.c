/*

File:	 lalr.c
Purpose: Grammar analysis and LALR parser generation.

Last Update Time-stamp: "97/07/30 18:36:58 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifdef DEBUG_LALR 	/* Define to check run-time assertions. */
#undef NDEBUG
#endif

#include "attribs.h"
#include "comptab.h"
#include "error.h"
#include "gram.h"
#include "lalr.h"
#include "options.h"
#include "outvec.h"
#include "scan.h"
#include "xset.h"

#include "area.h"
#include "autobuf.h"
#include "iset.h"
#include "memalloc.h"
#include "zutil.h"

/* System includes via port.h. */

/*			  	GLOBALS.				*/

static struct {
  Count2 nTerms;	/* # of terminals. */
  Count2 nNonTerms;	/* # of non-terminals. */
  Count2 nRules;	/* # of rules. */
  Count2 nRHSSyms;	/* Total # of RHS symbols. */
  Count2 nStates;	/* # of LR0 states. */
  Count2 testOffset;	/* nRules + nStates: used for coding test post-acts. */
  Count2 maxNSI;	/* Max. # of non-terminal state items for any state. */
  Index2 errTerm;	/* Token # for error token. */
  Count2 nTests;	/* # of %tests used. */
  Count2 nExpected;	/* # of expected conflicts. */
} globs;

#define G globs

/*			STATIC PROTOTYPES.				*/

struct NameWithLen;
struct SIList;
struct Digraph;
typedef Index2 Node;
typedef Index NodeI;

typedef Count (*SuccFnP) PROTO((NodeI n, Node succs[], Size maxSucc));
typedef ISet (*FValFnP) PROTO((NodeI x));
typedef VOID (*SCCErrFnP) PROTO((NodeI z, struct Digraph *dP));

typedef Index2 PreAct;
typedef Index PreActX;
typedef Index2 PostAct;
typedef Index PostActX;

static VOID initGlobs PROTO((VOID_ARGS));
static Boolean isNullableRestRule PROTO((Index itemN, Boolean isNullable[]));
static VOID computeNullables PROTO((VOID_ARGS));
static VOID computeRestRuleNullables PROTO((VOID_ARGS));
static VOID initNonTermTrans PROTO((CONST Count nNonTerms));
static VOID initStateItems PROTO((VOID_ARGS));
static Index newStateItem PROTO((Index siType, Index state, Index item));
static VOID initClosureNonTerms PROTO((Count nNonTerms));
static VOID delClosureNonTerms PROTO((VOID_ARGS));
static VOID doClosure PROTO((Index state, Index ntLo, Index ntHi));
static VOID initStatesHash PROTO((CONST Count nTerms, CONST Count nNonTerms));
static VOID delStatesHash PROTO((VOID_ARGS));
static Index newState PROTO((Index symI, CONST Index2 start[], Count hashSumI));
static Index consState PROTO((Index symI, CONST struct SIList *siListBase, 
			      Index siListIndex,  Count hashSumI));
static Index initialState PROTO((VOID_ARGS));
static Index getState PROTO((Index symI, CONST struct SIList *siListBase, 
			     Index siListIndex, Count hashSumI));
static Index getStateAccessSym PROTO((Index s));
static VOID computeLR0 PROTO((VOID_ARGS));
static VOID initDigraph PROTO((Count nNodes, SuccFnP succ, FValFnP f, 
			       SCCErrFnP sccErr, struct Digraph *dP));
static VOID freeDigraph PROTO((struct Digraph *digraphP));
static VOID digraphTraverse PROTO((NodeI x, struct Digraph *dP));
static VOID digraphAll PROTO((struct Digraph *dP));
static ISet drSet PROTO((Index ntTrans));
static Count readsSucc PROTO((NodeI ntTrans, Node succs[], Size succsSize));
static ISet readsF PROTO((NodeI ntTrans));
static VOID computeReads PROTO((VOID_ARGS));
static VOID initSLR PROTO((VOID_ARGS));
static Count slrSucc PROTO((NodeI a, Node succs[], Size succsSize));
static ISet slrF PROTO((NodeI a));
static VOID computeFollows PROTO((VOID_ARGS));
static Boolean hasPath PROTO((Index s1, Index s2, Index itemLo, Index itemHi));
static Count includesSucc PROTO((NodeI ntTrans, Node succs[], Size succsSize));
static ISet lalrF PROTO((NodeI ntTrans));
static ISet ntTransFollowSet PROTO((Index ntTrans));
static ISet lalrLook PROTO((Index xTrans));
static Index newAction PROTO((Index term, PostActX postAct));
static VOID printAction PROTO((FILE *outFile, PostActX postAct));
static VOID initActLists PROTO((Area *actListsP));
static VOID resetActLists PROTO((Area *actListsP));
static VOID addActList PROTO((Area *actListsP, Index term, PreActX preAct));
static VOID delActList PROTO((Area *actListsP, Index term, PreActX preAct));
static Index nTestSets PROTO((VOID_ARGS));
static Index testSetsTest PROTO((Index t));
static Index testSetsAct PROTO((Index t));
static Index getTestSetN PROTO((Area *actListsP, Index start));
static VOID initTests PROTO((VOID_ARGS));
static ISet slrToLALR PROTO((Area *actListsP, Index reduceX));
static Index resolveSRConflict PROTO((Area *actListsP, Index term,
				      Index shiftPreAct, Index reducePreAct,
				      Boolean *doDefaultP));
static Index resolveRRConflict PROTO((Area *actListsP, Index term, 
				      Index reducePreAct1, 
				      Index reducePreAct2));
static Index resolveConflictsLo PROTO((Area *actListsP, Index term,
				       Index shiftPreAct, Index reducePreAct,
				       Index resolution, Boolean *doDefaultP));
static VOID resolveConflicts PROTO((Area *actListsP, Index term,
				    Boolean *doDefaultP));
static VOID warnConflicts PROTO((VOID_ARGS));
static Index chooseDefaultRuleN PROTO((Area *actListsP, ISet terms));
static VOID setActions PROTO((Area *actListsP, ISet terms, 
			      Index defaultRuleN));
static VOID buildActions PROTO((VOID_ARGS));
static VOID initLALR PROTO((VOID_ARGS));

static VOID buildGotos PROTO((VOID_ARGS));
static int baseCmp PROTO((CONST VOIDP p1, CONST VOIDP p2));
static Index adjustBase PROTO((Index2 *CONST tBase, 
			       CONST Index2 *CONST tOrder, 
			       Count tBaseNEntries, Index2 *nBase, 
			       CONST Index2 *CONST nOrder,
			       Count nBaseNEntries, Index2 *fillValP));
static int tTableCmp PROTO((CONST VOIDP p1, CONST VOIDP p2));
static int nTableCmp PROTO((CONST VOIDP p1, CONST VOIDP p2));
static Index2 *adjustCheck PROTO((Index2 *oldCheck, CONST Size currentCheckSize,
				  CONST Size newCheckSize));
static VOID compressTables PROTO((VOID_ARGS));
static ConstString quote PROTO((ConstString s));
static ConstString getQuotedTermName PROTO((Index term));
static Index ruleRHSLo PROTO((Index ruleN));
static Index ruleLen PROTO((Index ruleN));
static Index getRuleLHS PROTO((Index ruleN));
static Index getRHSSymLocal PROTO((Index rhsIndex));
static VOID printHTMLString PROTO((FILE *outFile, ConstString s));
static VOID printHTMLDef1 PROTO((FILE *outFile, ConstString textFmt, 
				 ConstString nameFmt, Index refNum, 
				 struct NameWithLen *nameWithLenP));
static VOID printHTMLRef1 PROTO((FILE *outFile, Index refdFileIndex,
				 ConstString textFmt, 
				 ConstString nameFmt, Index refNum, 
				 struct NameWithLen *nameWithLenP));
static VOID printHTMLDef2 PROTO((FILE *outFile, ConstString textFmt, 
				 ConstString nameFmt, 
				 Index refNum1, Index refNum2, 
				 struct NameWithLen *nameWithLenP));
static VOID printHTMLRef2 PROTO((FILE *outFile, Index refdFileIndex,
				 ConstString textFmt, 
				 ConstString nameFmt, 
				 Index refNum1, Index refNum2, 
				 struct NameWithLen *nameWithLenP));
static VOID printHTMLHeading PROTO((FILE *outFile, ConstString heading, 
				    Index level, 
				    struct NameWithLen *nameWithLenP));
static VOID printHTMLRule PROTO((FILE *outFile, Index ruleN, Index itemN, 
				 BooleanX isDef, 
				 struct NameWithLen *nameWithLenP));
static VOID printHTMLAllRules PROTO((FILE *outFile, 
				     struct NameWithLen *nameWithLenP));
static VOID  printHTMLAction PROTO((FILE *outFile, Index postAct, 
				    Index state, 
				    struct NameWithLen *nameWithLenP));
static VOID printHTMLTestSet PROTO((FILE *outFile, Index testsIndex, 
				    Index state, 
				    struct NameWithLen *nameWithLenP));
static VOID printHTMLStateActions PROTO((FILE *outFile, Index s, 
					 struct NameWithLen *nameWithLenP));
static VOID printHTMLStateGotos PROTO((FILE *outFile, Index s, 
				       struct NameWithLen *nameWithLenP));
static VOID printHTMLAllStates PROTO((FILE *outFile, 
				      BooleanX includeClosureItems,
				      struct NameWithLen *nameWithLenP));
static VOID printHTMLAllStatesClosure 
  PROTO((FILE *outFile, struct NameWithLen *nameWithLenP));
static VOID printHTMLAllStatesNoClosure 
  PROTO((FILE *outFile, struct NameWithLen *nameWithLenP));
static VOID printHTMLAccessedStates PROTO((FILE *outFile, 
					   struct NameWithLen *nameWithLenP));
static VOID printHTMLReduceLooks PROTO((FILE *outFile, 
				      struct NameWithLen *nameWithLenP));
static VOID printHTMLTests PROTO((FILE *outFile, 
				  struct NameWithLen *nameWithLenP));
static VOID printHTMLUserActs PROTO((FILE *outFile, 
				     struct NameWithLen *nameWithLenP));
static VOID printHTMLMain PROTO((FILE *outFile, 
				 struct NameWithLen *nameWithLenP));
static VOID printHTMLConflicts PROTO((FILE *outFile, 
				      struct NameWithLen *nameWithLenP));





static VOID 
initGlobs()
{
  G.nTerms= gramNTerms(); 
  G.nNonTerms= gramNNonTerms();
  G.nRules= gramNRules();
  G.nRHSSyms= gramNRHSSyms();
  G.errTerm= getErrorTerm();
  G.nTests= nTests();
  VOID_RET();
}

/*			NULLABLE COMPUTATION.				*/

/* Can reduce memory usage by making isNullable, inRuleStk and
 * restItemsNullable ISets.  Or don't precompute restItemsNullable and
 * don't destroy isNullable and use isNullableRestRule().
 */

/* Returns TRUE if rule from itemN onwards is nullable (according to 
 * isNullable[]. 
 */
static Boolean 
isNullableRestRule(itemN, isNullable)
  Index itemN;
  Boolean isNullable[];
{
  Index i;
  for (i= itemN; SYM_TYPE(RHS_SYM(i)) != RULE_SYM; i++) {
    if (SYM_TYPE(RHS_SYM(i)) == TERM_SYM 
	|| !isNullable[SYM_NUM(RHS_SYM(i))])
      return FALSE;
  }
  return TRUE;
}

static Boolean *isNullable;

#define IS_NULLABLE(nonTerm)	(isNullable[nonTerm])

/* For each non-terminal n set isNullable[n] to TRUE iff n is nullable.
 * Algorithm uses a stack of rules being considered.  Initially every rule
 * is considered.  If a considered rule for non-terminal n is nullable 
 * according to the current isNullable[], then all rules with n in their 
 * RHS but without any RHS terminals will also be considered if the 
 * nullability of their LHS non-terminal is not yet known.
 */
static VOID 
computeNullables()
{
  CONST Count nRules= G.nRules;
  Index2 *CONST ruleStk= (Index2 *) MALLOC(nRules * sizeof(Index2));
  Boolean *CONST inRuleStk= (Boolean *) MALLOC(nRules * sizeof(Boolean));
  Index ruleStkSP= nRules;	/* Points 1 beyond top-entry in ruleStk. */
  Index i;
  for (i= 0; i < nRules; i++) { /* Initially we'll consider every rule. */
    ruleStk[i]= i; inRuleStk[i]= TRUE;
  }
  assert(ruleStkSP > 0);
  do { /* while ruleStk[] is not empty */
    CONST Index ruleN= ruleStk[--ruleStkSP]; /* ruleN popped off ruleStk. */
    CONST Index n= RULE_NON_TERM(ruleN); /* n is ruleN LHS non-term. */	
    inRuleStk[ruleN]= FALSE;	/* ruleN no longer in ruleStk. */
    if (!isNullable[n] 
	&& isNullableRestRule(RULE_RHS_LO(ruleN), isNullable)) {
      isNullable[n]= TRUE;  /* n is a new nullable non-term. */
      for (i= nonTermRHSIndex(n); i != NIL; i= RHS_SUCC(i)) {
	/* i contains RHS indexes of non-terminal n. */
	Index r= getItemRuleN(i);	/* r is rule which contains item i. */
	if (!inRuleStk[r] && !isNullable[RULE_NON_TERM(r)]) {
	  CONST Index hi= RULE_RHS_HI(r);
	  Index j;
	  for (j= RULE_RHS_LO(r); j < hi; j++) {
	    if (SYM_TYPE(RHS_SYM(j)) == TERM_SYM) break;
	  }
	  if (j == hi) { /* Rule r is potentially nullable - push it. */
	    ruleStk[ruleStkSP++]= r; inRuleStk[r]= TRUE;
	  }
	}
      } /* for (i= nonTermRHSIndex(n); ...) */
    } /* if (!isNullable[n] && ... ) */
  } while (ruleStkSP > 0); /* while ruleStk[] is not empty.  */
  FREE(ruleStk); FREE(inRuleStk);
  VOID_RET();
}

static Boolean *restRuleNullables;

#define REST_RULE_NULLABLE(i)	(restRuleNullables[i])

static VOID 
computeRestRuleNullables()
{
  
  Index i;
  CONST Count nNonTerms= G.nNonTerms;
  isNullable= (Boolean *) CALLOC(nNonTerms, sizeof(Boolean));
  computeNullables();
  /* Fill in restItemsNullable[]. */
  restRuleNullables= (Boolean *) MALLOC(G.nRHSSyms * sizeof(Boolean));
  i= G.nRHSSyms;
  while (i > 0) {
    assert(SYM_TYPE(RHS_SYM(i - 1)) == RULE_SYM);
    while (i > 0 && (SYM_TYPE(RHS_SYM(i - 1)) == RULE_SYM ||
	   ( SYM_TYPE(RHS_SYM(i - 1)) == NON_TERM_SYM 
	     && isNullable[SYM_NUM(RHS_SYM(i - 1))]))) {
      restRuleNullables[--i]= TRUE;
    }
    /* some cc's get upset if assert's arguments are not within 1 line! */
    assert(SYM_TYPE(RHS_SYM(i - 1)) == TERM_SYM || ( SYM_TYPE(RHS_SYM(i - 1)) == NON_TERM_SYM &&  !isNullable[SYM_NUM(RHS_SYM(i - 1))] ));
    while (i > 0 && SYM_TYPE(RHS_SYM(i - 1)) != RULE_SYM) {
      restRuleNullables[--i]= FALSE;
    }
  }
  VOID_RET();
}

#ifdef TEST_NULL

#include "scan.h"

int main PROTO((int argc, char **argv));

int 
main(argc, char **argv)
  int argc; 
  char **argv;
{
  Index i;
  CONST Count nRules= G.nRules;
  initScan(argc > 1 ? argv[1] : NULL);
  doGram();
  initGlobs();
  computeRestRuleNullables();
  for (i= 0; i < nRules; i++) {
    Index j;
    printf("%3d: %s--> ", i, getNonTermString(RULE_NON_TERM(i)));
    for (j= RULE_RHS_LO(i); j < RULE_RHS_HI(i); j++) {
      printf("%s%c ", SYM_TYPE(RHS_SYM(j)) == TERM_SYM 
	              ? getTermString(SYM_NUM(RHS_SYM(j)))
	              : getNonTermString(SYM_NUM(RHS_SYM(j))),
	     REST_RULE_NULLABLE(j) ? '+' : '-');
    }
    assert(SYM_TYPE(RHS_SYM(j)) == RULE_SYM);
    printf("%%Rule %d%c\n", SYM_NUM(RHS_SYM(j)), 
	   REST_RULE_NULLABLE(j) ? '+' : '-');
  }
  return 0;
}

#endif /* ifdef TEST_NULL */



/*                         LR0 MACHINE STATES.                          */

/*
 
LR0 states consist of set of items.  We refer to an item associated
with a state as a state-item (typedef'd to StateItem).

A state-item can be of three types:

A terminal item: The symbol to the right of the dot is terminal.

A non-terminal item: The symbol to the right of the dot is a non-terminal.
The state-item corresponds to a non-terminal transition.

A reducing item: The dot is at the extreme right of the rule.

In addition each item can also be classified as a closure item (the dot
is at the extreme left of the rule RHS) or a non-closure item (there is
at least one symbol to the left of the dot).

A non-reducing state-item represents the transition (state, item.sym)
where state is the state to which the state-item belongs, and item.sym
is the symbol to the right of the dot.  If item.sym is a non-terminal,
then the transition is a non-terminal transition.

Every state-item corresponding to a non-terminal transition is linked
together in a separate list for each non-terminal.  This facilitates
SLR computation.

Since we store non-terminal (N) state-items in a separate array from
other (X) state-items, many data structures use a 2 element vector:
element 0 always refers to the other state-items, whereas element 1
always refers to the non-terminal state-items.

Items within a state are ordered by an implementation order induced by
the closure operation.  Hence we do not bother to sort items before 
identifying states by comparing items.  Since the predecessor of each item 
is unique, a simple inductive argument suffices to show that the items
within a state are always ordered using this implementation order.

The implementation order has one aspect which is used in the code: 
non-closure items always precede closure items in a state.

*/



/*				STATE ITEMS.				*/
	
typedef enum {
  X_SI= 0,		/* Terminal and reducing state-items. */
  N_SI= 1,		/* Non-terminal state items. */
  LIMIT_SI= 2		/* 1 beyond last possible state-item. */
} SIType;

typedef struct {
  Index2 state;		/* The state this item belongs to. */
  Index2 item;		/* Index in RHSs. */
  union {
    Index2 succ;	/* Succ. state for a term or non-term state-item. */
    ISet lalrLook;	/* LALR lookahead set for a reducing state-item. 
			 * BAD_ISET if SLR-lookahead set used. 
			 */
  } u;
  union {
    Index2 nonTermChain;/* Chain all state-items for same non-term. */
    Index2 reduceChain;	/* If reduction, then chain reducts for LHS nonterm. */
  } u1;
} StateItem;

/* stateItems is defined as an array of LIMIT_SI StateItem Areas.
 * This facilitates easy processing via loops.  However, when using
 * the A_NENTRIES operation to get the current limit of an Area, the
 * prototype used should be A_NENTRIES(stateItems[SIType]).  When using
 * STATE_ITEMS to access a StateItem, the prototype used should be
 * STATE_ITEMS(SIType)[Index].  
 *
 * RESIST THE TEMPTATION TO CACHE STATE_ITEMS(SIType): the base of
 * the array could be relocated!
 */


static Area stateItems[LIMIT_SI];
#define STATE_ITEMS(i, j) A_ACCESS(stateItems[i], StateItem, j)
#define LG2_SI_INC 10	/* Increment used for growing state-item areas. */


typedef struct {	/* Extra info associated with non-term state-items. */
  ISet readFollow;	/* 1st computed as Reads and then follow set. */
} NSIInfo;	  


/* This area is maintained in parallel with STATE_ITEMS(1, *). */
static AREA (nsiInfo, NSIInfo, LG2_SI_INC, UINT2_MAX);
#define NSI_INFO(i) A_ACCESS(nsiInfo, NSIInfo, i)

/* The code uses the fact that if (p, i) is a non-terminal state-item
 * for some non-terminal nonTerm (corresponding to item i), and (p, j)
 * is another state-item for the same state p and same non-terminal
 * nonTerm (corresponding to item j) then the intervening chain
 * connecting these state-items in the nonTermChain for nonTerm will
 * not have any state-items (q, k) with q != p; This implies that all
 * non-term items for the same state are adjacent on the nonTermChain
 * for a particular non-term. 
 */

typedef struct {	/* To chain all non-terminal trans for a non-term. */
  Index2 chain;		/* Transitions on this non-terminal. */
  Index2 reduceChain;	/* Chain all reductions for this non-terminal. */
} NonTermTrans;

static NonTermTrans *nonTermTrans; /* One entry for every non-term. */

static VOID 
initNonTermTrans(nNonTerms)
  CONST Count nNonTerms;
{
  Index i;
  nonTermTrans= (NonTermTrans *)MALLOC(nNonTerms * sizeof(NonTermTrans));
  for (i= 0; i < nNonTerms; i++) {
    nonTermTrans[i].chain= nonTermTrans[i].reduceChain= NIL;
  }
  VOID_RET();
}

static VOID 
initStateItems()
{
  SIType t;
  for (t= X_SI; t < LIMIT_SI; t++) 
    INIT_AREA(stateItems[t], StateItem, LG2_SI_INC, UINT2_MAX);
  VOID_RET();
}

/* This routine creates a new state item of the specified type.  It
 * fills in all the fields, and if the type is N_SI, then it will
 * allocate a new NSI_INFO[] structure and link into the nonTermTrans 
 * chain.
 */
static Index 
newStateItem(siType, state, item)
  Index siType;
  Index state;
  Index item;
{
  SIType type= siType;
  Index i= A_NEXT(stateItems[type]);
  StateItem *CONST p= &STATE_ITEMS(type, i);
  assert(type == X_SI ? !IS_NON_TERM_ITEM(item) : IS_NON_TERM_ITEM(item));
  p->item= item;  p->state= state;
  if (IS_REDUCING_ITEM(item)) {
    CONST Index lhsNonTerm= RULE_NON_TERM(getItemRuleN(item));
    p->u.lalrLook= BAD_ISET;
    p->u1.reduceChain= nonTermTrans[lhsNonTerm].reduceChain;
    nonTermTrans[lhsNonTerm].reduceChain= i;
  }  
  else {
    p->u.succ= NIL;
  }
  if (type == N_SI) {
    Index j= A_NEXT(nsiInfo);
    Index nonTerm= SYM_NUM(RHS_SYM(item));
    NSIInfo *CONST q= &NSI_INFO(j);
    assert(i == j);
    q->readFollow= BAD_ISET;
    /* some cc's get upset if assert's arguments are not within 1 line! */    
    assert(nonTermTrans[nonTerm].chain == NIL || nonTermTrans[nonTerm].chain < i);
    p->u1.nonTermChain= nonTermTrans[nonTerm].chain;
    nonTermTrans[nonTerm].chain= i;  
  }
  return i;
}

/*			CLOSURE OPERATION.				*/

/* 

We could precompute all the non-terminals induced into a closure by
adding a particular closure non-terminal.  This could be done using
digraph on the relation left-corner-connected where non-terminal A is
left-corner-connected to non-terminal B if there is a rule A-> B
alpha.  However, this does not appear to be worthwhile, since the
current implementation drags out the induced non-terminals relatively
rapidly. 

*/

/* Non-terminals whose rules are being added by the closure operation. */
static XSet closureNonTerms;

static VOID 
initClosureNonTerms(nNonTerms)
  Count nNonTerms;
{
  INIT_XSET(closureNonTerms, nNonTerms);
  VOID_RET();
}

static VOID 
delClosureNonTerms() 
{
  DEL_XSET(closureNonTerms);
  VOID_RET();
}

/* Add closure items for items STATE_ITEMS(N_SI, ntLo..ntHi) to end
 * of STATE_ITEMS(N_SI, *) and STATE_ITEMS(X_SI, *).  We use the XSet
 * closureNonTerms to accumulate the non-terminals whose RHSs must
 * be added.  We first loop thru the non-terminal state items
 * [ntLo...ntHi) adding non-terminals to the above XSet.  We then
 * iterate thru the elements of the XSet, adding their rule RHSs to
 * STATE_ITEMS(N_SI, *) or STATE_ITEMS(X_SI, *) depending on whether the
 * rule starts with a non-term or not.  As we do this iteration, if we
 * encounter a closure rule which starts with a non-term not in
 * closureNonTerms, we add that into the XSet to be iterated over on
 * a subsequent iteration.
 * This code depends on the fact that rules for a non-terminal are
 * ordered so that rules whose RHSs start with a non-terminal always
 * precedes other rules.
 */
static VOID 
doClosure(state, ntLo, ntHi)
  Index state;
  Index ntLo;
  Index ntHi;
{
  Index i;
  Index nonTerm;
  assert(ntHi == A_NENTRIES(stateItems[N_SI]));
  assert(SIZE_XSET(closureNonTerms) == 0);
  if (ntHi == ntLo) VOID_RET();		/* Nothing to do. */
  for (i= ntLo; i < ntHi; i++) { /* Add non-terms directly in closure. */
    Index item= STATE_ITEMS(N_SI, i).item;
    assert(!IS_CLOSURE_ITEM(item));
    assert(IS_NON_TERM_ITEM(item));
    nonTerm= SYM_NUM(RHS_SYM(item));
    ADD_XSET(closureNonTerms, nonTerm);
  }
  FOR_EACH_IN_XSET(i, nonTerm, closureNonTerms) { /* Moving target. */
    Index j;
    for (j= nonTermRHSRule(nonTerm); j != NIL; j= RHS_SUCC(j)) {
      /* Loop thru those rules whose RHSs start with a non-terminal. */
      CONST Index r= SYM_NUM(RHS_SYM(j));
      CONST Index item= RULE_RHS_LO(r);
      assert(SYM_TYPE(RHS_SYM(j)) == RULE_SYM);
      assert(IS_CLOSURE_ITEM(item));
      if (!IS_NON_TERM_ITEM(item)) break;
      ADD_XSET(closureNonTerms, SYM_NUM(RHS_SYM(item)));
    }
  }
  SORT_XSET(closureNonTerms); /* This sorting orders all state-items. */
  FOR_EACH_IN_XSET(i, nonTerm, closureNonTerms) {
    Index j;
    for (j= nonTermRHSRule(nonTerm); j != NIL; j= RHS_SUCC(j)) {
      /* Add closure rules. */
      CONST Index r= SYM_NUM(RHS_SYM(j));
      CONST Index item= RULE_RHS_LO(r);
      assert(SYM_TYPE(RHS_SYM(j)) == RULE_SYM);
      assert(IS_CLOSURE_ITEM(item));
      /* Create a new entry in STATE_ITEMS(N_SI). */
      newStateItem(SYM_TYPE(RHS_SYM(item)) == NON_TERM_SYM ? N_SI : X_SI, 
		   state, item);
    }
  }
  EMPTY_XSET(closureNonTerms);
  VOID_RET();
}


/*			    LR0 STATES.					*/

typedef struct {
  Index2 itemsLo[LIMIT_SI];	/* Index in STATE_ITEMS(). */
  Index2 itemsHi[LIMIT_SI];	/* 1 beyond last item in STATE_ITEMS(). */
  Index2 succ;			/* Link to next state with same access-sym. */
  Count2 hashSum;		/* Sum of all non-closure items in state. */
  Count2 nConflicts;		/* Number of conflicts in the state. */
  Index2 actionsLo;		/* Index of first action in ACTIONS[]. */
  Index2 actionsHi;		/* 1 beyond index of last action ACTIONS[]. */
  Index1 lookStatus;		/* LookStatus */
} State;

static AREA (states, State, 8, UINT2_MAX);
#define STATES(i) A_ACCESS(states, State, i)

/* Each state is accessed by a unique terminal or non-terminal accessing
 * symbol.  This accessing symbol is used as a hash-function when searching
 * for a state.
 */
static Index2 *statesHash[2];

static VOID 
initStatesHash(nTerms, nNonTerms)
  CONST Count nTerms;
  CONST Count nNonTerms;
{
  Index i;
  CONST Count n= nTerms + nNonTerms;
  statesHash[0]= (Index2 *)MALLOC(n * sizeof(Index2));
  for (i= 0; i < n; i++) statesHash[0][i]= NIL;
  statesHash[1]= &statesHash[0][nTerms];
  VOID_RET();
}

static VOID 
delStatesHash()
{
  FREE(statesHash[0]);
  VOID_RET();
}

typedef struct SIList {
  SIType type;	/* The type of state-item. */
  Index2 index;	/* Index within STATE_ITEMS(type). */
  Index2 succ;	/* Index of the next entry on this list. */
} SIList;

/* Create and return a new state, linked into the hash-entry.  Initialize
 * all fields.
 */
static Index 
newState(symI, start, hashSumI)
  Index symI;
  CONST Index2 start[];
  Count hashSumI;
{
  Sym sym= (Sym)symI;
  Count2 hashSum= (Count2)hashSumI;
  Index s= A_NEXT(states);
  Index2 *hP= &statesHash[SYM_TYPE(sym) == NON_TERM_SYM][SYM_NUM(sym)];
  SIType t;
  for (t= X_SI; t < LIMIT_SI; t++) {
    STATES(s).itemsLo[t]= start[t];  
    STATES(s).itemsHi[t]= A_NENTRIES(stateItems[t]);
  }
  STATES(s).succ= *hP; *hP= s; STATES(s).hashSum= hashSum;
  STATES(s).nConflicts= 0; 
  STATES(s).actionsLo= STATES(s).actionsHi= NIL;
  { CONST Count nNSI= STATES(s).itemsHi[N_SI] - STATES(s).itemsLo[N_SI];
    if (nNSI > G.maxNSI) G.maxNSI= nNSI;
  }
  return s;
}

/* Compute the closure of the items in in siListBase starting at
 * siListIndex.  Return a new state with transitions whose lower bound
 * is given by the entry A_NENTRIES of stateItems[type] and upper bounds
 * by the exit A_NENTRIES of stateItems[type].  Set the successor of all the
 * transitions in the siList to the new state.
 */
static Index 
consState(symI, siListBase, siListIndex, hashSumI)
  Index symI;
  CONST SIList *siListBase;
  Index siListIndex;
  Count hashSumI;
{
  Sym sym= (Sym)symI;
  Count2 hashSum= (Count2)hashSumI;
  Index s0= A_NENTRIES(states);
  Index s;
  Index2 start[LIMIT_SI];
  Index i;
  start[X_SI]= A_NENTRIES(stateItems[X_SI]); 
  start[N_SI]= A_NENTRIES(stateItems[N_SI]);
  for (i= siListIndex; i != NIL; i= siListBase[i].succ) {
    CONST SIList *CONST p= &siListBase[i];
    CONST Index item1= STATE_ITEMS(p->type, p->index).item + 1;
    STATE_ITEMS(p->type, p->index).u.succ= s0;
    newStateItem(SYM_TYPE(RHS_SYM(item1)) == NON_TERM_SYM ? N_SI : X_SI,
		 s0, item1);
  }
  doClosure(s0, start[N_SI], A_NENTRIES(stateItems[N_SI]));
  s= newState(sym, start, hashSum);
  assert(s == s0);
  return s;
}

/* Create the initial state for the initial item (0). */
static Index 
initialState()
{
  CONST Index nLo= A_NENTRIES(stateItems[N_SI]);
  Index2 start[LIMIT_SI];
  Index s;
  start[X_SI]= 0; start[N_SI]= 0;
  assert(nLo == 0);
  newStateItem(N_SI, 0, 0);
  doClosure(0, nLo, A_NENTRIES(stateItems[N_SI]));
  s= newState(MAKE_SYM(NON_TERM_SYM, 0), start, 0);
  assert(s == 0);
  return s;
}

#ifdef TEST_LR0
static Count oldStates= 0;
#endif

/* Get the state for the successor (+ 1) of the state-items specified by
 * siList. If such a state does not exist, create it.  Set the successors 
 * for the state-items in siList to the returned state.
 */
static Index 
getState(symI, siListBase, siListIndex, hashSumI)
  Index symI;
  CONST struct SIList *siListBase;
  Index siListIndex;
  Count hashSumI;
{
  Sym sym= (Sym)symI;
  Count2 hashSum= (Count2)hashSumI;
  Index s;
  for (s= statesHash[SYM_TYPE(sym) == NON_TERM_SYM][SYM_NUM(sym)];
       s != NIL;
       s= STATES(s).succ) {
    if (STATES(s).hashSum == hashSum) { /* Possible match. */
      Index i;
      SIType t;
      Index sIndex[LIMIT_SI];
      Index hi[LIMIT_SI];
      for (t= X_SI; t < LIMIT_SI; t++) {
	sIndex[t]= STATES(s).itemsLo[t]; hi[t]= STATES(s).itemsHi[t];
      }
      for (i= siListIndex; i != NIL; i= siListBase[i].succ) {
	CONST SIList *CONST p= &siListBase[i];
	CONST Index item1= STATE_ITEMS(p->type, p->index).item + 1;
	CONST SIType t= SYM_TYPE(RHS_SYM(item1)) == NON_TERM_SYM ? N_SI : X_SI;
	if (sIndex[t] >= hi[t] || 
	    STATE_ITEMS(t, sIndex[t]).item != item1) break;
	STATE_ITEMS(p->type, p->index).u.succ= s; /* Assume success. */
	sIndex[t]++;
      }
      if (i == NIL) {
	SIType t;
	for (t= X_SI; t < LIMIT_SI; t++) {
	  if (sIndex[t] != STATES(s).itemsHi[t] && 
	      !IS_CLOSURE_ITEM(STATE_ITEMS(t, sIndex[t]).item))
	    break;
	}
	if (t == LIMIT_SI) break; /* found. */
      }
    } /* if (STATES(s).hashSum == hashSum) */
  } /* for (s= statesHash[SYM_TYPE(sym) == NON_TERM_SYM][ ...) */
#ifdef TEST_LR0
  oldStates+= (s != NIL);
#endif
  if (s == NIL) { /* Unsuccessful search; create a new state. */  
    s= consState(sym, siListBase, siListIndex, hashSum);
  }
  return s;
}

/* Return the non-term items for state.  cursorP is a cursor returned
 * by this routine so that on successive calls successive items are
 * returned.  For the first call, *cursorP should be passed in as NIL.
 * Non-closure items are returned before closure items.  Otherwise
 * there is no restriction on the returned items.  After all the items
 * have been returned the next call returns NIL.
 */
Index 
nextNonTermItem(state, cursorP)
  Index state;
  Index *cursorP;
{
  Index item;
  assert(state < A_NENTRIES(states));
  if (*cursorP == NIL) { *cursorP= STATES(state).itemsLo[N_SI]; }
  if (*cursorP >= STATES(state).itemsHi[N_SI]) return NIL;
  assert(STATE_ITEMS(N_SI, *cursorP).state == state);
  item= STATE_ITEMS(N_SI, *cursorP).item;
  (*cursorP)++;
  return item;
}

Count 
lalrNStates()
{
  return A_NENTRIES(states);
}

/* Return the max. # of non-terminal state items for any state. */     
Count 
maxNonTermItems()
{
  return G.maxNSI;
}

/* Return the access sym for state s. */
static Index 
getStateAccessSym(s)
  Index s;
{
  if (s == 0) {	/* Return "$S". */
    return MAKE_SYM(NON_TERM_SYM, 0);
  }
  else {
    SIType t;
    for (t= X_SI; t < LIMIT_SI; t++) {
      CONST Index i= STATES(s).itemsLo[t];
      Index item= STATE_ITEMS(t, i).item;
      if (!IS_CLOSURE_ITEM(item)) return RHS_SYM(item - 1); 
    }
    INTERNAL_ERROR();
    return 0;
  }
}


/*			STATE LOOKAHEAD CLASSIFICATION	 		*/

/*

Each state is classified as a LOOK_0, a LOOK_1 or a LOOK_INDETERMINATE,
depending on whether or not the action in that state requires a
lookahead.

Similary, each nonterminal is classified as LOOK_0 if *all* the rules for
that nonterminal are reduced only in LOOK_0 states, as LOOK_1 if *all* the
rules for that nonterminal are reduced only in LOOK_1 states; as
LOOK_INDETERMINATE otherwise.

The detailed classification works as follows:

If the total number of terminal and reducing items in a state is > 1, then
it is always classified as a LOOK_1.

If the state a single reducing item then its classification depends on
the type of its accessing symbol.  If it is accessed by a terminal
symbol then it is always classified as LOOK_0.  If it is accessed by a
non-terminal symbol then its classification is that of its accessing
symbol.

*/


typedef enum {
  LOOK_UNKNOWN,		/* not computed */
  LOOK_0,		/* never requires a lookahead */
  LOOK_1,		/* always requires a lookahead */
  LOOK_INDETERMINATE,	/* requires a lookahead, but not in all situations */
  LOOK_IS_COMPUTING	/* currently computing lookahead; prevents looping */
} LookStatus;

static LookStatus
getStateLookStatus(state)
  Index state;
{
  State *CONST sP= &STATES(state);
  Index lookStatus= sP->lookStatus;
  if (lookStatus == LOOK_IS_COMPUTING) {
    lookStatus= LOOK_0;
  }
  else if (lookStatus == LOOK_UNKNOWN) {
    Count nXSI= sP->itemsHi[X_SI] - sP->itemsLo[X_SI];
    if (nXSI > 1) {
      lookStatus= LOOK_1;
    }
    else if (nXSI == 1 && SYM_TYPE(getStateAccessSym(state)) == TERM_SYM) {
      lookStatus= LOOK_0;
    }
    else if (nXSI == 1) {
      CONST Sym accessSym= getStateAccessSym(state);
      CONST Index symNum= SYM_NUM(accessSym);
      Index i;
      assert(SYM_TYPE(accessSym) == NON_TERM_SYM);
      sP->lookStatus= LOOK_IS_COMPUTING;
      for (i= nonTermTrans[symNum].reduceChain; i != NIL; 
	   i= STATE_ITEMS(X_SI, i).u1.reduceChain) {
	Index predState= STATE_ITEMS(X_SI, i).state;
	LookStatus predLookStatus= getStateLookStatus(predState);
	assert(lookStatus != LOOK_INDETERMINATE);
	if (lookStatus == LOOK_UNKNOWN || 
	    predLookStatus == LOOK_INDETERMINATE) {
	  lookStatus= predLookStatus;
	}
	else if ((lookStatus == LOOK_0 && predLookStatus == LOOK_1) ||
		 (lookStatus == LOOK_1 && predLookStatus == LOOK_0)) {
	  lookStatus= LOOK_INDETERMINATE;
	}
	if (lookStatus == LOOK_INDETERMINATE) break;
      } /* for (i= nonTermTrans[symNum].reduceChain; i != NIL; ...) */
    } /* else if (nXSI == 1) */
    sP->lookStatus= lookStatus;
  } /* else if (lookStatus == LOOK_UNKNOWN) */
  assert(lookStatus != LOOK_UNKNOWN);
  assert(lookStatus != LOOK_IS_COMPUTING);
  return lookStatus;
}


/*				LR0 BUILDING ROUTINE.			*/

static VOID 
computeLR0()
{ 
  CONST Count nTerms= G.nTerms;
  CONST Count nNonTerms= G.nNonTerms;
  CONST Count nMax= nTerms > nNonTerms ? nTerms : nNonTerms;
  Index s;
  Area syms;
#define SYMS(i) A_ACCESS(syms, Sym, i)
  Area siList;
#define SI_LIST(i) A_ACCESS(siList, SIList, i)
  typedef struct {
    Index2 listBase;	/* Index first list element in SI_LIST[]. */
    Index2 listEnd;	/* Index last list element in SI_LIST[]. */
    Count2 hashSum;	/* Sum of next items in list. */
  } SymInfo;
  SymInfo *symInfo= (SymInfo *)MALLOC(nMax * sizeof(SymInfo));
  INIT_AREA(syms, Sym, 5, UINT2_MAX); INIT_AREA(siList, SIList, 7, UINT2_MAX);
  { 
    Index i;
    for (i= 0; i < nMax; i++) { 
      symInfo[i].listBase= NIL; symInfo[i].hashSum= 0;
    }
  }
  initStateItems(); initNonTermTrans(nNonTerms);
  initClosureNonTerms(nNonTerms); initStatesHash(nTerms, nNonTerms);
  initialState();   /* Build initial state for initial item 0. */
  for (s= 0; s < A_NENTRIES(states); s++) { /* Moving target. */
    SIType t;
    for (t= X_SI; t < LIMIT_SI; t++) {
      Index i;
      CONST Index iHi= STATES(s).itemsHi[t];
      for (i= 0; i < nMax; i++) {
	assert(symInfo[i].listBase == NIL); assert(symInfo[i].hashSum == 0);
      }
      for (i= STATES(s).itemsLo[t]; i < iHi; i++) {
	CONST Index item= STATE_ITEMS(t, i).item;
	if (!IS_REDUCING_ITEM(item)) {
	  CONST Sym sym= RHS_SYM(item);
	  CONST Index symNum= SYM_NUM(sym);
	  CONST Index j= A_NEXT(siList);
	  SymInfo *CONST siP= &symInfo[symNum];
	  if (siP->listBase == NIL) { 
	    Index k= A_NEXT(syms); 
	    SYMS(k)= sym; siP->listBase= j;
	  }
	  else {
	    SI_LIST(siP->listEnd).succ= j;
	  }
	  siP->hashSum+= (item + 1); siP->listEnd= j;
	  SI_LIST(j).type= t; SI_LIST(j).index= i; SI_LIST(j).succ= NIL;
	} /* if (!IS_REDUCING_ITEM(item)) */
      } /* for (i= STATES(s).itemsLo[t]; i < iHi; i++) */
      
      { /* Get successor states for all symbols in SYMS. */
	CONST Index hi= A_NENTRIES(syms);
	for (i= 0; i < hi; i++) {
	  CONST Sym sym= SYMS(i);
	  CONST Index symNum= SYM_NUM(sym);
	  assert(symInfo[symNum].listBase != NIL);
	  getState(sym, &SI_LIST(0), symInfo[symNum].listBase, 
		   symInfo[symNum].hashSum);
	  symInfo[symNum].listBase= NIL; symInfo[symNum].hashSum= 0;
	}
	A_CUT(syms, 0); A_CUT(siList, 0);
      }
    } /* for (t= X_SI; t < LIMIT_SI; t++) */
  } /* for (s= 0; s < A_NENTRIES(states); s++) */
  G.nStates= A_NENTRIES(states);
  G.testOffset= G.nStates + G.nRules;
  delClosureNonTerms(); 
  FREE(symInfo);
  VOID_RET();
#undef SYMS
#undef SI_LIST
}


/*			LR0 TEST ROUTINE.				*/
#ifdef TEST_LR0

#include "scan.h"

#include <stdio.h>

static VOID dumpLR0 PROTO((VOID_ARGS));
int main PROTO((int argc, char **argv));

static VOID 
dumpLR0()
{
  Index s;
  CONST Count nTerms= G.nTerms;
  CONST Count nNonTerms= G.nNonTerms;
  CONST Count nRules= G.nRules;
  CONST Count nStates= G.nStates;
  CONST Count nItems= G.nRHSSyms;
  printf("# of terminals= %d; # of non-terminals= %d.\n",
	 nTerms, nNonTerms);
  printf("# of rules= %d;\n", nRules);
  {
    Count nTermItems= 0, nNonTermItems= 0, nReduceItems= 0;
    Index i;
    for (i= 0; i < nItems; i++) {
      if (IS_TERMINAL_ITEM(i)) nTermItems++;
      else if (IS_NON_TERM_ITEM(i)) nNonTermItems++;
      else { assert(IS_REDUCING_ITEM(i)); nReduceItems++; }
    }
    printf("# of items= %d (%dN, %dT, %dR).\n",
	   nItems, nNonTermItems, nTermItems, nReduceItems);
  }
  printf("# of states= %d; Old states encountered= %d.\n", 
	 nStates, oldStates);
  {
    Count nTermTrans= 0, nRedTrans= 0;
    Index i;
    for (i= 0; i < A_NENTRIES(stateItems[X_SI]); i++) {
      if (IS_REDUCING_ITEM(STATE_ITEMS(X_SI, i).item)) nRedTrans++;
      else { 
	assert(IS_TERMINAL_ITEM(STATE_ITEMS(X_SI, i).item)); nTermTrans++; 
      }
    }
    printf("# of state-items= %d (%dN, %dT, %dR).\n",
	   A_NENTRIES(stateItems[X_SI]) + A_NENTRIES(stateItems[N_SI]),
	   A_NENTRIES(stateItems[N_SI]), nTermTrans, nRedTrans);
  }
  {
    Count nonClosures[LIMIT_SI]= { 0, 0 };
    Count closures[LIMIT_SI]= { 0, 0 };
    SIType t;
    for (t= X_SI; t < LIMIT_SI; t++) {
      Index i;
      for (i= 0; i < A_NENTRIES(stateItems[t]); i++) {
	if (IS_CLOSURE_ITEM(STATE_ITEMS(t, i).item))
	  closures[t]++;
	else
	  nonClosures[t]++;
      }
    }
    printf("Average # of closure items/state= %6.3f (%6.3fX/%6.3fN).\n", 
	   ((double)(closures[X_SI] + closures[N_SI]))/nStates, 
	   ((double)(closures[X_SI]))/nStates, 
	   ((double)(closures[N_SI]))/nStates);
    printf("Average # of non-closure items/state= %6.3f (%6.3fX/%6.3fN).\n",
	   ((double)(nonClosures[X_SI] + nonClosures[N_SI]))/nStates, 
	   ((double)(nonClosures[X_SI]))/nStates, 
	   ((double)(nonClosures[N_SI]))/nStates);
  }
  for (s= 0; s < nStates; s++) {
    SIType t;
    printf("STATE %d\n", s);
    for (t= X_SI; t < LIMIT_SI; t++) {
      Index i;
      CONST Index hi= STATES(s).itemsHi[t];
      for (i= STATES(s).itemsLo[t]; i < hi; i++) {
	Index item= STATE_ITEMS(t, i).item;
	printRule(stdout, getItemRuleN(item), item);
	if (!IS_REDUCING_ITEM(item)) {
	  assert(STATE_ITEMS(t, i).u.succ != NIL);
	  printf(" / %d", STATE_ITEMS(t, i).u.succ);
	}
	printf("\n");
      }
    }
    printf("\n");
  }
  VOID_RET();
}

int 
main(argc, argv)
  int argc;
  char **argv;
{
  initScan(argc > 1 ? argv[1] : NULL);
  doGram();
  initGlobs();
  computeRestRuleNullables();
  computeLR0();
  if (argc <= 2) dumpLR0();
  return 0;
}

#endif /* ifdef TEST_LR0 */

/*                          ALGORITHM DIGRAPH.                          */

/* 

Digraph algorithm from DeRemer and Pennello [TOPLAS, Oct. 82].
Algorithm implemented directly as given in the paper, except that F
is assumed to have effectively been initialized to F' for each
digraph node before digraph() is called. 

In the following struct, n[] is an array of NodeStatus indexed by
node.  The entry for n[node] is set to 0 if node has never been
visited and set to NODE_INFINITY if the node has been completely
processed.  Otherwise, if node is still under consideration, then
n[node] is set to a non-nil +'ve number equal to the depth (1-origin)
at which it was first visited.  The set of successors of a node is
gotten by using the function succ(node, succs, maxElements) which
should add upto maxElements successors of node to the Node array
starting at succs and return the number of successors added.  If the
number of successors is greater than maxElements it should return NIL.
The f-set associated with node is gotten by using the function
f(node).  Note that if a node has no successors, it's f-set is neither
read nor updated --- hence it will never even get its f' value.  Since
sets are passed by reference, changing the set returned by f(n)
changes the set stored for node n.  

If sccErr is non-NULL then it is a function which is called on
detecting a non-trivial SCC.

*/

typedef UChar NodeStatus;	/* Type giving visit status of a node. */
enum { 
  NODE_INFINITY= UCHAR_MAX 	/* Value for NodeStatus saying visit done. */
};


typedef struct Digraph {
  Count2 nNodes;	/* # of nodes in digraph being searched. */
  NodeStatus *n;	/* n[nNodes] vector, giving visit status of nodes. */
  Index2 stkIndex;	/* Index of next free location in stk. */
  Size2 stkSize;	/* Total # of allocated location in stk. */
  Node *stk;		/* Stack used for digraph-traversal. */
  Index2 succIndex;	/* Next free element in succNodes[]. */
  Size2 succSize;	/* Current size of succNodes[]. */
  Node *succNodes;	/* Used to return nodes by succ() function. */
  SuccFnP succ;		/* Adds succs of node n to succNodes[]. */
  FValFnP f;		/* Function to return current f-value of node n. */
  SCCErrFnP sccErr;	/* Function to be called on a non-trivial SCC. */
} Digraph;

#define VISITED(node, digraphP)	((digraphP)->n[node] != 0)

/* Initialize digraph *dP with the values given by the other arguments as
 * well as suitable other values.
 */
static VOID 
initDigraph(nNodes, succ, f, sccErr, dP)
  Count nNodes;
  SuccFnP succ;
  FValFnP f;
  SCCErrFnP sccErr;
  struct Digraph *dP;
{
  dP->nNodes= nNodes;
  dP->n= (NodeStatus *)CALLOC(nNodes, sizeof(NodeStatus));
  dP->stkIndex= dP->stkSize= 0;  dP->stk= 0;
  dP->succIndex= dP->succSize= 0; dP->succNodes= 0;
  dP->succ= succ; dP->f= f; dP->sccErr= sccErr;
  VOID_RET();
}

/* Free all storage used by digraph *digraphP. */
static VOID 
freeDigraph(digraphP)
  struct Digraph *digraphP;
{
  FREE(digraphP->n); 
  FREE(digraphP->stk); 
  FREE(digraphP->succNodes);
  VOID_RET();
}


/* Do a depth-first traversal of a digraph starting at node x updating
 * the f-set of each node to be the union of the f-sets of its
 * successors. 
 */
static VOID 
digraphTraverse(x, dP)
  NodeI x;
  struct Digraph *dP;
{
  enum { DIGRAPH_STK_INC= 16 };	/* Increment for stack size. */
  enum { SUCC_NODES_INC= 32 };	/* Increment for succNodes[]. */
  CONST Index d= dP->stkIndex + 1;/* Depth of stk. after pushing x. */
  CONST Index succBase= 	/* Index in succNodes where first successor */
    dP->succIndex; 		/* of x is computed. */
  Count nSucc= 			/* # of successors of x. */
    dP->succ(x, &dP->succNodes[succBase], dP->succSize - succBase);	
  ISet fSet=			/* Can be changed, since ISets are indirect. */
    (nSucc == 0)		/* Compute f only if x has successors. */
    ? BAD_ISET 			/* No fSet needed. */
    : dP->f(x);			/* Computed even if nSucc == NIL. */
  Index i;			/* Counts from 0...nSucc. */
  assert(dP->n[x] == 0);	/* We visit a node x only if n[x] == 0. */
  if (d >= NODE_INFINITY) fatal("stack overflow in digraph()");
  if (dP->stkIndex >= dP->stkSize) { /* Need to grow stk. */
    dP->stk= 
      (Node *)REALLOC(dP->stk, 
		      (dP->stkSize+= DIGRAPH_STK_INC)*sizeof(Node));
  }
  dP->stk[dP->stkIndex++]= x; 	/* Push x on stk. */
  dP->n[x]= d;			/* Remember we are visiting x at depth d. */
  while (nSucc == NIL) { 	/* We need to grow succNodes. */
    /* Repeatedly realloc() till succNodes[] large enough. */
    dP->succNodes= 
      (Node *)REALLOC(dP->succNodes,
		      (dP->succSize+= SUCC_NODES_INC)*sizeof(Node));
    nSucc= dP->succ(x, &dP->succNodes[succBase], dP->succSize - succBase);
  }
  dP->succIndex+= nSucc;	/* Reserve succNodes for possible 
				   recursive traversal. */
  for (i= 0; i < nSucc; i++) { 	/* Traverse unvisited successors. */
    Node y= 			/* Ranges over the successors of x. */
      dP->succNodes[succBase + i]; 
    if (dP->n[y] == 0) {	/* Node y has never been visited before. */
      digraphTraverse(y, dP);	/* Recursively visit it. */
    }
    if (dP->n[y] < dP->n[x]) { 	/* Part of a SCC. */
      dP->n[x]= dP->n[y]; 	/* n[x]= min(n[x], n[y]). */
    }
    assert(fSet != BAD_ISET);	/* Since we have at least 1 successor. */
    unionISet(fSet, dP->f(y));	/* Union in f-set of successor. */
  }
  assert(dP->succIndex == succBase + nSucc); /* Recursive calls reset. */
  dP->succIndex= succBase;	/* Forget successors by resetting succIndex. */
  if (dP->n[x] == d) {		/* We have the entry-node to an SCC. */
    if (dP->stk[dP->stkIndex - 1] == x) { /* Trivial SCC. */
      dP->n[x]= NODE_INFINITY; --dP->stkIndex;
    }
    else { /* Non-trivial SCC. */
      assert(fSet != BAD_ISET);
      if (dP->sccErr) dP->sccErr(x, dP);
      do { 
	CONST Node top= dP->stk[dP->stkIndex - 1];
	dP->n[top]= NODE_INFINITY; assignISet(dP->f(top), fSet);
      } while (dP->stk[--dP->stkIndex] != x); /* Pop entries till x. */
    }
  }
  VOID_RET();
}

/* Do a depth-first traversal of all components of digraph *dP 
 * starting at arbitrary nodes in each component. 
 */
static VOID 
digraphAll(dP)
  struct Digraph *dP;
{
  Index i;
  for (i= 0; i < dP->nNodes; i++) {
    if (dP->n[i] == 0) digraphTraverse(i, dP);
  }
  VOID_RET();
}


/*			    READS SET COMPUTATION.			*/

/* Return the direct-read set for the non-terminal transition specified
 * by STATE_ITEMS(N_SI, ntTrans).  Specifically if ntTrans represents the
 * non-terminal transition (p, A), then return the set of terminals
 * { t | t is a terminal such that p --A--> q --t--> }.
 */
static ISet 
drSet(ntTrans)
  Index ntTrans;
{
  ISet s= newISet();
  Index i;
  CONST Index q= STATE_ITEMS(N_SI, ntTrans).u.succ;
  CONST Index hi= STATES(q).itemsHi[X_SI];
  assert(q != NIL);
  for (i= STATES(q).itemsLo[X_SI]; i < hi; i++) {
    Index item= STATE_ITEMS(X_SI, i).item;
    assert(IS_TERMINAL_ITEM(item) || IS_REDUCING_ITEM(item));
    if (IS_TERMINAL_ITEM(item)) addISet(s, SYM_NUM(RHS_SYM(item)));
  }
  return s;
}

/* Return the reads successors of non-terminal transition ntTrans in
 * array succs[] having upto succsSize entries.  If the number of
 * successors is > succsSize, then return NIL, else return the number
 * of successors.  Specifically, if ntTrans represents the
 * non-terminal transition (p, A), then its reads successors is given
 * by the set of non-terminal transitions: 
 *
 * { (q, C) | p --A--> q --C--> and C is nullable. }. 
 *
 * This function can be used as the succ() function required by digraph.
 */
static Count 
readsSucc(ntTrans, succs, succsSize)
  NodeI ntTrans;
  Node succs[];
  Size succsSize;
{
  Index i;
  CONST Index q= STATE_ITEMS(N_SI, ntTrans).u.succ;
  CONST Index hi= STATES(q).itemsHi[N_SI];
  Count nSucc= 0;
  assert(q != NIL);
  for (i= STATES(q).itemsLo[N_SI]; i < hi; i++) {
    Index item= STATE_ITEMS(N_SI, i).item;
    Index c= SYM_NUM(RHS_SYM(item));
    assert(SYM_TYPE(RHS_SYM(item)) == NON_TERM_SYM);
    if (IS_NULLABLE(c)) {
      if (nSucc >= succsSize) return NIL;
      succs[nSucc++]= i;
    }
  }
  return nSucc;
}

/* Returns the stored set stored in its readFollow component for
 * non-terminal transition ntTrans.  If the stored set is a BAD_ISET,
 * then return it after initializing it to the direct-read set
 * associated with ntTrans.
 *
 * This function can be used as the f() function required by digraph. 
 */
static ISet 
readsF(ntTrans)
  NodeI ntTrans;
{
  ISet *readFollowP= &NSI_INFO(ntTrans).readFollow;
  assert(ntTrans < A_NENTRIES(nsiInfo));
  return (*readFollowP == BAD_ISET) 
         ? (*readFollowP= drSet(ntTrans))
	 : *readFollowP;
}


#if 0
VOID readsSCC(n, dP)
  NodeI n;
  struct Digraph *dP;
{
  error("reads SCC in digraph.  Grammar not LALR");
  VOID_RET();
}
#endif

/* Compute the read-set for all non-terminal transitions which have
 * reads-successors.  If a non-terminal transition does not have any
 * successors, then its read-set remains at BAD_ISET.
 */
static VOID 
computeReads()
{
  Digraph readDigraph;
  initDigraph(A_NENTRIES(stateItems[N_SI]), readsSucc, readsF, 
	      NULL, &readDigraph);
  digraphAll(&readDigraph);
  freeDigraph(&readDigraph);
  VOID_RET();
}

/*			SLR COMPUTATION.				*/

/* Entry for each nonTerms.  Element is TRUE if corresponding nonTerm 
 * already returned in this call of slrSucc(). 
 */
static Boolean *checkSet;

/* SLR follow-sets for each non-terminal. */
static ISet *nonTermFollows;

static VOID 
initSLR()
{
  Index i;
  CONST Count nNonTerms= G.nNonTerms;
  checkSet= (Boolean *)CALLOC(nNonTerms, sizeof(Boolean));
  nonTermFollows= (ISet *)MALLOC(nNonTerms * sizeof(ISet));
  for (i= 0; i < nNonTerms; i++) nonTermFollows[i]= BAD_ISET;
  VOID_RET();
}

/* Return the non-terminals B related to non-terminal a such that
 * B-> alpha A gamma is a production and gamma is nullable.
 */
static Count 
slrSucc(a, succs, succsSize)
  NodeI a; 
  Node succs[];
  Size succsSize;
{
  Index i;
  Count nSucc= 0;
  Index retVal= 0;  /* If still 0 at end of loop, then return nSucc. */
  for (i= nonTermTrans[a].chain; i != NIL; 
       i= STATE_ITEMS(N_SI, i).u1.nonTermChain) {
    Index item= STATE_ITEMS(N_SI, i).item;
    assert(SYM_TYPE(RHS_SYM(item)) == NON_TERM_SYM);
    if (REST_RULE_NULLABLE(item + 1)) {
      CONST Index b= RULE_NON_TERM(getItemRuleN(item));
      if (!checkSet[b]) {
	if (nSucc >= succsSize) { retVal= NIL; break; }
	succs[nSucc++]= b; checkSet[b]= TRUE;
      }
    }
  }
  if (retVal == 0) retVal= nSucc;
  for (i= 0; i < nSucc; i++) checkSet[succs[i]]= FALSE;
  return retVal;
}

/* Return the follow-set associated with non-terminal A.  If
 * the follow set is BAD_ISET, then initialize and returns the union
 * of all the read-sets for transitions on A.  If any of the read-sets
 * is found to be BAD_ISET, then initialize it to the direct-read set.
 */
static ISet 
slrF(a)
  NodeI a;
{
  ISet s= nonTermFollows[a];
  if (s == BAD_ISET) {
    Index i;
    nonTermFollows[a]= s= newISet();
    for (i= nonTermTrans[a].chain; i != NIL; 
	 i= STATE_ITEMS(N_SI, i).u1.nonTermChain) {
      Index item= STATE_ITEMS(N_SI, i).item;
      assert(SYM_TYPE(RHS_SYM(item)) == NON_TERM_SYM);
      if (NSI_INFO(i).readFollow == BAD_ISET)
	NSI_INFO(i).readFollow= drSet(i);
      unionISet(s, NSI_INFO(i).readFollow);
    }
  }
  return s;
}

static VOID 
computeFollows() 
{
  CONST Count nNonTerms= G.nNonTerms;
  Digraph slrDigraph;
  Index i;
  computeReads();
  initSLR();
  initDigraph(nNonTerms, slrSucc, slrF, NULL, &slrDigraph);
  digraphAll(&slrDigraph);
  FREE(checkSet); 
  freeDigraph(&slrDigraph);
  for (i= 0; i < nNonTerms; i++) {
    if (nonTermFollows[i] == BAD_ISET) slrF(i);
    assert(nonTermFollows[i] != BAD_ISET);
  }
  VOID_RET();
}

#ifdef TEST_SLR

static VOID dumpSLR PROTO((VOID_ARGS));
int main PROTO((int argc, char **argv));

static VOID 
dumpSLR() 
{
  Index i;
  CONST Count nNonTerms= G.nNonTerms;
  for (i= 0; i < nNonTerms; i++) {
    ISetElement e;
    ISetIter isIter;
    printf("%s: {", getNonTermString(i));
    assert(nonTermFollows[i] != BAD_ISET);
    FOR_EACH_IN_ISET(e, nonTermFollows[i], isIter) {
      printf("%s ", getTermString(e));
    }
    printf("}\n");
  }
  VOID_RET();
}

#include "scan.h"

int 
main(argc, argv)
  int argc;
  char **argv;
{
  initScan(argc > 1 ? argv[1] : NULL);
  doGram();
  initGlobs();
  computeRestRuleNullables();
  computeLR0();
  computeFollows();
  if (argc <= 2) dumpSLR();
  return 0;
}

#endif /* ifdef TEST_SLR */


/*			LALR COMPUTATION.				*/

/*
We generalize Penello and De Remer's lookback relation:

                                              alpha
(q, A-> alpha . beta) lookback (p, A) iff  p --...--> q

(The paper's definition required beta to be empty).  This generalized
lookback is useful for both lookahead computation (as in the paper), as
well as for non-terminal transition follow set computation.

*/

/* If alpha are the symbols in RHS[itemLo, ..., itemHi), return TRUE iff
 * state s1 goes to state s2 under alpha.  This routine involves some
 * searching, as we have to search for a matching transition in each state.
 */
static Boolean 
hasPath(s1, s2, itemLo, itemHi)
  Index s1;
  Index s2;
  Index itemLo;
  Index itemHi;
{
  Index i;
  Index s= s1;
  for (i= itemLo; i < itemHi; i++) {
    Index sym= RHS_SYM(i);
    SymType symType= SYM_TYPE(sym);
    SIType t= (symType == NON_TERM_SYM) ? N_SI : X_SI;
    CONST Index hi= STATES(s).itemsHi[t];
    Index j;
    assert(symType == TERM_SYM || symType == NON_TERM_SYM);
    for (j= STATES(s).itemsLo[t]; j < hi; j++) {
      Index symX= RHS_SYM(STATE_ITEMS(t, j).item);
      if (sym == symX) { s= STATE_ITEMS(t, j).u.succ; break; }
    }
    if (j == hi) break;
    assert(s != NIL);
  } /* for (i= itemLo; i < itemHi; i++) */
  return (i == itemHi && s == s2);
}

/* Return the includes successors of non-terminal transition ntTrans in
 * array succs[] having upto succsSize entries.  If the number of
 * successors is > succsSize, then return NIL, else return the number
 * of successors.  Specifically, if ntTrans represents the
 * non-terminal transition (p, A), then its includes successors is given
 * by the set of non-terminal transitions: 
 *
 * { (q, B) | B->alpha A gamma, gamma is nullable & q --alpha--> p }
 *
 * This function can be used as the succ() function required by digraph.
 */
static Count 
includesSucc(ntTrans, succs, succsSize)
  NodeI ntTrans;
  Node succs[];
  Size succsSize;
{
  CONST Index item= STATE_ITEMS(N_SI, ntTrans).item;
  CONST Index item1= item + 1;
  CONST Index pState= STATE_ITEMS(N_SI, ntTrans).state;
  Count nSucc= 0;
  assert(ntTrans < A_NENTRIES(stateItems[N_SI]));
  assert(IS_NON_TERM_ITEM(item));
  if (REST_RULE_NULLABLE(item1)) {
    CONST Index ruleN= getItemRuleN(item1);
    CONST Index itemLo= RULE_RHS_LO(ruleN);
    Index i;
    Index lastQState= NIL;
    Boolean isConnected= FALSE;	/* Cache  hasPath() here. */
    for (i= nonTermTrans[RULE_NON_TERM(ruleN)].chain; 
	 i != NIL; 
	 i= STATE_ITEMS(N_SI, i).u1.nonTermChain) {
      CONST Index qState= STATE_ITEMS(N_SI, i).state;
      if (qState != lastQState) { /* Cached value invalid. */
	isConnected= hasPath(qState, pState, itemLo, item);
      }
      if (isConnected) {
	if (nSucc == succsSize) return NIL;
	succs[nSucc++]= i;
      }
      lastQState= qState;
    } /* for (i= nonTermTrans[RULE_NON_TERM(ruleN)].chain; ...) */
  }
  return nSucc;    
}

/* Return the reads set associated with ntTrans.  If it is a BAD_ISET,
 * then initialize it to its direct-read set before returning.
 */
static ISet 
lalrF(ntTrans)
  NodeI ntTrans;
{
  ISet *readFollowP= &NSI_INFO(ntTrans).readFollow;
  if (*readFollowP == BAD_ISET) *readFollowP= drSet(ntTrans);
  return *readFollowP;  
}

static Digraph lalrDigraph;

#if 0
static VOID 
lalrSCC(n, dP)
  NodeI n;
  struct Digraph *dP;
{
  error("scc in includes relation.  Grammar probably not LALR");
  VOID_RET();
}
#endif

/* Return the follow set for non-terminal transition ntTrans. */
static ISet 
ntTransFollowSet(ntTrans)
  Index ntTrans;
{
  if (!VISITED(ntTrans, &lalrDigraph)) {
    digraphTraverse(ntTrans, &lalrDigraph);
  }
  if (NSI_INFO(ntTrans).readFollow == BAD_ISET)
    NSI_INFO(ntTrans).readFollow= drSet(ntTrans);
  return NSI_INFO(ntTrans).readFollow;
}

/* Return the LALR lookahead set for reduction xTrans in STATE_ITEMS(X_SI). 
 * The returned set is freshly allocated.
 */
static ISet 
lalrLook(xTrans)
  Index xTrans;
{
  ISet look= newISet();
  Index item= STATE_ITEMS(X_SI, xTrans).item;
  CONST Index ruleN= SYM_NUM(RHS_SYM(item));
  CONST Index itemLo= RULE_RHS_LO(ruleN);
  CONST Index pState= STATE_ITEMS(X_SI, xTrans).state;
  Index i;
  Index lastQState= NIL;
  Boolean isConnected= FALSE;		/* Cache  hasPath() here. */
  assert(IS_REDUCING_ITEM(item));
  for (i= nonTermTrans[RULE_NON_TERM(ruleN)].chain; 
       i != NIL; 
       i= STATE_ITEMS(N_SI, i).u1.nonTermChain) {
    CONST Index qState= STATE_ITEMS(N_SI, i).state;
    if (qState != lastQState) { /* Cached isConnected value invalid. */
      isConnected= hasPath(qState, pState, itemLo, item);
    }
    if (isConnected) {
      unionISet(look, ntTransFollowSet(i));
    }
    lastQState= qState;
  }
  return look;
}


/*				PRE-ACTIONS.				*/

/*

Pre-actions are used during LALR construction.

Pre-Actions are of the following types: 
    a.  Test by test-num T encoded as T (note that actual test #s are >0; 0
        is used for as a special test number to indicate the default action
	when all tests fail).
    b.  Shift to state s encoded as nTests() + s.
    c.  Reduce by rule with end at STATE_ITEMS(X_SI, x) encoded as 
        nTests + nStates + x.

Note that with this encoding we can use integer < on the coded preactions
and guarantee tests < shift and shift < reductions.

*/	

/* Constructors. */
#define MAKE_SHIFT_PRE_ACT(s)		(s + G.nTests)
#define MAKE_REDUCE_PRE_ACT(r)		((r) + G.nTests + G.nStates)
#define MAKE_TEST_PRE_ACT(t)		(t)

/* Selectors. */
#define PRE_ACT_SHIFT(a)		((a) - G.nTests)
#define PRE_ACT_REDUCTION_ITEM(a)	((a) - G.nTests - G.nStates)
#define PRE_ACT_TEST_NUM(a)		(a)

/* Type Predicates. */
#define IS_SHIFT_PRE_ACT(a)						\
  (G.nTests <= (a) && (a) < G.nTests + G.nStates)
#define IS_REDUCE_PRE_ACT(a)	(G.nTests + G.nStates <= (a))
#define IS_TEST_PRE_ACT(a)	((a) < G.nTests)

#if 0 /* Defined at start of file. */
typedef Index2 PreAct;
typedef Index PreActX;
#endif


/*				POST ACTIONS.				*/
/*
Encoding of actions used after LALR construction complete.
Encoding as follows:

Shift s: Shift to state s: encoded as s.
Reduce ruleN: Reduce by rule ruleN: encoded as r + nStates.
Test i: Shift by test-set starting at index i: encoded as i + nStates + nRules.

*/

/* Constructors. */
#define MAKE_SHIFT_POST_ACT(s)		((s))
#define MAKE_REDUCE_POST_ACT(r)		((r) + G.nStates)
#define MAKE_TEST_POST_ACT(i)		((i) + G.testOffset)

/* Selectors. */
#define POST_ACT_SHIFT(a)		(a)
#define POST_ACT_REDUCTION_RULEN(a)	((a) - G.nStates)
#define POST_ACT_TEST_INDEX(a)		((a) - G.testOffset)

/* Type Predicates. */
#define IS_SHIFT_POST_ACT(a)						\
  ((a) < G.nStates)
#define IS_REDUCE_POST_ACT(a)	(G.nStates <= (a) && (a) < G.testOffset)
#define IS_TEST_POST_ACT(a)	(G.testOffset <= (a))

/* Convert from a PRE to a POST act.  Does not work for test actions. */
#define PRE_TO_POST_ACT(pre)						\
   ( (IS_REDUCE_PRE_ACT(pre))						\
     ? MAKE_REDUCE_POST_ACT(						\
 	 SYM_NUM(RHS_SYM(STATE_ITEMS(X_SI,				\
			    PRE_ACT_REDUCTION_ITEM(pre)).item)))	\
     : MAKE_SHIFT_POST_ACT(PRE_ACT_SHIFT(pre))				\
   )


#if 0 /* Defined at start of file. */
typedef Index2 PostAct;
typedef Index PostActX;
#endif

/* Actions which are finally built. */
typedef Entry Action;	/* Entry defined in compTab.h. */

static AREA (actions, Action, 10, UINT2_MAX);
#define ACTIONS(i) A_ACCESS(actions, Action, i)

static Index 
newAction(term, postAct)
  Index term;
  PostActX postAct;
{
  Index a= A_NEXT(actions);
  ACTIONS(a).input= term; ACTIONS(a).value= postAct;
  return a;
}

static VOID 
printAction(outFile, postAct)
  FILE *outFile;
  PostActX postAct;
{
  if (IS_SHIFT_POST_ACT(postAct)) {
    fprintf(outFile, "SHIFT %d", POST_ACT_SHIFT(postAct));
  }
  else if (IS_TEST_POST_ACT(postAct)) {
    Index testSetN= POST_ACT_TEST_INDEX(postAct);
    fprintf(outFile, "TEST SET %d", testSetN);
  }
  else {
    Index ruleN= POST_ACT_REDUCTION_RULEN(postAct); 
    assert(IS_REDUCE_POST_ACT(postAct));
    assert(ruleN < G.nRules);
    fprintf(outFile, "REDUCE %d (%s)", ruleN, 
	    getNonTermString(ruleNonTerm(ruleN)));
  }
  VOID_RET();
}


/*			      ACTION LISTS				*/

/*

As each state is processed, we build a list of actions for each
terminal.  We do this in a ActList area which is setup so that for
each terminal t, the element at index t in the area contains the
header for the action list for that terminal.  Conflict resolution may
destructively modify the list (without bothering to free deleted
entries: they are recovered only when the state is completely
processed).

*/

typedef struct {	/* Struct keeps track of list of encoded actions. */
  Index2 act;		/* Encoded action. */
  Index2 succ;		/* Next action for same terminal. */
} ActList;

/* Following macro is used in many routines with param actListsP. */
#define ACT_LISTS(i) A_ACCESS(*actListsP, ActList, i)

/* For each termintal t, initialize *actListsP area to contain a dummy
 * list header at ACT_LISTS(t).
 */
static VOID
initActLists(actListsP)
  Area *actListsP;
{
  CONST Count nTerms= G.nTerms;
  Index i;
  INIT_AREA(*actListsP, ActList, 7, UINT2_MAX);
  for (i= 0; i < nTerms; i++) {
    Index j= A_NEXT(*actListsP);
    assert(i == j);
    ACT_LISTS(j).act= ACT_LISTS(j).succ= NIL;
  }
  VOID_RET();
}

/* Reset ACT_LISTS[] area to contain only the action list headers. */
static VOID
resetActLists(actListsP)
  Area *actListsP;
{
  A_CUT(*actListsP, G.nTerms);
}

/* Insert the PreAct preAct for terminal term in the action list
 * (maintained in area *actListsP) based at ACT_LISTS(term).  The list
 * is maintained as tests (sorted by test-number), a single shift,
 * reductions (sorted by reducing item #).
 */
static VOID
addActList(actListsP, term, preAct)
  Area *actListsP;
  Index term;
  PreActX preAct;
{
  Index i;
  Index i0; 			/* Lags i. */
  assert(term < G.nTerms);
  assert(term < A_NENTRIES(*actListsP));

  for (i0= term,  i= ACT_LISTS(i0).succ;
       i != NIL && ACT_LISTS(i).act < preAct; 
       i0= i, i= ACT_LISTS(i0).succ) {
    /* Find first act which is >= preAct. */
  }
  if (i != NIL && preAct == ACT_LISTS(i).act) {
    /* The only action which may be inserted multiple times is a shift. */
    assert(IS_SHIFT_PRE_ACT(preAct));
  }
  else { /* Insert into list after i0. */
    Index j= A_NEXT(*actListsP);
    ACT_LISTS(j).act= preAct; 
    ACT_LISTS(j).succ= i;  ACT_LISTS(i0).succ= j;
  }
  VOID_RET();
}

/* Delete preAct from the action list for term.  preAct is usually
 * present.  If the list becomes empty, then set the act for the list
 * header to NIL.
 */
static VOID
delActList(actListsP, term, preAct)
  Area *actListsP;
  Index term;
  PreActX preAct;
{
  Index i;
  Index i0; 			/* Lags i. */
  assert(term < G.nTerms);
  assert(term < A_NENTRIES(*actListsP));

  for (i0= term,  i= ACT_LISTS(i0).succ;
       i != NIL && ACT_LISTS(i).act < preAct; 
       i0= i, i= ACT_LISTS(i0).succ) {
    /* Find first act which is >= preAct. */
  }
  if (i != NIL && ACT_LISTS(i).act == preAct) { /* Found entry to delete. */
    ACT_LISTS(i0).succ= ACT_LISTS(i).succ; /* Delete it. */
    if (i0 == term && ACT_LISTS(i0).succ == NIL) {
      /* List has become empty: ensure header postaction is NIL. */
      ACT_LISTS(i0).act= NIL;
    }
  }
  VOID_RET();
}


/*				   TEST SETS.				*/

/*

A test set is a sequence of test preactions terminated by a non-test
preaction corresponding to the default pre-action if all tests fail.
Each preaction is chained into a hash-table to facilitate detection of
duplicate test sets.  The test-sets are ordered by preact numbers.
The terminating non-test preaction is NIL if the default is to be
error.

*/

typedef struct {
  Index2 act;	/* Preaction. */
  Index2 succ;	/* Next entry on hash-chain. */
} TestSet;

static AREA (testSets, TestSet, 6, UINT2_MAX); 	/* Set of tests. */
#define TEST_SETS(i) A_ACCESS(testSets, TestSet, i)

#define TESTS_HASH_SIZE 32
static Index2 testsHashTab[TESTS_HASH_SIZE];
#define TEST_HASH(preAct)  ((preAct) % TESTS_HASH_SIZE)

/* Return the number of entries in TEST_SETS[]. */
static Index
nTestSets()
{
  return A_NENTRIES(testSets);
}

/* Return the test-number for TEST_SETS(t), transforming any non-test
 * preActs to 0. 
 */
static Index 
testSetsTest(t)
  Index t;
{
  CONST Index act= TEST_SETS(t).act;
  assert(t < A_NENTRIES(testSets));
  return ( act != NIL && IS_TEST_PRE_ACT(act) ) ? PRE_ACT_TEST_NUM(act) : 0;
}

/* Return the post-act for TEST_SETS(t). */
static Index 
testSetsAct(t)
  Index t;
{
  CONST Index act= TEST_SETS(t).act;
  assert(t < A_NENTRIES(testSets));
  return 
    (act == NIL) 
       ? MAKE_REDUCE_POST_ACT(G.nRules - 1)  /* Error rule. */
       : ( (IS_TEST_PRE_ACT(act)) 
           ? MAKE_REDUCE_POST_ACT(testRuleN(PRE_ACT_TEST_NUM(act)))
           : PRE_TO_POST_ACT(act)
	 );
}

/* Searches for a sequence of tests in TEST_SETS[] for the list of tests
 * starting at ACT_LISTS(start).  If found, return the index in TEST_SETS[]
 * of the first element in the sequence.  Else insert at the end
 * of the TEST_SETS[] sequence, returning the index.  We hash on only
 * the first action in the list in an attempt to find previously entered
 * lists which are duplicates of the one to be entered.
 */
static Index 
getTestSetN(actListsP, start)
  Area *actListsP;
  Index start; 
{
  CONST Index h= TEST_HASH(ACT_LISTS(start).act);
  Index i;
  assert(IS_TEST_PRE_ACT(ACT_LISTS(start).act));
  for (i= testsHashTab[h]; i != NIL; i= TEST_SETS(i).succ) {
    Index j, k;
    for (j= i, k= start; k != NIL; j++, k= ACT_LISTS(k).succ) {
	if (TEST_SETS(j).act != ACT_LISTS(k).act) break;
    }  
    if (k == NIL) break;
  }
  if (i == NIL) { /* Create new test set starting at end of TEST_SETS[]. */
    CONST Index end= A_NENTRIES(testSets);
    for (i= start; i != NIL; i= ACT_LISTS(i).succ) {
      CONST Index t= A_NEXT(testSets);
      CONST Index act= ACT_LISTS(i).act;
      CONST Index testHash= TEST_HASH(act);
      TEST_SETS(t).act= act; 
      TEST_SETS(t).succ= testsHashTab[testHash];
      testsHashTab[testHash]= t;
    }
    i= end;
  }
  return i;
}


static VOID 
initTests()
{
  Index i;
  for (i= 0; i < TESTS_HASH_SIZE; i++) { 
    testsHashTab[i]= NIL;
  }
  VOID_RET();
}


/*				CONFLICT RESOLUTION			*/

/*

Before conflict resolution, the list of actions for a terminal look like:

test1, ..., testN, shift, reduce1, ..., reduceM

any of the test..., shift or reduce..., may be missing.

Tho' the conflict resolution works basically on the shift,
reduce... sequence it folds the result as the default action into the
test sequence if it is present.

To resolve shift, reduce1, ..., reduceM each of the reduce actions is
iteratively resolved against the shift action (if present) and the
current resolution.  

If a shift-reduce conflict is resolved as error (possible if
%nonassoc), then that error may be knocked out by a subsequent
resolution.

The post-action for the winning shift, reduce or test-set number is
put into the header for each terminal action list.

*/

/* Update the lookahead set for the reduction in
 * STATE_ITEMS(X_SI, reduceX) to its LALR lookahead set, returning the
 * LALR lookahead set.  Remove those elements in ACT_LISTS[] equal to
 * MAKE_REDUCE_PRE_ACT(reduceX) which are in the SLR lookset but not
 * in the LALR lookset.  Called only when the lalrLookSet is BAD_ISET.
 */
static ISet 
slrToLALR(actListsP, reduceX)
  Area *actListsP;
  Index reduceX;
{
  Index nonTerm= 
    RULE_NON_TERM(SYM_NUM(RHS_SYM(STATE_ITEMS(X_SI, reduceX).item)));
  ISet slrLookSet= nonTermFollows[nonTerm];
  ISet lalrLookSet= lalrLook(reduceX);
  assert(STATE_ITEMS(X_SI, reduceX).u.lalrLook == BAD_ISET);
  STATE_ITEMS(X_SI, reduceX).u.lalrLook= lalrLookSet;
  if (compareISet(slrLookSet, lalrLookSet) != 0) {
    ISet diffSet= diffISet(copyISet(slrLookSet), lalrLookSet);
    CONST Index reduceXAct= MAKE_REDUCE_PRE_ACT(reduceX);
    ISetElement t;
    ISetIter isIter;
    assert(isSubsetISet(lalrLookSet, slrLookSet));
    FOR_EACH_IN_ISET(t, diffSet, isIter) {
      delActList(actListsP, t, reduceXAct);
    }
    delISet(diffSet);
  }
  return lalrLookSet;
}

/* Count of # of shift-reduce conflicts. */
static Count nSRConflicts= 0;

/* Resolve shift-reduce conflict on term between shiftPreAct and
 * reducePreAct.  Return winning preAct (NIL if resolution is for
 * error).  First try assoc-prec resolution, then try using LALR
 * lookahead set for the reduction.  If not resolved, then record
 * looser in ACTIONS[] and increment nSRConflicts.  If the resolution
 * is for an error, because of %nonassoc, set *doDefaultP to FALSE.
 */
static Index
resolveSRConflict(actListsP, term, shiftPreAct, reducePreAct, doDefaultP)
  Area *actListsP;
  Index term;
  Index shiftPreAct;
  Index reducePreAct;
  Boolean *doDefaultP;
{
  CONST Index stateItem1= PRE_ACT_REDUCTION_ITEM(reducePreAct);
  CONST Index ruleN1= SYM_NUM(RHS_SYM(STATE_ITEMS(X_SI, stateItem1).item));
  Boolean isNonAssoc= FALSE;
  Index resolution= assocPrecResolve(term, ruleN1, reducePreAct, shiftPreAct,
				     &isNonAssoc);
  assert(IS_SHIFT_PRE_ACT(shiftPreAct));
  assert(IS_REDUCE_PRE_ACT(reducePreAct));
  if (resolution == NIL) {
    if (isNonAssoc) { /* Resolved to error. */
      *doDefaultP= FALSE;	/* No default reduction. */
    }
    else {  /* Not resolved. */
      Boolean resolved= FALSE;
      resolution= shiftPreAct;
      if (STATE_ITEMS(X_SI, stateItem1).u.lalrLook == BAD_ISET) {
	ISet lalrLookSet= slrToLALR(actListsP, stateItem1);
	resolved= !inISet(term, lalrLookSet);
      }
      if (!resolved) {
	nSRConflicts++;
	assert(!IS_TEST_PRE_ACT(reducePreAct));
	newAction(term, PRE_TO_POST_ACT(reducePreAct));
      }
    } /* else not resolved. */
  } /* if (resolution == NIL) */
  return resolution;
}

/* Count of # of reduce-reduce conflicts. */
static Count nRRConflicts= 0;

/* Resolve reduce-reduce conflict on term between preact reductions
 * reducePreAct1 and reducePreAct2.  First see if LALR lookahead set
 * for reducePreAct1 does it, then try LALR lookahead set for
 * reducePreAct2.  If still unresolved, then choose winner depending
 * on setting of optionsP->doRRLonger, record looser in ACTIONS[] and
 * increment nRRConflicts counter.  Return winning preAct.  Note that
 * as LALR lookahead sets are computed, reduction actions may be
 * removed from *actListsP for those elements of the SLR lookset not
 * in the LALR look set.
 */
static Index
resolveRRConflict(actListsP, term, reducePreAct1, reducePreAct2)
  Area *actListsP;
  Index term;
  Index reducePreAct1;
  Index reducePreAct2;
{
  CONST Index stateItem1= PRE_ACT_REDUCTION_ITEM(reducePreAct1);
  CONST Index ruleN1= SYM_NUM(RHS_SYM(STATE_ITEMS(X_SI, stateItem1).item));
  CONST Index stateItem2= PRE_ACT_REDUCTION_ITEM(reducePreAct2);
  CONST Index ruleN2= SYM_NUM(RHS_SYM(STATE_ITEMS(X_SI, stateItem2).item));
  Index resolution= NIL;
  assert(IS_REDUCE_PRE_ACT(reducePreAct1));
  assert(IS_REDUCE_PRE_ACT(reducePreAct2));
  if (STATE_ITEMS(X_SI, stateItem1).u.lalrLook == BAD_ISET) {
    ISet lalrLookSet= slrToLALR(actListsP, stateItem1);
    if (!inISet(term, lalrLookSet)) {
      resolution= reducePreAct2;
    }
  }
  if (resolution == NIL && 
      STATE_ITEMS(X_SI, stateItem2).u.lalrLook == BAD_ISET) {
    ISet lalrLookSet= slrToLALR(actListsP, stateItem2);
    if (!inISet(term, lalrLookSet)) {
      resolution= reducePreAct1;
    }
  }
  if (resolution == NIL) {
    CONST Boolean ruleN1Wins= 
      (optionsP->doRRLonger && RULE_LEN(ruleN1) != RULE_LEN(ruleN2)) 
      ? ( RULE_LEN(ruleN1) > RULE_LEN(ruleN2) )
      : ( ruleN1 < ruleN2 );
    resolution= ruleN1Wins ? reducePreAct1 : reducePreAct2;
    nRRConflicts++;
    newAction(term, MAKE_REDUCE_POST_ACT(ruleN1Wins ? ruleN2 : ruleN1));
  }
  assert(resolution == reducePreAct1 || resolution == reducePreAct2);
  return resolution;
}

/* Return the resolution of reducePreAct against shiftPreAct and resolution. 
 * Specifically, if shiftPreAct is non-NIL, then try to resolve reducePreAct
 * against shiftPreAct; then resolve the result with resolution.  Return
 * winner (NIL if error resolution).  Make *doDefaultP FALSE if resolution
 * is error because of %nonassoc.
 */
static Index
resolveConflictsLo(actListsP, term, shiftPreAct, reducePreAct, resolution,
		   doDefaultP)
  Area *actListsP;
  Index term;
  Index shiftPreAct;
  Index reducePreAct;
  Index resolution;
  Boolean *doDefaultP;
{
  Index srResolution;
  assert(shiftPreAct == NIL || IS_SHIFT_PRE_ACT(shiftPreAct));
  assert(reducePreAct != NIL && IS_REDUCE_PRE_ACT(reducePreAct));
  assert(resolution != reducePreAct);
  assert(resolution == NIL || !IS_TEST_PRE_ACT(resolution));
  srResolution= (shiftPreAct == NIL) 
                ? reducePreAct
                : resolveSRConflict(actListsP, term, shiftPreAct, 
				    reducePreAct, doDefaultP);
  if (resolution == NIL || resolution == shiftPreAct) {
    return srResolution;
  }
  else if (srResolution == NIL) {
    return resolution;
  }
  else { /* Non-trivial resolving between srResolution and resolution. */
    assert(resolution != srResolution);
    assert(IS_REDUCE_PRE_ACT(resolution));
    assert(srResolution != NIL);
    assert(IS_SHIFT_PRE_ACT(srResolution) || IS_REDUCE_PRE_ACT(srResolution));
    return 
      (IS_SHIFT_PRE_ACT(srResolution))
      ? resolveSRConflict(actListsP, term, srResolution, resolution, doDefaultP)
      : resolveRRConflict(actListsP, term, srResolution, resolution);
  }
}

/* Resolve all conflicts on term on the list for term in Area
 * *actListsP.  Put the post-action into the acts component of the
 * header for the list for term in *actListsP.  Make *doDefaultP FALSE
 * if resolution is error because of %nonassoc.
 */
static VOID
resolveConflicts(actListsP, term, doDefaultP)
  Area *actListsP;
  Index term;
  Boolean *doDefaultP;
{
  Index i;
  Index i0;	/* Lags i */
  for (i0= term, i= ACT_LISTS(i0).succ;
       i != NIL && IS_TEST_PRE_ACT(ACT_LISTS(i).act);
       i0= i,  i= ACT_LISTS(i0).succ) {
  }
  /* i indexes first preact which is not a test. */
  if (i == NIL) { /* No non-test act. */
    if (i0 != term) { /* Have tests, no others.  Terminate with error act. */
      Index j= A_NEXT(*actListsP);
      assert(IS_TEST_PRE_ACT(ACT_LISTS(i0).act));
      ACT_LISTS(j).act= NIL; /* For error-rule. */
      ACT_LISTS(j).succ= NIL; ACT_LISTS(i0).succ= j;
    }
  }
  else if (ACT_LISTS(i).succ != NIL) { /* More than one non-test act. */
    PreAct resolution= ACT_LISTS(i).act;
    Index shiftPreAct= IS_SHIFT_PRE_ACT(resolution) ? resolution : NIL;
    Index i1;
    for (i1= ACT_LISTS(i).succ; i1 != NIL; i1= ACT_LISTS(i1).succ) {
      Index reducePreAct= ACT_LISTS(i1).act;
      assert(IS_REDUCE_PRE_ACT(reducePreAct));
      resolution= resolveConflictsLo(actListsP, term, shiftPreAct, 
				     reducePreAct, resolution, doDefaultP);
    }
    ACT_LISTS(i).act= resolution; ACT_LISTS(i).succ= NIL;
  } /* else did find a non-test act. */
  /* ACT_LISTS(i).act contains resolution. */

  /* Record post-action in header. */
  if (i0 != term) { /* We have 1 or more tests. */
    ACT_LISTS(term).act= 
      MAKE_TEST_POST_ACT(getTestSetN(actListsP, ACT_LISTS(term).succ));
  }
  else if (i == NIL || ACT_LISTS(i).act == NIL) { /* No actions at all. */
    ACT_LISTS(term).act= NIL;
  }
  else {
    PreAct preAct= ACT_LISTS(i).act;
    assert(IS_SHIFT_PRE_ACT(preAct) || IS_REDUCE_PRE_ACT(preAct));
    ACT_LISTS(term).act= PRE_TO_POST_ACT(preAct);
  }
  VOID_RET();
}

VOID
setExpect(nExpected)
  unsigned nExpected;
{
  G.nExpected= nExpected;
}

static VOID 
warnConflicts() 
{
  if (nSRConflicts + nRRConflicts != G.nExpected) {
    if (nSRConflicts > 0) {
      warn("%d shift-reduce conflict%s.", nSRConflicts, 
	   (nSRConflicts > 1) ? "s" : "");
    }
    if (nRRConflicts > 0) {
      warn("%d reduce-reduce conflict%s.", nRRConflicts, 
	   (nRRConflicts > 1) ? "s" : "");
    }
  }
  VOID_RET();
}


/*			CHOOSE DEFAULT REDUCTION			*/

/* Choose default reductions from actListsP with maximum terminals in
 * set terms.  We maintain a array of reduction counts; this array is
 * maintained using a linear search which should be ok as typically
 * the number of possible reductions in a state will be very small.
 * This routine should be called only when there is more than 1
 * reduction in a state.
 * Since test-sets can contain a default shift action, we never choose
 * the reduction corresponding to a test-set as a default reduction.
 */
static Index
chooseDefaultRuleN(actListsP, terms)
  Area *actListsP;
  ISet terms;
{
  typedef struct {
    Index2 act;		/* Reduction postact. */
    Count2 count;	/* # of terminals which reduce by this. */
  } ReductCount;
  Index maxReduct= 0;	/* Index in reductCount of reduction with max. count. */
  Count maxCount= 0;	/* # of terminals on which maxReduct fires. */
  Index nReducts= 0;	/* # of distinct reductions: #entries in reductCount. */
  Index t;
  ISetIter isIter;
  DCL_AUTO_BUF(ReductCount, 16) reductCount;
  INIT_AUTO_BUF(reductCount);
  FOR_EACH_IN_ISET(t, terms, isIter) {
    Index act= ACT_LISTS(t).act;
    if (act != NIL && IS_REDUCE_POST_ACT(act)) {
      Index i;
      for (i= 0; i < nReducts && AUTO_BUF(reductCount)[i].act != act; i++) {
	/* Linear search reductCount[] for act. */
      }
      if (i == nReducts) { /* Not found: create a new entry. */
	nReducts++;
	CHECK_AUTO_BUF(reductCount, nReducts);
	AUTO_BUF(reductCount)[i].act= act; AUTO_BUF(reductCount)[i].count= 1;
      }
      else { /* Increment count for old entry. */
	AUTO_BUF(reductCount)[i].count++;
      }
      if (AUTO_BUF(reductCount)[i].count > maxCount) {
	maxCount= AUTO_BUF(reductCount)[i].count; maxReduct= i;
      }
    }
  }
  FREE_AUTO_BUF(reductCount);
  return (nReducts > 0) 
         ? POST_ACT_REDUCTION_RULEN(AUTO_BUF(reductCount)[maxReduct].act)
         : G.nRules - 1;  /* Error rule. */
}


/*				BUILD ACTIONS				*/

/* Default actions: simply rule #s (not encoded). */
static Index2 *defaultActs;	

/* Read out actions for each terminal in set terms from area *actListsP
 * into ACTIONS[].  Reset actions in *actListsP and clear out bit-set terms.
 */
static VOID
setActions(actListsP, terms, defaultRuleN)
  Area *actListsP;
  ISet terms;
  Index defaultRuleN;
{
  CONST Index defaultPostAct= 
    defaultRuleN == NIL ? MAKE_REDUCE_POST_ACT(defaultRuleN) : NIL;
  Index t;
  ISetIter isIter;
  FOR_EACH_IN_ISET(t, terms, isIter) {
    if (ACT_LISTS(t).act != NIL)
      if (ACT_LISTS(t).act != defaultPostAct) {
	newAction(t, ACT_LISTS(t).act);
      }
    ACT_LISTS(t).act= ACT_LISTS(t).succ= NIL;
  }
  resetActLists(actListsP); diffISet(terms, terms);
  VOID_RET();
}


/* Build up actions for all states in ACTIONS[]. Decide on default rule #s
 * to reduce by in defaultActs[].
 */
static VOID
buildActions()
{
  CONST Count nStates= G.nStates;
  CONST Count nRules= G.nRules;
  CONST Index errRuleN= nRules - 1; 	/* Pseudo error-rule. */
  ISet stateTerms= newISet();	/* Active terminals for each state. */
  Area actLists;
  Index s;

  defaultActs= (Index2 *)MALLOC(nStates * sizeof(Index2));
  initActLists(&actLists);

  for (s= 0; s < nStates; s++) {
    CONST Index limit=	STATES(s).itemsHi[X_SI];/* Used as limits */
    CONST Index lo= STATES(s).itemsLo[X_SI];	/* for loop indexes. */
    Boolean doDefault= TRUE;	/* TRUE if state can have default redn. */
    Count nReductions= 0;	 /* # of reductions in s (not incl %tests). */
    Index defaultRuleN= errRuleN;/* Default rule to use in state s. */
    Index term;
    Index i;

    assert(sizeISet(stateTerms) == 0);

    STATES(s).actionsLo= A_NENTRIES(actions);

    if (limit == lo + 1) { /* Check for single reduction state. */
      CONST Index onlyItem= STATE_ITEMS(X_SI, lo).item;
      if (IS_REDUCING_ITEM(onlyItem)) {
	CONST Index ruleN= SYM_NUM(RHS_SYM(onlyItem));
	if (nonTermTest(RULE_NON_TERM(ruleN)) == NIL) { /* Not a test rule. */
	  CONST Index ruleLook= RULE_LOOK(ruleN);
	  defaultActs[s]= ruleN;
	  assert(STATES(s).actionsLo == A_NENTRIES(actions));
	  STATES(s).actionsHi= STATES(s).actionsLo; 
	  if (ruleLook <= 1) {
	    CONST LookStatus stateLook= getStateLookStatus(s);
	    if ((ruleLook == 0 && stateLook != LOOK_0) ||
		(ruleLook == 1 && stateLook != LOOK_1)) {
	      warn("Rule %d (%s) %s lookahead in state %d.",
		   ruleN, getNonTermString(ruleNonTerm(ruleN)), 
		   ruleLook == 0 ? "needs" : "does not need",
		   s);
	    }
	  }
	  continue; /* With next state. */
	}
      }
    }

    for (i= lo; i < limit; i++) { /* Loop thru terminal and reducing items. */
      CONST StateItem *CONST xP= &STATE_ITEMS(X_SI, i);
      CONST Index item= xP->item;
      Index term;
      assert(IS_TERMINAL_ITEM(item) || IS_REDUCING_ITEM(item));
      if (IS_TERMINAL_ITEM(item)) { /* Shift action. */
	CONST Index shiftAct= MAKE_SHIFT_PRE_ACT(xP->u.succ);
      	term= SYM_NUM(RHS_SYM(item));
	doDefault= doDefault && (term != G.errTerm);
	addISet(stateTerms, term);
	addActList(&actLists, term, shiftAct);
      }
      else { /* A reducing item: may be a %test. */
        CONST Index ruleN= SYM_NUM(RHS_SYM(item));
	Index testNum= nonTermTest(ruleNonTerm(ruleN));
        assert(IS_REDUCING_ITEM(item));
	if (testNum != NIL) { /* It is a %test. */
	  CONST Index testPreAct= MAKE_TEST_PRE_ACT(testNum);
          ISet lalrLookSet= lalrLook(i);
	  ISetIter isIter;
	  assert(STATE_ITEMS(X_SI, i).u.lalrLook == BAD_ISET);
	  STATE_ITEMS(X_SI, i).u.lalrLook= lalrLookSet;
	  FOR_EACH_IN_ISET(term, lalrLookSet, isIter) { 
	    addISet(stateTerms, term);
	    addActList(&actLists, term, testPreAct);
	  }
	}
	else { /* plain reduction. */
	  CONST Index reducePreAct= MAKE_REDUCE_PRE_ACT(i);
	  ISet lookSet= nonTermFollows[RULE_NON_TERM(ruleN)];
	  ISetIter isIter;
	  assert(IS_REDUCING_ITEM(item));
	  assert(nonTermTest(ruleNonTerm(SYM_NUM(RHS_SYM(item)))) == NIL);
	  assert(xP->u.lalrLook == BAD_ISET);
	  defaultRuleN= ruleN; nReductions++;
	  if (RULE_LOOK(ruleN) == 0) {
	    warn("Rule %d (%s) requires lookahead in state %d.",
		 ruleN, getNonTermString(ruleNonTerm(ruleN)), s);
	  }
	  FOR_EACH_IN_ISET(term, lookSet, isIter) {
	    addISet(stateTerms, term);
	    addActList(&actLists, term, reducePreAct);
	  }
	} /* else plain reduction. */
      } /* else a reducing item: may be a %test. */
    } /* for (i= lo; i < limit; i++) */

    { ISetIter isIter;
      FOR_EACH_IN_ISET(term, stateTerms, isIter) {
	resolveConflicts(&actLists, term, &doDefault);
      }
    }
    /* The actions recorded so far are loosing conflict actions. */
    STATES(s).nConflicts= A_NENTRIES(actions) - STATES(s).actionsLo;

    if (!doDefault) { /* Had shift on error, or error act due to %nonassoc. */
      defaultRuleN= errRuleN;
    }
    else if (nReductions > 1) {
      defaultRuleN= chooseDefaultRuleN(&actLists, stateTerms);
    }
    defaultActs[s]= defaultRuleN;
    
    setActions(&actLists, stateTerms, defaultRuleN);
    STATES(s).actionsHi= A_NENTRIES(actions); /* All actions recorded. */

  } /* for (s= 0; s < nStates; s++) */

  delArea(&actLists);
  delISet(stateTerms);
  warnConflicts();
  VOID_RET();
}

static VOID 
initLALR()
{
  initDigraph(A_NENTRIES(stateItems[N_SI]), includesSucc, lalrF, 
	      NULL, &lalrDigraph);
  initTests();
  VOID_RET();
}

	
/*			   COMPUTE GOTOS				*/

/* 
Compute goto-actions in a form suitable for table compression.  For
each non-term, compute its default goto to the most common successor
state. Put defaults into defaultGotos[] vector, rest into Area gotos,
indexed via gotoActs[].
*/

typedef Entry Goto;

static AREA (gotos, Goto, 10, UINT2_MAX);
#define GOTOS(i) A_ACCESS(gotos, Goto, i)

typedef struct {
  Index2 gotosLo;
  Index2 gotosHi;
} GotoAct;

static GotoAct *gotoActs;

static Index2 *defaultGotos;

static VOID 
buildGotos()
{
  CONST Count nNonTerms= G.nNonTerms;
  CONST Count nStates= G.nStates;
  Index nonTerm;
  Count2 *succCounts;
  gotoActs= (GotoAct *)MALLOC(nNonTerms * sizeof(GotoAct));
  defaultGotos= (Index2 *)MALLOC(nNonTerms * sizeof(Index2));
  succCounts= (Count2 *)CALLOC(nStates,  sizeof(Count2));
  for (nonTerm= 0; nonTerm < nNonTerms; nonTerm++) {
    Index defaultSucc= 0;	/* Default successor state so far. */
    Index defaultCount= 0;	/* # of occurrences of defaultSucc so far. */
    Count nActs=0;		/* # of actions to be inserted. */
    CONST Index gotosLo= A_NENTRIES(gotos);
    Index lastSrc= NIL;
    Index i;
    for (i= 0; i < nStates; i++) assert(succCounts[i] == 0);
    i= nonTermTrans[nonTerm].chain; 
    while(i != NIL) { /* Pick default successor. */
      CONST Index srcState= STATE_ITEMS(N_SI, i).state;
      CONST Index succState= STATE_ITEMS(N_SI, i).u.succ;
      do { 
	i= STATE_ITEMS(N_SI, i).u1.nonTermChain; 
      } while (i != NIL && STATE_ITEMS(N_SI, i).state == srcState);
      if (++succCounts[succState] > defaultCount) {
	defaultCount= succCounts[succState]; defaultSucc= succState;
      }
      nActs++;
      assert(lastSrc == NIL || lastSrc > srcState); /* Sorted by srcState. */
      assert((lastSrc= srcState) != NIL); /* Kill assignment when NDEBUG. */
    }
    defaultGotos[nonTerm]= defaultSucc;

    nActs-= defaultCount;	/* Actions will not include default. */
    gotoActs[nonTerm].gotosLo= gotosLo;
    for (i= 0; i < nActs; i++) A_NEXT(gotos); /* Create gotos[] entries. */
    gotoActs[nonTerm].gotosHi= A_NENTRIES(gotos);

    i= nonTermTrans[nonTerm].chain; 
    while (i != NIL) { /* Build up goto actions, filtering default succ. */
      CONST Index srcState= STATE_ITEMS(N_SI, i).state;
      CONST Index succState= STATE_ITEMS(N_SI, i).u.succ;
      do { 
	i= STATE_ITEMS(N_SI, i).u1.nonTermChain; 
      } while (i != NIL && STATE_ITEMS(N_SI, i).state == srcState);
      succCounts[succState]= 0; 
      if (succState != defaultSucc) {
	Index j= gotosLo + nActs - 1;
	GOTOS(j).input= srcState; GOTOS(j).value= succState;
	nActs--;
      }
    }
    assert(nActs == 0);
  }
  
  FREE(succCounts);
  VOID_RET();
}


/* 			COMPRESS TABLES.				*/

typedef struct {
  Index2 index;		/* Original index: used to prevent equal compare. */
  Index2 val;		/* The base index. */
  Boolean isNBase;	/* True if from nBase[]. */
} Base;

/* Comparison routine for qsort().  Compares Index pointed to by arg.
 * pointers.  Never return 0 to avoid implementation-dependent results
 * from qsort() not being stable: when two values are equal compare
 * using index component.
 */
static int 
baseCmp(p1, p2)
  CONST VOIDP p1;
  CONST VOIDP p2;
{
  CONST Base *bP1= ((CONST Base *)p1);
  CONST Base *bP2= ((CONST Base *)p2);
  Index v1= bP1->val;
  Index v2= bP2->val;
  return 
    (v1 < v2) ? -1 : ((v1 > v2) ? 1 : ((bP1->index < bP2->index) ? -1 : 1));
}

/* This routine fills the remaining entries in the base arrays with
 * the smallest base value which does not occur in either of the two
 * tBase[] and nBase[] arrays and returns the fill value via
 * *fillValP.  It also returns 1 greater than the max. legal index
 * into the next[]/check[] array using the base values in tBase[] and
 * nBase[]. 
 */
static Index 
adjustBase(tBase, tOrder, tBaseNEntries, nBase, nOrder,
	   nBaseNEntries, fillValP)
  Index2 *CONST tBase;
  CONST Index2 *CONST tOrder;
  Count tBaseNEntries;
  Index2 *nBase;
  CONST Index2 *CONST nOrder;
  Count nBaseNEntries;
  Index2 *fillValP;
{
  CONST Count nTerms= G.nTerms;
  CONST Count nNonTerms= G.nNonTerms;
  CONST Count nStates= G.nStates;
  CONST Count nEntries= (tBaseNEntries + nBaseNEntries);
  Index maxIndex1= 0, maxTIndex1= 0, maxNIndex1= 0, maxFillIndex1= 0;
  Base *bases= (Base *)MALLOC(nEntries * sizeof(Base));
  Index fillVal;
  Index i;
  for (i= 0; i < tBaseNEntries; i++) {
    bases[i].val= tBase[tOrder[i]]; bases[i].isNBase= FALSE;
    bases[i].index= i;
  }
  for (i= tBaseNEntries; i < nEntries; i++) {
    bases[i].val= nBase[nOrder[i - tBaseNEntries]]; bases[i].isNBase= TRUE;
    bases[i].index= i;
  }
  qsort(bases, nEntries, sizeof(Base), baseCmp);
  fillVal= bases[nEntries - 1].val + 1; /* In case all entries are identical. */
  for (i= 1; i < nEntries; i++) { /* Scan for 1st missing base value. */
    if (bases[i].val - bases[i - 1].val > 1) {
      fillVal= bases[i - 1].val + 1; break;
    }
  }
  for (i= tBaseNEntries; i < nStates; i++) tBase[tOrder[i]]= fillVal;
  for (i= nBaseNEntries; i < nNonTerms; i++) nBase[nOrder[i]]= fillVal;
  for (i= nEntries; i > 0; i--) {
    if (!bases[i-1].isNBase) { 
      maxTIndex1= bases[i - 1].val + nTerms; break; 
    }
  }
  for (i= nEntries; i > 0; i--) {
    if (bases[i-1].isNBase) { 
      maxNIndex1= bases[i - 1].val + nStates; break; 
    }
  }
  maxIndex1= (maxNIndex1 > maxTIndex1) ? maxNIndex1 : maxTIndex1;
  maxFillIndex1= fillVal + ((nStates > nTerms) ? nStates : nTerms);
  if (maxFillIndex1 > maxIndex1) maxIndex1= maxFillIndex1;
  FREE(bases);
  *fillValP= fillVal; return maxIndex1;
}
			

/* Compare two states: first via width of transitions and then via number
 * of non-default transitions.  The idea of using the width comes from 
 * bison.
 * Never return 0 to avoid implementation-dependent results
 * from qsort() not being stable: when two values are equal return
 * comparison via state #s.
 */
static int 
tTableCmp(p1, p2)
  CONST VOIDP p1;
  CONST VOIDP p2;
{
  CONST Index s1= *((Index2 *)p1);
  CONST Index s2= *((Index2 *)p2);
  CONST State *CONST sP1= &STATES(s1);
  CONST State *CONST sP2= &STATES(s2);
  CONST Index s1ActsLo= sP1->actionsLo + sP1->nConflicts;
  CONST Index s2ActsLo= sP2->actionsLo + sP2->nConflicts;
  CONST Count nActs1= sP1->actionsHi - s1ActsLo;
  CONST Count nActs2= sP2->actionsHi - s2ActsLo;
  if (nActs1 > 0 && nActs2 > 0) {
    CONST Size w1= 
      ACTIONS(sP1->actionsHi - 1).input - ACTIONS(s1ActsLo).input + 1;
    CONST Size w2= 
      ACTIONS(sP2->actionsHi - 1).input - ACTIONS(s2ActsLo).input + 1;
    if (w1 < w2) return 1;
    else if (w1 > w2) return -1;
  }
  return (nActs1 < nActs2) ? 1 : (nActs1 > nActs2) ? -1 : (s1 < s2) ? -1 : 1;
}

/* Compare two nonterms: first via width of transitions and then via number
 * of non-default transitions.  The idea of using the width comes from 
 * bison.
 * Never return 0 to avoid implementation-dependent results
 * from qsort() not being stable: when two values are equal use comparison
 * of goto indexes.
 */
static int 
nTableCmp(p1, p2)
  CONST VOIDP p1;
  CONST VOIDP p2;
{
  CONST Index g1= *((Index2 *)p1);
  CONST Index g2= *((Index2 *)p2);
  CONST GotoAct *CONST gP1= &gotoActs[g1];
  CONST GotoAct *CONST gP2= &gotoActs[g2];
  CONST Index g1ActsLo= gP1->gotosLo;
  CONST Index g2ActsLo= gP2->gotosLo;
  CONST Count nActs1= gP1->gotosHi - g1ActsLo;
  CONST Count nActs2= gP2->gotosHi - g2ActsLo;
  if (nActs1 > 0 && nActs2 > 0) {
    CONST Size w1= 
      GOTOS(gP1->gotosHi - 1).input - GOTOS(g1ActsLo).input + 1;
    CONST Size w2= 
      GOTOS(gP2->gotosHi - 1).input - GOTOS(g2ActsLo).input + 1;
    if (w1 < w2) return 1;
    else if (w1 > w2) return -1;
  }
  return (nActs1 < nActs2) ? 1 : (nActs1 > nActs2) ? -1 : (g1 < g2) ? -1 : 1;
}

static struct {		/* Table information saved here. */
  Index2 *tBase;	/* Base tables for terminal shift/reduce actions. */
  Index2 *nBase;	/* Base table for goto actions. */
  Index2 *next;		/* Action array for shift/reduce/goto actions. */
  Index2 *check;	/* Check array for above next array. */	
  Index2 nextSize;	/* Number of entries in next[]. */
  Index2 checkSize;	/* Number of entries in check[]. */
  Index2 badBase;	/* Bad base value. */
} globTabs;

static Index2 *
adjustCheck(oldCheck, currentCheckSize, newCheckSize)
  Index2 *oldCheck;
  CONST Size currentCheckSize;
  CONST Size newCheckSize;
{  
  CONST Index nStates= G.nStates;
  CONST Count nTerms= G.nTerms;
  CONST Index badCheck= nStates > nTerms ? nStates : nTerms;
  Index i;
  Index2 *check= 
    (Index2 *)REALLOC((VOIDP) oldCheck, newCheckSize * sizeof(Index2));
  for (i= currentCheckSize; i < newCheckSize; i++) check[i]= badCheck;
  return check;
}

static VOID 
compressTables()
{
  Index s, n;
  CompTab compTab;
  CONST Count nTerms= G.nTerms;
  CONST Count nNonTerms= G.nNonTerms;
  CONST Count nStates= G.nStates;
  Index2 *CONST order= 
    (Index2 *)MALLOC((nStates + nNonTerms) * sizeof(Index2));
  Index2 *CONST tOrder= &order[0];
  Index2 *CONST nOrder= &order[nStates];
  globTabs.tBase= (Index2 *)MALLOC(nStates * sizeof(Index2));
  globTabs.nBase= (Index2 *)MALLOC(nNonTerms * sizeof(Index2));
  initCompTab(&compTab, nStates > nTerms ? nStates : nTerms);

  /* Compress shift/reduce actions. */
  for (s= 0; s < nStates; s++) tOrder[s]= s;
  qsort(tOrder, nStates, sizeof(Index2), tTableCmp);
  for (s= 0; s < nStates; s++) {
    CONST Index state= tOrder[s];
    CONST State *sP= &STATES(state);
    CONST Count nEntries= sP->actionsHi - (sP->actionsLo + sP->nConflicts);
    if (nEntries == 0) break;
    globTabs.tBase[state]= 
      getRowBaseCompTab(&compTab, &ACTIONS(sP->actionsLo + sP->nConflicts),
			nEntries);
#ifdef TEST_COMP
    printf("%3d %4d %5d\n", state, nEntries, globTabs.tBase[state]);
#endif
  }

  /* Compress goto tables. */
  for (n= 0; n < nNonTerms; n++) nOrder[n]= n;
  qsort(nOrder, nNonTerms, sizeof(Index2), nTableCmp);
  for (n= 0; n < nNonTerms; n++) {
    CONST Index nonTerm= nOrder[n];
    CONST GotoAct *gP= &gotoActs[nonTerm];
    CONST Count nEntries= gP->gotosHi - gP->gotosLo;
    if (nEntries == 0) break;
    globTabs.nBase[nonTerm]= 
      getRowBaseCompTab(&compTab, &GOTOS(gP->gotosLo), nEntries);
#ifdef TEST_COMP
    printf("%3d %4d %5d\n", nonTerm, nEntries, globTabs.nBase[nonTerm]);
#endif
  }
  globTabs.nextSize= 
    trimCompTab(&compTab, &globTabs.next, &globTabs.check);
  globTabs.checkSize= adjustBase(globTabs.tBase, tOrder, s, 
				 globTabs.nBase, nOrder, n, 
				 &globTabs.badBase);
  assert(globTabs.nextSize <= globTabs.checkSize);

  FREE(order);

  globTabs.check= adjustCheck(globTabs.check, globTabs.nextSize, 
			      globTabs.checkSize);
  VOID_RET();
}


/*			TOP-LEVEL LALR ROUTINE.				*/

VOID doLALR()
{
  initGlobs();
  computeRestRuleNullables();
  computeLR0();
  computeFollows();
  initLALR();
  buildActions();
  buildGotos();
  compressTables();
  VOID_RET();
}

#ifdef TEST_COMP


#include <stdio.h>

static VOID reportCompTab PROTO((VOID_ARGS));
int main PROTO((int argc, char **argv));

static VOID 
reportCompTab()
{
  CONST Count size= globTabs.nextSize;
  CONST Count nStates= G.nStates;
  CONST Count nTerms= G.nTerms;
  CONST Count nCols= (nStates > nTerms) ? nStates : nTerms;
  CONST Index2 *CONST check= globTabs.check;
  Count nFree= 0;
  Index i;
  for (i= 0; i < size; i++) {
    if (check[i] >= nCols) nFree++;
  }
  printf("Bad base= %d; Check size= %d.\n", 
	 globTabs.badBase, globTabs.checkSize);
  printf("Next size: %d; %d wasted (%5.1f%% wastage).\n",
	 size, nFree, ((double)nFree * 100.0)/size);
  VOID_RET();
}

#include "scan.h"

int 
main(argc, argv)
  int argc;
  char **argv;
{
  initScan(argc > 1 ? argv[1] : NULL);
  doGram();
  doLALR();
  /*  if (argc <= 2) doVerbose(stdout); */
  reportCompTab();
  return 0;
}


#endif /* ifdef TEST_COMP */

/*			     OUTPUT TABLE ROUTINE.			*/

/* Return s with any occurrences of '\\' and '"' quoted.  Uses a static
 * buffer --- hence returned string is valid only till next call.
 */
static ConstString 
quote(s)
  ConstString s;
{
  static struct {
    Char *buf;
    Size size;
  } quoteBuf= { NULL, 0 };
  Index len= 0;
  enum { BUF_INC= 16 };
  ConstString p;
  for (p= s; *p; p++) {
    if (quoteBuf.size < len + 3) { /* 3 == 1 '\\' + 1 char + 1 '\0'. */
      quoteBuf.buf= 
	(Char *) REALLOC((VOIDP) quoteBuf.buf, (quoteBuf.size+= BUF_INC));
    }
    if (*p == '\\' || *p == '"') quoteBuf.buf[len++]= '\\';
    quoteBuf.buf[len++]= *p;
  }
  quoteBuf.buf[len]= '\0';
  return quoteBuf.buf;
}

static ConstString 
getQuotedTermName(term)
  Index term;
{
  return quote(getTermString(term));
}

static Index 
ruleRHSLo(ruleN)
  Index ruleN;
{
  return RULE_RHS_LO(ruleN);
}

static Index 
ruleLen(ruleN)
  Index ruleN;
{
  return RULE_RHS_HI(ruleN) - RULE_RHS_LO(ruleN);
}

static Index 
getRuleLHS(ruleN)
  Index ruleN;
{
  return RULE_NON_TERM(ruleN);
}

static Index 
getRHSSymLocal(rhsIndex)
  Index rhsIndex;
{
  return getRHSSym(rhsIndex);
}

Count 
outGramTabs(outFile)
  FILE *outFile;
{
  Count lineN= 0;
  fputs("#if YY_ZYACC_DEBUG\n\n", outFile); lineN+= 2;

  lineN+= outSFnVec(outFile, "YY_CONST char *yyTermNames", 
		    G.nTerms, getQuotedTermName);
  lineN+= outSFnVec(outFile, "YY_CONST char *yyNonTermNames", G.nNonTerms, 
		    getNonTermString);
  lineN+= outIFnVec(outFile, "YY_CONST YYXRHS yyRHS", G.nRules, ruleRHSLo);
  lineN+= outIFnVec(outFile, "YY_CONST YYXNonTerm yyLHS1", 
		    G.nRules, getRuleLHS);
  lineN+= outIFnVec(outFile, "YY_CONST YYXSym yySyms", 
		    G.nRHSSyms, getRHSSymLocal);
  lineN+= outIFnVec(outFile, "YY_CONST YYXSym yyAccess", 
		    G.nStates, getStateAccessSym);
  lineN+= outIFnVec(outFile, "YY_CONST YYLineNum yyLineNums", G.nRules,
		    ruleLineNum);

  fputs("#endif /* #if YY_ZYACC_DEBUG */\n\n", outFile); lineN+= 2;

  { 
    Index2 nEntries;
    CONST Index2 *translateTab= computeTranslateTab(&nEntries);  
    lineN+= outIValVec(outFile, "YYTok yyTranslate", 
		       nEntries, translateTab);
    freeTranslateTab(translateTab);
  }
  lineN+= outIFnVec(outFile, "YYLen yyLength", G.nRules, ruleLen);
  lineN+= outIFnVec(outFile,  "YYNonTerm yyLHS", G.nRules, getRuleLHS);
  lineN+= outIValVec(outFile,  "YYBase yyActBase", 
		     G.nStates, globTabs.tBase);
  lineN+= outIValVec(outFile,  "YYRuleN yyActDef", G.nStates, defaultActs);
  lineN+= outIValVec(outFile,  "YYBase yyGotoBase", G.nNonTerms, 
		     globTabs.nBase);
  lineN+= outIValVec(outFile,  "YYState yyGotoDef", 
		     G.nNonTerms, defaultGotos);
  lineN+= outIValVec(outFile,  "YYAct yyNext", globTabs.nextSize, 
		     globTabs.next);
  lineN+= outIValVec(outFile,  "YYCheck yyCheck", globTabs.checkSize, 
		     globTabs.check);
  if (nTests()) {
    lineN+= outIFnVec(outFile, "YYTest yyTests", nTestSets(), 
		      testSetsTest);
    lineN+= outIFnVec(outFile,  "YYAct yyTestActs", nTestSets(),
		      testSetsAct);
  }

  return lineN;
}


/*			OUTPUT LALR PARAMETERS.				*/

Count 
outLALRParams(outFile)
  FILE *outFile;
{
  Count lineCount= 0;

  fprintf(outFile, "#define YY_BAD_BASE %d\n", globTabs.badBase); 
  lineCount++;

  fprintf(outFile, "#define YY_N_CHECK %d\n", globTabs.checkSize);
  lineCount++;

  fprintf(outFile, "#define YY_N_NEXT %d\n", globTabs.nextSize);
  lineCount++;

  fprintf(outFile, "#define YY_N_STATES %d\n", G.nStates);
  lineCount++;

  fprintf(outFile, "#define YY_TESTS_SIZE %d\n", nTestSets()); 
  lineCount++;

  return lineCount;
}




/*			VISUALIZATION USING HTML.			*/

/*

Output HTML Level 1 document with hypertext links as follows:

Each grammar symbol points to a list of states accessed by that symbol.

Each rule nnn points to the rule whose # is nnn.

Each shift n or state n points to state n.

Each reduce points to lookaheads for that reduction.

Each #ACT nn points to code for action nn.

Each test set # points to lists of tests and default test action.

Each test nn points to code for test nn.

It would be nice to redo this code using a more interpretive approach
so that the code may get shorter.

*/

/* Formats for HTML names used in links.  A name consists of a
 * number(s) substituted into a template.  The template is meaningful
 * as it may be shown by the browser to the user as the mouse moves
 * over it.
 */
#define NT_STATE_LIST_FMT						\
  "List-of-states-accessed-by-non-terminal-%d"
#define T_STATE_LIST_FMT						\
  "List-of-states-accessed-by-terminal-%d"
#define RULE_FMT							\
  "Rule-%d"
#define REDUCE_FMT							\
  "Lookaheads-for-reducing-by-%d-in-state-%d"
#define ACT_FMT								\
  "Act-%d"
#define TEST_SET_FMT							\
  "Testset-%d"
#define TEST_FMT							\
  "Test-%d"
#define STATE_FMT							\
  "State-%d-without-closure-items"
#define CLOSURE_STATE_FMT						\
  "State-%d-with-closure-items"

/* Max. length of string appended to the baseName to form the rootName for
 * different HTML files.
 */
#define X_LEN		2

/* Macro to keep all HTML files information in synch with corresponding
 * enum.
 */
#define HTML_FILES_INFO \
  HTML_MACRO(MAIN_HTML, printHTMLMain, 					\
	     "Parser Description File",	"%.*s.%s"),			\
  HTML_MACRO(RULES_HTML, printHTMLAllRules, 				\
	     "Rules", "%.*s_1.%s"),					\
  HTML_MACRO(NC_STATES_HTML, printHTMLAllStatesNoClosure, 		\
	     "States Without Closure Items", "%.*s_2.%s"),		\
  HTML_MACRO(C_STATES_HTML, printHTMLAllStatesClosure,	 		\
	     "States With Closure Items", "%.*s_3.%s"),			\
  HTML_MACRO(ACCESS_HTML, printHTMLAccessedStates,			\
	     "States Accessed by Grammar Symbols", "%.*s_4.%s"),	\
  HTML_MACRO(REDUCTION_LOOKS_HTML, printHTMLReduceLooks,		\
	     "Reduction Lookaheads", "%.*s_5.%s"),			\
  HTML_MACRO(ACTS_HTML, printHTMLUserActs,				\
	     "User Specified Actions",  "%.*s_6.%s"),			\
  HTML_MACRO(TESTS_HTML, printHTMLTests,				\
	     "User Specified Tests", "%.*s_7.%s")			\

/* Use HTML_FILES_INFO to define a enum used to index htmlFiles[] array. */
#undef HTML_MACRO
#define HTML_MACRO(a, b, c, d)	a
enum { 
  HTML_FILES_INFO
};

/* Pointer to this struct is passed to routines: this provides the
 * first two arguments needed by the nameFmt member within a htmlFiles[]
 * entry.
 */
typedef struct NameWithLen {
  ConstString name;		/* A string. */
  Size len;			/* # of significant characters for name. */
} NameWithLen;

typedef VOID (*PrintFnP) PROTO((FILE *outFile, NameWithLen *nameWithLenP));

typedef struct {	/* Information stored for each HTML file. */
  ConstString nameFmt;	/* Format specifier for file name: int + 2 strings. */
  ConstString title;	/* Title for html file. */
  PrintFnP printFnP;	/* Function to print out html file information. */
} HTMLFile;

/* Use HTML_FILES_INFO to define htmlFiles[] array containing information
 * for each generated html file. 
 */
#undef HTML_MACRO
#define HTML_MACRO(a, b, c, d) 	{ d, c, b }
static HTMLFile htmlFiles[]= {
  HTML_FILES_INFO
};  

#define OUT_HTML_FILE_NAME(outFile, htmlFileIndex, nameWithLenP)	\
  fprintf(outFile, htmlFiles[htmlFileIndex].nameFmt,			\
	  nameWithLenP->len, nameWithLenP->name, HTML_EXT)

#if MSDOS | _MSDOS | __MSDOS__ | __TURBOC__
#define HTML_EXT		"htm"
#define BASE_LEN(n)		(((n) + X_LEN > 8) ? 8 - X_LEN : (n))
#else
#define HTML_EXT		"html"
#define BASE_LEN(n)		(n)
#endif

/* Quote HTML special characters. */
static VOID
printHTMLString(outFile, s)
  FILE *outFile;
  ConstString s;
{
  ConstString p;
  for (p= s; *p != '\0'; p++) {
    Int c= *p;
    switch (c) {
      case '<':
        fputs("&lt;", outFile);
	break;
      case '>':
	fputs("&gt;", outFile);
	break;
      case '&':
	fputs("&amp;", outFile);
	break;
      default:
	fputc(c, outFile);
	break;
      }
  } /* for (p= s; *p != '\0'; p++) */
  VOID_RET();
}

/*
*** NOTE ***
We assume that extra arguments to the printf() family are simply ignored.
*/

typedef char Scratch[64];

/* Output textFmt as a definition of nameFmt.  Quote any HTML special
 * characters.  The Fmt arguments are printf-format-string templates
 * into which the refNum1 argument is substituted.
 */
static VOID
printHTMLDef1(outFile, textFmt, nameFmt, refNum, nameWithLenP)
  FILE *outFile;
  ConstString textFmt;
  ConstString nameFmt;
  Index refNum;
  NameWithLen *nameWithLenP;
{
  if (nameWithLenP) {
    Scratch scratch;
    fputs("<a name= \"", outFile);  
    fprintf(outFile, nameFmt, refNum);  
    fputs("\">", outFile);
    sprintf(scratch, textFmt, refNum);
    printHTMLString(outFile, scratch);
    fputs("</a>", outFile);
  }
  else {
    fprintf(outFile, textFmt, refNum);
  }
  VOID_RET();
}

/* Output textFmt as a reference to nameFmt.  Quote any HTML special
 * characters.  The Fmt arguments are printf-format-string templates
 * into which the refNum1 argument is substituted.
 */
static VOID
printHTMLRef1(outFile, refdFileIndex, textFmt, nameFmt, refNum, nameWithLenP)
  FILE *outFile;
  Index refdFileIndex;
  ConstString textFmt;
  ConstString nameFmt;
  Index refNum;
  NameWithLen *nameWithLenP;
{
  if (nameWithLenP) {
    Scratch scratch;
    fputs("<a href= \"", outFile);  
    OUT_HTML_FILE_NAME(outFile, refdFileIndex, nameWithLenP);
    fputs("#", outFile);
    fprintf(outFile, nameFmt, refNum);  
    fputs("\">", outFile);
    sprintf(scratch, textFmt, refNum);
    printHTMLString(outFile, scratch);
    fputs("</a>", outFile);
  }
  else {
    fprintf(outFile, textFmt, refNum);
  }
  VOID_RET();
}

/* Output textFmt as a definition of nameFmt.  Quote any HTML special
 * characters.  The Fmt arguments are printf-format-string templates
 * into which the refNum arguments are substituted.
 */
static VOID
printHTMLDef2(outFile, textFmt, nameFmt, refNum1, refNum2, nameWithLenP)
  FILE *outFile;
  ConstString textFmt;
  ConstString nameFmt;
  Index refNum1;
  Index refNum2;
  NameWithLen *nameWithLenP;
{
  if (nameWithLenP) {
    Scratch scratch;
    fputs("<a name= \"", outFile);  
    fprintf(outFile, nameFmt, refNum1, refNum2);  
    fputs("\">", outFile);
    sprintf(scratch, textFmt, refNum1, refNum2);
    printHTMLString(outFile, scratch);
    fputs("</a>", outFile);
  }
  else {
    fprintf(outFile, textFmt, refNum1, refNum2);
  }
  VOID_RET();
}

/* Output textFmt as a reference to nameFmt.  Quote any HTML special
 * characters.  The Fmt arguments are printf-format-string templates
 * into which the refNum arguments are substituted.
 */
static VOID
printHTMLRef2(outFile, refdFileIndex, textFmt, nameFmt, 
	      refNum1, refNum2, nameWithLenP)
  FILE *outFile;
  Index refdFileIndex;
  ConstString textFmt;
  ConstString nameFmt;
  Index refNum1;
  Index refNum2;
  NameWithLen *nameWithLenP;
{
  if (nameWithLenP) {
    Scratch scratch;
    fputs("<a href= \"", outFile);  
    OUT_HTML_FILE_NAME(outFile, refdFileIndex, nameWithLenP);
    fputs("#", outFile);
    fprintf(outFile, nameFmt, refNum1, refNum2);  
    fputs("\">", outFile);
    sprintf(scratch, textFmt, refNum1, refNum2);
    printHTMLString(outFile, scratch);
    fputs("</a>", outFile);
  }
  else {
    fprintf(outFile, textFmt, refNum1, refNum2);
  }
  VOID_RET();
}

static VOID
printHTMLHeading(outFile, heading, level, nameWithLenP)
  FILE *outFile;
  ConstString heading;
  Index level;
  NameWithLen *nameWithLenP;
{
  if (nameWithLenP) {
    fprintf(outFile, "<h%d>\n%s\n</h%d>\n\n", level, heading, level);
  }
  else {
    enum { LINE_WIDTH= 72 };
    Size nSpace= (LINE_WIDTH - strlen(heading))/2;
    ConstString p;
    fprintf(outFile, "%*s", nSpace, "");
    for (p= heading; *p != '\0'; p++) putc(toupper(*p), outFile);
    fputs("\n\n", outFile);
  }
  VOID_RET();
}

#define BEGIN_SECTION(outFile, heading, nameWithLenP)			\
  do {									\
    printHTMLHeading(outFile, heading, 2, nameWithLenP);		\
    if (nameWithLenP) fputs("<PRE>\n", outFile);			\
  } while (0)

#define END_SECTION(outFile, nameWithLenP)				\
  do {									\
    if (nameWithLenP) fputs("</PRE>\n", outFile);			\
  } while (0)

static VOID
printHTMLRule(outFile, ruleN, itemN, isDef, nameWithLenP)
  FILE *outFile;
  Index ruleN;
  Index itemN;
  BooleanX isDef;
  NameWithLen *nameWithLenP;
{
  enum { MAX_LINE_WIDTH= 72, RHS_INDENT_LEN= 8 };
  ConstString rhsIndent= "        ";	/* RHS_INDENT_LEN spaces. */
  CONST Index lhsNonTerm= ruleNonTerm(ruleN);
  Size lhsLen;
  ConstString lhsNonTermString= 
    getNonTermStringWithLen(lhsNonTerm, &lhsLen);
  CONST Index lo= RULE_RHS_LO(ruleN);
  CONST Index hi= RULE_RHS_HI(ruleN);
  Index colN;
  Index i;
  assert(itemN == NIL || (lo <= itemN && itemN <= hi));
  if (isDef) {
    printHTMLDef1(outFile, "%3d: ", RULE_FMT, ruleN, nameWithLenP);
  }
  else {
    fprintf(outFile,"%3d: ", ruleN);
  }
  colN= 5;
  printHTMLRef1(outFile, ACCESS_HTML, lhsNonTermString, 
		NT_STATE_LIST_FMT, lhsNonTerm, nameWithLenP);
  colN+= lhsLen;
  fputs(": ", outFile); colN+= 2;
  if (lo == hi) {
    fprintf(outFile, "%s/* EMPTY */", itemN == NIL ? "" : ". ");
  }
  else {
    for (i= lo; i < hi; i++) {
      Sym sym= RHS_SYM(i);
      CONST SymType type= SYM_TYPE(sym);
      CONST Index symNum= SYM_NUM(sym);
      Size symLen;
      ConstString symString= (type == TERM_SYM) 
	                     ? getTermStringWithLen(symNum, &symLen) 
		             : getNonTermStringWithLen(symNum, &symLen);
      ConstString nameFmt= (type == TERM_SYM) 
                           ? T_STATE_LIST_FMT 
	                   : NT_STATE_LIST_FMT;
      assert(SYM_TYPE(sym) == TERM_SYM || SYM_TYPE(sym) == NON_TERM_SYM);
      if (colN + (itemN == i) * 2 + symLen >= MAX_LINE_WIDTH) {
	fprintf(outFile, "\n%s", rhsIndent); colN= RHS_INDENT_LEN;
      }
      if (itemN == i) fputs(". ", outFile); colN+= 2;
      printHTMLRef1(outFile, ACCESS_HTML, symString, nameFmt, 
		    symNum, nameWithLenP); 
      colN+= symLen;
      fputs(" ", outFile); colN+= 1;
    }
  }
  if (itemN == hi) { fputs(". ", outFile); colN+= 2; }
  if (RULE_ACTN(ruleN) != NIL) {
    if (colN + 5 + 3 >= MAX_LINE_WIDTH) {
      fprintf(outFile, "\n%s", rhsIndent); colN= RHS_INDENT_LEN;
    }
    printHTMLRef1(outFile, ACTS_HTML, "#Act_%d", ACT_FMT, 
		  RULE_ACTN(ruleN), nameWithLenP);
  }
  fputs("\n", outFile);
  VOID_RET();
}

static VOID printHTMLAllRules(outFile, nameWithLenP)
  FILE *outFile;
  NameWithLen *nameWithLenP;
{
  CONST Count nRules= G.nRules;
  Index r;
  if (nameWithLenP) fputs("<PRE>\n", outFile);
  for (r= 0; r < nRules; r++) {
    printHTMLRule(outFile, r, NIL, TRUE, nameWithLenP);
  }
  if (nameWithLenP) fputs("</PRE>\n", outFile);
  VOID_RET();
}

static VOID 
printHTMLAction(outFile, postAct, state, nameWithLenP)
  FILE *outFile;
  Index postAct;
  Index state;
  NameWithLen *nameWithLenP;
{
  if (IS_SHIFT_POST_ACT(postAct)) {
    CONST Index shiftState= POST_ACT_SHIFT(postAct);
    printHTMLRef1(outFile, NC_STATES_HTML, "SHIFT %d", STATE_FMT, 
		  shiftState, nameWithLenP);
  }
  else if (IS_TEST_POST_ACT(postAct)) {
    CONST Index testSetN= POST_ACT_TEST_INDEX(postAct);
    printHTMLRef1(outFile, NC_STATES_HTML, "TEST SET #%d", 
		  TEST_SET_FMT, testSetN, nameWithLenP);
  }
  else {
    CONST Index ruleN= POST_ACT_REDUCTION_RULEN(postAct);
    CONST Index nonTerm= ruleNonTerm(ruleN);
    assert(IS_REDUCE_POST_ACT(postAct));
    assert(ruleN < G.nRules);
    printHTMLRef2(outFile, REDUCTION_LOOKS_HTML, "REDUCE %d", REDUCE_FMT, 
		  ruleN, state, nameWithLenP);
    fputs(" (", outFile);
    printHTMLRef1(outFile, ACCESS_HTML, getNonTermString(nonTerm),
		 NT_STATE_LIST_FMT, nonTerm, nameWithLenP);
    fputs(")", outFile);
  }
  VOID_RET();
}

static VOID 
printHTMLTestSet(outFile, testsIndex, state, nameWithLenP)
  FILE *outFile;
  Index testsIndex;
  Index state;
  NameWithLen *nameWithLenP;
{
  Index i;
  assert(testsIndex < nTestSets());
  fputs("\t", outFile);
  printHTMLDef1(outFile, "TEST SET #%d", TEST_SET_FMT, 
		testsIndex, nameWithLenP);
  fputs(".\n", outFile);
  for (i= testsIndex; testSetsTest(i) != 0; i++) {
    assert(i < nTestSets());
    fputs("\t  ", outFile);
    printHTMLRef1(outFile, TESTS_HTML, "Test %d", 
		 TEST_FMT, testSetsTest(i), nameWithLenP);
    fputs(": ", outFile);
    printHTMLAction(outFile, testSetsAct(i), state, nameWithLenP);
    fputs("\n", outFile);
  }
  fputs("\tELSE ", outFile);
  printHTMLAction(outFile, testSetsAct(i), state, nameWithLenP);
  fputs("\n", outFile);
  VOID_RET();
}

typedef struct {
  Index2 testSetN;	/* Index in testSets[]. */
  Index2 index;		/* Used to prevent equal comparisons in qsort(). */
} TestOut;

static int 
testOutCmp(p1, p2)
  CONST VOIDP p1;
  CONST VOIDP p2;
{
  CONST TestOut *tP1= ((CONST TestOut *)p1);
  CONST TestOut *tP2= ((CONST TestOut *)p2);
  Index2 i1= tP1->testSetN;
  Index2 i2= tP2->testSetN;
  return (i1 < i2) ? -1 : (i1 > i2) ? 1 : (tP1->index < tP2->index ? -1 : 1);
}

/* Print actions for state s on outFile. */
static VOID 
printHTMLStateActions(outFile, s, nameWithLenP)
  FILE *outFile;
  Index s;
  NameWithLen *nameWithLenP;
{
  Index i;
  CONST Index defRedN= defaultActs[s];
  CONST Index defActN= MAKE_REDUCE_POST_ACT(defRedN);
  CONST Count nRules= G.nRules;
  Area testOuts;
#define TEST_OUTS(i) A_ACCESS(testOuts, TestOut, i)
  ConstString prefix0= "      ";
  ConstString prefix1[2], suffix[2];
  prefix1[0]= (STATES(s).nConflicts) ? "  " : "";
  prefix1[1]= "[ ";
  suffix[0]= "\n"; suffix[1]= " ]\n";
  INIT_AREA(testOuts, TestOut, 3, UINT2_MAX);
  for (i= 0; i < 2; i++) {	/* (i) ? Loosing actions : Winning actions. */
    CONST Index lo= STATES(s).actionsLo + ((i) ? 0 : STATES(s).nConflicts);
    CONST Index hi= (i) 
                    ? STATES(s).actionsLo + STATES(s).nConflicts
		    : STATES(s).actionsHi; 
    Index j;
    for (j= lo; j < hi; j++) {
      CONST Index act= ACTIONS(j).value;
      if (act != defActN || i) {
	fputs(prefix0, outFile); fputs(prefix1[i], outFile);
	printHTMLAction(outFile, act, s, nameWithLenP);
	fputs(" on ", outFile);
	printHTMLRef1(outFile, ACCESS_HTML, getTermString(ACTIONS(j).input), 
		      NT_STATE_LIST_FMT, ACTIONS(j).input, nameWithLenP);
	fputs(suffix[i], outFile);
	if (IS_TEST_POST_ACT(act)) {
	  CONST Index testSetN= POST_ACT_TEST_INDEX(act);
	  Index t= A_NEXT(testOuts);
	  TEST_OUTS(t).testSetN= testSetN; TEST_OUTS(t).index= t;
	}
      }
    }
  }

  fputs("DEFAULT: ", outFile);
  if (defRedN != nRules - 1) { /* Print default reduction. */
    printHTMLAction(outFile, MAKE_REDUCE_POST_ACT(defRedN), s, nameWithLenP);
  }
  else {
    fputs("Error", outFile);
  }
  fputs("\n", outFile);
  if (A_NENTRIES(testOuts) > 0) {
    Index i;
    CONST Index n= A_NENTRIES(testOuts);
    Index last= NIL;
    qsort((VOIDP)&TEST_OUTS(0), n, sizeof(TestOut), testOutCmp);
    for (i= 0; i < n; i++) {
      if (TEST_OUTS(i).testSetN != last) {
	printHTMLTestSet(outFile, TEST_OUTS(i).testSetN, s, nameWithLenP); 
	last= TEST_OUTS(i).testSetN;
      }
    }
    delArea(&testOuts);
  }

  VOID_RET();
}

/* Print gotos for state s. Terminate each goto by '\n'.
 * There may be multiple state-items for the same non-terminal in
 * state s.  Hence to avoid printing duplicate gotos, we use the fact
 * that if the nonTermChain -1 entry for a non-terminal state-item
 * points within the current state, then that goto must have already
 * been printed. 
 */
static VOID 
printHTMLStateGotos(outFile, s, nameWithLenP)
  FILE *outFile;
  Index s;
  NameWithLen *nameWithLenP;
{ /* Print gotos. */
  Index i;
  CONST Index lo= STATES(s).itemsLo[1];
  CONST Index hi= STATES(s).itemsHi[1];
  for (i= lo; i < hi; i++) {
    Index nonTerm= SYM_NUM(RHS_SYM(STATE_ITEMS(N_SI, i).item));
    assert(STATE_ITEMS(N_SI, i).u.succ != NIL);
    if (STATE_ITEMS(N_SI, i).u1.nonTermChain == NIL || 
	STATE_ITEMS(N_SI, i).u1.nonTermChain < lo) {
      Index gotoState= STATE_ITEMS(N_SI, i).u.succ;
      fputs("      ", outFile);
      printHTMLRef1(outFile, NC_STATES_HTML, "GOTO %3d", STATE_FMT, 
		    gotoState, nameWithLenP);
      fputs(" on ", outFile);
      printHTMLRef1(outFile, ACCESS_HTML, getNonTermString(nonTerm), 
		    NT_STATE_LIST_FMT, nonTerm, nameWithLenP);
      fputs("\n", outFile);
    }
    else {
      /* some cc's get upset if assert's arguments are not within 1 line! */
      assert(STATE_ITEMS(N_SI, i).u1.nonTermChain == NIL || STATE_ITEMS(N_SI, i).u1.nonTermChain < hi);
    }
  }
  VOID_RET();
}

static VOID
printHTMLAllStates(outFile, includeClosureItems, nameWithLenP)
  FILE *outFile;
  BooleanX includeClosureItems;
  NameWithLen *nameWithLenP;
{
  ConstString stateFmt= 
    (includeClosureItems) ? CLOSURE_STATE_FMT : STATE_FMT;
  ConstString otherStateFmt=
    (includeClosureItems) ? STATE_FMT : CLOSURE_STATE_FMT;
  ConstString otherStateTextFmt=
    (includeClosureItems) 
    ? "Click here for state %d without closure items"
    : "Click here for state %d with closure items";
  Index otherStateFileIndex=
    (includeClosureItems) ? NC_STATES_HTML : C_STATES_HTML;
  CONST Count nStates= G.nStates;
  Index s;
  if (nameWithLenP) fputs("<PRE>\n", outFile);

  for (s= 0; s < nStates; s++) {
    SIType t;
    printHTMLDef1(outFile, "State %3d", stateFmt, s, nameWithLenP);
    fputs(":\t", outFile);
    if (nameWithLenP) {
      printHTMLRef1(outFile, otherStateFileIndex, otherStateTextFmt, 
		    otherStateFmt, s, nameWithLenP);
    }
    fputs("\n", outFile);
    for (t= X_SI; t < LIMIT_SI; t++) {
      CONST Index hi= STATES(s).itemsHi[t];
      Index i;
      for (i= STATES(s).itemsLo[t]; i < hi; i++) {
	CONST Index item= STATE_ITEMS(t, i).item;
	if (!includeClosureItems && IS_CLOSURE_ITEM(item)) break;
	printHTMLRule(outFile, getItemRuleN(item), item, FALSE, nameWithLenP);
      } /* for (i= STATES(s).itemsLo[t]; t < hi; i++) */
    } /* for (t= X_SI; t < LIMIT_SI; t++) */
    printHTMLStateGotos(outFile, s, nameWithLenP);
    printHTMLStateActions(outFile, s, nameWithLenP);
    printInAttribExps(outFile, s);
    fputs("\n", outFile);
  } /* for (s= 0; s < nStates; s++) */
  if (nameWithLenP) fputs("</PRE>\n", outFile);
  VOID_RET();
}

static VOID
printHTMLAccessedStates(outFile, nameWithLenP)
  FILE *outFile;
  NameWithLen *nameWithLenP;
{
  Index i;
  Count n[2];
  n[0]= G.nTerms; n[1]= G.nNonTerms;
  for (i= 0; i < 2; i++) {
    Index j;
    ConstString heading= (i) 
                         ? "States Accessed by Non-Terminal Symbols"
                         : "States Accessed by Terminal Symbols";
    BEGIN_SECTION(outFile, heading, nameWithLenP);
    for (j= 0; j < n[i]; j++) {
      Index k;
      Scratch scratch;
      sprintf(scratch, "States accessed by %s `%s'", 
              (i) ? "non-terminal" : "terminal",
	      (i) ? getNonTermString(j) : getTermString(j));
      printHTMLDef1(outFile, scratch, 
		    (i) ? NT_STATE_LIST_FMT : T_STATE_LIST_FMT,
		    j,  nameWithLenP);
      fputs("\n\t", outFile);
      for (k= statesHash[i][j]; k != NIL; k= STATES(k).succ) {
	printHTMLRef1(outFile, NC_STATES_HTML, "%d", STATE_FMT, 
		      k, nameWithLenP);
	fputs(" ", outFile);
      }
      fputs("\n", outFile);
    } /* for (j= 0; j < n[i]; j++) */
    END_SECTION(outFile, nameWithLenP);
  } /* for (i= 0; i < 2; i++) */
  VOID_RET();
}

static VOID
printHTMLReduceLooks(outFile, nameWithLenP)
  FILE *outFile;
  NameWithLen *nameWithLenP;
{
  CONST Count nStates= G.nStates;
  Index s;
  for (s= 0; s < nStates; s++) {
    Index i;
    CONST Index hi= STATES(s).itemsHi[X_SI];
    for (i= STATES(s).itemsLo[X_SI]; i < hi; i++) {
      CONST Index item= STATE_ITEMS(X_SI, i).item;
      if (IS_REDUCING_ITEM(item)) {
	CONST Index ruleN= getItemRuleN(item);
	CONST Index lhsNonTerm= RULE_NON_TERM(ruleN);
	Boolean isSLR= (STATE_ITEMS(X_SI, i).u.lalrLook == BAD_ISET);
	ISet lookSet= 
	  isSLR ? nonTermFollows[lhsNonTerm] : STATE_ITEMS(X_SI, i).u.lalrLook;
	Index term;
	if (nameWithLenP) fputs("<P>", outFile);
	fputs(isSLR ? "SLR " : "LALR ", outFile);
	printHTMLDef2(outFile, 
		      "lookaheads for reducing by ",
		      REDUCE_FMT, ruleN, s, nameWithLenP);
	printHTMLRef1(outFile, RULES_HTML, "rule %d", RULE_FMT, ruleN,
		      nameWithLenP);
	fputs(" (", outFile);
	printHTMLRef1(outFile, ACCESS_HTML, getNonTermString(lhsNonTerm), 
		      NT_STATE_LIST_FMT, lhsNonTerm, nameWithLenP);
	fputs(") in ", outFile);
	printHTMLRef1(outFile, NC_STATES_HTML, "state %d", STATE_FMT,
		      s, nameWithLenP);
	fputs(nameWithLenP ? "\n<DIR>\n" : "\n", outFile);
	{ ISetIter isIter;
	  FOR_EACH_IN_ISET(term, lookSet, isIter) {
	    fputs(nameWithLenP ? "<LI>" : "\t", outFile);
	    printHTMLRef1(outFile, ACCESS_HTML, getTermString(term), 
			  T_STATE_LIST_FMT, term, nameWithLenP);
	    fputs("\n", outFile);
	  }
	}
	fputs(nameWithLenP ? "\n</DIR>\n" : "\n\n", outFile);
      } /* if (IS_REDUCING_ITEM(STATE_ITEMS(X_SI, i))) */
    } /* for (i= STATES(s).itemsLo[X_SI]; i < hi; i++) */
  } /* for (s= 0; s < nStates; s++) */
  END_SECTION(outFile, nameWithLenP);
}

static VOID 
printHTMLTests(outFile, nameWithLenP)
  FILE *outFile;
  NameWithLen *nameWithLenP;
{
  if (nTests()) {
    CONST Count nNonTerms= gramNNonTerms();
    Index nonTerm;
    if (nameWithLenP) fputs("<PRE>\n", outFile);
    for (nonTerm= 0; nonTerm < nNonTerms; nonTerm++) {
      CONST Index t= nonTermTest(nonTerm);
      if (t != NIL) {
	printHTMLDef1(outFile, "Test %d", TEST_FMT, t, nameWithLenP);
	fputs(": ", outFile);
	outTest(outFile, t, nonTerm);
	fputs("\n", outFile);
      }
    }
    if (nameWithLenP) fputs("</PRE>\n", outFile);
  } /* if (nTest()) */
  VOID_RET();
}

static VOID 
printHTMLUserActs(outFile, nameWithLenP)
  FILE *outFile;
  NameWithLen *nameWithLenP;
{
  CONST Count nActs= nActions();
  Index a;
  if (nameWithLenP) fputs("<PRE>\n", outFile);
  for (a= 0; a < nActs; a++) {
    printHTMLDef1(outFile, "#Act_%d", ACT_FMT, a, nameWithLenP);
    fputs(":\n", outFile);
    printAct(outFile, a);
    fputs("\n", outFile);
  }
  if (nameWithLenP) fputs("</PRE>\n", outFile);
  VOID_RET();
}

static VOID
printHTMLConflicts(outFile, nameWithLenP)
  FILE *outFile;
  NameWithLen *nameWithLenP;
{
  if (nSRConflicts > 0 || nRRConflicts > 0) { /* Conflict report. */
    CONST Count nStates= G.nStates;
    Index s;
    BEGIN_SECTION(outFile, "Conflicts", nameWithLenP);
    fprintf(outFile, "%d conflicts (%d shift-reduce, %d reduce-reduce).\n",
	    nSRConflicts + nRRConflicts, nSRConflicts, nRRConflicts);
    for (s= 0; s < nStates; s++) {
      if (STATES(s).nConflicts > 0) {
	printHTMLRef1(outFile, NC_STATES_HTML, "State %d", STATE_FMT, 
		      s, nameWithLenP);
	fprintf(outFile, " has %2d conflicts.\n", STATES(s).nConflicts);
      }
    }
    fputs("\n", outFile);
    END_SECTION(outFile, nameWithLenP);
  }
  VOID_RET();
}

#define HYPERLINK_CONVENTIONS "\
<H2>HyperLink Conventions</H2>\n<P>\n\
Activating a hyperlink on one of the following types of symbols\n\
results in displaying:\n\
<DL>\n\
<DT>Terminal<DD>The states accessed by the terminal.\n\
<DT>Nonterminal<DD>The states accessed by the nonterminal.\n\
<DT>Rule <VAR>nn</VAR><DD>The rule <VAR>nn</VAR>.\n\
<DT>Shift/Goto <VAR>s</VAR><DD>The items and actions in state <VAR>s</VAR>.\n\
<DT>Reduce <VAR>r</VAR><DD>The lookaheads for reducing by <VAR>r</VAR>.\n\
<DT>#Act <VAR>a</VAR><DD>The user action <VAR>a</VAR>.\n\
<DT>Test set #<VAR>t</VAR><DD>The test set <VAR>t</VAR>.\n\
<DT>Test <VAR>t</VAR><DD>The user test <VAR>t</VAR>.\n\
</DL>\n\
"

static VOID
printHTMLMain(outFile, nameWithLenP)
  FILE *outFile;
  NameWithLen *nameWithLenP;
{
  printHTMLConflicts(outFile, nameWithLenP);
  if (nameWithLenP) {
    CONST Count n= N_ELEMENTS(htmlFiles);
    Index i;
    fputs("<H2>Menu</H2>\n<MENU>", outFile);
    for (i= 0; i < n; i++) {
      if (i != MAIN_HTML) {
	fputs("<LI><a href=\"", outFile);
	OUT_HTML_FILE_NAME(outFile, i, nameWithLenP);
	fprintf(outFile, "\">%s</a>\n", htmlFiles[i].title);
      }
    }
    fputs("</MENU>", outFile);
    fputs(HYPERLINK_CONVENTIONS, outFile);
  }
}


static VOID
printHTMLAllStatesClosure(outFile, nameWithLenP)
  FILE *outFile;
  NameWithLen *nameWithLenP;
{
  printHTMLAllStates(outFile, TRUE, nameWithLenP);
}

static VOID
printHTMLAllStatesNoClosure(outFile, nameWithLenP)
  FILE *outFile;
  NameWithLen *nameWithLenP;
{
  printHTMLAllStates(outFile, FALSE, nameWithLenP);
}


/* Return the root name NAME given a outName of the form NAME.tab.c or
 * NAME.c or NAME.
 */
static Size
getBaseNameLen(outName)
  ConstString outName;
{
  CONST Size outLen= strlen(outName);
  return
    (outLen >= 6 && strcmp(outName + (outLen - 6), ".tab.c") == 0)
    ? (outLen - 6)
    : ( (outLen >= 2 && strcmp(outName + (outLen - 2), ".c") == 0)
        ? (outLen - 2)
        : outLen
      );
}



#ifndef PRG_NAME
#define PRG_NAME	"zyacc"
#endif

#ifndef PRG_HOME
#define PRG_HOME							\
  "<A HREF=\"http://opal.cs.binghamton.edu/~zdu/zyacc\">zyacc</A>"
#endif

#ifndef VERSION
#define VERSION		"1.0"
#endif


VOID doHTML(inName, outName)
  ConstString inName;
  ConstString outName;
{
  CONST Count n= N_ELEMENTS(htmlFiles);
  CONST Size baseNameLen= getBaseNameLen(outName);
  CONST Size rootLen= BASE_LEN(baseNameLen);
  CONST Size htmlNameLen= rootLen + X_LEN + strlen(HTML_EXT);
  NameWithLen nameWithLen;
  DCL_AUTO_BUF(char, 128) htmlName;
  Index i;
  INIT_AUTO_BUF(htmlName);
  nameWithLen.name= outName; nameWithLen.len= rootLen;
  CHECK_AUTO_BUF(htmlName, htmlNameLen + 1);
  for (i= 0; i < n; i++) {
    FILE *outFile;
    sprintf(AUTO_BUF(htmlName), htmlFiles[i].nameFmt, rootLen, 
	    outName, HTML_EXT);
    outFile= safeOpen(AUTO_BUF(htmlName), PRG_NAME);
    sprintf(AUTO_BUF(htmlName), htmlFiles[MAIN_HTML].nameFmt, rootLen, 
	    outName, HTML_EXT);
    fputs("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n", outFile);
    fputs("<HEAD>", outFile);
    fprintf(outFile, "<TITLE>%s for %s</TITLE>\n", htmlFiles[i].title, inName);
    fputs("</HEAD>", outFile);
    fputs("<BODY>", outFile);
    fprintf(outFile, "<H1>%s for %s</H1>\n", htmlFiles[i].title, inName);
    fprintf(outFile,
	    "Automatically generated from %s by %s version %s<BR>\n", 
	     inName, PRG_HOME, VERSION);
    if (i != MAIN_HTML) {
      fprintf(outFile, 
	      "<P>Click <a href=\"%s\">here</a> to return to the main menu.\n",
	      AUTO_BUF(htmlName));
    }
    (*htmlFiles[i].printFnP)(outFile, &nameWithLen);
    if (i != MAIN_HTML) {
      fprintf(outFile, 
	      "<P>Click <a href=\"%s\">here</a> to return to the main menu.\n",
	      AUTO_BUF(htmlName));
    }
    fputs("</BODY>", outFile);
    fclose(outFile);
  }
  FREE_AUTO_BUF(htmlName);
  VOID_RET();
}

#if MSDOS | _MSDOS | __MSDOS__ | __TURBOC__
#define OUT_EXT			"out"
#else
#define OUT_EXT			"output"
#endif

VOID doVerbose(inName, outName)
  ConstString inName;
  ConstString outName;
{
  CONST Count n= N_ELEMENTS(htmlFiles);
  CONST Size rootLen= getBaseNameLen(outName);
  CONST Size verboseNameLen= rootLen + strlen(OUT_EXT);
  DCL_AUTO_BUF(char, 128) verboseName;
  FILE *outFile;
  Index i;
  INIT_AUTO_BUF(verboseName);
  CHECK_AUTO_BUF(verboseName, verboseNameLen + 1);
  sprintf(AUTO_BUF(verboseName), "%.*s.%s", (int)rootLen, outName, OUT_EXT);
  outFile= safeOpen(AUTO_BUF(verboseName), PRG_NAME);
  fprintf(outFile, 
	  "Automatically generated from %s by %s version %s.\n", 
	  inName, PRG_NAME, VERSION);
  for (i= 0; i < n; i++) {
    printHTMLHeading(outFile, htmlFiles[i].title, 1, NULL);
    (*htmlFiles[i].printFnP)(outFile, NULL);
  }
  fclose(outFile);
  FREE_AUTO_BUF(verboseName);
  VOID_RET();
}
