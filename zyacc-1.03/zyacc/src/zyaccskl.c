/*

File:	 zlexskl.c
Purpose: Skeleton for zyacc parser generator.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/
#define ZYACC

/* 			PARSER PARAMETERS.			*/

/* 

Define following parameters:

YY_HAS_IN_ATTRIBS:	1 if parser uses %in attributes.
YY_BAD_TOK:		Used to indicate an invalid token.
YY_EMPTY_TOK:		Used to indicate no lookahead.
YY_ERR_TOK:		Token number associated with error token.
YY_ERR_TOK_VAL:		Token value associated with error token.
YY_HAS_MULTI_START:	1 if we have multiple %start non-terminals.
YY_N_TESTS:		# of semantic %tests used by gramar.
YY_MAX_TOK_VAL:		Maximum (user or automatically assigned) token value.
YY_MAX_RULE_LEN:	Maximum length of a rule.
YY_N_NON_TERMS:		# of non-terminal symbols (including start-symbol).
YY_N_TERMS:		# of terminal symbols.
YY_N_RHS:		# of elements on RHS of all rules.
YY_N_RULES:		# of rules (including accept & error pseudo-rule).
			  Doesn't include %test pseudo-reductions.
YY_N_TOKS:		# of tokens (active & inactive) 
                        Including YYEMPTY & YY_BAD_TOK.
YY_BAD_BASE:		If used in base array, then use default entry.
YY_N_CHECK:		# of entries in yyCheck[] vector.
YY_N_NEXT:		# of entries in yyNext[] vector.
YY_N_STATES:		# of states.
YY_TESTS_SIZE:		# of entries in yyTests[].
YY_CMD_LINE_DEBUG:	1 if debugging requested via cmd-line; 0 if not.
YY_PREFIX		Prefix for external names (default yy).
YY_IS_PURE:		1 if %pure_parser requested; 0 otherwise.
YY_HAS_LOC:		1 if @n construct used; 0 otherwise.
YY_IS_TYPED:		1 if <type> used.
YY_SRC_NAME:		Source file name.
*/

/*!*/

/* Combine YY_CMD_LINE_DEBUG and YYDEBUG into YY_ZYACC_DEBUG. */
#ifndef YY_ZYACC_DEBUG
#if YY_CMD_LINE_DEBUG == 1
#define YY_ZYACC_DEBUG 1
#endif
#endif

#ifndef YY_ZYACC_DEBUG
#if YYDEBUG == 1
#define YY_ZYACC_DEBUG 1
#endif
#endif

/*

File:	 libzyacc.h
Purpose: Interface to runtime zyacc library.

Last Update Time-stamp: "97/07/28 12:07:05 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/* This file will be included *within* the generated scanner, as well as 
 * included *by* the library source modules.
 */

#ifndef _LIBZYACC_H
#define	_LIBZYACC_H

#ifdef DEBUG_ZYACC 	/* Define to check run-time assertions. */
#undef NDEBUG
#endif

#include <assert.h>

/*			 EXTERNAL LIBRARY ROUTINES.			*/

#if __STDC__
#include <limits.h>
#include <stdlib.h>	/* for malloc() and friends. */
#define YY_VOIDP void *
#define YY_VOID void
#define YY_VOID_RET() return
#define YY_CONST const
#define YY_PROTO(p)	p
#else  /* ! __STDC__ */
#define YY_VOIDP char *
#define YY_VOID int
#define YY_VOID_RET() return 0
#define YY_CONST
#define YY_PROTO(p)	()

#if 0 
/* It is probably safest to not declare these here.  Most compilers
 * should do the right thing after producing warnings about missing
 * declarations.
 */
char *malloc(unsigned);
YY_VOID free(char *);
char *realloc(char *, unsigned);
#endif /* if 0 */

#endif /* #if __STDC__ */


typedef unsigned char	YYUChar;
typedef unsigned short	YYUShrt;
typedef unsigned int	YYUInt;
typedef unsigned long	YYULong;

/* Types used external to generated parser */
typedef YYUChar		YYBoolean;
typedef YYUInt 		YYXState;	/* a state */
typedef YYUInt 		YYXSym;		/* an encoded symbol */
typedef YYUShrt 	YYXNonTerm;	/* a nonterminal number */
typedef YYUShrt		YYXTerm;	/* a terminal number */
typedef YYUShrt		YYXRuleN;	/* a rule number */
typedef YYUInt		YYXRHS;		/* index in array of rule RHS's */
typedef YYUShrt		YYLineNum;	/* line #s in source file */

#if YY_ZYACC_DEBUG

#include <stdio.h>

/* Used for encoding terminals, non-terminals and rules into symbols 
 * Must be identical to encoding used within parser generator.
 */
typedef enum {
  YY_TOKEN_SYM= 0,		/* A token symbol. */
  YY_NON_TERM_SYM= 1,		/* A non-terminal symbol. */
  YY_RULE_SYM= 2		/* Rule symbol --- used only rule end. */
} YYSymType;

#define YY_SYM_TYPE_BIT 2	/* Min. # of bits for YYSymType. */

/* Symbol accessors and constructor */
#define YY_SYM_TYPE(sym)						\
  ((YYSymType) ((sym) & ((1L << YY_SYM_TYPE_BIT) - 1)))
#define YY_SYM_NUM(sym)		((sym) >> YY_SYM_TYPE_BIT)
#define YY_MAKE_SYM(symType, symNum)	 				\
  ((((symNum) << YY_SYM_TYPE_BIT)) | (symType))

enum {
  YY_DEBUG_IGNORE_F			/* misc flag set to ignore debugger */
};

typedef YY_VOID (*YYSemFnP)
     YY_PROTO((FILE *out, int tokNum, YY_VOIDP yylvalP));

/* Debugging data */
typedef struct {
  YY_CONST char *srcName;		/* name of source file */
  YY_CONST char **YY_CONST termNames;	/* terminal names */
  YY_CONST char **YY_CONST nonTermNames;/* nonterminal names */
  YY_CONST YYXRHS *YY_CONST rhs;	/* start of each rule in yySyms */
  YY_CONST YYXNonTerm *YY_CONST lhs;	/* LHS nonterm # for each rule */
  YY_CONST YYXSym *YY_CONST syms;	/* rule right-hand sides */
  YY_CONST YYXSym *YY_CONST access;	/* access symbols for each state */
  YYUChar *YY_CONST breaks;		/* currently set break/displ points */
  YY_CONST YYLineNum *lineNums;		/* line #s of rule actions */
  YYUInt tempBreakN;			/* number for local temp break/displ */
  FILE *in;				/* debug input stream */
  FILE *out;				/* debug output stream */
  FILE *err;				/* debug error stream */
  YY_VOIDP stkBase;			/* base of stack */
  YY_CONST YYUInt sizeStkEntry;		/* sizeof(each stack entry) */
  YYUInt maxDepth;			/* max depth of stack to be printed */
  YY_CONST YYUInt nStates;		/* # of states */
  YY_CONST YYUInt nNonTerms;		/* # of non-terminals */
  YY_CONST YYUInt nTerms;		/* # of terminals */
  YY_CONST YYUInt nRules;		/* # of rules */
  YY_CONST YYUInt nRHS;			/* # of elements in yyRHS[] array */
  YY_CONST YYXTerm errTok;		/* token # for "error" */
  YY_CONST YYXRuleN errRuleN;		/* number of special error rule */
  YY_CONST YYSemFnP semFn;		/* function to get semantics */
  YYUInt semMax;			/* max # of chars in semantics */
  YYUChar tempLocalFlags;		/* local flags for temp break/displ */
  YYUChar tempGlobalFlags;		/* global flags for temp break/displ */
  YYUChar flags;			/* global break/display + misc flags */
} YYDebugData;


YY_VOID yyDebugHandler 
YY_PROTO((YYDebugData *YY_CONST debugDataP, int actN, YYXTerm tok, 
	  int yyChar, YY_VOIDP yylvalP, 
	  YYXState gotoState, YY_VOIDP sp));

#endif /* if YY_ZYACC_DEBUG */

#endif /* ifndef _LIBZYACC_H */



/*           DEFINE TYPES WHICH DEPEND ON PARSER PARAMETERS.            */


/* Define max. value YY_CHAR_BIT if YY_CHAR_BIT undef. */
#ifndef YY_CHAR_BIT
#ifdef CHAR_BIT	
#define YY_CHAR_BIT CHAR_BIT		/* From <limits.h> */
#else
#define YY_CHAR_BIT 8		/* A "reasonable" value. */
#endif
#endif



/* Define max. value YY_UCHAR_MAX if YY_UCHAR_MAX undef. */
#ifndef YY_UCHAR_MAX
#ifdef UCHAR_MAX	
#define YY_UCHAR_MAX UCHAR_MAX		/* From <limits.h> */
#else
#define YY_UCHAR_MAX 255		/* A "reasonable" value. */
#endif
#endif



/* Define max. value YY_USHRT_MAX if YY_USHRT_MAX undef. */
#ifndef YY_USHRT_MAX
#ifdef USHRT_MAX	
#define YY_USHRT_MAX USHRT_MAX		/* From <limits.h> */
#else
#define YY_USHRT_MAX 65535		/* A "reasonable" value. */
#endif
#endif




/* typedef YYLen to represent values in 0, ..., YY_MAX_RULE_LEN. */
#if (YY_MAX_RULE_LEN <= YY_UCHAR_MAX)
typedef YYUChar YYLen;
#elif (YY_MAX_RULE_LEN <= YY_USHRT_MAX)
typedef YYUShrt YYLen;
#else
  #error Type YYLen cannot be represented.
#endif


/* typedef YYNonTerm to represent values in 0, ..., (YY_N_NON_TERMS - 1). */
#if ((YY_N_NON_TERMS - 1) <= YY_UCHAR_MAX)
typedef YYUChar YYNonTerm;
#elif ((YY_N_NON_TERMS - 1) <= YY_USHRT_MAX)
typedef YYUShrt YYNonTerm;
#else
  #error Type YYNonTerm cannot be represented.
#endif


/* typedef YYRuleN to represent values in 0, ..., (YY_N_RULES - 1). */
#if ((YY_N_RULES - 1) <= YY_UCHAR_MAX)
typedef YYUChar YYRuleN;
#elif ((YY_N_RULES - 1) <= YY_USHRT_MAX)
typedef YYUShrt YYRuleN;
#else
  #error Type YYRuleN cannot be represented.
#endif


/* typedef YYState to represent values in 0, ..., (YY_N_STATES - 1). */
#if ((YY_N_STATES - 1) <= YY_UCHAR_MAX)
typedef YYUChar YYState;
#elif ((YY_N_STATES - 1) <= YY_USHRT_MAX)
typedef YYUShrt YYState;
#else
  #error Type YYState cannot be represented.
#endif


/* typedef YYCheck to represent values in 0, ..., ((YY_N_STATES > YY_N_TERMS) ? YY_N_STATES : YY_N_TERMS). */
#if (((YY_N_STATES > YY_N_TERMS) ? YY_N_STATES : YY_N_TERMS) <= YY_UCHAR_MAX)
typedef YYUChar YYCheck;
#elif (((YY_N_STATES > YY_N_TERMS) ? YY_N_STATES : YY_N_TERMS) <= YY_USHRT_MAX)
typedef YYUShrt YYCheck;
#else
  #error Type YYCheck cannot be represented.
#endif


/* typedef YYTok to represent values in 0, ..., (YY_N_TOKS - 1). */
#if ((YY_N_TOKS - 1) <= YY_UCHAR_MAX)
typedef YYUChar YYTok;
#elif ((YY_N_TOKS - 1) <= YY_USHRT_MAX)
typedef YYUShrt YYTok;
#else
  #error Type YYTok cannot be represented.
#endif


/* typedef YYBase to represent values in 0, ..., (YY_N_NEXT - 1). */
#if ((YY_N_NEXT - 1) <= YY_UCHAR_MAX)
typedef YYUChar YYBase;
#elif ((YY_N_NEXT - 1) <= YY_USHRT_MAX)
typedef YYUShrt YYBase;
#else
  #error Type YYBase cannot be represented.
#endif


/* typedef YYTest to represent values in 0, ..., (YY_N_TESTS - 1). */
#if ((YY_N_TESTS - 1) <= YY_UCHAR_MAX)
typedef YYUChar YYTest;
#elif ((YY_N_TESTS - 1) <= YY_USHRT_MAX)
typedef YYUShrt YYTest;
#else
  #error Type YYTest cannot be represented.
#endif


/* typedef YYAct to represent values in 0, ..., (YY_N_RULES + YY_TESTS_SIZE + YY_N_STATES - 1). */
#if ((YY_N_RULES + YY_TESTS_SIZE + YY_N_STATES - 1) <= YY_UCHAR_MAX)
typedef YYUChar YYAct;
#elif ((YY_N_RULES + YY_TESTS_SIZE + YY_N_STATES - 1) <= YY_USHRT_MAX)
typedef YYUShrt YYAct;
#else
  #error Type YYAct cannot be represented.
#endif


/*                          TOKEN DEFINITIONS.                          */

/*!*/

/*	USER DEFINITIONS FROM SECTION 1 OF YACC FILE.			*/

/*!*/

/*                      SEMANTIC TYPE DEFINITIONS.	               */

/* Definitions for:
 * YYSTYPE:	%union declaration + types of all terminals.
 * YYOut:	union of all synthesized attribute classes, including YYSTYPE. 
 * YYIn:	union of all inherited attribute classes.
 */

/*!*/




/* 		NAMES FOR EXTERN PARSER OBJECTS.			*/
/* Default names provided, if macros not defined in section 1. 		*/

/*!*/


/*                           GRAMMAR TABLES.                            */

/*

#if YY_ZYACC_DEBUG				#Grammar info. for debugging.

static char *yyTermNames[YY_N_TOKS];		#Terminal names. 
static char *yyNonTermNames[YY_N_NONTERMS];	#NonTerminal names.
static YYXRHS yyRHS[YY_N_RULES];		#Start of each rule in yySyms.
static YYXNonTerm yyLHS1[YY_N_RULES];		#LHS nonterm # for each rule.
static YYXSym yySyms[YY_N_RHS];			#Rule right-hand sides.
static YYXSym yyAccess[YY_N_STATES];		#Access symbols for each state.

#endif

static YYTok yyTranslate[YY_MAX_TOK];		#Xlate ext. to internal tokens.

static YYLen yyLength[YY_N_RULES];		#Rule lengths.
static YYNonTerm yyLHS[YY_N_RULES];		#Rule LHS non-terminal numbers.

static YYBase yyActBase[YY_N_STATES];		#Base array for acts.
static YYRuleN yyActDef[YY_N_STATES];		#Default reduce rule.

static YYBase yyGotoBase[YY_N_NONTERMS];	#Base array for gotos.
static YYState yyGotoDef[YY_N_NONTERMS];	#Default state for gotos.

static YYAct yyNext[YY_N_NEXT];			#Action.
static YYCheck yyCheck[YY_N_CHECK];	 	#Action check array.

static YYTest yyTests[];
static YYAct yyTestActs[];

*/

/*!*/

/*                PUBLIC MACROS NOT REDEFINABLE BY USER.                */

#define	YYACCEPT	do { yyRetVal= 0; goto yyTerminate; } while (0)
#define YYABORT		do { yyRetVal= 1; goto yyTerminate; } while (0)
#define YYEMPTY		YY_EMPTY_TOK
#define YYERROR		goto yyError
#define YYFAIL		goto yyFail
#define yyclearin	(YY_CHAR= YYEMPTY)
#define yyerrok		(yyErrShift= 0)
#define YYRECOVERING	(YY_CHAR == YY_ERR_TOK_VAL)
#define YY_ACCEPT_RULE	0

/*                        PRIVATE TYPES & MACROS                        */

/* Resynchronize after YY_CHAR is changed. */
#define YY_CHAR_SYNC							\
  do { yyTok= YY_TRANSLATE(YY_CHAR); } while (0)

/* Pseudo-error rule used to shift to error processing. */
#ifndef YY_ERR_RULE
#define YY_ERR_RULE (YY_N_RULES - 1)
#endif

/* Min. # of tokens shifted after an error before resignalling an error. */
#ifndef YY_ERR_SHIFT
#define YY_ERR_SHIFT 3
#endif

/* Max. size of stack before overflow. */
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Initial stack size.  Also used for stack increment. */
#ifndef YYINITDEPTH
#define YYINITDEPTH 200
#endif

#if (YY_HAS_LOC)
#ifndef YYLTYPE
/* Default struct to maintain location information. */
#define YYLTYPE								\
  struct {								\
    int first_line;							\
    int last_line;							\
    int first_column;							\
    int last_column;							\
  }
#endif

typedef YYLTYPE YYLoc;

#endif /* if (YY_HAS_LOC) */

typedef struct {		/* Stack type. */
  YYXState state;		/* State stored in stack entry. */
  YYOut outVals;		/* Semantic value. */
#if (YY_HAS_IN_ATTRIBS)
  YYIn inVals;
#endif
#if (YY_HAS_LOC)
  YYLoc loc;			/* Location information. */
#endif
} YYStk;

#if YY_IS_TYPED
#define YY_NUM_VAR(offset, type)	(yySP[offset].outVals.yySType.type)
#else
#define YY_NUM_VAR(offset, type)	(yySP[offset].outVals.yySType)
#endif

#define YY_IN_VAR(offset, selector)	(yySP[offset].inVals.selector)
#define YY_TERM_VAR(offset, selector)	(yySP[offset].outVals.yySType.selector)
#define YY_NON_TERM_VAR(offset, selector) (yySP[offset].outVals.selector)
#if (YY_HAS_LOC)
#define YY_LOC_VAR(offset)		(yySP[offset].loc)
#endif

/*                      EXTERN SCANNER VARIABLES.                       */

#if (!YY_IS_PURE)
YYSTYPE YY_LVAL;		/* Semantic value of current token. */
int YY_CHAR= 0;			/* Current lookahead token (actual value). */
int YY_NERRS= 0;		/* Number of errors so far. */
#if (YY_HAS_LOC)
YYLoc yylloc;			/* Location information for current token. */
#endif
#if YY_ZYACC_DEBUG
int YY_DEBUG= 0;		/* Flag to turn debug messages on/off. 	*/
#endif
#endif

#if YY_ZYACC_DEBUG

#define YY_MAX(a, b)		(((a) > (b)) ? (a) : (b))
#define YY_N_BREAKS							\
  (YY_MAX(YY_MAX(YY_N_TERMS, YY_N_NON_TERMS), YY_N_RULES))

static YYUChar yyBreaks[YY_N_BREAKS];

#ifndef YY_SEM_FN
#define YY_SEM_FN	NULL
#endif

#ifndef YY_SEM_MAX
#define YY_SEM_MAX	10
#endif

static YYDebugData yyDebugData= {
  YY_SRC_NAME,			/* name of source file */
  yyTermNames,			/* terminal names */
  yyNonTermNames,		/* nonterminal names */
  yyRHS,			/* start of each rule in yySyms */
  yyLHS1,			/* LHS nonterm # for each rule */
  yySyms,			/* rule right-hand sides */
  yyAccess,			/* access symbols for each state */
  yyBreaks,			/* currently set break/display points */
  yyLineNums,			/* line #s of rule actions */
  0,				/* number associated with temp break */
  NULL,				/* debug input stream */
  NULL,				/* debug output stream */
  NULL,				/* debug error stream */
  NULL,				/* base of stack */
  sizeof(YYStk),		/* sizeof(each stack entry) */
  0,				/* max. depth of stack to be printed */
  YY_N_STATES,			/* # of states */
  YY_N_NON_TERMS,		/* # of non-terminals */
  YY_N_TERMS,			/* # of terminals */
  YY_N_RULES,			/* # of rules */
  YY_N_RHS,			/* # of elements in yyRHS[] array */
  YY_ERR_TOK,			/* number of 'error' token */
  YY_ERR_RULE,			/* number of special error rule */
  YY_SEM_FN,			/* function to get token semantics */
  YY_SEM_MAX,			/* max # of chars in semantics */
  0,				/* local flags for temp break/displ */
  0,				/* global flags for temp break/displ */
  0				/* global break/display + misc flags */
};
#endif 

/*                      CODE MACROS FOR YY_PARSE().                   */

/* Defines how yylex() is called. */
#ifndef YY_LEX_CALL
#if YY_IS_PURE
#if YY_HAS_LOC
#define YY_LEX_CALL()	YY_LEX(&YY_LVAL, &yylloc)
#else
#define YY_LEX_CALL()	YY_LEX(&YY_LVAL)
#endif
#else
#define YY_LEX_CALL()	YY_LEX()
#endif
#endif /* ifndef YY_LEX_CALL */

/* Translate external tokens from yylex() to internal tokens. */
#define YY_TRANSLATE(t)							\
  (((0 <= t) && (t) <= YY_MAX_TOK_VAL) ? yyTranslate[t] : YY_BAD_TOK)

/* Macros to decode acts stored in yyActNext[] & yyActDef[] tables. */
#define YY_IS_SHIFT(act)	((act) < YY_N_STATES)
#define YY_IS_REDUCE(act)	((act) >= YY_N_STATES)
#define YY_ACT_RULEN(act)	((act) - YY_N_STATES)
#define YY_ACT_STATE(act)	(act)

#define YY_GOTO_STATE(s, n)						\
  do {									\
    unsigned i= yyGotoBase[n] + (s);					\
    s= (yyCheck[i] == (s)) ? yyNext[i] : yyGotoDef[n];			\
  } while (0)

#define YY_SHIFT_LHS  yyState= (yySP++)->state

/* Stack size check & growing. */
#define YY_STK_CHK(mainBase, mainSP, size, inc)				\
  do {									\
    if ((mainSP - mainBase) >= (size))					\
      if (!(size= yyGrowStk(&mainBase, &mainSP, size + inc))) YYABORT;	\
  } while (0)

#if (YY_HAS_LOC)

/* Location info for disp from stack top. */
#define YY_STK_LOC(disp)	(yySP[disp].loc)

/* Location info for element on rule RHS with index rhsIndex (0= 1st, etc)
 * given that rule has length ruleLen.  Valid only at reduction.
 */
#define YY_RHS_LOC(rhsIndex, ruleLen)					\
  YY_STK_LOC(rhsIndex - ruleLen)

#ifndef YY_UPDATE_LOC

/* Default method of computing location information in locZ, after
 * reducing by a rule having rule length ruleLen.
 */
#define YY_UPDATE_LOC(ruleLen, locZ)					\
  do {									\
    if (ruleLen) {							\
      YY_CONST YYLoc *YY_CONST locFirstP= &YY_RHS_LOC(0, ruleLen);	\
      YY_CONST YYLoc *YY_CONST locLastP= 				\
	&YY_RHS_LOC(ruleLen - 1, ruleLen);				\
      (locZ).first_line= locFirstP->first_line;				\
      (locZ).first_column= locFirstP->first_column;			\
      (locZ).last_line= locLastP->last_line; 				\
      (locZ).last_column= locLastP->last_column;			\
    }									\
    else { /* Use location of previous element. */			\
      (locZ)= YY_STK_LOC(-1);						\
    }									\
  } while (0)

#endif /* #ifndef YY_UPDATE_LOC */
#endif /* #if YY_HAS_LOC */


/*                        STACK GROWING ROUTINE.                        */

static YYUInt yyGrowStk 
  YY_PROTO((YYStk **mainBase, YYStk **mainSP, YYUInt newSize));

static YYUInt 
yyGrowStk (mainBase, mainSP, newSize)
  YYStk **mainBase;
  YYStk **mainSP;
  YYUInt newSize;
{ 
  if (newSize > YYMAXDEPTH) {
    YY_ERROR("stack overflow"); 
    return 0;
  }
  else {
    YYUInt disp= *mainSP - *mainBase;
    if (!(*mainBase= (YYStk *)realloc(*mainBase, newSize * sizeof(YYStk)))) {
      YY_ERROR("out of memory");
      return 0;
    }
    *mainSP= *mainBase + disp;
#if YY_ZYACC_DEBUG
    yyDebugData.stkBase= (YY_VOIDP)(*mainBase);
#endif    
    return newSize;
  }
}


/*			DECLARATION FOR YY_PARSE.			*/

/* Name of formal parameter used to pass start non-terminal. */
#ifndef YY_START
#define YY_START yyStart
#endif

#ifndef YY_DECL

/* Define K&R declaration for YY_PARSE. */
#if YY_HAS_MULTI_START

#ifdef YYPARSE_PARAM
#define YY_DECL \
  int YY_PARSE(YY_START, YYPARSE_PARAM) int YY_START; YY_VOIDP YYPARSE_PARAM;
#else /* !defined YYPARSE_PARAM */
#define YY_DECL int YY_PARSE(YY_START) int YY_START;
#endif /* else !defined YYPARSE_PARAM */

#else /* !YY_HAS_MULTI_START */

#ifdef YYPARSE_PARAM
#define YY_DECL int YY_PARSE(YYPARSE_PARAM) YY_VOIDP YYPARSE_PARAM;
#else /* !defined YYPARSE_PARAM */
#define YY_DECL int YY_PARSE()
#endif /* else !defined YYPARSE_PARAM */

#endif /* else !YY_HAS_MULTI_START */

#endif /* #ifndef YY_DECL */


/*			MAIN PARSER FUNCTION.				*/

YY_DECL
{
  enum { YY_STK_INC= YYINITDEPTH };
  /* malloc() initially, as some reallocs have problems with NULL 1st arg. */
  YYStk *yyStk= (YYStk *)malloc(YY_STK_INC * sizeof(YYStk)); /* Stk base. */
  YYStk *yySP= yyStk;					/* Main stk. ptr. */
  YYUInt yyStkSize= YY_STK_INC;	/* Current size of main & location stks. */
#if YY_HAS_MULTI_START
  YYXState yyState= YY_START;	/* Current parser state. */
#else
  YYXState yyState= 0;		/* Current parser state. */
#endif
  YYTok yyTok= YYEMPTY;		/* Translated value of current lookahead. */
  int yyCharSave= 0;		/* Saved val of yyChar in error processing. */
  YYUChar yyErrShift= 0;	/* # of tokens to be shifted when error. */
  YYUChar yyRetVal;		/* Value to be returned by yyparse(). */
#if YY_IS_PURE
  YYSTYPE YY_LVAL;		/* Lexical semantic value. */
  int YY_CHAR;			/* Current lookahead token (actual value). */
  int YY_NERRS= 0;		/* Number of errors. */
#if (YY_HAS_LOC)
  YYLoc yylloc;			/* Lexical location value. */
#endif
#endif
  /* Local definitions from user. */
/*!*/
  /* Confirm initial stack allocation ok. */
  if (!yyStk) {
    YY_ERROR("Out of memory.");	/* Initial stk. allocation failed. */
  }
#if YY_ZYACC_DEBUG
  yyDebugData.stkBase= (YY_VOIDP)yyStk;
#endif  
  yySP->state= yyState; yySP++;		/* Push initial state. */

#if YY_ZYACC_DEBUG
  if ((yyDebugData.flags & (1 << YY_DEBUG_IGNORE_F)) == 0) {
    yyDebugHandler(&yyDebugData, -1, yyTok, YY_CHAR, &YY_LVAL, 
	           0, (YY_VOIDP)yySP);
  }
#endif
  while (1) {				/* Terminate only via return. */
    YYUInt yyRuleN;			/* Rule # used for reduction. */

    assert(yyState == yySP[-1].state);	/* yyState mirrors state on TOS. */

    /* yyTok & YY_CHAR must agree. */
    assert(yyTok == YYEMPTY || yyTok == YY_TRANSLATE(YY_CHAR)); 

    /* Ensure space for at least 1 more stack entry. */
    YY_STK_CHK(yyStk, yySP, yyStkSize, YY_STK_INC);
#if YY_HAS_IN_ATTRIBS
    switch (yyState) { /* Don't check for non-empty reduction. */
/*!*/	
    }	
#endif /* #if YY_HAS_IN_ATTRIBS */
    { /* Lookup action table & branch either to yyShift or yyReduce. */
      YY_CONST YYUInt base= yyActBase[yyState];
      if (base == YY_BAD_BASE) { 
	yyRuleN= yyActDef[yyState];		/* Use default entry */
	goto yyReduce;
      }
      else { /* Use lookahead to consult yyNext[] & yyCheck[]. */
	YYUInt index;
	if (yyTok == YYEMPTY) { /* No lookahead --- read input. */
	  YY_CHAR= YY_LEX_CALL(); YY_CHAR_SYNC;
	}
	index= base + yyTok;
	if (yyCheck[index] == yyTok) {
	  YY_CONST YYUInt act= yyNext[index];
	  if (YY_IS_REDUCE(act)) {
            yyRuleN= YY_ACT_RULEN(act); goto yyReduce;
	  }
	  else {
	    yyState= YY_ACT_STATE(act); goto yyShift;
	  }
	}
	else {
	  yyRuleN= yyActDef[yyState]; goto yyReduce;
	}
      }
    }

  yyShift:	/* Begin shift action. */
#if YY_ZYACC_DEBUG
    if ((yyDebugData.flags & (1 << YY_DEBUG_IGNORE_F)) == 0) {
      yyDebugHandler(&yyDebugData, yyState, yyTok, 
	             YY_CHAR, &YY_LVAL, 0, (YY_VOIDP)yySP);
    }
#endif
    /* Push state on stack. */
    yySP->state= yyState; yySP->outVals.yySType= YY_LVAL; 
#if (YY_HAS_LOC)
    yySP->loc= yylloc;
#endif
    yySP++;

    if (yyErrShift) {	/* Error processing in shift action. */
      if (yyTok == YY_ERR_TOK) { /* We successfully shifted error. */
	assert(yyErrShift == YY_ERR_SHIFT); /* No normal tokens shifted. */
	YY_CHAR= yyCharSave; YY_CHAR_SYNC; /* Restore original lookahead. */
      }
      else {			/* Successful shift of normal token. */
	yyErrShift--;		/* 1 fewer token to shift before resignal. */
	yyTok= YYEMPTY;
      }
    }
    else {
      yyTok= YYEMPTY;	/* Clear shifted token. */
    }
    continue;	/* End shift action. Continue while(1) loop.  */

  yyReduce: 	/* Begin reduce action. */
#if YY_N_TESTS > 0
    if (yyRuleN >= YY_N_RULES) { /* Test action. */
      int yyResult= 0; 	/* Or of tests tested so far. */
      YYUInt yyT;	/* Indexes yyTests[] and yyTestActs[]. */	
      if (yyTok == YYEMPTY) { /* No lookahead --- read input. */
	YY_CHAR= YY_LEX_CALL(); YY_CHAR_SYNC;
      }
      for (yyT= yyRuleN - YY_N_RULES; yyTests[yyT] > 0; yyT++) {
	switch (yyTests[yyT]) {
/*!*/	
	}
	if (yyResult) break;
      }
      { YY_CONST YYUInt act= yyTestActs[yyT];
        if (YY_IS_SHIFT(act)) {
	  yyState= YY_ACT_STATE(act); goto yyShift;
        }
        else {
	  yyRuleN= YY_ACT_RULEN(act);
	  assert(yyRuleN < YY_N_RULES);
        }
      }
    }
#endif /* if YY_N_TESTS > 0 */
    { YYUInt yyDefault= 0;	/* 1 if default action. */

#if YY_ZYACC_DEBUG
    if ((yyDebugData.flags & (1 << YY_DEBUG_IGNORE_F)) == 0) {
      if (yyRuleN == YY_ERR_RULE || yyRuleN == YY_ACCEPT_RULE) {
        yyDebugHandler(&yyDebugData, yyRuleN + YY_N_STATES, yyTok, 
		       YY_CHAR, &YY_LVAL, 0, (YY_VOIDP)yySP);
      }
      else { /* need to lookup goto state */
        int len= yyLength[yyRuleN];	/* Length of reducing rule. */
      	YYXNonTerm n= yyLHS[yyRuleN];  
        YYXState s= yySP[-len -1].state;
	YY_GOTO_STATE(s, n);
        yyDebugHandler(&yyDebugData, yyRuleN + YY_N_STATES, yyTok, 
		       YY_CHAR, &YY_LVAL, s, (YY_VOIDP)yySP);
      }
    }
#endif /* if YY_ZYACC_DEBUG */
      if (yyTok == YY_ERR_TOK) goto yyFail; /* err triggered a default redn. */
      switch (yyRuleN) {	/* Perform relevant user or default action. */
	/* Use yySP->outVals as temporary to store new semantic value. */
	case YY_ACCEPT_RULE:	/* Accepting rule. */
	  YYACCEPT; break;
	/* User actions go here. */
/*!*/
	case YY_ERR_RULE:	/* "Rule" used to break to a parse error. */
	  goto yyError;
	default:		/* Perform default action. */
	  yyDefault= 1;		/* Set flag to indicate no semantic copy. */
	  break;
      }
      {	YYLen yyLen= yyLength[yyRuleN];	/* Length of reducing rule. */
      	YYNonTerm yyN= yyLHS[yyRuleN];	/* LHS nonTerm #. */
      	YYStk *yyReduceP= yySP - yyLen;	/* Point to stk entry for rule LHS. */
#if YY_HAS_LOC
	YY_UPDATE_LOC(yyLen, yySP->loc); /* Use yySP->loc as temp. entry. */
#endif
        /* Update semantic value and location info. for LHS stk. location. */
        if (yyLen > 0) { /* Stack location for rule LHS not at yySP. */
#if YY_HAS_LOC
	  yyReduceP->loc= yySP->loc; 	/* Copy tmp. loc. entry to loc. stk. */
#endif
	  if (!yyDefault) {		/* Semantic copy required. */
	    yyReduceP->outVals= yySP->outVals;	/* Do semantic copy. */
	  }
        } /* if (yyLen > 0) */
        /* Compute new state. */
        assert(yyStk < yyReduceP);
        yyState= yyReduceP[-1].state;	/* Set yyState to uncovered state. */
        YY_GOTO_STATE(yyState, yyN);	
        yyReduceP->state= yyState;
        yySP= yyReduceP + 1;		/* Pop stack. */
      } /* YYLen yyLen= yyLeng[yyRuleN]; */
    } /* Reduce action: YYUInt yyDefault= 0; */
    continue;	/* End reduce action. Continue with while(1) loop.  */
  yyError:
    if (!yyErrShift) { YY_ERROR("parse error"); YY_NERRS++; }
  yyFail:
    if (!yyErrShift) { 		/* Must have just gotten an error. */
      yyErrShift= YY_ERR_SHIFT;	/* # of tokens to shift before next error. */
      yyCharSave= YY_CHAR;
      YY_CHAR= YY_ERR_TOK_VAL; YY_CHAR_SYNC;
    }
    else if (yyTok == YY_ERR_TOK) { /* We need to pop current state. */
      yySP--; 
      if (yySP == yyStk) YYABORT;
      yyState= yySP[-1].state; 
    }
    else if (yyErrShift == YY_ERR_SHIFT) { /* No tokens shifted after error. */
      if (YY_CHAR == 0) YYABORT;	/* Quit at EOF. */
      YY_CHAR= YY_LEX_CALL(); YY_CHAR_SYNC;
    }
    else { /* We got a second error within YY_ERR_SHIFT tokens. */
      yyErrShift= 0; goto yyFail;
    }
    continue;	/* End error processing. */
  }  /* while (1) */
yyTerminate:
  free(yyStk);
  return yyRetVal;
}

/* 			SECTION 3 CODE					*/

/*!*/
