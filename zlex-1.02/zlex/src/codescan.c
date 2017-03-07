/*

File:	 codescan.c
Purpose: Output DFA as code.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "chrclass.h"
#include "codescan.h"
#include "dfa.h"
#include "files.h"
#include "options.h"
#include "patterns.h"
#include "scan.h"
#include "sklnames.h"

#include "area.h"
#include "iset.h"
#include "error.h"
#include "memalloc.h"

/*

Produce directly coded DFA with states corresponding to positions in the
code.  

Type 0 States: start states.
Type 1 States: non-accepting states.
Type 2 States: accepting states with output transitions.
Type 3 States: accepting states without output transitions.

SPECIAL CASES

The code produced depends on the following factors:

a) Whether reject is used.

b) Whether equivalence classes are used.

c) Whether yy_act is used.

d) Whether the target C allows labels as first class objects.

HANDLING OVERSCAN

First note that because of the implicit pattern, a start state will
never be terminal; i.e. a transition will always be taken off a
start-state.

The distance from state s1 to s2 is defined to be the number of
transitions required to get from s1 to s2 on all paths between s1 and
s2.  It is defined only if the number of transitions is the same for all
paths from s1 to s2.

Because of the presence of the implicit pattern, all states at
distance 1 from start-states will be accepting states.

The accept-predecessors of a state s are the set of accepting states
s' such that there is a path from some start state to s such that s'
is the last accepting state on that path.  This implies that if s is
an accepting state then the accept-predecessors of s is the set {s}.

A state s is a action-known state iff the distances to s from each of
its accept-predecessors are the same and all its accept predecessors are
the same state (previously, instead of the second clause, we required
that the actions for each of its accept-predecessors are the same, but
that is not enough for a scanner with REJECTs; the new definition is
stronger in preventing states from being action-known, but in practice
will probably not make much of a difference).  A state which is not
action-known is said to be action-unknown.

A consequence of the above definitions is that only non-accepting
states can be action-unknown states.

An accepting state requires recording iff there is a path from it to
an action-unknown state which does not pass through any accepting
states.  When the scanner transits an accepting state which requires 
recording, we record its action and input position in some scanner
variables.  

If the scanner hits a jam transition in a action-unknown state, we use
the previously recorded scanner variables to backup the scanner to the
recorded input position and perform the recorded action.  If the
scanner hits a jam transition in a action-known state, we merely
backup the input by the accepting distance and perform the known
action.

The above quantities can be computed using a iterative algorithm.  For
each non-start state s a pair (state, dist) is initialized to (s, 0)
where s is the state # for accepting states, NIL for non-accepting
states.  For an action-unknown state, the acceptDist component will be
set to NIL by iterating the following steps until there is no change:

For each non-start state s such that s is an accepting state or
s.state != 0:

1.  If s.state is NIL, then for each non-accepting successor s'
of s, set s'.state and s'.dist to NIL.

2.  If s.dist is non-NIL, then for each non-accepting successor s' of
s, if s'.dist == 0 (just initialized) set s'.state to s.state and s'.dist
to s.dist + 1; else if s'.state != s.state or s'.dist != s.dist + 1, then
set s'.state and s'.acceptDist to NIL.


STATE AND ACTION SWITCHES

If the target C allows labels as first class objects, then state
numbers and action numbers can be encoded as labels (supporting yy_act
however necessitates a otherwise unnecessary assignment before each
action).  If the target C does not allow labels as first class
objects, then a state-switch is used to transfer to a state given its
number and a action-switch is used to transfer to a action given its
number.  The state-switch can also be used to transfer control to a
start state given its state number.  This switch is output to the
generated scanner using macros so that the user can choose between
normal switches and label variables at compile time.


STRUCTURE OF CODE FOR A STATE

The code for a state looks as follows:

	Input code;
	Transition code;

The input code will read the current input char into yyChar, transform
it into its equivalence class if so requested, and advance the input
pointer.  The transition code will make the transition to the next
state or to the appropriate action.  The transition code is discussed
in more detail below.

No input or transition code is produced for non-significant accepting
states.

The transition code is derived as follows: For each state, we first
compute the # of distinct targets and the # of distinct ranges.  If
the # of distinct targets is < LIN_THRESHOLD, we use a linear search
using boolean maps to collapse noncontiguous ranges into a single
test.  If the # of ranges is < BIN_THRESHOLD we use a binary search.
Else we use a switch.

When encoding binary searches we watch out for 1-element ranges which
breakup an otherwise contiguous range.  We special-case detect ranges
which are interrupted by upto 3 1-element subranges (when a otherwise
contiguous range is interrupted by 3 1-element subranges, we have a
total of 7 subranges, which would require upto 3 tests in a binary
search).

For the transition code of each state we will have some code devoted
to jamming.  The exact code will depend on several factors but will
always start with a check for a sentinel:
	if (yyChar == SENTINEL_CHAR) {
	  yyState= state #; goto sentinelCode;
	}

Else if it is a accepting state:
	Backup input pointer by 1 character to compensate for overscan;
	Code for action for state (or goto to it if it occurs earlier);

Else it is a action-known non-accepting state:
	Backup input pointer by accepting distance;
	Goto code for known action;

Else it is a action-unknown non-accepting state:
	Backup input pointer to lastAcceptCP;
	Goto code for action specified by lastAct;


IMPLEMENTATION

The current implementation makes several passes over the DFA.  Some of these
passes could be combined.  In fact, some of the information could be
gathered when the DFA is first built, but I prefer not to touch the other
modules at this point.
*/

/*			STATIC FUNCTION DECLARATIONS			*/

/* Forward struct declarations for pointers. */
struct Range;
struct Context;
struct LinTest;

static VOID initMaps PROTO((VOID_ARGS));
static Index getMap PROTO((ISet map));
static VOID freeMaps PROTO((VOID_ARGS));

static VOID initAccepts PROTO((VOID_ARGS));
static VOID freeAccepts PROTO((VOID_ARGS));
static VOID computeAccepts PROTO((VOID_ARGS));
static Boolean isRecording PROTO((Index state));
static VOID findRecordingStates PROTO((VOID_ARGS));

static VOID initLinTests PROTO((VOID_ARGS));
static VOID freeLinTests PROTO((VOID_ARGS));
static VOID addLinTestInput PROTO((struct LinTest *linTestP, Index input));
static Index addLinearTests PROTO((struct LinTest succInputs[], 
				   Count nSuccs, Index defaultIndex, 
				   Index loopIndex, Index sentinelIndex));
static VOID buildLinearTests PROTO((VOID_ARGS));

static VOID initRanges PROTO((VOID_ARGS));
static VOID freeRanges PROTO((VOID_ARGS));
static VOID buildRanges PROTO((Index s, Index *loP, Index *hiP));

static ConstString testCharString PROTO((Index c, Char *str));
static Count outTransition PROTO((FILE *outFile, Index state, Index succ,
				  Boolean isStart, Index tab, 
				  Boolean *jamDoneP));
static Count outLinState PROTO((FILE *outFile, Index s, Index tab, 
				Boolean *jamDoneP));
static Count outBinState PROTO((FILE *outFile, Index s,
				Index rangeLo, Index rangeHi,
				Boolean isStart, Count tab, Boolean *jamDoneP));
static VOID outSuccLabel PROTO((FILE *outFile, Index state, Index succ));
static Index mostCommonSuccState PROTO((Index rangeLo, Index rangeHi));
static Count outSwitchState PROTO((FILE *outFile, Index s,
				   Index rangeLo, Index rangeHi,
				   Boolean isStart, Count tab));
static Count outState PROTO((FILE *outFile, Index state, Index tab));
static Count outStateSwitch PROTO((FILE *outFile, Index tab));
static Count outJamSwitch PROTO((FILE *outFile, Index tab));
static VOID freeCode PROTO((VOID_ARGS));
static VOID printCodeStats PROTO((FILE *outFile));
static Count outCode PROTO((FILE *outFile));



/* 				GLOBAL CONTEXT				*/

/* Each routine will point to global context using a variable contextP. */

typedef struct Context {
  Area maps;		/* Contents of character maps. */
  Area linTests;	/* Linear tests. */
  Area ranges;		/* Input ranges with same successors. */ 
  Index2 *mapsHashTab;	/* Hash-table for character maps. */
  Boolean *visited;	/* Keep track of visited states. */
  struct Accept *accepts;/* Track action-known/recording states. */
  Index2 *stateTests;	/* Track which states have linear tests. */
  Count2 nActs;		/* # of source actions (including declarations). */
  Count2 nPats;		/* # of user patterns. */
  Boolean doneStateSwitch; /* True if state switch has been output. */
} Context;

static Context *contextP;

#define JAM_STATE		(dfaParams->nStates)	


/*				CHAR MAPS				*/

enum { 
  MAPS_HASH_SIZE= 16 /* Forward declaration for maps. */
};

typedef struct {
  ISet map;	/* Set of chars in map. */
  Index2 succ;	/* Next element on hash-chain. */
} Map;

/* A map is always accessed thru the context pointer. */
#define MAPS(i)		A_ACCESS(contextP->maps, Map, i)

static VOID
initMaps()
{
  Index i;
  contextP->mapsHashTab= MALLOC_VEC(MAPS_HASH_SIZE, Index2);
  INIT_AREA(contextP->maps, Map, 4, UINT2_MAX);
  for (i= 0; i < MAPS_HASH_SIZE; i++) contextP->mapsHashTab[i]= NIL;
  VOID_RET();
}

static VOID
freeMaps()
{
  delArea(&contextP->maps);
  FREE(contextP->mapsHashTab); 
}

/* Return index of map in MAPS[]. If it is found, delete ISet map.  Otherwise
 * create a new entry and insert map into it.
 */
static Index
getMap(map)
  ISet map;
{
  CONST Index h= hashSumISet(map) % MAPS_HASH_SIZE;
  Index2 *CONST mapsHashTab= contextP->mapsHashTab;
  Index i= mapsHashTab[h];
  while (i != NIL) {
    if (compareISet(MAPS(i).map, map) == 0) {
      delISet(map); return i;
    }
    i= MAPS(i).succ;
  }
  i= A_NEXT(contextP->maps);
  MAPS(i).map= map; 
  MAPS(i).succ= mapsHashTab[h]; mapsHashTab[h]= i;
  return i;
}



/*		DETERMINE KNOWN/UNKNOWN NON-ACCEPTING STATES		*/	

typedef struct Accept {
  Index2 state;		/* Last accepting state. */
  Count2 dist;		/* Distance to accepting state. */
} Accept;

static VOID
initAccepts()
{
  contextP->accepts= MALLOC_VEC(dfaParams->nSigStates, Accept);
  VOID_RET();  
}

static VOID
freeAccepts()
{
  FREE(contextP->accepts);
}

/* Compute accepts[] array giving Accept entry for each state.  On
 * exit for non-accepting states, the dist entry is NIL for
 * action-unknown states.  For accepting states, the dist entry is
 * always 0 on exit.
 */
static VOID
computeAccepts()
{
  CONST Count nInitialStates= dfaParams->nInitialStates;
  CONST Count nNonAcceptStates= dfaParams->nNonAcceptStates;
  CONST Count nSigStates= dfaParams->nSigStates;
  Accept *CONST accepts= contextP->accepts;
  Boolean changed;
  Index s;
  for (s= 0; s < nInitialStates; s++) { 
    accepts[s].state= NIL; accepts[s].dist= NIL;
  }
  for (s= nInitialStates; s < nNonAcceptStates; s++) {
    accepts[s].state= NIL; accepts[s].dist= 0;
  }
  for (s= nNonAcceptStates; s < nSigStates; s++) {
    assert(DFA_STATES[s].actions != BAD_ISET);
    accepts[s].state= s;
    accepts[s].dist= 0;
  }
  do { /* while changed */
    changed= FALSE;
    for (s= nInitialStates; s < nSigStates; s++) {
      if (s >= nNonAcceptStates || accepts[s].dist != 0) {
	/* s is a accepting state or a already processed non-accepting state. */
	CONST Index transLo= DFA_STATES[s].transLo;
	CONST Index transHi= DFA_STATES[s].transHi;
	Index i;
	assert(DFA_STATES[s].actions != BAD_ISET ? accepts[s].dist == 0 : 1);
	if (accepts[s].dist == NIL) { /* Action-unknown non-accepting state. */
	  for (i= transLo; i <= transHi; i++) { /* Use transHi for sentinel. */
	    CONST Index succ= (i == transHi)
	                      ? DFA_STATES[s].sentinelSucc
	                      : DFA_TRANS[i].nextState;
	    if (succ != NIL && succ < nNonAcceptStates 
		&& accepts[succ].dist != NIL) {
	      accepts[succ].state= accepts[succ].dist= NIL; changed= TRUE;
	    }	
	  } /* for (i= transLo; i <= transHi; i++) */
	} /* if (accepts[s].dist == NIL) */
	else { /* else (accepts[s].dist != NIL) */
	  CONST Size sDist= accepts[s].dist;
	  CONST Index sAccept= accepts[s].state;
	  for (i= transLo; i <= transHi; i++) { /* Use transHi for sentinel. */
	    CONST Index succ= (i == transHi)
	                      ? DFA_STATES[s].sentinelSucc
	                      : DFA_TRANS[i].nextState;
	    if (succ != NIL && DFA_STATES[succ].actions == BAD_ISET) { 
	      /* Non-accepting state. */
	      if (accepts[succ].dist == 0) { /* Just initialized. */
		accepts[succ].dist= sDist + 1;	accepts[succ].state= sAccept;
		changed= TRUE;
	      }
	      else if (accepts[succ].dist != NIL) { /* Previously seen. */
		if (accepts[succ].dist != sDist + 1 
		    || accepts[succ].state != sAccept) {
		  accepts[succ].state= accepts[succ].dist= NIL; changed= TRUE;
		}
	      } /* else if (accepts[succ].dist != NIL) */	
	    } /* if (succ != NIL && DFA_STATES[succ].actions == NIL) */
	  } /* for (i= transLo; i <= transHi; i++) */
	} /* else (accepts[s].dist != NIL) */
      } /* if (s >= nNonAcceptStates || accepts[s].dist != 0) */
    } /* for (s= 0; s < nSigStates; s++) */
  } while (changed);
  VOID_RET();
}


/*			DETERMINE RECORDING STATES			*/

/* Return TRUE iff there is a path from state which does not pass thru
 * a accepting state to some action-unknown non-accepting state.
 */
static Boolean 
isRecording(state)
  Index state;
{
  CONST Count nNonAcceptStates= dfaParams->nNonAcceptStates;
  CONST Index transLo= DFA_STATES[state].transLo;
  CONST Index transHi= DFA_STATES[state].transHi;
  CONST Accept *CONST accepts= contextP->accepts;
  Boolean *CONST visited= contextP->visited;
  Index i;
  assert(dfaParams->nInitialStates <= state && state < dfaParams->nSigStates);
  for (i= transLo; i <= transHi; i++) {	/* Use i == transHi for sentinel. */
    CONST Index succ= (i == transHi)
                      ? DFA_STATES[state].sentinelSucc
	              : DFA_TRANS[i].nextState;
    if (succ != NIL && succ < nNonAcceptStates && !visited[succ]) {
      assert(DFA_STATES[succ].actions == BAD_ISET);
      if (accepts[succ].dist == NIL) return TRUE;
      visited[succ]= TRUE;
      if (isRecording(succ)) return TRUE; 
    }
  }
  return FALSE;
}


/* For each accepting state s, set the act in accepts[s] to NIL if
 * there is a path from s to some action-unknown state which does not pass
 * thru an accepting state.  After this routine has been called the
 * accepts[] arrays should be as follows:
 * Initial States:			state == NIL; dist == NIL.
 * Action-known non-accepting states: 	state != NIL; dist != NIL; dist != 0.
 * Action-unknown non-accepting states: state == NIL; dist == NIL.
 * Non-recording accepting states:	state != NIL; dist == 0.
 * Recording accepting states:		state == NIL; dist == 0.
 */
static VOID
findRecordingStates()
{
  CONST Count nInitialStates= dfaParams->nInitialStates;
  CONST Count nNonAcceptStates= dfaParams->nNonAcceptStates;
  CONST Count nSigStates= dfaParams->nSigStates;
  Boolean *visited= contextP->visited;
  Accept *CONST accepts= contextP->accepts;
  Index s;
  for (s= nNonAcceptStates; s < nSigStates; s++) {
    Index s1;
    assert(accepts[s].state != NIL && accepts[s].dist == 0);
    if (isRecording(s)) {
      accepts[s].state= NIL;
    }
    for (s1= nInitialStates; s1 < nNonAcceptStates; s1++) {
      /* Never use entries in range [0...nInitialStates). */
      visited[s1]= FALSE;
    }
  }
  VOID_RET();
}
 
#define IS_ACTION_KNOWN_STATE(s)	(contextP->accepts[s].state != NIL)
#define IS_RECORDING_STATE(s)		\
  (dfaParams->nNonAcceptStates <= (s) && (s) < dfaParams->nSigStates \
   && contextP->accepts[s].state == NIL)


/*			    LINEAR TESTS				*/

typedef struct LinTest {
  union {
    UChar chars[2];	/* Chars for AND or OR; chars[0] for SINGLE. */
    Index2 mapIndex;	/* Index of map for MAP_TEST when in LIN_TESTS[]. */
    ISet mapSet;	/* Set used for MAP_TEST when in succInputs[]. */
  } u;
  Index2 succ;		/* Successor state. */
  UChar testType;	/* TestType */
} LinTest;

typedef enum { 	/* Types of tests we can have for linear tests. */
  NO_TEST,	/* No test is necessary for last one in test chain. */
  MAP_TEST, 	/* if (map[yyChar]) ... */
  AND_TEST, 	/* if (chars[0] <= yyChar && chars[1] <= hi) ... */
  OR_TEST, 	/* if (chars[0] == yyChar || chars[1] == yyChar) ... */
  SINGLE_TEST   /* if (chars[0] == yyChar) ... */
} TestType;


static VOID
initLinTests()
{
  INIT_AREA(contextP->linTests, LinTest, 6, UINT2_MAX);
  contextP->stateTests= MALLOC_VEC(dfaParams->nSigStates, Index2);
  VOID_RET();
}

static VOID
freeLinTests()
{
  delArea(&contextP->linTests);
  FREE(contextP->stateTests);
  VOID_RET();
}

/* Add input to the LinTest pointed to by linTestP.  Change testType
 * of linTestP suitably.  We assume that on entry input is not
 * included in *linTestP.
 */
static VOID
addLinTestInput(linTestP, input)
  LinTest *linTestP;
  Index input;		/* Input to be added to linTestP. */
{
  switch (linTestP->testType) {
    case MAP_TEST:
      addISet(linTestP->u.mapSet, input);
      break;
    case OR_TEST: {
      ISet s= unaryISet(input);
      addISet(s, linTestP->u.chars[0]); addISet(s, linTestP->u.chars[1]);
      linTestP->u.mapSet= s;
      linTestP->testType= MAP_TEST;
      break;
    }
    case AND_TEST: 
      if (input == linTestP->u.chars[1] + 1) {
	linTestP->u.chars[1]= input;
      }
      else {
	ISet s= newISet();
	rangeISet(s, linTestP->u.chars[0], linTestP->u.chars[1]);
	addISet(s, input);
	linTestP->u.mapSet= s;
	linTestP->testType= MAP_TEST;
      }
      break;
    case SINGLE_TEST:
      linTestP->u.chars[1]= input;
      linTestP->testType= 
	(input == linTestP->u.chars[0] + 1) ? AND_TEST : OR_TEST;
      break;
    default:
      INTERNAL_ERROR();
  } /* switch (linTestP->testType) */
  VOID_RET();
}

#define LIN_TESTS(i) A_ACCESS(contextP->linTests, LinTest, i)

/* Add the nSuccs tests in succInputs[] to LIN_TESTS[], with last one
 * having testType == NO_TEST.  Convert map-sets to map-indexes by
 * interning the map-sets using getMap().  Sorts transitions so that
 * loop transition (if any) is first, sentinel transition second-last
 * and default transition last.  Returns index of the first test added
 * to LIN_TESTS[].
 */
static Index 
addLinearTests(succInputs, nSuccs, defaultIndex, 
	       loopIndex, sentinelIndex)
  LinTest succInputs[];
  Count nSuccs;
  Index defaultIndex;	/* Default transition: will be output last. */
  Index loopIndex;	/* Self transition (if any); will be output first. */
  Index sentinelIndex;	/* Sentinel transition; will be output second-last. */
{
  Index first= A_NENTRIES(contextP->linTests);
  Index j= first + (loopIndex != NIL); /* Dest index for non-specials. */
  Index i;
  assert(defaultIndex != NIL);
  for (i= 0; i < nSuccs; i++) A_NEXT(contextP->linTests);
  for (i= 0; i < nSuccs; i++) {
    if (succInputs[i].testType == MAP_TEST && i != defaultIndex) {
      succInputs[i].u.mapIndex= getMap(succInputs[i].u.mapSet);
    }
    if (i == loopIndex) {
      LIN_TESTS(first)= succInputs[i];
    }
    else if (i == defaultIndex) {
      LIN_TESTS(first + nSuccs - 1).testType= NO_TEST;
      LIN_TESTS(first + nSuccs - 1).succ= succInputs[i].succ;
      if (succInputs[i].testType == MAP_TEST) delISet(succInputs[i].u.mapSet);
    }
    else if (i == sentinelIndex) {
      assert(succInputs[i].testType == SINGLE_TEST);
      LIN_TESTS(first + nSuccs - 2)= succInputs[i];
    }
    else {
      LIN_TESTS(j)= succInputs[i]; j++;
    }
  }
  return first;
}

/* Go thru all states and for each state s having <=
 * linCodeParamOption distinct successors construct a linear tests
 * with index given by stateTests[s].  (set stateTests[s] to NIL if
 * linear test not possible.)
 */
static VOID
buildLinearTests()
{
  CONST Index sentinelIn= dfaParams->sentinelIn;
  CONST Index sentinelState= dfaParams->sentinelState;
  CONST Count nSigStates= dfaParams->nSigStates;
  CONST Count nIn= dfaParams->nIn;
  Index2 *stateTests= contextP->stateTests;
  CONST Index linThreshold= optionsP->linCodeParam;
  LinTest *CONST succInputs= MALLOC_VEC(linThreshold + 1, LinTest);
  Count *CONST succCounts= MALLOC_VEC(linThreshold + 1, Count);
  
  Index s;
  for (s= 0; s < nSigStates; s++) {
    CONST Index transLo= DFA_STATES[s].transLo;
    CONST Index transHi= DFA_STATES[s].transHi;
    Index t= transLo;	/* Index within DFA_TRANS[]. */
    Count nSuccs= 0;	/* # of distinct successors seen so far for s. */
    Index loopIndex= NIL;	/* Index in succInputs[] of looping succ. */
    Index sentinelIndex= NIL; 	/* Index in succInputs[] of sentinel trans. */
    Index i;	
    
    for (i= 0; i < nIn; i++) {
      CONST Index succ= (i == sentinelIn)
	                ? sentinelState
	                : (t < transHi && i == DFA_TRANS[t].input)
	                ? DFA_TRANS[t++].nextState
                        : JAM_STATE;
      Index j;
      for (j= 0; j < nSuccs; j++) {
	if (succInputs[j].succ == succ) break;
      }
      if (j < nSuccs) { /* Found previous transition to succ. */
	addLinTestInput(&succInputs[j], i);
	succCounts[j]++;
      }
      else if (nSuccs <= linThreshold) { /* Create a new successor. */
	CONST Index newIndex= nSuccs++; /* Where new entry will be inserted. */
	succInputs[newIndex].testType= SINGLE_TEST;
	succCounts[newIndex]= 1;
	succInputs[newIndex].succ= succ;
	succInputs[newIndex].u.chars[0]= i;
	if (succ == s) loopIndex= newIndex;
	else if (i == sentinelIn) sentinelIndex= newIndex;
      }
      else { /* Too many successors: will not do a linear search. */
	for (j= 0; j < nSuccs; j++) {
	  if (succInputs[j].testType == MAP_TEST) {
	    delISet(succInputs[j].u.mapSet);
	  }
	}
	stateTests[s]= NIL;
	break;		/* Get out of loop with i < nIn. */
      }
    } /* for (i= 0; i < nIn; i++) */
    if (i == nIn) { /* We have a linear search. */
      Index j;
      Count max= 0;
      Index defaultIndex;	/* Index to which max. # of transitions. */
      for (j= 0; j < nSuccs; j++) { /* Find max of succCounts. */
	if (succCounts[j] > max && j != loopIndex) { 
	  max= succCounts[j]; defaultIndex= j; 
	}
      }
      stateTests[s]= addLinearTests(succInputs, nSuccs, 
				    defaultIndex, loopIndex, sentinelIndex);
    }
  } /* for (s= 0; s < nSigStates; s++) */
  FREE(succInputs); FREE((VOIDP)succCounts);
  VOID_RET();
}

/*			    	RANGES					*/

typedef struct Range {
  Index2 lo;	/* Inclusive lower bound for range. */
  Index2 hi;	/* Exclusive upper bound for range. */
  Index2 succ;	/* Successor state: 
		 * if >jamState, then succ - jamState is action #. 
		 */
} Range;

#define RANGES(i)	A_ACCESS(contextP->ranges, Range, i)

static VOID
initRanges()
{
  INIT_AREA(contextP->ranges, Range, 5, UINT2_MAX);
}

static VOID
freeRanges()
{
  delArea(&contextP->ranges);
}

/* Build ranges for state s in contextP->ranges[*loP, *hiP). */
static VOID
buildRanges(s, loP, hiP)
  Index s;			/* State for which ranges should be built. */
  Index *loP;			/* Inclusive lower index in contextP->ranges. */
  Index *hiP;			/* Exclusive upper index in contextP->ranges. */
{
  CONST Index sentinelIn= dfaParams->sentinelIn;
  CONST Index sentinelState= dfaParams->sentinelState;
  CONST Index transLo= DFA_STATES[s].transLo;
  CONST Index transHi= DFA_STATES[s].transHi;
  CONST Count nIn= dfaParams->nIn;
  Index lastSucc= NIL;
  Index r= NIL;		/* Index of currently active range. */
  Index t= transLo;
  Index i;
  *loP= A_NENTRIES(contextP->ranges);
  for (i= 0; i < nIn; i++) {
    CONST Index succ= (i == sentinelIn)
                      ? sentinelState
                      : (t < transHi && i == DFA_TRANS[t].input) 
                      ? DFA_TRANS[t++].nextState 
	              : JAM_STATE;
    if (succ != lastSucc) {
      if (r != NIL) RANGES(r).hi= i;
      r= A_NEXT(contextP->ranges);
      RANGES(r).lo= i; RANGES(r).succ= succ;
      lastSucc= succ;
    }
  }
  RANGES(r).hi= nIn;
  *hiP= r + 1;
  VOID_RET();
}


/*			      OUTPUT PARAMETERS.			*/	


/* Control over formatting of output code. */
enum { START_TAB= 8, TAB_INC= 2, MAX_TAB= 60 };

#define TAB(f, n)	fprintf(f, "%*s", n, "")


/*			    OUTPUT ROUTINES.				*/

Count
outMaps(outFile)
  FILE *outFile;
{
  enum { N_PER_LINE= 20 }; 	/* Number of entries per line. */
  Count nLines= 0;
  CONST Count nMaps= A_NENTRIES(contextP->maps);
  CONST Count nIn= dfaParams->nIn;
  Index i;
  for (i= 0; i < nMaps; i++) {
    ISet map= MAPS(i).map;
    Index j;
    fprintf(outFile, "static YYUChar %s%d[]= {\n  ", MAP_PREFIX, i); nLines++;
    for (j= 0; j < nIn; j++) {
      fprintf(outFile, "%d", inISet(j, map));
      if (j == nIn - 1) { /* Last entry.  Terminate. */
	fputs("\n};\n", outFile); nLines+= 2;
      }
      else { /* Not last entry. */
	fputs(", ", outFile);
	if ((j + 1) % N_PER_LINE == 0) {
	  fputs("\n  ", outFile); nLines++;
	}
      } /* else not last entry. */
    } /* for (j= 0; j < nIn; j++) */
  } /* for (i= 0; i < nMaps; i++) */
  return nLines;
}


static Count
outTransition(outFile, state, succ, isStart, tab, jamDoneP)
  FILE *outFile;
  Index state;		/* Originating state of transition. Used if !isStart. */
  Index succ;		/* Successor state of transition. */
  Boolean isStart;	/* True if initial start-state transition. */
  Index tab;		/* Current tab position. */
  Boolean *jamDoneP;	/* True if transition to jam-state processed. */
{
  Count nLines= 0;
  assert(succ >= dfaParams->nInitialStates);
  if (succ == dfaParams->sentinelState) {
    assert(!isStart);
    fprintf(outFile, "{ %s= %d; goto %s; }\n", 
	    STATE_VAR, state, DO_SENTINEL1_LAB); 
    nLines++;
  }
  else if (succ < dfaParams->nStates) { /* compress transition. */
    if (!optionsP->transitionCompress || contextP->visited[succ]) {
      fprintf(outFile, "goto %s%d;\n", STATE_LABEL_PREFIX, succ); nLines++;
    }
    else {
      fputs("\n", outFile); nLines++;      
      nLines+= outState(outFile, succ, tab);
      TAB(outFile, tab);
      if (isStart) {
	fputs("/* Back in starting state. */\n", outFile);
      }
      else {
	fprintf(outFile, "/* Back in state %d. */\n", state); 
      }
      nLines++;
    }
  }
  else { /* jam transition. */
    assert(!isStart);
    assert(succ == JAM_STATE);
    if (*jamDoneP) { /* Jam transition processed previously. */
      fprintf(outFile, "goto %s%d;\n", JAM_LABEL_PREFIX, state); nLines++;
    }
    else { /* First time jam transition seen. */
      Index tab1= tab + TAB_INC;
      *jamDoneP= TRUE;
      fputs("{\n", outFile); nLines++;
      TAB(outFile, tab); 
      fprintf(outFile, "%s%d:\n", JAM_LABEL_PREFIX, state); nLines++;
      TAB(outFile, tab1);
      if (state >= dfaParams->nNonAcceptStates) { /* Accepting state. */
#if 0
	if (optionsP->doReject) {
	  fprintf(outFile, "%s= %d; ", STATE_VAR, state);
	}
#endif
	fprintf(outFile, "%s--; %s= %d; goto %s;", CP_VAR, STATE_VAR, 
		state, TERMINATE_TEXT_LAB); 
      }
      else if (IS_ACTION_KNOWN_STATE(state)) { 
	CONST Accept *CONST p= &contextP->accepts[state];
	fprintf(outFile, "%s-= %d; %s= %d; goto %s;", CP_VAR, p->dist + 1,
		STATE_VAR, p->state, TERMINATE_TEXT_LAB); 
      }
      else { /* Action unknown non-accepting state. */
	fprintf(outFile, "goto %s;", GET_RECORDED_LAB);
      }
      fputs("\n", outFile); nLines++; 
      TAB(outFile, tab); fputs("}\n", outFile); nLines++;
    } /* else first time jam transition seen. */
  } /* else jam transition. */
  return nLines;
}

static ConstString
testCharString(c, str)
  Index c;
  Char *str;
{
  if (optionsP->equivClasses) {
    sprintf(str, "%d", c);
  }
  else if (isprint(c) && c != '\'' && c != '\\') {
    sprintf(str, "'%c'", c);
  }
  else {
    ConstString escChars= "\b\f\n\r\t\v\\\'";
    ConstString escCodes= "bfnrtv\\\'";
    ConstString p= strchr(escChars, c);
    if (p && p < escChars + strlen(escChars)) { /* 2nd test for c == '\0'. */
      sprintf(str, "'\\%c'", escCodes[p - escChars]);
    }
    else {
      sprintf(str, "'\\%03o'", c);
    }
  }
  return str;
}

static Count
outLinState(outFile, s, tab, jamDoneP)
  FILE *outFile;
  Index s;
  Index tab;
  Boolean *jamDoneP;
{
  Count nLines= 0;
  char str[8];
  Index i;
  assert(contextP->stateTests[s] != NIL);
  for (i= contextP->stateTests[s]; LIN_TESTS(i).testType != NO_TEST; i++) {
    TAB(outFile, tab);
    switch (LIN_TESTS(i).testType) {
      case SINGLE_TEST:
	fprintf(outFile, "if (%s == %s) ", CHAR_VAR, 
		testCharString(LIN_TESTS(i).u.chars[0], str));
	break;
      case AND_TEST: /* 2 fprintf() statements cause str is used by both. */
	fprintf(outFile, "if (%s <= %s && ", 
		testCharString(LIN_TESTS(i).u.chars[0], str), CHAR_VAR); 
	fprintf(outFile, "%s <= %s) ", 
		CHAR_VAR, testCharString(LIN_TESTS(i).u.chars[1], str));
 	break;
      case OR_TEST: /* 2 fprintf() statements cause str is used by both. */
	fprintf(outFile, "if (%s == %s || ", 
		CHAR_VAR, testCharString(LIN_TESTS(i).u.chars[0], str)); 
	fprintf(outFile, "%s == %s) ", 
		CHAR_VAR, testCharString(LIN_TESTS(i).u.chars[1], str));
	break;
      case MAP_TEST:
	fprintf(outFile, "if (%s%d[%s]) ", MAP_PREFIX, 
		LIN_TESTS(i).u.mapIndex, CHAR_VAR);
	break;
    }
    nLines+= outTransition(outFile, s, LIN_TESTS(i).succ, 
			   FALSE, tab, jamDoneP);
  }
  TAB(outFile, tab);
  nLines+= outTransition(outFile, s, LIN_TESTS(i).succ, 
			 FALSE, tab, jamDoneP);
  return nLines;
}

/* Output if-tree for sub-ranges in [rangeLo..rangeHi).  Handle situation
 * where a 1-element sub-range separates two subranges with identical
 * successors.
 */
static Count 
outBinState(outFile, s, rangeLo, rangeHi, isStart, tab, jamDoneP)
  FILE *outFile;
  Index s;		/* Source state. */
  Index rangeLo;	/* Start index in ranges. */
  Index rangeHi;	/* End index in ranges (exclusive). */
  Boolean isStart;	/* True if initial start-state transition. */
  Count tab;
  Boolean *jamDoneP;
{
  ConstString var= isStart ? STATE_VAR : CHAR_VAR;
  Count nLines= 0;
  Char str[8];
  assert(rangeHi > rangeLo);
  assert(!isStart);
  switch (rangeHi - rangeLo) {
    case 1:
      TAB(outFile, tab); 
      nLines+= outTransition(outFile, s, RANGES(rangeLo).succ, 
			     isStart, tab, jamDoneP);
      break;
    case 2:
      TAB(outFile, tab); 
      fprintf(outFile, "if (%s < %s) ", var, 
	      testCharString(RANGES(rangeLo).hi, str));
      nLines+= outTransition(outFile, s, RANGES(rangeLo).succ, 
			     isStart, tab, jamDoneP);
      TAB(outFile, tab); 
      nLines+= outTransition(outFile, s, RANGES(rangeHi - 1).succ, 
			     isStart, tab, jamDoneP);
      break;
    case 3:
      if (RANGES(rangeLo+1).lo == RANGES(rangeLo + 1).hi - 1 &&
	  RANGES(rangeLo).succ == RANGES(rangeHi - 1).succ) { /* Optimize. */
	TAB(outFile, tab); 
	fprintf(outFile, "if (%s == %s) ", var, 
		testCharString(RANGES(rangeLo + 1).lo, str));
	nLines+= outTransition(outFile, s, RANGES(rangeLo + 1).succ, 
			       isStart, tab, jamDoneP);
	TAB(outFile, tab); 
	nLines+= outTransition(outFile, s, RANGES(rangeLo).succ, 
			       isStart, tab, jamDoneP);
      }
      else {
	TAB(outFile, tab); 
	fprintf(outFile, "if (%s < %s) ", var, 
		testCharString(RANGES(rangeLo).hi, str));
	nLines+= outTransition(outFile, s, RANGES(rangeLo).succ, 
			       isStart, tab, jamDoneP);
	TAB(outFile, tab); 
	fprintf(outFile, "if (%s < %s) ", var, 
		testCharString(RANGES(rangeLo + 1).hi, str));
	nLines+= outTransition(outFile, s, RANGES(rangeLo + 1).succ, 
			       isStart, tab, jamDoneP);
	TAB(outFile, tab); 
	nLines+= outTransition(outFile, s, RANGES(rangeHi - 1).succ,
			       isStart, tab, jamDoneP);
      }
      break;
    default: {
      Index mid= (rangeHi - 1 + rangeLo)/2;
      /* Correct to prevent split on special uni-range. */
      if (RANGES(mid).lo == RANGES(mid).hi - 1 &&
	  RANGES(mid - 1).succ == RANGES(mid + 1).succ) {
	mid++;
      }
      else if (RANGES(mid + 1).lo == RANGES(mid + 1).hi - 1 &&
	       RANGES(mid).succ == RANGES(mid + 2).succ) {
	mid--;
      }
      TAB(outFile, tab); 
      fprintf(outFile, "if (%s < %s) {\n", var, 
	      testCharString(RANGES(mid).hi, str)); 
	      nLines++;
      nLines+= outBinState(outFile, s, rangeLo, mid + 1, 
			   isStart, tab + TAB_INC, jamDoneP);
      TAB(outFile, tab); fputs("}\n", outFile); nLines++;
      TAB(outFile, tab); 
      fprintf(outFile, "else { /* (%s >= %s) */\n", var, 
	      testCharString(RANGES(mid).hi, str)); 
      nLines++;
      nLines+= outBinState(outFile, s, mid + 1, rangeHi,
			   isStart, tab + TAB_INC, jamDoneP);
      TAB(outFile, tab); 
      fprintf(outFile, "} /* else (%s >= %s) */\n", var, 
	      testCharString(RANGES(mid).hi, str)); 
      nLines++;
    } /* default */
  } /* switch (rangeHi - rangeLo) */
  return nLines;
}

static VOID
outSuccLabel(outFile, state, succ)
  FILE *outFile;
  Index state;
  Index succ;
{
  if (succ == dfaParams->sentinelState) {
    fprintf(outFile, "%s%d", SENTINEL_LABEL_PREFIX, state);
  }
  else if (succ == JAM_STATE) {
    fprintf(outFile, "%s%d", JAM_LABEL_PREFIX, state);
  }
  else { /* Transition to either non-sentinel, non-jam state. */
    fprintf(outFile, "%s%d", STATE_LABEL_PREFIX, succ);
  }
}

static Index
mostCommonSuccState(rangeLo, rangeHi)
  Index rangeLo;
  Index rangeHi;
{
  CONST Count n= dfaParams->nStates + 1;
  Count2 *succCounts= (Count2*)CALLOC(n, sizeof(Count2));
  Count mostCommonSuccCount= 0;
  Index mostCommonSucc;
  Index i;
  for (i= rangeLo; i < rangeHi; i++) {
    Range *rP= &RANGES(i);
    Index succ= rP->succ;
    Count cnt= succCounts[succ]+= rP->hi - rP->lo;
    assert(succ < n);
    if (cnt > mostCommonSuccCount) {
      mostCommonSucc= succ; mostCommonSuccCount= cnt;
    }
  }
  assert(mostCommonSuccCount > 0);
  FREE(succCounts);
  return mostCommonSucc;
}

static Count
outSwitchState(outFile, s, rangeLo, rangeHi, isStart, tab)
  FILE *outFile;
  Index s;		/* Source state. */
  Index rangeLo;
  Index rangeHi;
  Boolean isStart;
  Count tab;
{
  ConstString var= isStart ? STATE_VAR : CHAR_VAR;
  CONST Index tab1= tab + TAB_INC;
  Count nLines= 0;
  Boolean hasSentinelSucc= FALSE;
  CONST Index defaultSucc= mostCommonSuccState(rangeLo, rangeHi);
  Char str[8];
  Index i;
  TAB(outFile, tab); 
  fprintf(outFile, "%s(%sS%d, %s, ", SWITCH_MACRO, 
	  LABEL_TABLE_PREFIX, s, var);
  outSuccLabel(outFile, s, defaultSucc); fprintf(outFile, ",\n");
  nLines++;
  for (i= rangeLo; i < rangeHi; i++) {
    CONST Index succ= RANGES(i).succ;
    CONST Count hi= RANGES(i).hi;
    Index j;
    for (j= RANGES(i).lo; j < hi; j++) {
      TAB(outFile, tab1);
      if (isStart) {
	fprintf(outFile, "%s(%d, ", 
		succ == defaultSucc ? DEFAULT_CASE_MACRO : CASE_MACRO, 
		j);
      }
      else {
	fprintf(outFile, "%s(%s, ", 
		succ == defaultSucc ? DEFAULT_CASE_MACRO : CASE_MACRO, 
		testCharString(j, str));
      }
      outSuccLabel(outFile, s, succ);
      fputs(")\n", outFile); nLines++;
    }
    if (succ == dfaParams->sentinelState) hasSentinelSucc= TRUE;
  } /* for (i= 0; i < nRanges; i++) */
  TAB(outFile, tab); fprintf(outFile, ") /* %s */\n", SWITCH_MACRO); nLines++;
  if (hasSentinelSucc) {
    Boolean jamDone= FALSE;
    assert(!isStart);
    TAB(outFile, tab); fprintf(outFile, "%s%d: ", SENTINEL_LABEL_PREFIX, s);
    nLines+= outTransition(outFile, s, dfaParams->sentinelState, 
			   isStart, tab, &jamDone);
  }
  return nLines;
}

static Count
outState(outFile, state, tab)
  FILE *outFile;
  Index state;
  Index tab;
{
  Boolean *CONST visited= contextP->visited;
  CONST Index tab1= tab + TAB_INC;
  Count nLines= 0;
  Boolean jamDone= FALSE;

  assert(state < dfaParams->nStates && !visited[state]);
  visited[state]= TRUE;

  TAB(outFile, tab);
  fprintf(outFile, "%s%d: {\n", STATE_LABEL_PREFIX, state); nLines++;
  if (state < dfaParams->nSigStates) {
    if (IS_RECORDING_STATE(state)) {
      TAB(outFile, tab1);
      fprintf(outFile, "%s= %s; %s= %d;\n", LAST_ACCEPT_CP_VAR, CP_VAR,
	      LAST_ACCEPT_STATE_VAR, state);
      nLines++;
    }
    TAB(outFile, tab1); 
    fprintf(outFile, "%s= %s(*%s++); %s(%s, %s);\n", 
	    CHAR_VAR, CHAR_TO_UNSIGNED_MACRO, CP_VAR, EQ_CLASS, CHAR_VAR, 
	    CHAR_VAR); 
    nLines++;
  }
  if (state >= dfaParams->nSigStates) { /* accepting non-sig state. */
    fputs("\n", outFile); nLines++;      
    TAB(outFile, tab);
#if 0
    if (optionsP->doReject) {
      fprintf(outFile, "%s= %d; ", STATE_VAR, state);
    }
#endif
    fprintf(outFile, "%s= %d; goto %s;\n", STATE_VAR, state, 
	    TERMINATE_TEXT_LAB);
    nLines++;
  }
  else if (contextP->stateTests[state] != NIL) { /* Linear tests. */
    nLines+= outLinState(outFile, state, tab1, &jamDone);
  }
  else {
    CONST Index binThreshold= optionsP->binCodeParam;
    Index lo, hi;
    buildRanges(state, &lo, &hi);
    if (hi - lo <= binThreshold) {
      nLines+= outBinState(outFile, state, lo, hi, 
			   FALSE, tab1, &jamDone);
    }
    else {
      nLines+= outSwitchState(outFile, state, lo, hi, FALSE, tab1);
    }
    A_CUT(contextP->ranges, lo);
  }
  if (!jamDone && 
      dfaParams->nInitialStates <= state && state < dfaParams->nSigStates) {
    TAB(outFile, tab1);
    nLines+= outTransition(outFile, state, JAM_STATE, FALSE, 
			   tab1, &jamDone);
  }
  TAB(outFile, tab); 
  fprintf(outFile, "} /* End state %d. */\n", state); nLines++;
  return nLines;
}


static Count
outStateSwitch(outFile, tab)
  FILE *outFile;
  Index tab;
{
  CONST Index lo= A_NENTRIES(contextP->ranges);
  CONST Count nStates= dfaParams->nStates;
  Count nLines= 0;
  Index hi;
  Index i;
  TAB(outFile, tab); fprintf(outFile, "%s:\n", STATE_SWITCH_LAB); nLines++;
  for (i= 0; i < nStates - 1; i++) { /* Omit sentinel and jam states. */
    Index j= A_NEXT(contextP->ranges);
    RANGES(j).lo= i; RANGES(j).hi= i + 1; 
    RANGES(j).succ= i;
  }
  hi= A_NENTRIES(contextP->ranges);
  nLines+= outSwitchState(outFile, 0, lo, hi, TRUE, tab);
  A_CUT(contextP->ranges, lo);
  contextP->doneStateSwitch= TRUE;
  return nLines;
}

static Count 
outStartCode(outFile, tab)
  FILE *outFile;
  Index tab;
{
  CONST Count nInitialStates= dfaParams->nInitialStates;
  CONST Index tab1= tab + TAB_INC;
  CONST Index linThreshold= optionsP->linCodeParam;
  Count nLines= 0;
  assert(nInitialStates > 1);
  TAB(outFile, tab1);
  fprintf(outFile, "%s= %s;\n", STATE_VAR, START_STATE); nLines++;
  if (nInitialStates == 1) {
    fprintf(outFile, "goto %s0;\n", STATE_LABEL_PREFIX); nLines++;
  }
  else if (nInitialStates < linThreshold || nInitialStates <= 2) { 
    /* Linear start transfer. */
    Index i;
    for (i= 0; i < dfaParams->nInitialStates; i++) {
      TAB(outFile, tab); 
      if (i < nInitialStates - 1) {
	fprintf(outFile, "if (%s == %d) ", STATE_VAR, i);
      }
      fprintf(outFile, "goto %s%d;\n", STATE_LABEL_PREFIX, i); 
      nLines++;
    }
  }
  else { /* Not a linear transfer. */
    nLines+= outStateSwitch(outFile, tab);
  } /* else not a linear transfer. */
  return nLines;
}

static Count
outJamSwitch(outFile, tab)
  FILE *outFile;
  Index tab;
{
  CONST Count nInitialStates= dfaParams->nInitialStates;
  CONST Count nSigStates= dfaParams->nSigStates;
  CONST Count nSigStates1= nSigStates - 1;
  CONST Index tab1= tab + TAB_INC;
  Count nLines= 0;
  Index s;
  TAB(outFile, tab); fprintf(outFile, "%s:\n", JAM_SWITCH_LAB); nLines++;
  if (nSigStates <= nInitialStates) {
    TAB(outFile, tab); fputs("assert(0);\n", outFile); nLines++;
  }
  else { /* need a switch. */
    TAB(outFile, tab); 
    fprintf(outFile, "assert(%d <= %s && %s < %d);\n", nInitialStates,
	    STATE_VAR, STATE_VAR, nSigStates);
    nLines++;
    TAB(outFile, tab);
    fprintf(outFile, "%s(%sJam, %s-%d, %s%d,\n", 
	    SWITCH_MACRO, LABEL_TABLE_PREFIX,
	    STATE_VAR, nInitialStates,
	    JAM_LABEL_PREFIX, nSigStates1);
    nLines++;
    for (s= nInitialStates; s < nSigStates; s++) {
      TAB(outFile, tab1);
      fprintf(outFile, "%s(%d, %s%d)\n", 
	      s == nSigStates1 ? DEFAULT_CASE_MACRO : CASE_MACRO, 
	      s - nInitialStates, JAM_LABEL_PREFIX, s);
      nLines++;
    }
    TAB(outFile, tab);
    fprintf(outFile, ") /* %s */\n", SWITCH_MACRO); nLines++;
  } /* else need a switch. */
  return nLines;
}

VOIDP
initCode()
{
  CONST Count nStates= dfaParams->nStates;
  CONST Count nActs= nSrcActs();
  Boolean *CONST visited= CALLOC_VEC(nStates, Boolean);

  contextP= MALLOC_VEC(1, Context);
  contextP->nActs= nActs; contextP->nPats= nUserPats();
  contextP->visited= visited;
  contextP->doneStateSwitch= FALSE;

  initMaps();
  initAccepts(); computeAccepts();
  findRecordingStates();
  initLinTests(); buildLinearTests();
  initRanges();
  return (VOIDP)contextP;
}

static Count 
outCode(outFile)
  FILE *outFile;
{
  CONST Count nStates= dfaParams->nStates;
  Boolean *CONST visited= contextP->visited;
  Count nLines= 0;
  Index s;
  if (dfaParams->nInitialStates > 1) {
    nLines+= outStartCode(outFile, START_TAB);
  }
  visited[dfaParams->sentinelState]= TRUE;
  for (s= 0; s < nStates; s++) {
    if (!visited[s]) {
      nLines+= outState(outFile, s, START_TAB);
    }
  }
  if (!contextP->doneStateSwitch) {
    nLines+= outStateSwitch(outFile, START_TAB);
  }
  nLines+= outJamSwitch(outFile, START_TAB);
  return nLines;
}

static VOID
freeCode()
{
  freeRanges();
  freeLinTests();
  freeAccepts();
  freeMaps();
  FREE(contextP->visited);
  FREE(contextP); contextP= NULL;
}

static VOID 
printCodeStats(outFile)
  FILE *outFile;
{
  CONST Count nStates= dfaParams->nStates;
  CONST Count nNonAcceptStates= dfaParams->nNonAcceptStates;
  Count nRecording= 0;
  Count nActionKnown= 0;
  Index s;
  for (s= dfaParams->nInitialStates; s < nStates; s++) {
    if (s < nNonAcceptStates) {
      if (IS_ACTION_KNOWN_STATE(s)) nActionKnown++;
    }
    else {
      if (IS_RECORDING_STATE(s)) nRecording++;
    }
  }
  fprintf(outFile, 
	  "# of accepting states which need recording= %d (%.1f%%);\n", 
	  nRecording, 100.0*nRecording/(nStates - nNonAcceptStates));
  fprintf(outFile, 
	  "# of non-accepting non-initial known action states= %d (%.1f%%);\n", 
	  nActionKnown, 
	  100.0*nActionKnown/(nNonAcceptStates - dfaParams->nInitialStates));
}

Count 
outDFACode(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  if (optionsP->doCode) {
    nLines+= outCode(outFile);
    if (files.dumpFile) printCodeStats(files.dumpFile);
    if (optionsP->isVerbose) printCodeStats(stdout);
    freeCode();
  };
  return nLines;
}


