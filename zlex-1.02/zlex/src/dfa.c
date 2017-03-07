/*

File:	 dfa.c
Purpose: Build scanner automaton.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "chrclass.h"
#include "codescan.h"
#include "dfa.h"
#include "files.h"
#include "options.h"
#include "outdfa.h"
#include "outvec.h"
#include "patterns.h"
#include "scan.h"
#include "sklnames.h"

#include "area.h"
#include "error.h"
#include "iset.h"
#include "memalloc.h"



/*			STATIC PROTOTYPES				*/

struct ActState;
struct ActStateInfo;

static VOID initDFABuild PROTO((VOID_ARGS));
static CONST Trans *delDFABuild PROTO((VOID_ARGS));
static State newState PROTO((VOID_ARGS));
static Boolean isSigPositionSet PROTO((ISet s));
static State getState PROTO((ISet s0, BooleanX addIntraTok));
static Count setupStartStates PROTO((VOID_ARGS));
static Count setupContextStartStates PROTO((VOID_ARGS));
static State setupSentinelState PROTO((VOID_ARGS));
static VOID closeDFA PROTO((VOID_ARGS));
static VOID processRedundantStartStates PROTO((VOID_ARGS));
static VOID createIntraPatStates PROTO((VOID_ARGS));
static XDFAState *remapDFA PROTO((VOID_ARGS));
static Index initActStateInfo PROTO((struct ActStateInfo *CONST actStateInfoP));
static VOID freeActStateInfo 
             PROTO((CONST struct ActStateInfo *CONST actStateInfoP));
static VOID buildRejects PROTO((CONST struct ActState *CONST actStates));
static VOID freeRejects PROTO((VOID_ARGS));
static Count outXAction 
             PROTO((FILE *outFile, 
		    CONST struct ActStateInfo *CONST actStateInfoP, Index a));
static Count outActState 
    PROTO((FILE *outFile, Index s, Index a,
	   CONST struct ActStateInfo *CONST actStateInfoP));
static VOID printDFAStats PROTO((FILE *outFile));
#if 0
static VOID makePseudoSentinel PROTO((VOID_ARGS));
#endif
#if DO_TRACE
static VOID dumpDFA PROTO((FILE *dumpFile));
#endif



/*			     GLOBAL DATA.				*/

#if 0 /*  Defined in dfa.h. */

typedef struct {
  CONST XDFAState *states;/* DFA states. */
  CONST Trans *trans;	/* Transitions. */
  Count2 nContextStarts;/* # of context start-states for var-context pats. */
  Count2 nIn;		/* # of inputs for DFA. */
  Count2 nInitialStates;/* # of start-states (including '^' states). */
  Count2 nIntraStates;	/* # of states with intra-tok actions. */
  Count2 nNonFinalStates; /* # of states which are non-accepting. */
  Count2 nSigStates;	/* # of significant states. */
  Count2 nStates;	/* Total number of states. */
  Count2 nNonAcceptStates; /* # of non-accepting states (incl start states). */
  Count2 nTrans;	/* # of entries in trans[] vector. */
  Index2 sentinelState;	/* State to perform sentinel processing. */
} DFAParams;

#endif

/* Global parameters. */
static DFAParams globals;
#define G (globals)

/* Readonly ptr to global parameters for external consumption. */
CONST DFAParams *CONST dfaParams= &globals;

#define SENTINEL_ACT	0


/*			      DFA STATES.				*/



typedef struct {
  ISet val;		/* Set of pattern positions.			*/
  ISet actions;		/* If accepting then mapped action #s else BAD_ISET. */
  Transition transLo;	/* Starting index in transitions.		*/
  Transition transHi;	/* Transitions are in [transLo, transHi).	*/
  State sentinelSucc;	/* Actual successor of state on sentinel input.	*/
                        /* (NIL if no successor).			*/
  union {
    Index2 succ;	/* Next entry in hash-chain.			*/
    State alt;		/* Alternate state for intra-token patterns. 	*/
  } u;
  Boolean addIntraTok;	/* If true, intra-token pattern added to this 
			 * state and its successors. 
			 */
} DFAState;

enum { 
  STATES_HASH_SIZE= 139
};

typedef struct {	/* Struct used to build DFA. */
  Index2 hashTab[STATES_HASH_SIZE];	/* Hash-table for states. */
  Area states;				/* Area containing DFAState's. */
  Area trans;				/* Area containing transitions. */
} BuildDFA;

#define TRANS(i)	A_ACCESS(buildP->trans, Trans, i)
#define STATES(i) 	A_ACCESS(buildP->states, DFAState, i)

/* Pointer to access dynamic BuildDFA structure as DFA is built. */
static BuildDFA *buildP;

static VOID 
initDFABuild() 
{
  Index i;
  buildP= MALLOC_VEC(1, BuildDFA);
  /* Initialize states hash-table. */
  for (i= 0; i < STATES_HASH_SIZE; i++) {
    buildP->hashTab[i]= NIL;
  }
  INIT_AREA(buildP->states, DFAState, 8, UINT2_MAX);
  INIT_AREA(buildP->trans, Trans, 11, UINT2_MAX);
  G.nIn= nEqClasses();
  G.nInitialStates= setupStartStates();
  G.nContextStarts= setupContextStartStates();
  G.sentinelState= setupSentinelState();
  VOID_RET();
}

/* Delete memory used during DFA build.  */
static CONST Trans *
delDFABuild()
{
  CONST Trans *newTrans= getMemFromArea(&buildP->trans);
  CONST Count nStates= G.nStates;
  Index s;
  for (s= 0; s < nStates; s++) {
    if (STATES(s).val != BAD_ISET) delISet(STATES(s).val);
  }
  delArea(&buildP->states);
  FREE(buildP); buildP= NULL;
  return newTrans;
}

/* Delete DFA when totally done with stuff exported thru dfaParams. */
VOID 
delDFA()
{
  CONST Count nStates= G.nStates;
  Index s;
  for (s= 0; s < nStates; s++) {
    if (G.states[s].actions != BAD_ISET) delISet(G.states[s].actions);
  }
  FREE(G.states);
  FREE(G.trans);
  VOID_RET();
}

static State
newState()
{
  Index s= A_NEXT(buildP->states);
  STATES(s).val= STATES(s).actions= BAD_ISET;
  STATES(s).transLo= STATES(s).transHi= 0;
  STATES(s).sentinelSucc= NIL; STATES(s).addIntraTok= FALSE;
  return s;
}


/* Return true of pattern-positions in set s has 1 or more transitions. */
static Boolean
isSigPositionSet(s)
  ISet s;
{
  CONST Index nClasses= nPatLeaves();
  Index e;
  VOIDP iterP;
  FOR_EACH_IN_ISET(e, s, iterP) {
    if (e < nClasses && !isEmptyISet(LEAF_FOLLOWS(e))) return TRUE;
  }
  return FALSE;
}

/* Get unique state for pattern-positions in set s.  Delete set s if 
 * already in states table.
 */
static State 
getState(s0, addIntraTokX)
  ISet s0;
  BooleanX addIntraTokX;
{
  Index2 *CONST statesHashTab= buildP->hashTab;
  Boolean addIntraTok= addIntraTokX;
  ISet s= (addIntraTok && nIntraTokPats() > 0 && isSigPositionSet(s0)) 
          ? unionISet(s0, getIntraFirsts()) 
          : s0;
  CONST Index h= hashSumISet(s) % STATES_HASH_SIZE;
  Index i= statesHashTab[h]; 
  while (i != NIL) {
    if (compareISet(s, STATES(i).val) == 0) { 
      delISet(s); 
      assert(STATES(i).addIntraTok == addIntraTok);
      return i; 
    }
    i= STATES(i).u.succ;    
  }
  i= newState();
  STATES(i).val= s; 
  STATES(i).addIntraTok= addIntraTok;
  STATES(i).u.succ= statesHashTab[h]; statesHashTab[h]= i;
  return i;
}


/*			    START STATES.				*/

/* Establish start-states.  If '^' anchor is used, then set up 2 states
 * for each start-state, with the even numbered state corresponding to
 * those patterns without any '^', and the odd-numbered state corresponding
 * to all (including '^') patterns.  Return the total number of states
 * created.
 *
 * It is possible that some of the created states are redundant, in
 * that they are identical.  In that case, still create the redundant
 * state but with val == BAD_ISET.  Don't chain it on the hash-table
 * chain, but point its hash-succ-link to the non-redundant identical
 * state.
 *
 * Note that for redundant states, the corresponding first sets delivered
 * by the patterns module will be deleted and replaced.
 */
static Count 
setupStartStates()
{
  CONST Count nSS= nStartStates();
  CONST Count nStatesPerSS= hasStartLinePat() ? 2 : 1;
  Index i;
  assert(A_NENTRIES(buildP->states) == 0);
  for (i= 0; i < nSS; i++) {
    Index j;
    ISet *firsts= getStartStateFirsts(i);
    for (j= 0; j < nStatesPerSS; j++) {
      State s;
      (VOID) GRAB_ISET(firsts[j]);
      s= getState(firsts[j], TRUE); 
      if (s != nStatesPerSS*i + j) {	/* Redundant start-state. */
	State dummy= newState(); 	/* Make dummy state with BAD_ISET. */
	assert(s <  nStatesPerSS*i + j);/* s was previously created state. */
	assert(dummy == nStatesPerSS*i + j);
	STATES(dummy).u.alt= s; 
#if 0
	first[j]= STATES(s).val; 	/* Replace first set. */
#endif
      }
    } /* for (j= 0; j < nStatesPerSS; j++) */
  } /* for (i= 0; i < nSS; i++) */
  assert(A_NENTRIES(buildP->states) == nSS*nStatesPerSS);
  return A_NENTRIES(buildP->states);
}


/*			    CONTEXT STATES.				*/

/* Create states to start automaton when searching variable-context
 * patterns.  Specifically, for a variable context pattern pat/context
 * create a state which scans merely pat and another which scans
 * merely context.  Return the # of states created.
 *
 * Assumes that pat & context are disjoint in the pattern space
 * with the conjunction of pat & context (which will be scanned 
 * before the searching begins).  Also assumes that pat & context
 * have been terminated with dummy actions.
 */
static Count
setupContextStartStates()
{
  CONST Index nStates= A_NENTRIES(buildP->states); /* # of states on entry. */
  Index v;
  for (v= nextVarContext(NIL); v != NIL; v= nextVarContext(v)) {
    Index i;
    ISet *firsts= varContextFirsts(v);
    Index2 *rescanStates= varContextStates(v);
    for (i= 0; i < 2; i++) {
      (VOID) GRAB_ISET(firsts[i]);
      rescanStates[i]= getState(firsts[i], FALSE);
      /* The rescan state must be new. */
      assert(rescanStates[i] == A_NENTRIES(buildP->states) - 1); 
    }
  }
  return A_NENTRIES(buildP->states) - nStates;
}


/*			SENTINEL STATE SETUP.				*/

/* Setup a special state for runtime sentinel processing with val
 * field set to the empty position set.  Return the state number.  The
 * sentinel state is actually used at runtime when
 * --backup-optimize=1.  When --backup-optimize=0, no sentinel state
 * is ever created.  When --code-scan=1, a sentinel state is created
 * and used during the building of the code scanner but is not used at
 * runtime.
 */
static State
setupSentinelState()
{
  Index sentinelState= NIL;
  if (optionsP->backupOptimize || optionsP->doCode) {
    sentinelState= newState();
    STATES(sentinelState).actions= unaryISet(SENTINEL_ACT);
  }
  return sentinelState;
}


/*			       DFA CLOSURE.				*/

/* Ensure that all successors of any DFA state are contained in the DFA.
 * Essentially the algorithm on pg. 141 of Aho, Sethi, Ullman with some
 * of the loops turned inside out. 
 */
static VOID
closeDFA() 
{
  CONST Count nIn= G.nIn;
  ISet *transVec= MALLOC_VEC(nIn, ISet);
  CONST Index2 sentinelIn= optionsP->equivClasses 
                           ? getEqClass(optionsP->sentinelChar) 
                           : optionsP->sentinelChar;
  CONST Index nClasses= nPatLeaves();
  CONST ISet *CONST classSets= getClassISets();
  BuildDFA *CONST bP= buildP;
  Area *CONST statesP= &bP->states;
  Area *CONST transP= &bP->trans;
  Index i;

  G.sentinelIn= sentinelIn;
  for (i= 0; i < nIn; i++) transVec[i]= BAD_ISET;

  /* Main closure loop.  Continue until no unprocessed states. */
  for (i= 0; i < A_NENTRIES(*statesP); i++) { /* moving target. */
    CONST Boolean addIntraTok= STATES(i).addIntraTok;
    Boolean isSig= FALSE; /* TRUE if state i has non-error transitions. */
    ISetIter valIter;
    ISetElement e;
    Index j;
    /* Do not cache ptr to STATES(i) as STATES can change. */

    if (STATES(i).val == BAD_ISET) continue; /* Dup. state for start-state. */

    for (j= 0; j < nIn; j++) assert(transVec[j] == BAD_ISET);

    FOR_EACH_IN_ISET(e, STATES(i).val, valIter) {
      /* Position e from STATES(i).val must correspond to either an
       * ACT_PAT or a CLASS_PAT.  CLASS_PATs will have e < nClasses;
       * all others will be ACT_PATs.  If an ACT_PAT, then add the
       * action to the state.  If a CLASS_PAT, then for each input
       * element of that char. class, union the follow set of that
       * position with the set indexed by the input element in
       * transVec.
       */
      if (e >= nClasses) { /* ACT_PAT */
        if (STATES(i).actions == BAD_ISET) STATES(i).actions= newISet();
        addISet(STATES(i).actions, e - nClasses);
      }
      else if (!isEmptyISet(LEAF_FOLLOWS(e))) { /* CLASS_PAT. */
        ISet inputs= classSets[LEAF_CLASSES(e)];
        ISetElement input;
	ISetIter inputsIter;
	FOR_EACH_IN_ISET(input, inputs, inputsIter) {
          if (transVec[input] == BAD_ISET) transVec[input]= newISet();
          unionISet(transVec[input], LEAF_FOLLOWS(e));
	  isSig= TRUE;
        }
      }
    } /* FOR_EACH_IN_ISET(e, STATES(i).val, valIter) */
    STATES(i).transLo= A_NENTRIES(*transP);
    if (isSig) {	/* State has at least one non-error transition. */
      for (j= 0; j < nIn; j++) {
        /* Trans. for sent. & each non-NIL transVec element. */
	if (j == sentinelIn /* && addIntraTok */) {
#if 0
	  Transition t= A_NEXT(*transP);
	  TRANS(t).input= sentinelIn; 
	  TRANS(t).nextState= G.sentinelState;
#endif
	  STATES(i).sentinelSucc= 
	    transVec[j] == BAD_ISET ? NIL : getState(transVec[j], TRUE);
	  transVec[j]= BAD_ISET;
	}
	else if (transVec[j] != BAD_ISET) {
	  Transition t= A_NEXT(*transP);
	  TRANS(t).input= j;  
	  TRANS(t).nextState= getState(transVec[j], addIntraTok);
	  transVec[j]= BAD_ISET;
	}
      } /* for (j= 0; j < nIn; j++) */
    } /* if (isSig) */

    STATES(i).transHi= A_NENTRIES(*transP);
  }
  FREE(transVec);
  VOID_RET();
}


/* 			PROCESS REDUNDANT START STATES.			*/

/* Fill in redundant  start-states (those with BAD_ISET val entries). 
 * Make deep-copies of transitions and val-set.
 */
static VOID 
processRedundantStartStates() 
{
  CONST Count n= G.nInitialStates;
  Area *CONST transP= &buildP->trans;
  Index i;
  for (i= 0; i < n; i++) {	
    if (STATES(i).val == BAD_ISET) {
      Index dup= STATES(i).u.alt;
      Index dupTransHi= STATES(dup).transHi;
      Index j;
      STATES(i)= STATES(dup);	/* Shallow copy for most fields. */
      STATES(i).transLo= A_NENTRIES(*transP);
      for (j= STATES(dup).transLo; j < dupTransHi; j++) {
	/* Copy transitions, so they can be modified for pseudo-sentinels. */
	Transition t= A_NEXT(*transP);
	TRANS(t)= TRANS(j);
      }
      STATES(i).transHi= A_NENTRIES(*transP);
      /* Ensure fresh copy of val set. */
      STATES(i).val= copyISet(STATES(i).val);
    }
  }
  VOID_RET();
}


/*			CREATE INTRA-PATTERN STATES.			*/

/* Make any state which has an intra-pattern action into a intra-pattern
 * state.  Create alternate states for those intra-pattern states which
 * have other actions or non-error transitions.
 */
static VOID
createIntraPatStates()
{
  CONST Count nIntra= nIntraTokPats();
  CONST ISet intraPos= getIntraPos();
  if (nIntra > 0) { /* We do have intra-token patterns. */
    CONST Count nStates= A_NENTRIES(buildP->states);
    Index jamChain= NIL; /* Chain of states which need alt set to jam-state. */
    State s, nextS, jamState;
    for (s= 0; s < nStates; s++) {
      /* Do not cache ptr to STATES(s) as STATES may change. */
      if (STATES(s).actions != BAD_ISET && s != G.sentinelState) {
	ISet acts= STATES(s).actions;
	Index intraAct= firstElementISet(acts);
	if (intraAct <= nIntra) { /* We have a intra-token action. */
	  ISetIter actsIter;
	  assert(intraAct != 0);
	  STATES(s).actions= unaryISet(intraAct);
	  FOR_EACH_IN_ISET(intraAct, acts, actsIter) {
	    if (intraAct > nIntra) break;
	    rmISet(acts, intraAct);
	  }
	  if (isEmptyISet(acts) && isSubsetISet(STATES(s).val, intraPos)) {
	    delISet(acts); 
	    STATES(s).u.alt= jamChain; jamChain= s; /* Link onto jam chain. */
	    STATES(s).sentinelSucc= NIL;
	    STATES(s).transLo= STATES(s).transHi= 0;
	  }
	  else { /* Need to create a new alternate state. */
	    State alt= newState();
	    STATES(alt)= STATES(s); 
	    STATES(alt).val= BAD_ISET;
	    STATES(s).u.alt= alt;
	    STATES(s).sentinelSucc= NIL;
	    STATES(s).transLo= STATES(s).transHi= 0;
	    if (sizeISet(acts) > 0) {
	      STATES(alt).actions= acts;
	    }
	    else {
	      STATES(alt).actions= BAD_ISET; delISet(acts);
	    }
	  } /* else need to create a new alternate state. */
	} /* If we have a intra-token action. */
      } /* if (STATES(s).actions != BAD_ISET && s != G.sentinelState) */
    } /* for (s= 0; s < nStates; s++) */
    jamState= A_NENTRIES(buildP->states);
    for (s= jamChain; s != NIL; s= nextS) {
      nextS= STATES(s).u.alt; STATES(s).u.alt= jamState;
    }
  } /* if (nIntra > 0) */
}
  
/*			   STATES REMAPPING.				*/
/* Map states of dfa so that states are ordered (<) as follows:	
 *   Start states (including var-context start states).				
 *   Non-accepting states with legal transitions.
 *   Accepting states with legal transitions.		
 *   Accepting states with no legal transitions with intra-token actions
 *   with non-jam alternate states.	
 *   Other accepting states with no legal transitions.
 *   Sentinel state (if backup optimize).
 * Non-accepting states with no legal transitions are impossible (except for
 * the jam-error state which is not explicitly included in the DFA).
 */
static XDFAState * 
remapDFA() 
{
  CONST Count nStates= G.nStates;
  CONST State first= G.nInitialStates + G.nContextStarts;
  CONST Count nIntra= nIntraTokPats();
  CONST Count nTrans= G.nTrans;
  Trans *transBase= &TRANS(0);
  State *map= MALLOC_VEC(nStates + 1, State);
  XDFAState *newStates= MALLOC_VEC(nStates, XDFAState);
  enum { N_TYPES= 4 };
  ISet stateTypes[N_TYPES];
  Index i;
  State s;

  /* stateTypes[0]: Non-accepting non-start states with legal transitions;
   * stateTypes[1]: Accepting states with legal transitions.
   * stateTypes[2]: Accepting states with no legal transitions with
   * intra-token actions and a non-jam alternate state.
   * stateTypes[3]: Other accepting states with no legal transitions.
   */
  for (i= 0; i < N_TYPES; i++) stateTypes[i]= newISet();
  for (s= first; s < nStates; s++) {
    CONST DFAState *sP= &STATES(s);
    if (sP->transLo != sP->transHi || sP->sentinelSucc != NIL) {
      /* State s has legal transitions. */
      addISet(stateTypes[sP->actions != BAD_ISET ? 1 : 0], s);
    }
    else { /* No legal transitions. */
      CONST Index firstAct= firstElementISet(sP->actions);
      CONST Index typeIndex= 
	(0 < firstAct && firstAct <= nIntra && sP->u.alt != nStates)
	? 2
	: 3;
      addISet(stateTypes[typeIndex], s);
    }
  }

  /* Setup # of non-accepting states. */
  G.nNonAcceptStates= first + sizeISet(stateTypes[0]);

  /* Build up map from states to newStates. */
  for (s= 0; s < first; s++) map[s]= s; /* Identity map for start-states. */
  for (i= 0; i < N_TYPES; i++) {
    ISetElement e;
    ISetIter stateTypesIter;
    FOR_EACH_IN_ISET(e, stateTypes[i], stateTypesIter) {
      if (e != G.sentinelState) map[e]= s++;
    }
  }
  if (G.sentinelState != NIL) map[G.sentinelState]= s++;
  assert(s == nStates);
  map[s]= nStates;			/* Identity map for jam-state. */

  /* Map states. */
  for (s= 0; s < nStates; s++) {
    CONST DFAState *sP0= &STATES(s);
    XDFAState *sP1= &newStates[map[s]];
    sP1->actions= sP0->actions; 
    sP1->transLo= sP0->transLo;  sP1->transHi= sP0->transHi; 
    sP1->alt= sP0->u.alt;
    sP1->sentinelSucc=
      (sP0->sentinelSucc == NIL) ? NIL : map[sP0->sentinelSucc];
    if (sP0->actions != BAD_ISET) {
      CONST Index a= firstElementISet(sP0->actions);
      if (0 < a && a <= nIntra) {
	assert(sizeISet(sP0->actions) == 1);
	sP1->alt= map[sP0->u.alt];
      }
    }
  }

  /* Map transitions. */
  for (i= 0; i < nTrans; i++) {
    transBase[i].nextState= map[transBase[i].nextState];
  }

  /* Map sentinel state. */
  if (G.sentinelState != NIL) G.sentinelState= map[G.sentinelState];

  G.nNonFinalStates= first + sizeISet(stateTypes[0]);
  G.nSigStates= G.nNonFinalStates + sizeISet(stateTypes[1]);
  G.nIntraStates= sizeISet(stateTypes[2]);

  FREE(map);
  for (i= 0; i < N_TYPES; i++) delISet(stateTypes[i]);
  return newStates;
}


/*			OUTPUT DFA PARAMETERS.				*/

/* Output #defines for all DFA parameters to outFile. Return # of 
 * newlines output. 
 */
Count 
outDFAParams(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  fprintf(outFile, "#define YY_N_INITIAL_STATES %d\n", G.nInitialStates); 
  nLines++;
  fprintf(outFile, "#define YY_N_INTRA_STATES %d\n", G.nIntraStates); 
  nLines++;
  fprintf(outFile, "#define YY_N_NON_FINAL_STATES %d\n", G.nNonFinalStates); 
  nLines++;
  fprintf(outFile, "#define YY_N_SIG_STATES %d\n", G.nSigStates); 
  nLines++;
  fprintf(outFile, "#define YY_N_STATES %d\n", G.nStates);
  nLines++;
  fprintf(outFile, "#define YY_SENTINEL_STATE %d\n", 
	  (G.sentinelState == NIL) ? 0 : G.sentinelState);
  nLines++;
  return nLines;
}


/*			OUTPUT ACTION SWITCH.				*/

/*

We establish a injective mapping from patActions to states by mapping
each action to the first state in which it is found as the first
action.  Actions which do not occur as the first action in any state
are referred to as orphan actions: since such orphans may be executed
upon REJECTs, they are mapped to pseudo states with negative state
numbers.

*/

enum {
  REJECT_STATE_INIT= -1,	/* Used for initializing a reject state #. */
  FIRST_ORPHAN_REJECT_STATE= -2 /* First orphan reject state. */
};

typedef struct ActState {	/* Track owning state for each action. */
  Int2 stateNum;		/* State which owns this act. */
  union {
    Boolean isMulti;		/* True if more than 1 state has this action. */
    Index2 orphanSucc;		/* Successor in orphan acts list. */
  } u;
} ActState;

typedef struct ActStateInfo {
  Index sentinelActN;	/* Sentinel action #. */
  Index defaultActN;	/* Default action. */
  Index yyLineNoActN;	/* yylineno action. */
  ActState *actStates;	/* Track owning state for each patAction. */
} ActStateInfo;

 
/* Initialize each action state entry to its owning state.  Create dummy
 * states (negative state #s) for actions which have no owning action and
 * return the index of the list of orphan actions (threaded thru the 
 * orphanSucc member of ActState).
 */
static Index
initActStateInfo(actStateInfoP)
  ActStateInfo *CONST actStateInfoP;
{
  CONST Count nUserPs= nUserPats();
  /*              Sentinel Act User acts Default     yylineno act.	*/
  CONST Count nActs=    1     + nUserPs +   1   + optionsP->doYYLineNo;

  CONST Index startStateInc= hasStartLinePat() ? 2 : 1;

  ActState *CONST actStates= MALLOC_VEC(nActs, ActState);
  Count nProcessedActs= 0;
  Index orphanIndex= NIL;
  Index a, s;

  actStateInfoP->sentinelActN= SENTINEL_ACT;
  actStateInfoP->defaultActN= nUserPs + 1;
  actStateInfoP->yyLineNoActN= optionsP->doYYLineNo ? nUserPs + 2 : NIL;
  actStateInfoP->actStates= actStates;

  for (a= 0; a < nActs; a++) {
    actStates[a].stateNum= REJECT_STATE_INIT; actStates[a].u.isMulti= FALSE;
  }

  for (s= 0; s < G.nInitialStates; s+= startStateInc) {
    Index a= getEOFAct(s/startStateInc);
    if (a != NIL) {
      if (actStates[a].stateNum == REJECT_STATE_INIT) {
	actStates[a].stateNum= s; nProcessedActs++;
      }
      else {
	actStates[a].u.isMulti= TRUE;
      }
    }
  } /* for (s= 0; s < G.nInitialStates; s+= startStateInc) */
  assert(nProcessedActs <= G.nInitialStates);

  for (s= G.nNonFinalStates; s < G.nStates; s++) {
    Index a= unmapAct(firstElementISet(G.states[s].actions));
    if (actStates[a].stateNum == REJECT_STATE_INIT) {
      actStates[a].stateNum= s; nProcessedActs++;
    }
    else {
      actStates[a].u.isMulti= TRUE;
    }
  }
  assert(nProcessedActs <= nActs);

  if (nProcessedActs < nActs) { /* We have some orphan acts. */
    Int dummyStateN= FIRST_ORPHAN_REJECT_STATE;
    for (a= 0; a < nActs; a++) {
      if (actStates[a].stateNum == REJECT_STATE_INIT) {
	if (a == SENTINEL_ACT) { /* Sentinel act orphan only if !backup-opt. */
	  assert(!optionsP->backupOptimize);
	}
	else {
	  actStates[a].stateNum= dummyStateN--;
	  actStates[a].u.orphanSucc= orphanIndex;
	  orphanIndex= a;
	}
      }
    }
  }

  return orphanIndex;
}

static VOID
freeActStateInfo(actStateInfoP)
  CONST ActStateInfo *CONST actStateInfoP;
{
  FREE(actStateInfoP->actStates);
  VOID_RET();
}

/* Output patAction # a, checking for special cases.  The special
 * cases are the default action, sentinel action, yylineno action (the
 * action is a call to the appropriate macro).
 */
static Count
outXAction(outFile, actStateInfoP, a)
  FILE *outFile;
  CONST ActStateInfo *CONST actStateInfoP;
  Index a;
{
  Count nLines= 0;
  if (a == actStateInfoP->sentinelActN) {
    fprintf(outFile, "\t  %s();\n", SENTINEL_STATE_MACRO); nLines++;
  }
  else if (a == actStateInfoP->defaultActN) {
    fprintf(outFile, "\t  %s();\n", DEFAULT_ACT_MACRO); nLines++;
  }
  else if (a == actStateInfoP->yyLineNoActN) {
    fprintf(outFile, "\t  %s();\n", YYLINENO_ACT_MACRO); nLines++;
  }
  else {
    assert(0 < a && a <= nUserPats());
    nLines+= outPatAct(outFile, a);
  }
  return nLines;
}

/* Output patAction a (NIL means no action) for state s.  If a is not
 * owned by s, then output a goto to an appropriate label.  Take care
 * of special cases for default EOF actions (no action output);
 * default action, sentinel action, yylineno action (action is a call
 * to the appropriate macro).  Terminate each action by a break macro.
 * Returns # of lines output.
 */
static Count
outActState(outFile, s, a, actStateInfoP)
  FILE *outFile;
  Index s;				/* State #. */
  Index a;				/* Action for state s. */
  CONST ActStateInfo *CONST actStateInfoP;
{
  Count nLines= 0;
  assert((s < G.nInitialStates || s >= G.nNonFinalStates) && s < G.nStates);
  if (a == NIL) {
    assert(s < G.nInitialStates);
  }
  else { /* a != NIL */
    CONST ActState *CONST actStates= actStateInfoP->actStates;
    CONST Index stateNum= actStates[a].stateNum;
    if (stateNum == s) {
      if (actStates[a].u.isMulti) {
	fprintf(outFile, "\t%s%d:\n", STATE_ACT_LABEL_PREFIX, s); nLines++;
      }
      nLines+= outXAction(outFile, actStateInfoP, a);
    } /* if (stateNum == s) */
    else { /* Not owner state. */
      assert(actStates[a].u.isMulti);
      assert(stateNum >= 0);
      fprintf(outFile, "\t  goto %s%d;\n", STATE_ACT_LABEL_PREFIX, stateNum);
      nLines++;
    }
  } /* else a != NIL */
  fprintf(outFile, "\t  %s;\n", BREAK_MACRO); nLines++;
  return nLines;
}

/* Output actions with state #s as case labels.  (negative state #s
 * for orphan actions).  Returns # of lines output.
 * 
 * This routine first decides that the scanner has pseudo-states if no
 * backup optimize and it isn't a code scanner.  It first outputs
 * orphan-states (with -ve numbers) with the orphaned actions.  Then
 * it output start-states with EOF actions.  Then if the scanner has
 * pseudo-states, it outputs non-final non-start states with a rescan
 * action.  Then it outputs accepting states (omitting the final
 * sentinel state if it has pseudo-states) with their actions.
 * Finally if scanner has pseudo-states, then it outputs a default
 * action which does sentinel processing.
 */
Count 
outActCases(outFile)
  FILE *outFile;
{
  ActStateInfo actStateInfo;
  CONST Index orphansIndex= initActStateInfo(&actStateInfo);
  CONST ActStateInfo *CONST actStateInfoP= &actStateInfo;
  CONST ActState *actStates= actStateInfoP->actStates;
  CONST Index startStateInc= hasStartLinePat() ? 2 : 1;
  CONST Boolean hasPseudoStates= !optionsP->backupOptimize && !optionsP->doCode;
  CONST Count nActiveStates= G.nStates;
  Count nLines= 0;
  Index s, i;
  if (optionsP->doReject) buildRejects(actStateInfoP->actStates);
  for (i= orphansIndex; i != NIL; i= actStates[i].u.orphanSucc) {
    fprintf(outFile, "\tcase %d:\n", actStates[i].stateNum); nLines++;
    nLines+= outXAction(outFile, actStateInfoP, i);
    fprintf(outFile, "\t  %s;\n", BREAK_MACRO); nLines++;
  }
  for (s= 0; s < G.nInitialStates; s+= startStateInc) {
    Index a= getEOFAct(s/startStateInc);
    fprintf(outFile, "\tcase %d:\n", s); nLines++;
    nLines+= outActState(outFile, s, a, actStateInfoP);
  }
  if (hasPseudoStates && G.nInitialStates < G.nNonFinalStates) {
    for (s= G.nInitialStates; s < G.nNonFinalStates; s++) {
      fprintf(outFile, "\tcase %d:\n", s); nLines++;
    }
    fprintf(outFile, "\t  %s(); %s;\n", RESCAN_MACRO, BREAK_MACRO); nLines++;
  }
  for (s= G.nNonFinalStates; s < nActiveStates; s++) {
    Index a= unmapAct(firstElementISet(G.states[s].actions));
    fprintf(outFile, "\tcase %d:\n", s); nLines++;
    nLines+= outActState(outFile, s, a, actStateInfoP);
  }
  if (hasPseudoStates) {
    fprintf(outFile, "\tdefault:\n\t  %s();\n", SENTINEL_PSEUDO_MACRO);
    nLines+= 2;
  }
  freeRejects();
  freeActStateInfo(actStateInfoP);
  return nLines;
}


/*				REJECTS					*/

/*

Choose alternate reject actions for each action (these are rejections
which backtrack to patterns of the same length).  In general, an
action may have multiple rejection acts, and the state needs to be
tested (at runtime) to determine the appropriate rejection act.  The
algorithm used here assumes that usually an action has a single
rejection act, and the code produced for multiple rejection acts will
be suboptimal (in particular, the choice of the default rejection
act), but the algorithm is simpler.

*/

typedef struct {
  Int2 rejectStateN;	/* REJECT_STATE_INIT-> init; 0-> backup.  */
  Index2 succ;		/* Index in ALT_REJECTS[] of next reject act. */
  ISet states;		/* Set of states in which reject act is applicable. */
} AltReject;

static AREA(altRejects, AltReject, 3, UINT2_MAX);
#define ALT_REJECTS(i)	A_ACCESS(altRejects, AltReject, i)

typedef struct {
  Int2 rejectStateN;	/* Default reject state to reject to. */
                        /* REJECT_STATE_INIT-> init; 0-> backup.  */
  Index2 succ;		/* Index in ALT_REJECTS[] of next reject act. */
} ActReject;

/* Indexed by source action #. */
static ActReject *actRejects= NULL;

static VOID
buildRejects(actStates)
  CONST ActState *CONST actStates;
{
  CONST Count nStates= G.nStates;
  CONST Count nActs= nSrcActs();
  CONST Count maxPatActN= nUserPats();
  CONST Index defaultActN= maxPatActN + 1;
  Index s, a;
  actRejects= MALLOC_VEC(nActs, ActReject);
  for (a= 0; a < nActs; a++) {
    actRejects[a].rejectStateN= REJECT_STATE_INIT; actRejects[a].succ= NIL;
  }
  for (s= G.nNonFinalStates; s < nStates; s++) {
    CONST ISet actions= G.states[s].actions;
    ISetIter actionsIter;
    assert(actions != BAD_ISET);
    FOR_EACH_IN_ISET(a, actions, actionsIter) { 
      /* for each output action # a in state s. */
      CONST Index a0= unmapAct(a);
      if (1 <= a0 && a0 <= maxPatActN) { /* User action. */
	CONST Index a1= patActToSrcAct(a0);
	CONST Int defRejStateN= actRejects[a1].rejectStateN;
	Int rejectStateN;
	{ Index rej= a;
	  Index rej0;
	  ISetIter rejIter;
	  do { /* Find reject output action for output action a. */
	    rejIter= VOID_ISET_ITER;
	    rej= nextElementISet(actions, rej, &rejIter); 
	    if (rejIter != VOID_ISET_ITER) rej0= unmapAct(rej);
	  } while (rejIter != VOID_ISET_ITER 
		   && !(1 <= rej0 && rej0 <= defaultActN));
	  rejectStateN= 
	    (rejIter == VOID_ISET_ITER) ? 0 : actStates[rej0].stateNum;
	}
	if (defRejStateN == REJECT_STATE_INIT) { 
	  /* No reject actions seen previously for a1. */
	  actRejects[a1].rejectStateN= rejectStateN;
	  actRejects[a1].succ= NIL;
	}
	else if (rejectStateN != defRejStateN) { /* Non-default rejection. */
	  Index i= actRejects[a1].succ; 
	  while (i != NIL) {
	    assert(i < A_NENTRIES(altRejects));
	    if (ALT_REJECTS(i).rejectStateN == rejectStateN) break;
	    i= ALT_REJECTS(i).succ;
	  }
	  if (i == NIL) { /* rej1 is new, and not default rejection act. */
	    i= A_NEXT(altRejects);
	    ALT_REJECTS(i).rejectStateN= rejectStateN; 
	    ALT_REJECTS(i).states= unaryISet(s);
	    ALT_REJECTS(i).succ= actRejects[a1].succ;
	    actRejects[a1].succ= i;
	  }
	  else {
	    addISet(ALT_REJECTS(i).states, s);
	  }
	} /* else if (rejectStateN != defRejStateN) non-default rejection. */
      } /* if (1 <= a && a <= maxPatActN)  User action. */
    } /* FOR_EACH_IN_ISET(a, actions, actionsIter) */
  } /* for (s= G.nNonFinalStates; s < nStates; s++) */
}

static VOID
freeRejects()
{
  CONST Count nAltRejects= A_NENTRIES(altRejects);
  Index i;
  for (i= 0; i < nAltRejects; i++) delISet(ALT_REJECTS(i).states);
  delArea(&altRejects);
  FREE(actRejects);
}

/* Output body for reject macro for action with source index srcActN to
 * outFile.  Return # of newlines output.
 */
Count
outRejectMacroBody(outFile, srcActN)
  FILE *outFile;
  Index srcActN;
{
  CONST Index defRejStateN= actRejects[srcActN].rejectStateN;
  Count nLines= 0;
  Index i= actRejects[srcActN].succ; 
  fputs("  do { \\\n", outFile); nLines++;
  while (i != NIL) {
    AltReject *arPi= &ALT_REJECTS(i);
    Count nInLine= 0;
    Index s;
    ISetIter rejectStatesIter;
    assert(i < A_NENTRIES(altRejects));
    fputs("    if (", outFile);
    FOR_EACH_IN_ISET(s, arPi->states, rejectStatesIter) {
      fprintf(outFile, " %s == %d ||", REJECT_STATE_VAR, s);
      nInLine++;
      if (nInLine%2 == 0) { fputs(" \\\n\t", outFile); nLines++; }
    }
    fputs(" 0) ", outFile);
    if (arPi->rejectStateN == 0) {
      fprintf(outFile, "goto %s;", BACKUP_LAB);
    }
    else {
      fprintf(outFile, "{ %s= %d; goto %s; }", 
	      STATE_VAR, arPi->rejectStateN, DO_ACTION_LAB);
    }
    fputs(" \\\n", outFile); nLines++;
    i= arPi->succ;
  }
  if (defRejStateN == REJECT_STATE_INIT || defRejStateN == 0) {
    fprintf(outFile, "    goto %s;", BACKUP_LAB);
  }
  else {
    fprintf(outFile, "    { %s= %d; goto %s; }", STATE_VAR, 
	    defRejStateN, DO_ACTION_LAB);
  }
  fputs(" \\\n", outFile); nLines++;
  fputs("  } while (0) \n", outFile); nLines++;
  return nLines;
}


/*			PRINT DFA STATISTICS.				*/

static VOID 
printDFAStats(outFile)
  FILE *outFile;
{
  fprintf(outFile, "DFA STATS:\n");
  fprintf(outFile, "# of states= %d;\n", G.nStates);
  fprintf(outFile, "# of accepting states= %d;\n", 
	  G.nStates - G.nNonFinalStates);
  fprintf(outFile, "# of non-accepting states= %d\n", G.nNonFinalStates);
  fprintf(outFile, "# of initial states= %d;\n", G.nInitialStates);
  fprintf(outFile, "# of context start states= %d\n", G.nContextStarts);
  fprintf(outFile, "# of significant states= %d;\n", G.nSigStates);
  fprintf(outFile, "# of non-error transitions= %d\n", G.nTrans);
  assert(G.sentinelState != NIL);
  fprintf(outFile, "Sentinel state= %d.\n", G.sentinelState);
  VOID_RET();
}


/*			     PSEUDO-SUCCESSORS.				*/

#if 0

/* If we have a table-driven scanner and !backupOptimize, then we
 * convert transitions from a state s to a successor which is a
 * jamState (sentinel-state) to a successor which is s + nStates1 (s +
 * 2*nStates1) where nStates1 is # of scanner states - 1 (since
 * sentinel states disappears in scanners with pseudo-states).
 */
static VOID
makePseudoSentinel()
{
  CONST Count nStates= G.nStates;
  CONST Count nSigStates= G.nSigStates;
  CONST Index sentinelState= G.sentinelState;
  CONST Count nStates1= nStates - 1;
  Index s;
  assert(!optionsP->doCode);
  assert(!optionsP->backupOptimize);
  for (s= 0; s < nSigStates; s++) {
    CONST Index hi= G.states[s].transHi;
    Index i;
    for (i= G.states[s].transLo; i < hi; i++) {
      if (G.trans[i].nextState == sentinelState) {
	((Trans*)(G.trans))[i].nextState= s + 2*nStates1; 
	break; /* Assume only a single transition to sentinel state. */
      }
    }
  }
  VOID_RET();
}

#endif



/*			MAIN DFA BUILDING ROUTINE.			*/

VOID 
buildDFA()
{
  initDFABuild();
  closeDFA();
  terminateLeaves(); delClassSets();
  processRedundantStartStates();
  createIntraPatStates();
  G.nStates= A_NENTRIES(buildP->states); G.nTrans= A_NENTRIES(buildP->trans);
  G.states= remapDFA();
  G.trans= delDFABuild();
#if 0
  if (!optionsP->doCode && !optionsP->backupOptimize) makePseudoSentinel();
#endif
  if (optionsP->isVerbose) printDFAStats(stdout);
#if DO_TRACE
  if (files.dumpFile && !optionsP->compress) dumpDFA(files.dumpFile);
#endif
  VOID_RET();
}

/*			     DUMP DFA.					*/

#ifdef DO_TRACE

VOID
dumpDFAState(dumpFile, s)
  FILE *dumpFile;
  Index s;
{
  enum { N_PER_LINE= 10 };
  CONST Count nIntra= nIntraTokPats();
  CONST XDFAState *sP= &G.states[s];
  CONST Index transHi= sP->transHi;
  CONST nUPats= nUserPats();
  Count nPerLine= 0;
  Index i;
  fprintf(dumpFile, "**%d**\t", s);
  for (i= sP->transLo; i < transHi; i++) {
    fprintf(dumpFile, "%d/%d ", G.trans[i].input, 
	    G.trans[i].nextState);
    nPerLine++;
    if (i != transHi - 1 && nPerLine == N_PER_LINE) {
      nPerLine= 0; fputs("\n\t", dumpFile);
    }
  } /* for (i= sP->transLo; i < sP->transHi; i++) */
  fputs("\n  ", dumpFile);
  if (sP->actions != BAD_ISET) {
    ISetIter actsIter;
    ISetElement a;
    fputs("Actions (src/pat/mapped): { ", dumpFile);
    FOR_EACH_IN_ISET(a, sP->actions, actsIter) {
      CONST Index patAct= unmapAct(a);
      if (patAct > 0) {
	fprintf(dumpFile, "%d/%d/%d ", 
		(patAct < nUPats) ? patActToSrcAct(patAct) : 0, patAct, a); 
      }
      else { /* sentinel action. */
	fputs("0/0/0 ", dumpFile);
      }
    }
    fputs("}\n", dumpFile);
    a= firstElementISet(sP->actions);
    if (0 < a && a <= nIntra) {
      fprintf(dumpFile, " Alt: %d;", sP->alt);
    }
  }
  if (sP->sentinelSucc != NIL) {
    fprintf(dumpFile, "Sentinel succ= %d; ", sP->sentinelSucc);
  }
  VOID_RET();
}

static VOID
dumpDFA(dumpFile)
  FILE *dumpFile;
{
  CONST Count nStates= G.nStates;
  Index s;
  fputs("DFA:\n", dumpFile);
  for (s= 0; s < nStates; s++) {
    dumpDFAState(dumpFile, s);
    fputs("\n", dumpFile);
  } /* for (s= 0; s < nStates; s++) */
  fprintf(dumpFile, "Total # of transitions to be stored= %d\n", 
	  G.nIn * G.nSigStates);
  fputs("\n", dumpFile);
  printDFAStats(dumpFile);
}

#endif /* ifdef DO_TRACE */


/*			      TEST ROUTINE				*/

#if TEST_DFA

#include "zparse.h"

static Options options;
CONST Options *CONST optionsP= &options;

int main()
{
  options.charSetSize= 256;
  options.compress= 1;
  initParse(NULL);
  yyparse();
  computeClassISets();
#ifdef DO_TRACE
  dumpPatterns(stdout);
  dumpEqClasses(stdout);
#endif
  buildDFA();
#ifdef DO_TRACE
  dumpDFA(stdout);
#endif
  outDFA(stdout);
  outUncompressedDFA(stdout);
  outDFAParams(stdout);
  return 0;
}

#endif /* if TEST_DFA */
