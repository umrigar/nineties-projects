/* Automatically generated from parse.y by zyacc version 1.03
 * using the command:
 * ./zyacc -d -v -o parse.c parse.y 
 */
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

#define YY_HAS_IN_ATTRIBS 0
#define YY_BAD_TOK 40
#define YY_EMPTY_TOK 41
#define YY_ERR_TOK 0
#define YY_ERR_TOK_VAL 256
#define YY_HAS_MULTI_START 0
#define YY_N_TESTS 0
#define YY_MAX_TOK_VAL 287
#define YY_MAX_RULE_LEN 4
#define YY_N_NON_TERMS 46
#define YY_N_TERMS 42
#define YY_N_RHS 273
#define YY_N_RULES 102
#define YY_N_TOKS 42
#define YY_BAD_BASE 1
#define YY_N_CHECK 539
#define YY_N_NEXT 442
#define YY_N_STATES 149
#define YY_TESTS_SIZE 0
#define YY_CMD_LINE_DEBUG 0
#define YY_PREFIX yy
#define YY_IS_PURE 0
#define YY_HAS_LOC 0
#define YY_IS_TYPED 1
#define YY_SRC_NAME "parse.y"
#define YY_ZYACC_MAJOR_VERSION 1
#define YY_ZYACC_MINOR_VERSION 03
#line 81 "parse.c"

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

#define EXPECT_TOK 257
#define ID_TOK 258
#define ID_VAR_TOK 259
#define IN_TOK 260
#define LEFT_TOK 261
#define LEX_ERR_TOK 262
#define LHS_VAR_TOK 263
#define LIT_TOK 264
#define MARK_TOK 265
#define LOOK_TOK 266
#define NONASSOC_TOK 267
#define NOWARN_TOK 268
#define NUM_TOK 269
#define NUM_VAR_TOK 270
#define OPTION_TOK 271
#define OPTION_VAL_TOK 272
#define OUT_TOK 273
#define PREC_TOK 274
#define PURE_TOK 275
#define RIGHT_TOK 276
#define SHORT_TEXT_TOK 277
#define START_TOK 278
#define TEST_TOK 279
#define TEXT_TOK 280
#define TOKEN_TOK 281
#define TYPE_TOK 282
#define TYPE_ID_TOK 283
#define UNION_TOK 284
#define XID_TOK 285
#line 398 "parse.c"

/*	USER DEFINITIONS FROM SECTION 1 OF YACC FILE.			*/

#line 39 "parse.y"


#include "attribs.h"
#include "gram.h"
#include "lalr.h"
#include "options.h"
#include "scan.h"
#include "zparse.h"

#include "error.h"


#line 115 "parse.y"


int yyerror PROTO((char *s));

#define yylex scan

static struct {
  Index2 assocPrec;	/* The assoc/prec. of the current declaration. */
  Index2 precLevel;	/* Precedence level counter. */
  Index2 prec;		/* The precedence associated with the declaration. */
  Index2 type;		/* The type associated with current declaration. */
  Count2 decCount;	/* Count of # of declarations. */
  Index2 lineN;		/* Line # for attrib. exprs. */
  Boolean isPure;	/* TRUE if %pure parser specified. */
  Boolean nowarn;	/* TRUE if %nowarn for current token declaration. */
} globs;

#define G globs


#line 436 "parse.c"

/*                      SEMANTIC TYPE DEFINITIONS.	               */

/* Definitions for:
 * YYSTYPE:	%union declaration + types of all terminals.
 * YYOut:	union of all synthesized attribute classes, including YYSTYPE. 
 * YYIn:	union of all inherited attribute classes.
 */

typedef union {
#line 52 "parse.y"
 
  Index id;			/* Index of identifier. */
  struct {			/* Literal in single quotes. */
    Index2 id;			/* Index literal text (including quotes). */
    Index2 val;			/* The integer value of the literal. */
  } lit;		
  NumVar numVar;		/* A numeric $-variable of form $<type>NN. */
  ConstString text;		/* Pointer to saved yytext. */
  char shortText 		/* Avoid interning short strings if returned */
    [sizeof(ConstString)];	/* text fits here. */
  Int num;			/* A number. */
  Int lineN;			/* Line # in source file. */
  Index type;			/* Identifier for <type>. */
  ActSem act;
  Count count;			/* Count of number in lists. */
  Index2 assocPrec;		/* Associativity and precedence. */

} YYSTYPE;

typedef union {
  YYSTYPE yySType;
} YYOut;
#line 470 "parse.c"




/* 		NAMES FOR EXTERN PARSER OBJECTS.			*/
/* Default names provided, if macros not defined in section 1. 		*/

#ifndef YY_CHAR
#define YY_CHAR yychar
#endif

#ifndef YY_DEBUG
#define YY_DEBUG yydebug
#endif

#ifndef YY_ERROR
#define YY_ERROR yyerror
#endif

#ifndef YY_LEX
#define YY_LEX yylex
#endif

#ifndef YY_LVAL
#define YY_LVAL yylval
#endif

#ifndef YY_NERRS
#define YY_NERRS yynerrs
#endif

#ifndef YY_PARSE
#define YY_PARSE yyparse
#endif

#line 506 "parse.c"


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

#if YY_ZYACC_DEBUG

static YY_CONST char *yyTermNames[42]= {
  "error", "<EOF>", "'('", "')'", 
  "','", "':'", "';'", "'{'", 
  "'|'", "'}'", "EXPECT_TOK", "ID_TOK", 
  "ID_VAR_TOK", "IN_TOK", "LEFT_TOK", "LEX_ERR_TOK", 
  "LHS_VAR_TOK", "LIT_TOK", "MARK_TOK", "LOOK_TOK", 
  "NONASSOC_TOK", "NOWARN_TOK", "NUM_TOK", "NUM_VAR_TOK", 
  "OPTION_TOK", "OPTION_VAL_TOK", "OUT_TOK", "PREC_TOK", 
  "PURE_TOK", "RIGHT_TOK", "SHORT_TEXT_TOK", "START_TOK", 
  "TEST_TOK", "TEXT_TOK", "TOKEN_TOK", "TYPE_TOK", 
  "TYPE_ID_TOK", "UNION_TOK", "XID_TOK", "'='", 
  "$yyBadTok", "$yyEmptyTok"
};
static YY_CONST char *yyNonTermNames[46]= {
  "$S", "act", "altElements", "idOrLit", 
  "optTokVal", "lookDepth", "attrDecs", "attrDecList", 
  "spec", "options", "defs", "rules", 
  "optionVals", "optSemi", "def", "startIDList", 
  "type", "nonTermList", "termDecDir", "termType", 
  "termList", "optNowarn", "nonTermDec", "termDec", 
  "YY_24", "attrDec", "attrType", "attrPrefix", 
  "idVar", "attrSuffix", "attrTextTok", "rule", 
  "ruleLHS", "alts", "altBody", "YY_35", 
  "rhsElement", "attrVals", "testExp", "attrExp", 
  "YY_40", "attrExpList", "attrExpTok", "numVar", 
  "actVars", "$Err"
};
static YY_CONST YYXRHS yyRHS[102]= {
      0,     3,     8,     9,    14,    16,    19,    23,    25,    26, 
     29,    33,    36,    40,    44,    49,    52,    55,    57,    60, 
     64,    67,    70,    73,    76,    78,    79,    82,    83,    85, 
     88,    92,    94,    96,    99,   103,   106,   109,   114,   119, 
    124,   125,   126,   128,   132,   137,   139,   142,   144,   147, 
    148,   151,   153,   157,   160,   164,   166,   170,   173,   174, 
    177,   178,   181,   184,   187,   192,   194,   199,   201,   203, 
    205,   210,   211,   212,   214,   218,   220,   223,   225,   227, 
    229,   231,   235,   236,   239,   242,   245,   247,   248,   250, 
    252,   253,   255,   257,   258,   260,   262,   264,   266,   268, 
    270,   272
};
static YY_CONST YYXNonTerm yyLHS1[102]= {
      0,     8,     9,     9,    12,    12,    12,    12,    10,    10, 
     14,    14,    14,    14,    14,    14,    14,    15,    15,    15, 
     18,    18,    18,    18,    21,    21,    19,    19,    17,    17, 
     17,    22,    20,    20,    20,    23,    23,    23,    23,     6, 
     24,     6,     7,     7,    25,    25,    27,    27,    29,    29, 
     11,    11,    31,    31,    32,    33,    33,    34,    35,     2, 
      2,    36,    36,    36,    36,    36,    36,     5,     5,    38, 
     37,    40,    37,    41,    41,    39,    39,    39,    42,    42, 
     42,     1,    44,    44,    44,    44,     4,     4,    16,    13, 
     13,    26,    26,    26,    30,    30,    43,    43,    28,     3, 
      3,    45
};
static YY_CONST YYXSym yySyms[273]= {
     33,     4,     2,    37,    41,    72,    45,     6,    10,    37, 
     96,    49,    53,    14,   100,    18,    49,   100,    22,    49, 
     16,   100,    26,     0,    30,    34,    41,    57,    38,   124, 
     61,    53,    42,   112,    53,    46,    40,    88,    53,    50, 
    140,    65,    69,    54,    73,    77,    81,    53,    58,   148, 
     53,    62,     0,    53,    66,    44,    70,    61,    44,    74, 
     61,    16,    44,    78,   136,    85,    82,    56,    85,    86, 
    116,    85,    90,    80,    85,    94,    84,    98,   102,    65, 
     25,   106,   110,    89,   114,    69,    89,   118,    69,    16, 
     89,   122,    44,   126,    93,   130,    81,    93,   134,    81, 
     16,    93,   138,    44,    17,   142,    68,    17,   146,    44, 
    156,    68,    17,   150,    68,   156,    44,    17,   154,     8, 
     97,    29,    12,   158,   162,   166,   101,   170,    29,    16, 
    101,   174,   105,   109,   113,   117,   178,     0,   182,   109, 
    121,   186,   121,   190,   117,   121,   194,   198,    45,   125, 
    202,   125,   206,   129,   133,    53,   210,     0,    53,   214, 
    152,    25,    20,   218,   137,   222,   133,    32,   137,   226, 
    141,     9,   230,   234,     9,   145,   238,   242,    44,   149, 
    246,    68,   149,   250,   108,    13,   254,    76,     8,    21, 
     12,   258,     5,   262,   128,     8,   153,    12,   266,   132, 
    270,   120,   274,   157,   278,     8,   161,   165,    12,   282, 
    286,   290,   157,   294,   165,    16,   157,   298,   169,   302, 
    157,   169,   306,     0,   310,   113,   314,   173,   318,   121, 
    322,    28,   177,    36,   326,   330,   177,   113,   334,   177, 
     92,   338,   177,    64,   342,    88,   346,   350,   144,   354, 
     24,   358,   362,    52,   366,   104,   370,   374,   132,   378, 
    120,   382,    92,   386,    64,   390,    48,   394,    44,   398, 
     68,   402,   406
};
static YY_CONST YYXSym yyAccess[149]= {
      1,    33,    37,     4,    96,    41,   100,     0,    49,    72, 
    124,   112,    40,   140,   148,     0,   136,    56,   116,    80, 
     57,    73,   100,    16,    24,    53,     0,   152,    45,   125, 
    129,    44,    61,    53,    88,   144,    65,    53,    53,    84, 
     85,    85,    85,    85,    77,    65,   100,    53,     8,    25, 
    125,   133,   137,   141,    44,    16,    53,    53,    44,    69, 
     89,    44,    68,    81,    93,    25,    97,    20,    32,    53, 
      9,    44,    16,    89,   156,    88,    17,   156,    17,    16, 
     53,    93,     0,    52,   104,    29,   101,   105,   137,    28, 
     44,    68,   108,    76,   128,   145,     5,    89,    68,    44, 
     93,    12,    16,   132,   120,   109,   121,   177,     8,   149, 
    149,    44,    68,    13,     8,     8,    17,    17,   101,    48, 
    113,   121,    36,    92,    64,   113,   161,   132,   120,    21, 
      0,    92,    64,   153,   157,   169,   113,   173,   121,   117, 
    165,   157,    12,    12,   169,   121,    12,    16,   157
};
static YY_CONST YYLineNum yyLineNums[102]= {
      0,   139,   142,   143,   146,   147,   148,   149,   152,   153, 
    156,   157,   158,   159,   160,   161,   162,   166,   168,   170, 
    174,   176,   178,   180,   184,   186,   190,   192,   194,   195, 
    196,   200,   203,   204,   205,   209,   213,   217,   221,   227, 
    226,   229,   232,   233,   236,   237,   240,   241,   244,   245, 
    248,   249,   252,   253,   258,   261,   262,   267,   265,   271, 
    273,   277,   279,   281,   283,   287,   289,   293,   295,   298, 
    301,   301,   302,   305,   306,   309,   310,   311,   314,   315, 
    316,   319,   322,   323,   324,   325,   328,   330,   334,   337, 
    338,   341,   342,   343,   346,   347,   350,   351,   354,   356, 
    357,   358
};
#endif /* #if YY_ZYACC_DEBUG */

static YYTok yyTranslate[288]= {
      1,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
      2,     3,    40,    40,     4,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,     5,     6, 
     40,    39,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,     7,     8,     9,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,    40,    40,    40,    40, 
     40,    40,    40,    40,    40,    40,     0,    10,    11,    12, 
     13,    14,    15,    16,    17,    18,    19,    20,    21,    22, 
     23,    24,    25,    26,    27,    28,    29,    30,    31,    32, 
     33,    34,    35,    36,    37,    38,    40,    41
};
static YYLen yyLength[102]= {
      2,     4,     0,     4,     1,     2,     3,     1,     0,     2, 
      3,     2,     3,     3,     4,     2,     2,     1,     2,     3, 
      2,     2,     2,     2,     1,     0,     2,     0,     1,     2, 
      3,     1,     1,     2,     3,     2,     2,     4,     4,     4, 
      0,     0,     1,     3,     4,     1,     2,     1,     2,     0, 
      2,     1,     3,     2,     3,     1,     3,     2,     0,     2, 
      0,     2,     2,     2,     4,     1,     4,     1,     1,     1, 
      4,     0,     0,     1,     3,     1,     2,     1,     1,     1, 
      1,     3,     0,     2,     2,     2,     1,     0,     1,     1, 
      0,     1,     1,     0,     1,     1,     1,     1,     1,     1, 
      1,     0
};
static YYNonTerm yyLHS[102]= {
      0,     8,     9,     9,    12,    12,    12,    12,    10,    10, 
     14,    14,    14,    14,    14,    14,    14,    15,    15,    15, 
     18,    18,    18,    18,    21,    21,    19,    19,    17,    17, 
     17,    22,    20,    20,    20,    23,    23,    23,    23,     6, 
     24,     6,     7,     7,    25,    25,    27,    27,    29,    29, 
     11,    11,    31,    31,    32,    33,    33,    34,    35,     2, 
      2,    36,    36,    36,    36,    36,    36,     5,     5,    38, 
     37,    40,    37,    41,    41,    39,    39,    39,    42,    42, 
     42,     1,    44,    44,    44,    44,     4,     4,    16,    13, 
     13,    26,    26,    26,    30,    30,    43,    43,    28,     3, 
      3,    45
};
static YYBase yyActBase[149]= {
      1,    15,   316,     1,     5,   342,     1,     1,    90,     7, 
     22,   194,    26,    11,   194,   194,    95,    95,    95,    95, 
      1,     8,     1,    47,     1,     1,   194,   115,     2,     1, 
      1,     1,   129,     1,   194,     1,    66,     1,     1,     1, 
      1,     1,     1,     1,     4,   115,     1,     1,     1,     3, 
      1,   168,     1,     1,     1,    68,     1,     1,     1,   304, 
      1,     0,    32,    64,     1,     1,   348,     1,     1,     1, 
    247,     1,    69,     1,    70,     1,     1,    72,     1,     4, 
      1,     1,     1,     1,     1,     9,     1,    25,     1,     1, 
    233,   233,    40,    83,    84,     1,     1,     1,   272,   272, 
      1,     1,   348,     1,     1,    29,     1,   161,     1,     1, 
      1,     1,     1,     1,    43,   122,     1,     1,     1,     1, 
      1,     1,     1,     1,     1,     1,   122,     1,     1,     6, 
      1,     1,     1,    86,   377,     1,     1,     1,     1,    23, 
     20,   265,     1,     1,     1,     1,     1,   122,   379
};
static YYRuleN yyActDef[149]= {
      2,   101,     8,     0,   101,   101,     4,     7,    90,   101, 
    101,    90,   101,   101,    90,    90,    25,    25,    25,    25, 
      9,    27,     5,   101,    89,     3,    90,    41,   101,    51, 
     58,    17,    90,    11,    90,    88,   101,    15,    16,    24, 
     20,    21,    22,    23,   101,    41,     6,    53,    40,   101, 
     50,    90,    55,    60,    18,   101,    10,    12,    31,    13, 
     28,    87,    87,    90,    32,    26,   101,    54,    58,    52, 
     57,    19,   101,    29,   101,    86,    35,   101,    36,   101, 
     14,    33,    45,    91,    92,   101,    42,   101,    56,    82, 
     72,    72,   101,   101,   101,    59,    65,    30,    87,    87, 
     34,    39,   101,    94,    95,   101,    47,   101,    71,    61, 
     62,    99,   100,    63,   101,   101,    37,    38,    43,    98, 
     49,    46,    81,    84,    85,    83,   101,    67,    68,   101, 
     77,    96,    97,   101,    69,    75,    78,    79,    80,    44, 
    101,    73,    64,    66,    76,    48,    70,   101,    74
};
static YYBase yyGotoBase[46]= {
      1,     1,     1,     1,    89,     1,    76,     1,     1,     1, 
      1,     1,     1,   390,     1,     1,    92,     1,     1,     1, 
      1,   173,    77,    93,     1,    31,     1,     1,    37,     1, 
     36,    98,     1,     1,    79,     1,     1,    21,     1,    39, 
      1,     1,    33,     1,     1,     1
};
static YYState yyGotoDef[46]= {
      0,    96,    70,   113,   117,   129,    65,    85,     1,     2, 
      5,    28,     8,    80,    20,    32,    45,    59,    21,    44, 
     63,    43,    97,   100,    66,   118,    87,   105,   136,   139, 
    138,    50,    30,    51,    88,    53,    95,   110,   133,   148, 
    126,   140,   144,   137,   107,     0
};
static YYAct yyNext[442]= {
    236,     0,    26,   150,   236,     7,   236,    26,    67,   142, 
    236,   236,   101,   102,   236,    61,     3,   236,   236,   176, 
    236,    62,    75,   146,   147,   176,   193,   193,   236,   236, 
      6,   236,   236,    31,   236,   236,   236,   236,   236,    74, 
     27,   119,   236,   236,    35,    27,   236,    35,    34,   236, 
    236,   111,   236,   104,    75,   104,   103,   112,   103,   104, 
    236,   236,   103,   236,   239,   239,   236,   236,    79,   236, 
     24,    77,    46,   128,   239,    61,   127,    58,   239,    71, 
     58,    62,   239,    99,   239,   114,   115,    98,   239,   143, 
    239,   239,   239,   239,    23,   239,    24,    86,   239,   239, 
    239,   239,   239,    49,   239,    36,   174,    29,   239,    52, 
    239,   109,   174,    60,   239,    22,    39,    48,   239,   239, 
    190,   239,   130,   106,   239,   239,   190,   239,   239,   239, 
    239,   174,   190,    55,   119,    24,    73,    64,   132,   239, 
     54,   121,   120,   239,   125,   131,     0,   239,   135,   239, 
     76,    78,   104,   239,   134,   103,    81,   239,   239,   135, 
    239,     0,     0,   239,   239,   141,   239,   239,   239,   239, 
    122,     0,     0,   119,    24,   145,    68,   124,   239,     0, 
    135,     0,   239,     0,   123,     0,   239,   116,   239,    40, 
     41,    42,   239,     0,   239,   239,   239,   239,     0,   239, 
     24,     0,   239,   239,   239,   239,   239,     0,   239,     0, 
      0,     0,   239,     0,   239,     0,     0,     0,   239,     0, 
      0,     0,   239,   239,     0,   239,     0,     0,   239,   239, 
      0,   239,   239,   221,   221,   108,     0,     0,     0,   221, 
    221,   221,     0,     0,   221,     0,     0,   206,   206,     0, 
    221,     0,   221,   206,    89,   206,     0,     0,    90,     0, 
    221,     0,     0,     0,    91,   221,    93,     0,   222,   222, 
      0,   221,   236,     0,    92,     0,   236,   119,   236,    94, 
      0,   132,   236,   236,     0,   206,   236,     0,   131,   236, 
    236,     0,   236,     0,    75,   104,     0,     0,   103,     0, 
    236,   236,     0,   236,   162,     0,   236,   236,    72,   236, 
      0,     0,     0,     0,   162,    58,   157,     0,   162,     0, 
      0,     0,   162,     0,   162,     0,   157,     0,     0,     0, 
    157,     0,   162,   162,   157,   162,   157,     0,   162,   162, 
      4,   162,    15,     0,   157,   157,     0,   157,    82,     0, 
    157,   157,    12,   157,     0,     0,    17,     0,     0,     0, 
      9,    83,    19,     0,     0,     0,     0,     0,     0,     0, 
     11,    18,     0,    10,    84,     0,    16,    13,   242,    14, 
    218,   242,   223,   223,     0,     0,     0,     0,     0,   119, 
      0,   119,     0,   132,     0,   132,     0,     0,    25,     0, 
    131,    33,   131,     0,    37,    38,     0,   104,     0,   104, 
    103,     0,   103,     0,     0,     0,    47,     0,     0,     0, 
      0,     0,    56,     0,    57,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,    69
};
static YYCheck yyCheck[539]= {
      0,   149,     0,     1,     4,     0,     6,     0,     5,     3, 
     10,    11,     3,     4,    14,    11,     1,    17,    18,    11, 
     20,    17,    22,     3,     4,    17,     3,     4,    28,    29, 
     25,    31,     0,    11,    34,    35,     4,    37,     6,    39, 
     38,    12,    10,    11,    36,    38,    14,    36,    22,    17, 
     18,    11,    20,    30,    22,    30,    33,    17,    33,    30, 
     28,    29,    33,    31,     0,     1,    34,    35,     4,    37, 
      6,    39,    25,    30,    10,    11,    33,    11,    14,    11, 
     11,    17,    18,    11,    20,     2,     2,    17,    24,     3, 
      0,     1,    28,    29,     4,    31,     6,    66,    34,    35, 
     10,    37,    38,    27,    14,    13,    11,     9,    18,    30, 
     20,    90,    17,    36,    24,    25,    21,     2,    28,    29, 
      5,    31,     0,    87,    34,    35,    11,    37,    38,     0, 
      1,    36,    17,     4,    12,     6,    59,    44,    16,    10, 
     11,   105,   105,    14,   107,    23,   149,    18,   115,    20, 
     61,    62,    30,    24,   115,    33,    63,    28,    29,   126, 
     31,   149,   149,    34,    35,   126,    37,    38,     0,     1, 
      9,   149,   149,    12,     6,   139,     8,    16,    10,   149, 
    147,   149,    14,   149,    23,   149,    18,    98,    20,    16, 
     17,    18,    24,   149,     0,     1,    28,    29,   149,    31, 
      6,   149,    34,    35,    10,    37,    38,   149,    14,   149, 
    149,   149,    18,   149,    20,   149,   149,   149,    24,   149, 
    149,   149,    28,    29,   149,    31,   149,   149,    34,    35, 
    149,    37,    38,     0,     1,     2,   149,   149,   149,     6, 
      7,     8,   149,   149,    11,   149,   149,     0,     1,   149, 
     17,   149,    19,     6,     7,     8,   149,   149,    11,   149, 
     27,   149,   149,   149,    17,    32,    19,   149,     3,     4, 
    149,    38,     0,   149,    27,   149,     4,    12,     6,    32, 
    149,    16,    10,    11,   149,    38,    14,   149,    23,    17, 
     18,   149,    20,   149,    22,    30,   149,   149,    33,   149, 
     28,    29,   149,    31,     0,   149,    34,    35,     4,    37, 
    149,   149,   149,   149,    10,    11,     0,   149,    14,   149, 
    149,   149,    18,   149,    20,   149,    10,   149,   149,   149, 
     14,   149,    28,    29,    18,    31,    20,   149,    34,    35, 
     24,    37,     0,   149,    28,    29,   149,    31,     0,   149, 
     34,    35,    10,    37,   149,   149,    14,   149,   149,   149, 
     18,    13,    20,   149,   149,   149,   149,   149,   149,   149, 
     28,    29,   149,    31,    26,   149,    34,    35,    30,    37, 
      3,    33,     3,     4,   149,   149,   149,   149,   149,    12, 
    149,    12,   149,    16,   149,    16,   149,   149,     8,   149, 
     23,    11,    23,   149,    14,    15,   149,    30,   149,    30, 
     33,   149,    33,   149,   149,   149,    26,   149,   149,   149, 
    149,   149,    32,   149,    34,   149,   149,   149,   149,   149, 
    149,   149,   149,   149,   149,   149,   149,   149,   149,   149, 
    149,    51,   149,   149,   149,   149,   149,   149,   149,   149, 
    149,   149,   149,   149,   149,   149,   149,   149,   149,   149, 
    149,   149,   149,   149,   149,   149,   149,   149,   149,   149, 
    149,   149,   149,   149,   149,   149,   149,   149,   149,   149, 
    149,   149,   149,   149,   149,   149,   149,   149,   149,   149, 
    149,   149,   149,   149,   149,   149,   149,   149,   149,   149, 
    149,   149,   149,   149,   149,   149,   149,   149,   149,   149, 
    149,   149,   149,   149,   149,   149,   149,   149,   149,   149, 
    149,   149,   149,   149,   149,   149,   149,   149,   149,   149, 
    149,   149,   149,   149,   149,   149,   149,   149,   149
};
#line 868 "parse.c"

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
#line 1175 "parse.c"
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
#line 1203 "parse.c"
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
#line 1272 "parse.c"
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
	case 4:
#line 146 "parse.y"
{ setOptions(YY_NUM_VAR(-1, text)); }
	break;
	case 5:
#line 147 "parse.y"
{ setOptions(YY_NUM_VAR(-1, text)); }
	break;
	case 6:
#line 148 "parse.y"
{ setOptions(YY_NUM_VAR(-1, text)); }
	break;
	case 11:
#line 157 "parse.y"
{ G.isPure= TRUE; }
	break;
	case 12:
#line 158 "parse.y"
{ setExpect(YY_NUM_VAR(-2, num)); }
	break;
	case 15:
#line 161 "parse.y"
{ dclTyped(); }
	break;
	case 17:
#line 166 "parse.y"
{ addStartNonTerm(YY_NUM_VAR(-1, id)); }
	break;
	case 18:
#line 168 "parse.y"
{ addStartNonTerm(YY_NUM_VAR(-1, id)); }
	break;
	case 19:
#line 170 "parse.y"
{ addStartNonTerm(YY_NUM_VAR(-1, id)); }
	break;
	case 20:
#line 174 "parse.y"
{ G.assocPrec= NIL;  }
	break;
	case 21:
#line 176 "parse.y"
{ G.assocPrec= MAKE_ASSOC_PREC(LEFT_ASSOC, G.precLevel++); }
	break;
	case 22:
#line 178 "parse.y"
{ G.assocPrec= MAKE_ASSOC_PREC(RIGHT_ASSOC, G.precLevel++); }
	break;
	case 23:
#line 180 "parse.y"
{ G.assocPrec= MAKE_ASSOC_PREC(NON_ASSOC, G.precLevel++); }
	break;
	case 24:
#line 184 "parse.y"
{ G.nowarn= TRUE; }
	break;
	case 25:
#line 186 "parse.y"
{ G.nowarn= FALSE; }
	break;
	case 26:
#line 190 "parse.y"
{ G.decCount= YY_NUM_VAR(-1, count); }
	break;
	case 27:
#line 192 "parse.y"
{ G.type= NIL; G.decCount= 0; }
	break;
	case 31:
#line 200 "parse.y"
{ dclNonTermAttribs(YY_NUM_VAR(-1, id), G.type); }
	break;
	case 35:
#line 209 "parse.y"
{ dclTermAttribs(YY_NUM_VAR(-2, id), NIL, NIL, YY_NUM_VAR(-1, num), G.assocPrec, 
		       G.type, G.decCount, G.nowarn); 
      }
	break;
	case 36:
#line 213 "parse.y"
{ dclTermAttribs(NIL, YY_NUM_VAR(-2, lit).id, YY_NUM_VAR(-2, lit).val, YY_NUM_VAR(-1, num), G.assocPrec, 
		       G.type, G.decCount, G.nowarn); 
      }
	break;
	case 37:
#line 217 "parse.y"
{ dclTermAttribs(YY_NUM_VAR(-4, id), YY_NUM_VAR(-2, lit).id, YY_NUM_VAR(-2, lit).val, YY_NUM_VAR(-1, num), G.assocPrec, 
		       G.type, G.decCount, G.nowarn); 
      }
	break;
	case 38:
#line 221 "parse.y"
{ dclTermAttribs(YY_NUM_VAR(-2, id), YY_NUM_VAR(-4, lit).id, YY_NUM_VAR(-4, lit).val, YY_NUM_VAR(-1, num), G.assocPrec, 
		       G.type, G.decCount, G.nowarn); 
      }
	break;
	case 40:
#line 226 "parse.y"
{ G.lineN= YY_NUM_VAR(-1, lineN); }
	break;
	case 39:
#line 227 "parse.y"
{ YY_NUM_VAR(0, count)= YY_NUM_VAR(-2, count); }
	break;
	case 41:
#line 229 "parse.y"
{ YY_NUM_VAR(0, count)= 0; }
	break;
	case 42:
#line 232 "parse.y"
{ addAttribSeq(G.lineN); YY_NUM_VAR(0, count)= 1; }
	break;
	case 43:
#line 233 "parse.y"
{ addAttribSeq(YY_NUM_VAR(-2, lineN)); YY_NUM_VAR(0, count)= YY_NUM_VAR(-3, count) + 1; }
	break;
	case 45:
#line 237 "parse.y"
{ setAttribParseError(); }
	break;
	case 54:
#line 258 "parse.y"
{ dclNonTermAttribs(YY_NUM_VAR(-3, id), NIL); }
	break;
	case 58:
#line 265 "parse.y"
{ beginRuleAttribs(); }
	break;
	case 57:
#line 267 "parse.y"
{ endRuleAttribs(YY_NUM_VAR(-1, count)); }
	break;
	case 59:
#line 271 "parse.y"
{ YY_NUM_VAR(0, count)= YY_NUM_VAR(-2, count) + 1; }
	break;
	case 60:
#line 273 "parse.y"
{ YY_NUM_VAR(0, count)= 0; }
	break;
	case 61:
#line 277 "parse.y"
{ addRHSAttribs(YY_NUM_VAR(-2, id), NIL); }
	break;
	case 62:
#line 279 "parse.y"
{ addRHSAttribs(YY_NUM_VAR(-2, lit).id, YY_NUM_VAR(-2, lit).val); }
	break;
	case 63:
#line 281 "parse.y"
{ setRulePrec(YY_NUM_VAR(-1, lit).id); }
	break;
	case 64:
#line 283 "parse.y"
{ if (YY_NUM_VAR(-2, num) != 0 && YY_NUM_VAR(-2, num) != 1) warn("%%look argument should be 0 or 1");
        setRuleLook(YY_NUM_VAR(-2, num) != 0); 
      }
	break;
	case 65:
#line 287 "parse.y"
{ addActAttribs(YY_NUM_VAR(-1, act).actN, YY_NUM_VAR(-1, act).lineNum); }
	break;
	case 66:
#line 289 "parse.y"
{ addTestAttribs(); }
	break;
	case 67:
#line 293 "parse.y"
{ int d; sscanf(YY_NUM_VAR(-1, text), "%d", &d); YY_NUM_VAR(0, num)= d; }
	break;
	case 68:
#line 295 "parse.y"
{ int d; sscanf(YY_NUM_VAR(-1, shortText), "%d", &d); YY_NUM_VAR(0, num)= d; }
	break;
	case 69:
#line 298 "parse.y"
{ addAttribSeq(G.lineN); }
	break;
	case 71:
#line 301 "parse.y"
{ G.lineN= YY_NUM_VAR(-1, lineN); }
	break;
	case 73:
#line 305 "parse.y"
{ addAttribSeq(G.lineN); }
	break;
	case 74:
#line 306 "parse.y"
{ addAttribSeq(YY_NUM_VAR(-2, lineN)); }
	break;
	case 77:
#line 311 "parse.y"
{ setAttribParseError(); }
	break;
	case 87:
#line 330 "parse.y"
{ YY_NUM_VAR(0, num)= NIL; }
	break;
	case 88:
#line 334 "parse.y"
{ G.type= YY_NUM_VAR(-1, type); }
	break;
	case 91:
#line 341 "parse.y"
{ addAttribTok(IN_ATOK, NULL);  }
	break;
	case 92:
#line 342 "parse.y"
{ addAttribTok(OUT_ATOK, NULL);  }
	break;
	case 93:
#line 343 "parse.y"
{ addAttribTok(OUT_ATOK, NULL); }
	break;
	case 94:
#line 346 "parse.y"
{ addAttribTok(TEXT_ATOK, (VOIDP)YY_NUM_VAR(-1, text)); }
	break;
	case 95:
#line 347 "parse.y"
{ addAttribTok(SHORT_TEXT_ATOK, (VOIDP)YY_NUM_VAR(-1, shortText)); }
	break;
	case 96:
#line 350 "parse.y"
{ addAttribTok(NUM_VAR_ATOK, (VOIDP)&YY_NUM_VAR(-1, numVar)); }
	break;
	case 97:
#line 351 "parse.y"
{ addAttribTok(NUM_VAR_ATOK, (VOIDP)&YY_NUM_VAR(-1, numVar)); }
	break;
	case 98:
#line 354 "parse.y"
{ addAttribTok(ID_VAR_ATOK, (VOIDP)&YY_NUM_VAR(-1, id)); }
	break;
	case 99:
#line 356 "parse.y"
{ YY_NUM_VAR(0, lit).id= YY_NUM_VAR(-1, id); YY_NUM_VAR(0, lit).val= NIL; }
	break;
#line 1549 "parse.c"
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

#line 360 "parse.y"


int 
yyerror(s)
  char *s;
{
  error("%s at `%s'", s, yytext);  
  return 0;
}

Boolean 
hasLookahead()
{
  return yychar != YYEMPTY;
}

Count 
outParseParams(outFile)
  FILE *outFile;
{
  Count lineCount= 0;
  fprintf(outFile, "#define YY_IS_PURE %d\n", G.isPure); lineCount++;
  return lineCount;
}

#ifdef TEST_GRAM

VOID 
printAssocPrec(assocPrec1)
  Index assocPrec1;
{
  Index2 assocPrec= (Index2) assocPrec1;
  if (assocPrec != NIL) {
    printf(" ");
    switch (ASSOC(assocPrec)) {
      case NON_ASSOC:
        printf("NON_ASSOC");
	break;
      case LEFT_ASSOC:
        printf("LEFT");
	break;
      case RIGHT_ASSOC:
        printf("RIGHT");
	break;
      default:
	INTERNAL_ERROR();
    }
    printf(" %d", PREC(assocPrec));
  }
  VOID_RET();
}


#endif

#line 1664 "parse.c"
