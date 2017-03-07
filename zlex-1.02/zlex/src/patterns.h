/*

File:	 patterns.h
Purpose: Patterns interface.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _PATTERNS
#define _PATTERNS


#include "chrclass.h"
#include "iset.h"
#include "port.h"

typedef Index2 Pattern;
typedef Index PatternX;

/* leafClasses[i] contains the character-class which occurs at
 * "position" i (after possible '|' combination) in the user patterns.
 */
extern CONST Class *leafClasses;	

/* Get the character class corresponding to terminal i. */
#define LEAF_CLASSES(i)	leafClasses[i]

/* leafFollows[i] contains the positions which can follow position i.
 * Access only using LEAF_FOLLOWS() macro.
 */
extern CONST ISet *leafFollows;
#define LEAF_FOLLOWS(i)	leafFollows[i]

/* # of entries in leafClasses[]/leafFollows[]. */
Count nPatLeaves PROTO((VOID_ARGS));

/* Terminate the data structures accessible by the above pointers. */
VOID terminateLeaves();

/* To be called on section 2 entry. */
VOID beginSec2Patterns PROTO((VOID_ARGS));

/* To be called on section 2 exit. */
VOID endSec2Patterns PROTO((VOID_ARGS));

/* Pattern constructors. */
Pattern altPat PROTO((PatternX left, PatternX right));
Pattern catPat PROTO((PatternX left, PatternX right));
Pattern classPat PROTO((Index class));
Pattern macroPat PROTO((Index textID));
Pattern repeatPat PROTO((PatternX pat, Index lo, Index hi)); /* For *, +, ?. */

/* Return TRUE if '^' start-of-line anchor was used in some pattern. */
Boolean hasStartLinePat PROTO((VOID_ARGS));

/* Top-Level patterns. */
Index addRegularPat PROTO((BooleanX isStartLine, ISet ss, Index defLineN,
			   PatternX pat, PatternX context, Index srcActN));
Index addEOFPat PROTO((ISet ss, Index defLineN, Index srcActN));
Index addIntraTokPat PROTO((Index defLineN, PatternX pat, Index srcActN));

/* Macro definitions. */
Index defMacro PROTO((Index textID, PatternX pat, Index defLineN));

/* Start states. */
VOID defStartState PROTO((Index textID, BooleanX isExclusive, Index defLineN));
Index getStartState PROTO((Index textID));
Count nStartStates PROTO((VOID_ARGS));
ISet *getStartStateFirsts PROTO((Index ssNum));
Index getEOFAct PROTO((Index startState));

/* Variable context patterns. */
Index nextVarContext PROTO((Index cursor));
ISet *varContextFirsts PROTO((Index cursor));
Index2 *varContextStates PROTO((Index cursor));

/* Map from output action # to source action #. */
Index patActToSrcAct PROTO((Index outActN));

/* Output routines. */
Count outSS PROTO((FILE *outFile));	 /* Output start-state #defines. */
Count outPatternParams PROTO((FILE *outFile)); /* Output parameter #defines */
Count outPatAct PROTO((FILE *outFile, Index outActN));

/* Module initialization and termination. */
VOID initPatterns PROTO((VOID_ARGS));
VOID terminatePatterns PROTO((VOID_ARGS));

/* Intra-token support. */
Index unmapAct PROTO((Index mappedAct));
ISet getIntraFirsts PROTO((VOID_ARGS));
ISet getIntraPos PROTO((VOID_ARGS));
Count nIntraTokPats PROTO((VOID_ARGS));

/* Return # of top-level user patterns: output action #s for user actions 
 * are in [1, nUserPats()]. Numbers for pseudo-actions are > nUserPats().
 */
Count nUserPats PROTO((VOID_ARGS));


#endif /* ifndef _PATTERNS */
