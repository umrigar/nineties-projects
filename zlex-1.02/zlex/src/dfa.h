/*

File:	 dfa.h
Purpose: Scanner automaton interface.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _DFA_H
#define _DFA_H

#include "port.h"
#include "iset.h"


/*			  DFA STATES TYPEDEFS.				*/

typedef Index2 Transition;
typedef Index2 State;

typedef struct {
  Index2 input;		/* Equiv-class or character. */
  State nextState;	/* Index in states. */
} Trans;

typedef struct {
  ISet actions;		/* if accepting then mapped action #s else BAD_ISET. */
  Transition transLo;	/* Starting index in transitions.		*/
  Transition transHi;	/* Transitions are in [transLo, transHi).	*/
  State sentinelSucc;	/* Actual successor of state on sentinel input.	*/
                        /* (NIL if no successor).			*/
  State alt;		/* Alternate state for intra-token patterns. 	*/
} XDFAState;


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
  Index2 sentinelIn;	/* Sentinel input (eq-class if any). */
  Index2 sentinelState;	/* State to perform sentinel processing. */
} DFAParams;

#define DFA_STATES	(dfaParams->states)
#define DFA_TRANS	(dfaParams->trans)	

/* Read-only access to DFA. */
extern CONST DFAParams *CONST dfaParams;

/* Build DFA based on start-state and context-state patterns.  Compress it
 * if compression has been requested, and compression is worth-while.
 */
VOID buildDFA PROTO((VOID_ARGS));

/* Delete DFA when totally done with stuff exported thru dfaParams. */
VOID delDFA PROTO((VOID_ARGS));

/* Output #defines for all DFA parameters to outFile. Return # of 
 * newlines output. 
 */
Count outDFAParams PROTO((FILE *outFile));

/* Output action cases. */
Count outActCases PROTO((FILE *outFile));

/* Output body for reject macro for action with source # srcActN to
 * outFile.  Return # of newlines output.
 */
Count outRejectMacroBody PROTO((FILE *outFile, Index srcActN));

#if DO_TRACE
VOID dumpDFAState PROTO((FILE *dumpFile, Index s));
#endif

#endif /* ifndef _DFA_H */
