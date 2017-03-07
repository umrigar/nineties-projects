/*

File:	 outdfa.c
Purpose: Compress and output DFA tables.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "chrclass.h"
#include "codescan.h"
#include "comptab.h"
#include "dfa.h"
#include "files.h"
#include "options.h"
#include "outdfa.h"
#include "outvec.h"
#include "patterns.h"
#include "sklnames.h"
#include "outprms.h"

#include "memalloc.h"
#include "zutil.h"

#include <stdlib.h>

static struct {
  Index2 stateIndex;	/* Used for getting state number from comb vector. */
  Index2 defaultIndex;	/* Used for getting default state from comb vector. */
  Count2 nCols;		/* # of columns when no compress. */
} globals;
#define G globals


/*				STATIC PROTOTYPES.			*/

struct StateInfo;

static struct StateInfo *computeStateInfo PROTO((VOID_ARGS));
static VOID delStateInfo PROTO((struct StateInfo *stateInfoP));
static Index mostCommonSucc PROTO((Index s, BooleanX isNonSigSucc,
				   Count2 *succCounts, Count *nTransP));
static VOID combStateInfoEntry 
  PROTO((Index s, struct StateInfo *CONST stateInfoP, Index def, 
	 Count nonDefCount));
static struct StateInfo *
  computeCombStateInfo PROTO((struct StateInfo *stateInfoP));
static VOID iterativeStateInfoEntry 
  PROTO((Index s, struct StateInfo *CONST stateInfoP, 
	 Index def, BooleanX isTerminal, Count nonDefCount));
static Count nStateDiff PROTO((Index s1, Index s2, Count maxDiff));
static Index findDefaultState 
  PROTO((Index s, CONST ISet terminalDefaults, CONST ISet *CONST nTransSets,
	 Count *CONST diffP));
static struct StateInfo *
  computeIterativeStateInfo PROTO((struct StateInfo *stateInfoP));
static Count getNCols PROTO((VOID_ARGS));
static VOID outBaseEntry PROTO((FILE *outFile, Index s));
static Count baseInitFn PROTO((FILE *outFile, BooleanX isDcl, 
			       Index *indentP));
static Count checkInitFn PROTO((FILE *outFile, BooleanX isDcl, 
			        Index *indentP));
static VOID outDefaultEntry PROTO((FILE *outFile, Index s));
static Count defaultInitFn PROTO((FILE *outFile, BooleanX isDcl, 
			          Index *indentP));
static Count dummyInitFn PROTO((FILE *outFile, BooleanX isDcl, 
			        Index *indentP));
static VOID outIntraEntry PROTO((FILE *outFile, Index intraIndex));
static Count intraInitFn PROTO((FILE *outFile, BooleanX isDcl, 
			        Index *indentP));
static Count outNoCompressNext PROTO((FILE *outFile, Index indent, 
				      Count nCols));
static VOID outCombEntry PROTO((FILE *outFile, Index s));
static Count nextInitFn PROTO((FILE *outFile, BooleanX isDcl, 
			       Index *indentP));
static VOID outSentinelEntry PROTO((FILE *outFile, Index s));
static Count sentinelInitFn PROTO((FILE *outFile, BooleanX isDcl, 
			           Index *indentP));
static Count sBeginInitFn PROTO((FILE *outFile, BooleanX isDcl, 
			         Index *indentP));
static Count sEndInitFn PROTO((FILE *outFile, BooleanX isDcl, 
			       Index *indentP));
static Count uBeginInitFn PROTO((FILE *outFile, BooleanX isDcl, 
			         Index *indentP));
static Count uEndInitFn PROTO((FILE *outFile, BooleanX isDcl, 
			       Index *indentP));
static VOID outActEntry PROTO((FILE *outFile, Index s));

#if DO_TRACE
static Count dumpCompStateInfo PROTO((FILE *dumpFile, Index s));
static VOID dumpCompDFA PROTO((FILE *dumpFile));
#endif



/*			   COMPRESSION ALGORITHM			*/

/*
Compress tab[i, j] possibilities:

1.  tab[i, j]= (check[base[i] + j] == i) ? next[base[i] + j] : default[i].

2.  tab[i, j]= (check[base[i] + j] == j) ? next[base[i] + j] : default[i].

(2) allows identical rows to share same entries.  (1) allows rows to
share same base (possible if rows are disjoint).  Seems a bit of a
toss-up; choosing (2) as identical rows are possible after defaults
removed.

Should we use a bit error-matrix to factor out transitions to the jam
state and then choose as default the most frequent non-jam state?  The
answer is no: eyeballing DFAs for some scanners showed that simply
defaulting most frequent transition (to either jam or non-jam state)
would be adequate.

When comb-compression is used, the basic state-transition algorithm to
compute the next state s based on current state s and input c:

  s= (check[base[s] + c] == c) ? next[base[s] + c] : default[s];

When iterative-compression is used the basic state-transition
algorithm to compute the next state s based on current state s and
input c:

do {
  if (check[base[s] + c] == c) {
    s= next[base[s] + c]; break;
  }
  else {
   s= default[s];
   if (isTerminalDefault(s)) break;
  }
} while (1);

where isTerminalDefault is explained below.

There are two types of default states for a state s:

1. A default state which is the most common successor of state s.  

2. A default state which is "similar" to state s in that it has many
of the same transitions.

When compress == COMB_COMPRESS, all default states are of type 1.  When 
compress == ITERATIVE_COMPRESS, defaults can be of type 1 or type 2.

We refer to defaults of type 1 as terminal defaults because if such a
default is reached in the iterative-transition algorithm, we have
completed the state transition.  All non-significant states can only
be terminal defaults.

We refer to defaults of type 2 as iterative defaults because if such a
default is reached in the iterative-transition algorithm, we continue
with the iteration.

The set of terminal defaults and set of iterative defaults must be disjoint. 

*/


/*			STATES INFORMATION FOR COMPRESSION.		*/


typedef struct StateInfo { /* Auxiliary information useful for compression. */
  State def;		/* Default state. */
  Size2 width;		/* Width of non-default transitions. */
  Count2 nTransitions;	/* # of non-default transitions. */
  Index2 hiTransition;	/* Highest non-default transition. */
  Boolean isTerminal;	/* Whether default def is terminal. */
} StateInfo;

/* This global variable is used for comparing states in stateCmp(). */
static StateInfo *stateInfo;	/* Have 1 entry for each significant state. */

static StateInfo *
computeStateInfo()
{
  assert(optionsP->compress == COMB_COMPRESS ||
	 optionsP->compress == ITERATIVE_COMPRESS);
  stateInfo= MALLOC_VEC(dfaParams->nSigStates, StateInfo);
  return (optionsP->compress == COMB_COMPRESS) 
         ? computeCombStateInfo(stateInfo)
         : computeIterativeStateInfo(stateInfo);
}

static VOID
delStateInfo(stateInfoP)
  StateInfo *stateInfoP;
{
  FREE(stateInfoP);
  stateInfo= NULL;
}

#if 0
/* Compare states pointed to by s1P and s2P.  Comparison is first on
 * width of transitions, then on number of transitions, then on the
 * hi transition.  We ensure we don't return 0 (arbitrarily comparing
 * states which are otherwise equal by state number) so as to ensure
 * portability.
 */
static int 
stateCmp(s1P, s2P)
  CONST VOIDP s1P;
  CONST VOIDP s2P;
{
  State s1= *((State *)s1P);
  State s2= *((State *)s2P);
  CONST Size w1= stateInfo[s1].width;
  CONST Size w2= stateInfo[s2].width;
  if (w1 < w2) return 1;
  else if (w1 > w2) return -1;
  else 
  {
    CONST Count n1= stateInfo[s1].nTransitions;
    CONST Count n2= stateInfo[s2].nTransitions;
    if (n1 == n2) {
      CONST Index hi1= stateInfo[s1].hiTransition;
      CONST Index hi2= stateInfo[s2].hiTransition;
      return hi1 < hi2 ? -1 : ( hi1 > hi2 ? 1 : (s1 < s2) ? -1 : 1 );
    }
    else {
      return (n1 < n2) ? 1 : -1;
    }
  }
}
#endif


/*			MOST COMMON SUCCESSOR				*/

/* Return the most common successor (a non-significant successor if
 * isNonSig) of state s and the number of transitions to it in
 * *nTransP.  On entry, succCounts[] should be a vector of zero counts
 * large enough to be indexed by any successor state (including
 * pseudo-states) and is returned with all zero counts too.
 */
static Index
mostCommonSucc(s, isNonSigSucc, succCounts, nTransP)
  Index s;
  BooleanX isNonSigSucc;
  Count2 *succCounts;
  Count *nTransP;
{    
  CONST Count nIn= dfaParams->nIn;
  CONST Count nStates= dfaParams->nStates;
  CONST Count nSigStates= dfaParams->nSigStates;
  CONST Index jamState= nStates;
  CONST Index transLo= DFA_STATES[s].transLo;
  CONST Index transHi= DFA_STATES[s].transHi;
  CONST Count nTransitions= (transHi - transLo);
  CONST Index sJamState= s + nStates;
  /* Assume most common succ. initially is jam-state. */
  State commonSucc= (optionsP->backupOptimize) ? jamState : sJamState;
  Count commonSuccCount= nIn - nTransitions;

  if (commonSuccCount < nTransitions) { /* Need to check all transitions. */
    Index i;
    for (i= transLo; i < transHi; i++) {
      State next= DFA_TRANS[i].nextState;
      if ((!isNonSigSucc || next >= nSigStates) 
	  && ++succCounts[next] > commonSuccCount) {
	commonSuccCount= succCounts[next]; commonSucc= next;
      }
    }
    /* Clear succCounts[] entries. */
    for (i= transLo; i < transHi; i++) succCounts[DFA_TRANS[i].nextState]= 0;
  }
  *nTransP= commonSuccCount;
  return commonSucc;
}


/*			COMB COMPRESSION INFORMATION			*/

/* Fill in state-information entry for state s with a default state def
 * with nonDefCount non-default transitions.
 */
static VOID
combStateInfoEntry(s, stateInfoP, def, nonDefCount)
  Index s;
  StateInfo *CONST stateInfoP;
  Index def;
  Count nonDefCount;
{
  CONST Index transLo= DFA_STATES[s].transLo;
  CONST Index transHi= DFA_STATES[s].transHi;
  CONST Boolean backupOptimize= optionsP->backupOptimize;
  CONST Index sJamState= 
    backupOptimize ? dfaParams->nStates : s + dfaParams->nStates;
  StateInfo *CONST sP= &stateInfoP[s];
  sP->def= def; sP->nTransitions= nonDefCount;
  if (def == sJamState) {
    assert((transHi - transLo) == nonDefCount);
    sP->hiTransition= DFA_TRANS[transHi - 1].input;
    sP->width= (sP->hiTransition - DFA_TRANS[transLo].input) + 1;
  }
  else if (nonDefCount == 0) { /* All transitions go to default. */
    sP->hiTransition= sP->width= 0;
  }
  else { /* Scan for first and last non-default transitions. */
    Index i;
    for (i= transHi - 1; DFA_TRANS[i].nextState != def; i--) ;
    assert(i >= transLo);
    sP->hiTransition= DFA_TRANS[i].input;
    for (i= transLo; DFA_TRANS[i].nextState != def; i++) ;
    assert(i < transHi);
    sP->width= (sP->hiTransition - DFA_TRANS[i].input) + 1;
  }
  sP->isTerminal= TRUE; /* All comb-compression defaults are terminal. */
  VOID_RET();
}

/* Compute auxiliary information useful for DFA compression into
 * stateInfoP[nSigStates].  The default state for each significant
 * state s is the most frequently occurring next state among the
 * transitions of s (including the jam error state).  The width
 * associated with state s is the maximum difference between the
 * inputs of non-default transitions of s.  The number of transitions
 * computed for a state only includes non-default transitions.
 */
static StateInfo *
computeCombStateInfo(StateInfo *stateInfoP)
{
  CONST Count nStates= dfaParams->nStates;
  CONST Count nSigStates= dfaParams->nSigStates;
  CONST Count nIn= dfaParams->nIn;
  CONST Boolean backupOptimize= optionsP->backupOptimize;
  CONST Count nTotalStates= (backupOptimize) ? nStates : 2*nStates + nSigStates;
  Count2 *CONST succCounts= CALLOC_VEC(nTotalStates, Count2); 
  Index s;

  for (s= 0; s < nSigStates; s++) {
    Count defCount;
    State def= mostCommonSucc(s, FALSE, succCounts, &defCount);
    combStateInfoEntry(s, stateInfoP, def, nIn - defCount);
  } /* for (s= 0; s < nSigStates; s++) */
  FREE(succCounts);
  return stateInfoP;
}


/*			ITERATIVE COMPRESSION INFORMATION		*/

enum { /* Parameters which control default state searching. */
  ACCEPTABLE_DIFF_PERCENT= 5, /* Stop searching if diff-percent < this. */
  MAX_DIFF_PERCENT= 50	/* diff-percent < this for any default. */
};

/* Add state information for state s to stateInfoP, based on a default
 * state def with nonDefCount non-default transitions.
 */
static VOID
iterativeStateInfoEntry(s, stateInfoP, def, isTerminal, nonDefCount)
  Index s;
  StateInfo *CONST stateInfoP;
  Index def;
  BooleanX isTerminal;
  Count nonDefCount;  
{
  StateInfo *CONST sP= &stateInfoP[s];
  if (isTerminal || nonDefCount == 0) {
    combStateInfoEntry(s, stateInfoP, def, nonDefCount);
  }
  else {
    CONST Count nIn= dfaParams->nIn;
    CONST Count nStates= dfaParams->nStates;
    CONST Index jamState= nStates;
    CONST Boolean backupOptimize= optionsP->backupOptimize;
    CONST Index sJamState= (backupOptimize) ? jamState : s + nStates;
    CONST Index dJamState= (backupOptimize) ? jamState : def + nStates;
    CONST Index sHi= DFA_STATES[s].transHi;
    CONST Index dHi= DFA_STATES[def].transHi;
    Index sI= DFA_STATES[s].transLo; 
    Index dI= DFA_STATES[def].transLo;
    Count nTrans= 0;
    Index loBound= nIn;
    Index hiBound;
    Index i;
    for (i= 0; i < nIn; i++) {
      Index sSucc= (sI < sHi && DFA_TRANS[sI].input == i) 
	           ? DFA_TRANS[sI++].nextState 
	           : sJamState;
      Index dSucc= (dI < dHi && DFA_TRANS[dI].input == i) 
	           ? DFA_TRANS[dI++].nextState 
	           : dJamState;
      if (sSucc != dSucc) {
	if (i < loBound) loBound= i;
	hiBound= i;
	nTrans++;
      }
    }
    assert(nTrans == nonDefCount);
    sP->def= def; sP->nTransitions= nonDefCount;
    sP->hiTransition= hiBound;
    sP->width= (hiBound - loBound) + 1;
  }
  sP->isTerminal= isTerminal;
  VOID_RET();
}

/* Return the number of differences between states s1 and s2.  If the #
 * of differences is > maxDiff, return value > maxDiff.
 *
 * Note that when !backupOptimize, the transformed pseudo-jam state
 * and pseudo-sentinel state stored for each state will be different
 * and hence we do not want them to compare equal.
 */
static 
Count nStateDiff(s1, s2, maxDiff)
  Index s1;
  Index s2;
  Count maxDiff;
{
  CONST Count nIn= dfaParams->nIn;
  CONST Index jamState= dfaParams->nStates;
  CONST Boolean backupOptimize= optionsP->backupOptimize;
  CONST Index hi1= DFA_STATES[s1].transHi;
  CONST Index hi2= DFA_STATES[s2].transHi;
  Index i1= DFA_STATES[s1].transLo;
  Index i2= DFA_STATES[s2].transLo;
  Count nDiff= 0;
  Index i;
  for (i= 0; i < nIn; i++) {
    CONST Index succ1= (i1 < hi1 && DFA_TRANS[i1].input == i) 
                       ? DFA_TRANS[i1++].nextState 
                       : jamState;
    CONST Index succ2= (i2 < hi2 && DFA_TRANS[i2].input == i) 
                       ? DFA_TRANS[i2++].nextState 
                       : jamState;
    if (succ1 != succ2 || (!backupOptimize && succ1 == jamState) ) {
      nDiff++;
      if (nDiff > maxDiff) break;
    }
  }
  return nDiff;
}


/* Return an iterative default state for s.  The default cannot belong
 * to set terminalDefaults.  nTransSets is a vector of sets such that
 * nTransSets[n] is the set of all states having n non-jam transitions
 * (for n in [0, nIn].  On entry *diffP should contain a
 * max-difference between s and a default.  On exit, if it finds a
 * default with difference less than the max-difference it returns the
 * iterative default with *diffP containing the computed difference;
 * else it returns NIL with *diffP unchanged.
 */
static Index
findDefaultState(s, terminalDefaults, nTransSets, diffP)
  Index s;
  CONST ISet terminalDefaults;
  CONST ISet *CONST nTransSets;
  Count *CONST diffP;
{
  CONST Count nIn= dfaParams->nIn;
  CONST Index transLo= DFA_STATES[s].transLo;
  CONST Index transHi= DFA_STATES[s].transHi;
  CONST Int nTrans= transHi - transLo;
  CONST Count acceptDiff= (ACCEPTABLE_DIFF_PERCENT * nTrans)/100;
  CONST Count maxDiff= *diffP;
  Count diff= maxDiff;
  Index def;
  Int j;

  for (j= 0; j < diff && diff >= acceptDiff; j= (j < 0) ? -j : -j - 1) { 
    /* Bounded search for default state. j in 0, -1, 1, -2, 2, ... */
    if (0 < nTrans + j && nTrans + j <= nIn) {
      Index d;
      ISetIter transIter;
      FOR_EACH_IN_ISET(d, nTransSets[nTrans + j], transIter) {
	if (d < s && !inISet(d, terminalDefaults)) {
	  CONST Count dDiff=  nStateDiff(s, d, diff);
	  if (dDiff < diff) {
	    def= d; diff= dDiff;
	    if (diff < acceptDiff) break;
	  }
	} /* if (d < s) */
      } /* FOR_EACH_IN_ISET(d, nTransSets[nTrans + j], transIter) */
    } /* if (0 < nTrans + j && nTrans + j <= nIn) */
  } /* for (j= 0; j < diff && diff >= acceptDiff; ... ) */

  if (diff < maxDiff) {
    *diffP= diff; return def;
  }
  else {
    return NIL;
  }
}

/* Compute state information for iterative table compression.  The
 * algorithm used here computes iterative defaults only if they are
 * better than terminal defaults: thus ensuring that
 * ITERATIVE_COMPRESS achieves compression at least as good as
 * COMB_COMPRESS.  We first iterate thru all the states getting a
 * terminal default for each state and then try to get an iterative
 * default which is better than the terminal default.  It is possible
 * that we first regard a state as a iterative default, but
 * subsequently it becomes a terminal default violating the constraint
 * that terminal defaults and iterative defaults are disjoint.  A
 * subsequent loop remedies this: if we find that a state has been
 * assigned a iterative default which is also a terminal default, we
 * first try to find another iterative default or, failing that, use a
 * non-significant successor as a terminal default (which is always 
 * possible because in the worst-case we use the jam-state as a default
 * (even if there are no transitions to it!)).
 */
static StateInfo *
computeIterativeStateInfo(StateInfo *stateInfoP)
{
  CONST Count nIn= dfaParams->nIn;
  CONST Count nSigStates= dfaParams->nSigStates;
  CONST Count nStates= dfaParams->nStates;
  CONST Boolean backupOptimize= optionsP->backupOptimize;
  /* nTransSets[n] is set of states with n non-jam transitions. */
  ISet *CONST nTransSets= MALLOC_VEC(nIn + 1, ISet); 
  CONST Count nTotalStates= backupOptimize ? nStates : 2*nStates + nSigStates;
  Count2 *CONST succCounts= CALLOC_VEC(nTotalStates, Count2); 
  ISet terminalDefaults= newISet(); /* Set of terminal non-sig states. */
  Index i, s;

  for (i= 0; i <= nIn; i++) nTransSets[i]= newISet();

  for (s= 0; s < nSigStates; s++) { /* 1st pass. */
    CONST Count nTrans= DFA_STATES[s].transHi - DFA_STATES[s].transLo;
    Count commonNTrans;		/* # of transitions to most common succ. */
    CONST Index commonSucc= 	/* Terminal default for state s. */
      mostCommonSucc(s, FALSE, succCounts, &commonNTrans);
    Count diff= nIn - commonNTrans; /* # of trans by which s & def defer. */
    Index def= findDefaultState(s, terminalDefaults, nTransSets, &diff);
    CONST Boolean isTerminal= (def == NIL);
    if (isTerminal) { /* No iterative default better than terminal default. */
      def= commonSucc;
      if (def < nSigStates) addISet(terminalDefaults, def); 
    }
    /* Remember information for 2nd pass. */
    stateInfoP[s].def= def; stateInfoP[s].isTerminal= isTerminal; 
    stateInfoP[s].nTransitions= diff;
    assert(nTrans > 0 || DFA_STATES[s].sentinelSucc != NIL); /* sig-state. */
    addISet(nTransSets[nTrans], s);
  } /* for (s= 0; s < nSigStates; s++) */

  for (s= 0; s < nSigStates; s++) { /* 2nd pass. */
    Boolean isTerminal= stateInfoP[s].isTerminal;
    Index def= stateInfoP[s].def;
    Count diff= stateInfoP[s].nTransitions;
    if (!isTerminal && inISet(def, terminalDefaults)) {
      /* Need to find a new default: code here is suboptimal as most-common
       * successor might be best, but is not considered as it may also
       * be an iterative default; sorting it out doesn't appear worth-while.
       */
      CONST Count nTrans= DFA_STATES[s].transHi - DFA_STATES[s].transLo;
      diff= (nTrans * MAX_DIFF_PERCENT) / 100;
      def= findDefaultState(s, terminalDefaults, nTransSets, &diff);
      isTerminal= (def == NIL);
      if (isTerminal) { /* Use most common nonsig succ as default. */
	Count commonNTrans;
	def= mostCommonSucc(s, TRUE, succCounts, &commonNTrans);
	assert(def >= nSigStates);
	diff= nIn - commonNTrans;
      }
    }
    iterativeStateInfoEntry(s, stateInfoP, def, isTerminal, diff);
  }
  /* Clean up. */
  for (i= 0; i <= nIn; i++) delISet(nTransSets[i]);

  delISet(terminalDefaults);
  FREE(nTransSets);
  FREE(succCounts);

  return stateInfoP;
}


/*			    DFA COMPRESSION.				*/


static CompTab dfaTabs;


static VOID
stateNTransitions(entries)
  CompTabEntry entries[];
{
  CONST Count nSigStates= dfaParams->nSigStates;
  CONST Count nXEntries= (optionsP->tableEntry == STATE_TABLE) ? 0 : 2;
  Index s;
  for (s= 0; s < nSigStates; s++) {
    entries[s].input= s; 
    entries[s].value= stateInfo[s].nTransitions + nXEntries;
  }
  VOID_RET();
}

/* While compressing the DFA, if !backupOptimize we change the
 * transition from state s to jamState to a transition to s + nStates.
 */
static Count
stateTransitions(s, entries)
  Index s;
  CompTabEntry entries[];
{
  CONST Count nStates= dfaParams->nStates;
  CONST State jamState= nStates;
  CONST Count nIn= dfaParams->nIn;
  CONST Count nXEntries= (optionsP->tableEntry == STATE_TABLE) ? 0 : 2;
  CONST Boolean backupOptimize= optionsP->backupOptimize;
  CONST Index transHi= DFA_STATES[s].transHi;
  CONST State sJamState= backupOptimize ? jamState : s + nStates;
  CONST Index def= stateInfo[s].def;
  Count nEntries= 0;
  Index sI= DFA_STATES[s].transLo;
  if (stateInfo[s].isTerminal) {
    /* Collect non-def trans. */
    Index j;
    for (j= 0; j < nIn; j++) {
      State nextState= (sI < transHi && DFA_TRANS[sI].input == j) 
	? DFA_TRANS[sI++].nextState 
	: sJamState;
      if (nextState != def) {
	entries[nEntries].input= j;  entries[nEntries].value= nextState;
	nEntries++;
      }
    }
  } /* if (stateInfo[s].isTerminal) */
  else { /* non-terminal default. */
    CONST Count dTransHi= DFA_STATES[def].transHi;
    CONST State dJamState= backupOptimize ? jamState : def + nStates;
    Index dI= DFA_STATES[def].transLo; 
    Index j;
    assert(optionsP->compress == ITERATIVE_COMPRESS);
    for (j= 0; j < nIn; j++) {
      State sSucc= (sI < transHi && DFA_TRANS[sI].input == j) 
	? DFA_TRANS[sI++].nextState
	: sJamState;
      State dSucc= (dI < dTransHi && DFA_TRANS[dI].input == j)
	? DFA_TRANS[dI++].nextState
	: dJamState;
      if (sSucc != dSucc) {
	entries[nEntries].input= j; entries[nEntries].value= sSucc;
	nEntries++;
      }
    } /* for (j= 0; j < nIn; j++) */
  } /* else non-terminal default. */
  if (nXEntries) { /* Insert extra entries for getting default and state. */
    CONST Index defaultIndex= nIn;
    CONST Index stateIndex= nIn + 1;
    G.stateIndex= stateIndex; G.defaultIndex= defaultIndex;
    entries[nEntries].input= defaultIndex;
    entries[nEntries++].value= def;
    entries[nEntries].input= stateIndex;
    entries[nEntries++].value= s;
  }
  assert(nEntries == stateInfo[s].nTransitions + nXEntries);
  return nEntries;
}

VOID
compressDFA()
{
  if (optionsP->compress == NO_COMPRESS) {
    G.nCols= getNCols();
  }
  else { /* do compression. */
    CONST Count nSigStates= dfaParams->nSigStates;
    CONST Count nIn= dfaParams->nIn;
    CONST Count nXEntries= (optionsP->tableEntry == STATE_TABLE) ? 0 : 2;
    computeStateInfo();
    compressTab(nSigStates, nIn + nXEntries, stateNTransitions,
		stateTransitions, &dfaTabs);
    if (optionsP->isVerbose) printCompDFAStats(stdout);
#if DO_TRACE    
    if (files.dumpFile) dumpCompDFA(files.dumpFile);
#endif
  }
}
/* Return # of columns in next array after padding to make it a power-of-2
 * if within optionsP->colWastePercent.
 */
static Count
getNCols()
{
  CONST Count nIn= dfaParams->nIn;
  CONST Count nIn1= 1U << ceilingLg2(nIn);
  CONST Count nExtraCol100= (nIn1 - nIn)*100;
  return nExtraCol100/nIn > optionsP->colWastePercent ? nIn : nIn1;
}



/*			    yyTab OUTPUT TABLES.			*/

enum {	/* Special enum values used within initialization specifiers enum. */
  AA_F= 1,	/* First enum value of init-specifiers. */
  ZZ_F= 0	/* Used for terminating vector of init-specifiers. */
};

#define YYTAB_INIT_SPECIFIERS						\
/*T(EnumName, initFnName)*/						\
  T(BASE_F= AA_F, baseInitFn),	/* base[] field. */			\
  T(CHECK_F, checkInitFn),	/* check[] field. */			\
  T(DEFAULT_F, defaultInitFn),	/* def[] field. */			\
  T(DUMMY_F, dummyInitFn),	/* dummy[] field. */			\
  T(EQUIV_F, equivInitFn),	/* equiv-classes (chrclass.c). */	\
  T(INTRA_F, intraInitFn),	/* intra[] field. */			\
  T(NEXT_F, nextInitFn),	/* next[] field. */			\
  T(SENTINEL_F, sentinelInitFn),/* sentinel[] field. */			\
  T(S_BEGIN_F, sBeginInitFn),	/* struct { */				\
  T(S_END_F, sEndInitFn),	/* } s */				\
  T(U_BEGIN_F, uBeginInitFn),	/* YY_UNION_STRUCT { */			\
  T(U_END_F, uEndInitFn)	/* } u */				


enum { /* Define initialization specifiers enum. */
#undef T
#define T(a, b) a
  YYTAB_INIT_SPECIFIERS
};

typedef Count (*InitFn) PROTO((FILE *outFile, BooleanX isDcl, Index *indentP));

static InitFn initFns[]= {
#undef T
#define T(a, b)	b
  YYTAB_INIT_SPECIFIERS
#undef T
};

typedef unsigned char InitSpec;

/* compress == NO_COMPRESS && table == ADDRESS_TABLE
 * typedef struct YYNext { YY_CONST struct YYNext *next; } YYNext;
 */
static InitSpec noCompressAddressTableSpec[]=  { 
   	                /* struct {				*/
    NEXT_F,		/*   YYNext next[nNext];		*/
    U_BEGIN_F,		/*   YY_UNION_STRUCT {			*/
      S_BEGIN_F,	/*     struct {				*/
        SENTINEL_F,	/*       YYXState sentinel[nSentinel];	*/
        INTRA_F,	/*       YYXState intra[nIntra];	*/
        EQUIV_F,	/*	 equiv-spec; (chrclass)		*/
      S_END_F,		/*     } s;				*/
      DUMMY_F,		/*     char dummy[nDummy];		*/
    U_END_F,		/*   } u;				*/
  ZZ_F			/* } yyTab;				*/
};
  
/* compress == NO_COMPRESS && table == DIFF_TABLE 
 * typedef short YYNext;
 */
static InitSpec noCompressIndexTableSpec[]= { 
   	                /* struct {				*/
    NEXT_F,		/*   YYNext next[nNext];		*/
    U_BEGIN_F,		/*   YY_UNION_STRUCT {			*/
      S_BEGIN_F,	/*     struct {				*/
        SENTINEL_F,	/*       YYXState sentinel[nSentinel];	*/
        INTRA_F,	/*       YYXState intra[nIntra];	*/
        EQUIV_F,	/*	 equiv-spec; (chrclass)		*/
      S_END_F,		/*     } s;				*/
      DUMMY_F,		/*     char dummy[nDummy];		*/
    U_END_F,		/*   } u;				*/
  ZZ_F	        	/* } yyTab;				*/
};

/* compress == NO_COMPRESS && table == STATE_TABLE 
 * typedef YYXState YYNext;
 */
static InitSpec noCompressStateTableSpec[]= {
   	                /* struct { 				*/
    NEXT_F,		/*   YYNext next[nSigStates][nCols]; 	*/
    U_BEGIN_F,		/*   YY_UNION_STRUCT { 			*/
      S_BEGIN_F,	/*     struct { 			*/
        SENTINEL_F,	/*       YYXState sentinel[nSentinel];	*/
        INTRA_F,	/*       YYXState intra[nIntra];	*/
        EQUIV_F,	/*	 equiv-spec; (chrclass)		*/
      S_END_F,		/*     } s;				*/
    U_END_F,		/*   } u;				*/
  ZZ_F	        	/* } yyTab;				*/
};
/* compress == COMB_COMPRESS && table == ADDRESS_TABLE
 * typedef struct YYNext { 
 *   YY_CONST struct YYNext *next; 
 *   YYCheck check;
 * } YYNext;
 */
static InitSpec combCompressAddressTableSpec[]= {
   	                /* struct {				*/
    NEXT_F,		/*   YYNext next[nNext];		*/
    U_BEGIN_F,		/*   YY_UNION_STRUCT {			*/
      S_BEGIN_F,	/*     struct {				*/
        SENTINEL_F,	/*       YYXState sentinel[nSentinel];	*/
        INTRA_F,	/*       YYXState intra[nIntra];	*/
        BASE_F,		/*	 YY_CONST YYNext *base[nSigStates];*/
        EQUIV_F,	/*	 equiv-spec; (chrclass)		*/
      S_END_F,		/*     } s;				*/
      DUMMY_F,		/*     char dummy[nDummy];		*/
    U_END_F,		/*   } u;				*/
  ZZ_F			/* } yyTab;				*/
};

/* compress == COMB_COMPRESS && table == DIFF_TABLE
 * typedef struct YYNext { 
 *   short next; 
 *   YYCheck check;
 * } YYNext;
 */
static InitSpec combCompressIndexTableSpec[]= {
   	                /* struct {				*/
    NEXT_F,		/*   YYNext next[nNext];		*/
    U_BEGIN_F,		/*   YY_UNION_STRUCT {			*/
      S_BEGIN_F,	/*     struct {				*/
        SENTINEL_F,	/*       YYXState sentinel[nSentinel];	*/
        INTRA_F,	/*       YYXState intra[nIntra];	*/
        BASE_F,		/*	 YY_CONST YYNext *base[nSigStates];*/
        EQUIV_F,	/*	 equiv-spec; (chrclass)		*/
      S_END_F,		/*     } s;				*/
      DUMMY_F,		/*     char dummy[nDummy];		*/
    U_END_F,		/*   } u;				*/
  ZZ_F			/* } yyTab;				*/
};

/* compress == COMB_COMPRESS && table == STATE_TABLE 
 * typedef YYXState YYNext;
 */
static InitSpec combCompressStateTableSpec[]= {
   	                /* struct {				*/
    NEXT_F,		/*   YYNext next[nNext];		*/
    CHECK_F,		/*   YYCheck check[nCheck];		*/
    DEFAULT_F,		/*   YYDef def[nSigStates];		*/       
    U_BEGIN_F,		/*   YY_UNION_STRUCT {			*/
      S_BEGIN_F,	/*     struct {				*/
        SENTINEL_F,	/*       YYXState sentinel[nSentinel];	*/
        INTRA_F,	/*       YYXState intra[nIntra];	*/
        BASE_F,		/*	 YYXState base[nSigStates];	*/
        EQUIV_F,	/*	 equiv-spec; (chrclass)		*/
      S_END_F,		/*     } s;				*/
    U_END_F,		/*   } u;				*/
  ZZ_F			/* } yyTab;				*/
};

/* compress == ITERATIVE_COMPRESS && table == ADDRESS_TABLE
 * typedef struct YYNext { 
 *   YY_CONST struct YYNext *next; 
 *   YYCheck check;
 * } YYNext;
 */
static InitSpec iterativeCompressAddressTableSpec[]= {
   	                /* struct {				*/
    NEXT_F,		/*   YYNext next[nNext];		*/
    U_BEGIN_F,		/*   YY_UNION_STRUCT {			*/
      S_BEGIN_F,	/*     struct {				*/
        SENTINEL_F,	/*       YYXState sentinel[nSentinel];	*/
        INTRA_F,	/*       YYXState intra[nIntra];	*/
        BASE_F,		/*	 YY_CONST YYNext *base[nSigStates];	*/
        EQUIV_F,	/*	 equiv-spec; (chrclass)		*/
      S_END_F,		/*     } s;				*/
      DUMMY_F,		/*     char dummy[nDummy];		*/
    U_END_F,		/*   } u;				*/
  ZZ_F			/* } yyTab;				*/
};

/* compress == ITERATIVE_COMPRESS && table == DIFF_TABLE
 * typedef struct YYNext { 
 *   short next; 
 *   YYCheck check;
 * } YYNext;
 */
static InitSpec iterativeCompressIndexTableSpec[]= {
   	                /* struct { 				*/
    NEXT_F,		/*   YYNext next[nNext]; 		*/
    U_BEGIN_F,		/*   YY_UNION_STRUCT { 			*/
      S_BEGIN_F,	/*     struct { 			*/
        SENTINEL_F,	/*       YYXState sentinel[nSentinel]; 	*/
        INTRA_F,     	/*       YYXState intra[nIntra]; 	*/
        BASE_F,		/*	 YY_CONST YYNext *base[nSigStates];*/
        EQUIV_F,	/*	 equiv-spec; (chrclass)		*/
      S_END_F,		/*     } s; 				*/
      DUMMY_F, 		/*     char dummy[nDummy]; 		*/
    U_END_F,		/*   } u; 				*/
  ZZ_F 		       	/* } yyTab; 				*/
};

/* compress == ITERATIVE_COMPRESS && table == STATE_TABLE
 * typedef YYXState YYNext;
 */
static InitSpec iterativeCompressStateTableSpec[]= {
   	                /* struct { 				*/
    NEXT_F,		/*   YYNext next[nNext]; 		*/
    CHECK_F,		/*   YYCheck check[nCheck]; 		*/
    DEFAULT_F,		/*   YYDef def[nSigStates];	 	*/       
    U_BEGIN_F,		/*   YY_UNION_STRUCT { 			*/
      S_BEGIN_F,	/*     struct { 			*/
        SENTINEL_F,	/*       YYXState sentinel[nSentinel];	*/
        INTRA_F,     	/*       YYXState intra[nIntra]; 	*/
        BASE_F,		/*	 YYXState base[nSigStates]; 	*/
        EQUIV_F,	/*	 equiv-spec; (chrclass)		*/
      S_END_F,		/*     } s; 				*/
    U_END_F,		/*   } u; 				*/
  ZZ_F 		       	/* } yyTab; 				*/
};

/* Code-scanner tables. */
static InitSpec codeTableSpec[]= {
   	                /* struct { 				*/
    U_BEGIN_F,		/*   YY_UNION_STRUCT { 			*/
      S_BEGIN_F,	/*     struct { 			*/
        SENTINEL_F,	/*       YYXState sentinel[nSentinel];	*/
        INTRA_F,     	/*       YYXState intra[nIntra]; 	*/
        EQUIV_F,	/*	 equiv-spec; (chrclass)		*/
      S_END_F,		/*     } s; 				*/
    U_END_F,		/*   } u; 				*/
  ZZ_F 		       	/* } yyTab; 				*/
};

static InitSpec *initSpecs[3][3]= {
  { noCompressAddressTableSpec, 
    noCompressIndexTableSpec, 
    noCompressStateTableSpec
  },
  { combCompressAddressTableSpec, 
    combCompressIndexTableSpec, 
    combCompressStateTableSpec
  },
  { iterativeCompressAddressTableSpec, 
    iterativeCompressIndexTableSpec, 
    iterativeCompressStateTableSpec
  }
};

/*			BASE ARRAY INITIALIZATION			*/

static VOID
outBaseEntry(outFile, s)
  FILE *outFile;
  Index s;
{
  assert(s < dfaParams->nSigStates);
  fprintf(outFile, "%s(%d)", BASE_INIT_MACRO, dfaTabs.base[s]);
  VOID_RET();
}

static Count 
baseInitFn(outFile, isDcl, indentP)
  FILE *outFile;
  BooleanX isDcl;
  Index *indentP;
{
  CONST Index indent= *indentP;
  CONST Count nSigStates= dfaParams->nSigStates;
  Count nLines= 0;
  assert(optionsP->compress != NO_COMPRESS);
  if (isDcl) {
    fprintf(outFile, "%*s%sbase[%d];\n", indent, "", 
	    optionsP->tableEntry == STATE_TABLE 
            ? "YYBase " 
            : "YY_CONST YYNext *", 
	    nSigStates);
    nLines++;
  }
  else {
    CONST Index indent1= indent + INDENT_INC;
    fprintf(outFile, "%*s{ /* base[%d] */\n", indent, "", nSigStates); 
    nLines++;
    nLines+= outVec(outFile, indent1, NLINE2, nSigStates, outBaseEntry);
    fprintf(outFile, "\n%*s} /* base[%d] */\n", indent, "", nSigStates); 
    nLines+= 2;
  }
  return nLines;
}


/*			CHECK ARRAY INITIALIZATION			*/

static Count
checkInitFn(outFile, isDcl, indentP)
  FILE *outFile;
  BooleanX isDcl;
  Index *indentP;
{
  CONST Index indent= *indentP;
  CONST Count checkSize= dfaTabs.checkSize;
  Count nLines= 0;
  assert(optionsP->compress != NO_COMPRESS);
  assert(optionsP->tableEntry == STATE_TABLE);
  if (isDcl) {
    fprintf(outFile, "%*sYYCheck check[%d];\n", indent, "", checkSize); 
    nLines++;
  }
  else {
    CONST Index indent1= indent + INDENT_INC;
    assert(optionsP->tableEntry == STATE_TABLE);
    fprintf(outFile, "%*s{ /* check[%d] */\n", indent, "", checkSize); 
    nLines++;
    nLines+= outIndex2Vec(outFile, indent1, NLINE1, checkSize, dfaTabs.check);
    fprintf(outFile, "\n%*s} /* check[%d] */\n", indent, "", checkSize); 
    nLines+= 2;
  }
  return nLines;
}


/*			   DEFAULT ARRAY INITIALIZATION			*/

static VOID
outDefaultEntry(outFile, s)
  FILE *outFile;
  Index s;
{
  assert(optionsP->compress != NO_COMPRESS);
  assert(optionsP->tableEntry == STATE_TABLE);
  assert(s < dfaParams->nSigStates);
  assert(stateInfo);
  fprintf(outFile, "%s(%d, %d, %d)", DEFAULT1_INIT_MACRO, 0, 
	  stateInfo[s].def, stateInfo[s].isTerminal);
  VOID_RET();
}

static Count
defaultInitFn(outFile, isDcl, indentP)
  FILE *outFile;
  BooleanX isDcl;
  Index *indentP;
{
  CONST Index indent= *indentP;
  CONST Count nSigStates= dfaParams->nSigStates;
  Count nLines= 0;
  assert(optionsP->compress != NO_COMPRESS);
  assert(optionsP->tableEntry == STATE_TABLE);
  if (isDcl) {
    fprintf(outFile, "%*sYYDef def[%d];\n", indent, "", nSigStates); 
    nLines++;
  }
  else {
    CONST Index indent1= indent + INDENT_INC;
    fprintf(outFile, "%*s{ /* def[%d] */\n", indent, "", nSigStates); 
    nLines++;
    nLines+= outVec(outFile, indent1, NLINE3, nSigStates, outDefaultEntry);
    fprintf(outFile, "\n%*s} /* def[%d] */\n", indent, "", nSigStates); 
    nLines+= 2;
  }
  return nLines;
}


/*			    DUMMY ARRAY INITIALIZATION			*/

static Count
dummyInitFn(outFile, isDcl, indentP)
  FILE *outFile;
  BooleanX isDcl;
  Index *indentP;
{
  CONST Index indent= *indentP;
  CONST Count nStates= dfaParams->nStates;
  CONST Count dummySize= (nStates - dfaParams->nSigStates) + 2*nStates;
  Count nLines= 0;
  assert(optionsP->tableEntry != STATE_TABLE);
  if (isDcl) {
    fprintf(outFile, "%*schar dummy[%d];\n", indent, "", dummySize);
    nLines++;
  }
  else {
    fprintf(outFile, "%*s/* No initializer for dummy[%d]. */\n", 
	    indent, "", dummySize);
    nLines++;
  }
  return nLines;
}


/*			    INTRA ARRAY INITIALIZATION			*/

static VOID
outIntraEntry(outFile, intraIndex)
  FILE *outFile;
  Index intraIndex;
{
  assert(intraIndex < dfaParams->nIntraStates);
  fprintf(outFile, "%5d", DFA_STATES[intraIndex + dfaParams->nSigStates].alt);
  VOID_RET();
}

static Count
intraInitFn(outFile, isDcl, indentP)
  FILE *outFile;
  BooleanX isDcl;
  Index *indentP;
{
  CONST Index indent= *indentP;
  CONST Count nIntra= dfaParams->nIntraStates;
  CONST Count nIntra1= (nIntra == 0) ? 1 : nIntra;
  Count nLines= 0;
  if (isDcl) {
    fprintf(outFile, "%*sYYXState intra[%d];\n", indent, "", nIntra1);
    nLines++;
  }
  else {
    CONST Index indent1= indent + INDENT_INC;
    fprintf(outFile, "%*s{ /* intra[%d] */\n", indent, "", nIntra1); 
    nLines++;
    if (nIntra) {
      nLines+= outVec(outFile, indent1, NLINE1, nIntra, outIntraEntry);
    }
    else {
      fprintf(outFile, "%*s0", indent1, "");
    }
    fprintf(outFile, "\n%*s} /* intra[%d] */\n", indent, "", nIntra1); 
    nLines+= 2;
  }
  return nLines;
}


/*			    NEXT ARRAY INITIALIZATION			*/


static
Count outNoCompressNext(outFile, indent, nCols)
  FILE *outFile;
  Index indent;
  Count nCols;
{
  CONST Count nSigStates= dfaParams->nSigStates;
  CONST Count nStates= dfaParams->nStates;
  CONST Count jamState= nStates;
  CONST Index indent1= indent + INDENT_INC;
  CONST Index indent2= indent1 + INDENT_INC;
  CONST Boolean isStateTable= (optionsP->tableEntry == STATE_TABLE);
  CONST Boolean backupOptimize= optionsP->backupOptimize;
  CONST Count nPerLine= isStateTable ? NLINE1 : NLINE3;
  CONST Index sentinelIn= dfaParams->sentinelIn;
  ConstString rowBegin= (isStateTable) ? "{" : "";
  ConstString rowEnd= (isStateTable) ? "}" : "";
  char nextDim[20];
  Count nLines= 0;
  State s;
  assert(optionsP->compress == NO_COMPRESS);
  if (isStateTable) {
    sprintf(nextDim, "[%d][%d]", nSigStates, nCols);
  }
  else {
    sprintf(nextDim, "[%d]", nSigStates * nCols);
  }
  fprintf(outFile, "%*s{ /* next%s */\n", indent, "", nextDim);
  nLines++;
  for (s= 0; s < nSigStates; s++) {
    CONST Index sJamState= backupOptimize ? jamState : s + nStates;
    CONST Index sSentinelState= 
      backupOptimize ? dfaParams->sentinelState : s + 2*nStates;
    CONST Index transHi= DFA_STATES[s].transHi;
    Index t= DFA_STATES[s].transLo;
    Index i;
    fprintf(outFile, "%*s%s /* begin state %d */\n%*s", indent1, "", rowBegin, 
	    s, indent2, ""); 
    nLines++;
    for (i= 0; i < nCols; i++) {
      Index next= (i == sentinelIn) 
	          ? sSentinelState
	          : ( (t < transHi && i == DFA_TRANS[t].input) 
	              ? DFA_TRANS[t++].nextState 
	              : sJamState
                    );
      if (isStateTable) { /* Output next directly. */
	fprintf(outFile, "%5d", next);
      }
      else if (next < nSigStates) {
	fprintf(outFile, "%s(%d, %d)", NEXT1_INIT_MACRO, 
		s * nCols, next * nCols);
      }
      else { /* transition to a non-sig or pseudo-state in dummy[]. */
	fprintf(outFile, "%s(%d, %d)", NEXT2_INIT_MACRO, s * nCols, next);
      }
      fprintf(outFile, "%s ", i == nCols - 1 ? "" : ",");
      if ((i + 1) % nPerLine == 0 && i != nCols - 1) { 
	fprintf(outFile, "\n%*s", indent2, ""); nLines++;
      }
    }
    fprintf(outFile, "\n%*s%s%s /* end state %d */\n", indent1, "", rowEnd,
	    ((s == nSigStates - 1) ? "" : ","), s); 
    nLines+= 2;
  }
  fprintf(outFile, "%*s} /* next%s */\n", indent, "", nextDim);
  nLines++;
  return nLines;
}

static VOID
outCombEntry(outFile, index)
  FILE *outFile;
  Index index;
{
  CONST Index checkVal= dfaTabs.check[index];
  CONST Index nextVal= index < dfaTabs.nextSize ? dfaTabs.next[index] : 0;
  assert(optionsP->compress != NO_COMPRESS);
  assert(optionsP->tableEntry != STATE_TABLE);
  assert(index < dfaTabs.checkSize);
  if (checkVal == G.stateIndex) {
    fprintf(outFile, "{ %s(%d), %d }", STATE_INIT_MACRO, nextVal, checkVal);
  }
  else if (checkVal == G.defaultIndex) {
    CONST Index s= dfaTabs.next[(index - checkVal) + G.stateIndex];
    if (nextVal < dfaParams->nSigStates) {
      fprintf(outFile, "{ %s(%d, %d, %d), %d }", DEFAULT1_INIT_MACRO, 
              index - checkVal, dfaTabs.base[nextVal], stateInfo[s].isTerminal,
	      checkVal);
    }
    else {
      fprintf(outFile, "{ %s(%d, %d, %d), %d }", DEFAULT2_INIT_MACRO, 
              index - checkVal, nextVal, stateInfo[s].isTerminal,
	      checkVal);
    }
  } 
  else if (nextVal < dfaParams->nSigStates) {
    fprintf(outFile, "{ %s(%d, %d), %d }", NEXT1_INIT_MACRO,
	    index - checkVal, dfaTabs.base[nextVal], checkVal);
  }
  else { 
    fprintf(outFile, "{ %s(%d, %d), %d }", NEXT2_INIT_MACRO,
	    index - checkVal, nextVal, checkVal);
  }
  VOID_RET();
}

static Count
nextInitFn(outFile, isDcl, indentP)
  FILE *outFile;
  BooleanX isDcl;
  Index *indentP;
{
  CONST Index indent= *indentP;
  Count nLines= 0;
  if (isDcl) {
    if (optionsP->compress == NO_COMPRESS) {
      CONST Count nCols= G.nCols;
      if (optionsP->tableEntry == STATE_TABLE) {
	fprintf(outFile, "%*sYYNext next[%d][%d];\n", 
		indent, "", dfaParams->nSigStates, nCols);
	nLines++;
      }
      else {
	fprintf(outFile, "%*sYYNext next[%d];\n", 
		indent, "", dfaParams->nSigStates * nCols);
	nLines++;
      }
    } /* if (optionsP->compress == NO_COMPRESS) */
    else { /* optionsP->compress != NO_COMPRESS */
      CONST Count nextSize= (optionsP->tableEntry == STATE_TABLE) 
	? dfaTabs.nextSize 
	: dfaTabs.checkSize;
      fprintf(outFile, "%*sYYNext next[%d];\n", indent, "", nextSize);
      nLines++;
    } /* else optionsP->compress != NO_COMPRESS */
  } /* if (isDcl) */
  else { 
    if (optionsP->compress == NO_COMPRESS) {
      nLines+= outNoCompressNext(outFile, indent, G.nCols);
    }
    else { /* optionsP->compress != NO_COMPRESS */
      CONST Count nNext= (optionsP->tableEntry == STATE_TABLE) 
	? dfaTabs.nextSize 
	: dfaTabs.checkSize;
      CONST Index indent1= indent + INDENT_INC;
      fprintf(outFile, "%*s{ /* next[%d] */\n", indent, "", nNext); nLines++;
      if (optionsP->tableEntry == STATE_TABLE) {
	nLines+= outIndex2Vec(outFile, indent1, NLINE1, nNext, dfaTabs.next);
      }
      else {
	nLines+= outVec(outFile, indent1, NLINE3, nNext, outCombEntry);
      }
      fprintf(outFile, "\n%*s} /* next[%d] */\n", indent, "", nNext); 
      nLines+= 2;
    }
  }
  return nLines;
}


/*			    SENTINEL ARRAY INITIALIZATION		*/	

static VOID
outSentinelEntry(outFile, s)
  FILE *outFile;
  Index s;
{
  CONST Count nStates= dfaParams->nStates;
  Index sentinelSucc= DFA_STATES[s].sentinelSucc;
  assert(s < dfaParams->nSigStates);
  fprintf(outFile, "%5d",
	  (sentinelSucc != NIL) 
          ? sentinelSucc
          : ( (optionsP->backupOptimize) ? nStates 
	                                 : 2*nStates + dfaParams->nSigStates )
	 );
  VOID_RET();
}

static Count
sentinelInitFn(outFile, isDcl, indentP)
  FILE *outFile;
  BooleanX isDcl;
  Index *indentP;
{
  CONST Index indent= *indentP;
  Count nLines= 0;
  static Count2 nSentinels;
  if (isDcl) {
    Index i;
    for (i= dfaParams->nSigStates; i > 0; i--) {
      if (DFA_STATES[i - 1].sentinelSucc != NIL) break;
    }
    fprintf(outFile, "%*sYYXState sentinel[%d];\n", indent, "", 
	    i == 0 ? 1 : i); 
    nLines++;
    nSentinels= i;
  }
  else {
    CONST Index indent1= indent + INDENT_INC;
    fprintf(outFile, "%*s{ /* sentinel[%d] */\n", indent, "", nSentinels); 
    nLines++;
    if (nSentinels == 0) {
      fprintf(outFile, "%*s%d", indent1, "",
	      optionsP->backupOptimize ? dfaParams->nStates 
	                               : 0 + dfaParams->nStates);
    }
    else {
      nLines+= outVec(outFile, indent1, NLINE1, nSentinels, outSentinelEntry);
    }
    fprintf(outFile, "\n%*s} /* sentinel[%d] */\n", indent, "", 
	    nSentinels == 0 ? 1 : nSentinels); 
    nLines+= 2;
  }
  return nLines;
}


/*			   STRUCT s INITIALIZATION			*/

static Count
sBeginInitFn(outFile, isDcl, indentP)
  FILE *outFile;
  BooleanX isDcl;
  Index *indentP;
{
  CONST Index indent= *indentP;
  Count nLines= 0;
  if (isDcl) {
    fprintf(outFile, "%*sstruct {\n", indent, ""); nLines++;
  }
  else {
    fprintf(outFile, "%*s{ /* struct s */\n", indent, ""); nLines++;
  }
  *indentP+= INDENT_INC;
  return nLines;
}

static Count
sEndInitFn(outFile, isDcl, indentP)
  FILE *outFile;
  BooleanX isDcl;
  Index *indentP;
{
  CONST Index indent= *indentP - INDENT_INC;
  Count nLines= 0;
  *indentP= indent;
  if (isDcl) {
    fprintf(outFile, "%*s} s;\n", indent, ""); nLines++;
  }
  else {
    fprintf(outFile, "%*s} /* struct s */\n", indent, ""); nLines++;
  }
  return nLines;
}


/*			   UNION u INITIALIZATION			*/

static Count
uBeginInitFn(outFile, isDcl, indentP)
  FILE *outFile;
  BooleanX isDcl;
  Index *indentP;
{
  CONST Index indent= *indentP;
  Count nLines= 0;
  if (isDcl) {
    fprintf(outFile, "%*sYY_UNION_STRUCT {\n", indent, ""); nLines++;
  }
  else {
    fprintf(outFile, "%*s{ /* YY_UNION_STRUCT u */\n", indent, ""); nLines++;
  }
  *indentP+= INDENT_INC;
  return nLines;
}

static Count
uEndInitFn(outFile, isDcl, indentP)
  FILE *outFile;
  BooleanX isDcl;
  Index *indentP;
{
  CONST Index indent= *indentP - INDENT_INC;
  Count nLines= 0;
  *indentP= indent;
  if (isDcl) {
    fprintf(outFile, "%*s} u;\n", indent, ""); nLines++;
  }
  else {
    fprintf(outFile, "%*s} /* YY_UNION_STRUCT u */\n", indent, ""); nLines++;
  }
  return nLines;
}



/*			       OUTPUT DFA.				*/


Count
outDFATabs(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  CONST InitSpec *CONST spec= 
    (optionsP->doCode) 
    ? codeTableSpec
    : initSpecs[optionsP->compress][optionsP->tableEntry];
  Index indent= 0;
  CONST InitSpec *p;

  if (optionsP->doCode) nLines+= outMaps(outFile);

  fprintf(outFile, "%*stypedef struct { /* YYTab */\n", indent, ""); nLines++;
  indent+= INDENT_INC;
  for (p= spec; *p != ZZ_F; p++) {
    nLines+= initFns[*p - AA_F](outFile, TRUE, &indent);
  }
  indent-= INDENT_INC;
  fprintf(outFile, "%*s} YYTab;\n", indent, ""); nLines++;

  fprintf(outFile, "%*sstatic YY_CONST YYTab yyTab= {\n", indent, ""); nLines++;
  indent+= INDENT_INC;
  for (p= spec; *p != ZZ_F; p++) {
    CONST Count fnLines= initFns[*p - AA_F](outFile, FALSE, &indent);
    nLines+= fnLines;
    if (fnLines > 0 && *p != S_BEGIN_F && *p != U_BEGIN_F && 
	p[1] != S_END_F && p[1] != U_END_F && p[1] != ZZ_F) {
      fprintf(outFile, "%*s,\n", indent, ""); nLines++;
    }
  }
  indent-= INDENT_INC;
  fprintf(outFile, "%*s};\n", indent, ""); nLines++;
  delCompTab(&dfaTabs);
  if (stateInfo) delStateInfo(stateInfo);
  return nLines;
}

Count
outCompDFAParams(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  fprintf(outFile, "#define YY_MAX_BASE %d\n", dfaTabs.maxBase); 
  nLines++;
  fprintf(outFile, "#define YY_N_COLS %d\n", G.nCols); 
  nLines++;
  fprintf(outFile, "#define YY_N_NEXT %d\n", 
	  (optionsP->doCode) 
	  ? 0
	  : ( (optionsP->compress == NO_COMPRESS)
	      ? G.nCols*dfaParams->nSigStates
              : dfaTabs.nextSize
            )
	 );
  nLines++;
  return nLines;
}

VOID
printCompDFAStats(outFile)
  FILE *outFile;
{
  fprintf(outFile, "Compressed tables: Next size= %d; Check size= %d\n", 
	  dfaTabs.nextSize, dfaTabs.checkSize);
  fprintf(outFile, "Next utilization= %g%%; Check utilization= %g%%\n",
	  100.0*dfaTabs.nTransitions/dfaTabs.nextSize,
	  100.0*dfaTabs.nTransitions/dfaTabs.checkSize);
  VOID_RET();
}


/*			OUTPUT ACTION TABLES.				*/

static VOID
outActEntry(outFile, s)
  FILE *outFile;
  Index s;
{
  CONST Index act= (DFA_STATES[s].actions == BAD_ISET) 
                   ? 0 
                   : unmapAct(firstElementISet(DFA_STATES[s].actions));
  assert(s < dfaParams->nStates);
  fprintf(outFile, "%5d", act);
  VOID_RET();
}

Count
outActionTabs(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  CONST Count indent= 0;
  fprintf(outFile, "%*sstatic YYAct yyActs[%d]= {\n", indent, "", 
	  dfaParams->nStates); 
  nLines++;
  nLines+= outVec(outFile, indent + INDENT_INC, NLINE1, dfaParams->nStates, 
		  outActEntry);
  fprintf(outFile, "\n%*s}; /* yyActs[%d] */\n", indent, "", 
	  dfaParams->nStates); 
  nLines+= 2;
  return nLines;
}



#ifdef DO_TRACE

/* Dump state information for state s, returning the # of transitions for s. */
static Count 
dumpCompStateInfo(dumpFile, s)
  FILE *dumpFile;
  Index s;
{
  if (s < dfaParams->nSigStates) {
    fprintf(dumpFile, "default= %d; width= %d; nTransitions= %d; ",
	    stateInfo[s].def, stateInfo[s].width, stateInfo[s].nTransitions);
    fprintf(dumpFile, "isTerminal= %d; base= %d.", stateInfo[s].isTerminal,
	    dfaTabs.base[s]);
    return stateInfo[s].nTransitions;
  }
  else {
    return 0;
  }
}

static VOID
dumpCompDFA(dumpFile)
  FILE *dumpFile;
{
  CONST Count nStates= dfaParams->nStates;
  Count nTransitions= 0;
  Index s;
  assert(optionsP->compress != NO_COMPRESS);
  fputs("DFA:\n", dumpFile);
  for (s= 0; s < nStates; s++) {
    dumpDFAState(dumpFile, s); 
    nTransitions+= dumpCompStateInfo(dumpFile, s);
    fputs("\n", dumpFile);
  } /* for (s= 0; s < nStates; s++) */
  fprintf(dumpFile, "Total # of transitions to be stored= %d\n", nTransitions);
  fputs("\n", dumpFile);
  printCompDFAStats(dumpFile);
}

#endif /* ifdef DO_TRACE */
