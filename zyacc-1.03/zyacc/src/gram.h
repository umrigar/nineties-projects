/*

File:	 gram.h
Purpose: Grammar interface.

Last Update Time-stamp: "97/07/18 13:32:05 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _GRAM_H
#define _GRAM_H

#include "port.h"

#include <stdio.h>

/*			ASSOCIATIVITY & PRECEDENCE. 			*/

typedef enum {
  NO_ASSOC,		/* No associativity declaration. */
  NON_ASSOC,		/* %nonassoc declaration. */
  LEFT_ASSOC,		/* %left declaration. */
  RIGHT_ASSOC		/* %right declaration. */
} AssocType;

enum {
  ASSOC_BIT= 2
};

#define ASSOC(ap)	((ap) & ((1L << ASSOC_BIT) - 1))
#define PREC(ap)	((ap) >> ASSOC_BIT)
#define MAKE_ASSOC_PREC(assoc, prec)	\
  ((assoc) | ((prec) << ASSOC_BIT))

/*			SYMBOL ENCODING.				*/

typedef enum {
  TERM_SYM= 0,		/* A terminal token. */
  NON_TERM_SYM= 1,	/* A non-terminal. */
  RULE_SYM,		/* A rule. */
  MISC_SYM		/* Kitchen sink. */
} SymType;

enum { 
  SYM_TYPE_BIT= 2	/* # of bits needed to hold SymType. */
};

#define SYM_TYPE(sym)		((sym) & ((1L << SYM_TYPE_BIT) - 1))
#define SYM_NUM(sym)		((sym) >> SYM_TYPE_BIT)
#define MAKE_SYM(type, num)	((type) | ((num) << SYM_TYPE_BIT))

typedef Index2 Sym;

typedef struct {
  Sym sym;		/* The symbol stored in this RHS position. */
  Index2 succ;		/* Chain occurrences.  Wasted for TERM_SYMs. */
} RHSSym;

/* The following are valid only after the grammar has been completed. */
extern CONST RHSSym *gramRHSSyms;

#define RHS_SYM(r)	(gramRHSSyms[r].sym)
#define RHS_SUCC(r)	(gramRHSSyms[r].succ)

#define IS_TERMINAL_ITEM(i)	(SYM_TYPE(RHS_SYM(i)) == TERM_SYM)
#define IS_NON_TERM_ITEM(i)	(SYM_TYPE(RHS_SYM(i)) == NON_TERM_SYM)
#define IS_REDUCING_ITEM(i)	(SYM_TYPE(RHS_SYM(i)) == RULE_SYM)
#define IS_CLOSURE_ITEM(i)	(i > 0 && IS_REDUCING_ITEM(i - 1))

typedef struct {	/* Information associated with a rule. */
  Index2 lhsNonTerm;	/* Non-terminal on LHS. */
  Index2 rhsIndexLo;	/* Index of 1st RHS symbol. */
  Index2 rhsIndexHi;	/* 1 beyond last RHS symbol (points to RULE_SYM). */
  Index2 assocPrec;	/* Encoded associativity & precedence of rule. */
  Index2 actN;		/* If non-NIL, then act # for end-of-rule action. */
  Index2 lineNum;	/* source line # for this rule */
  Index1 look;		/* 0 if %nolook; 1 if %look; 2 otherwise. */
} Rule;	

extern CONST Rule *gramRules;

#define RULE_NON_TERM(r)	(gramRules[r].lhsNonTerm)
#define RULE_RHS_LO(r)		(gramRules[r].rhsIndexLo)
#define RULE_RHS_HI(r)		(gramRules[r].rhsIndexHi)
#define RULE_LEN(r)							\
  (gramRules[r].rhsIndexHi - gramRules[r].rhsIndexLo)
#define RULE_ACTN(r)		(gramRules[r].actN)
#define RULE_LOOK(r)		(gramRules[r].look)

typedef struct {/* Semantic information associated with a number $-var. */	
  Int2 offset;	/* Offset in parse stack from current SP. */
  Index2 type;	/* Index of type identifier for %union. */
} NumVar;

VOID dclTyped PROTO((VOID_ARGS));
Index dclTerm PROTO((Index id, Index litID, Index litVal, Index val, 
	      Index assocPrec, Index typeID, BooleanX nowarn));
Index dclNonTerm PROTO((Index id, Index typeID));
VOID addStartNonTerm PROTO((Index nonTermID));
ConstString getTermString PROTO((Index term));
ConstString getNonTermString PROTO((Index nonTerm));
ConstString getTermStringWithLen PROTO((Index term, Size *len));
ConstString getNonTermStringWithLen PROTO((Index nonTerm, Size *len));

Index termRHSIndex PROTO((Index term));
Index getTermType PROTO((Index term));
Index nonTermRHSIndex PROTO((Index nonTerm));
Index nonTermRHSRule PROTO((Index nonTerm));
Index nonTermTest PROTO((Index nonTerm));
Index testRuleN PROTO((Index testNum));
Index getItemRuleN PROTO((Index item));
Index getErrorTerm PROTO((VOID_ARGS));

Index addBodySym PROTO((Index ID, Index val));
Index addBodyTest PROTO((VOID_ARGS));
Index addBodyAct PROTO((Index actN, Index lineNum));

Index ruleLHS PROTO((Index id));
Index ruleLineNum PROTO((Index ruleN));
Index beginRule PROTO((VOID_ARGS));
Index endRule PROTO((Count nRHS));
VOID setRulePrec PROTO((Index termID));
VOID setRuleLook PROTO((BooleanX isLook1));
Index internalActRule PROTO((VOID_ARGS));

Count gramNTerms PROTO((VOID_ARGS));
Count gramNNonTerms PROTO((VOID_ARGS));
Count gramNRules PROTO((VOID_ARGS));
Count gramNRHSSyms PROTO((VOID_ARGS));
Count nTests PROTO((VOID_ARGS));

CONST Index2 *computeTranslateTab PROTO((Index2 *nEntriesP));
VOID freeTranslateTab PROTO((CONST Index2 *xlatTab));

Count outGramParams PROTO((FILE *outFile));
Count outToks PROTO((FILE *outFile));

VOID translateNumVar PROTO((int varNum, Index varTypeID, 
				NumVar *numVarP));
VOID translateLHSVar PROTO((Index varTypeID, NumVar *numVarP));

int translateLocVar PROTO((int varNum));

VOID printRule PROTO((FILE *outFile, Index ruleN, Index item));

VOID doGram PROTO((VOID_ARGS));
Index assocPrecResolve PROTO((Index term, Index ruleN, 
			      Index reduceResolved, 
			      Index shiftResolved,
			      Boolean *isNonAssoc));

VOID outRefGram PROTO((FILE *f));

Sym getRHSSym PROTO((Index rhsIndex));

Index ruleNonTerm PROTO((Index ruleN));


#endif /* ifndef _GRAM_H */
