/* Automatically generated from parse.y by zyacc version 1.02
 * using the command:
 * zyacc -d -v -o parse.c parse.y 
 */
/*

File:	 zlexskl.c
Purpose: Skeleton for zyacc parser generator.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/
#define ZYACC

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
#define YY_CONST const
#define YY_PROTO 1
#else  /* ! __STDC__ */
#define YY_VOIDP char *
#define YY_VOID
#define YY_CONST
#define YY_PROTO 0
#if 0 
/* It is safest to not declare these here.  Most compilers should do the
 * right thing after producing warnings about missing declarations.
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



/* 			PARSER PARAMETERS.			*/

/* 

Define following parameters:

YY_HAS_IN_ATTRIBS:	1 if parser uses %in attributes.
YY_BAD_TOK:		Used to indicate an invalid token.
YY_EMPTY_TOK:		Used to indicate no lookahead.
YY_ERR_TOK_VAL:		Value associated with error token.
YY_HAS_MULTI_START:	1 if we have multiple %start non-terminals.
YY_N_TESTS:		# of semantic %tests used by gramar.
YY_MAX_TOK_VAL:		Maximum (user or automatically assigned) token value.
YY_MAX_RULE_LEN:	Maximum length of a rule.
YY_N_NON_TERMS:		# of non-terminal symbols (including start-symbol).
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
YY_HAS_LOC:		1 if @n construct used; 0 otherwise.
YY_IS_PURE:		1 if %pure_parser requested; 0 otherwise.
YY_IS_TYPED:		1 if <type> used.
*/

#define YY_HAS_IN_ATTRIBS 0
#define YY_BAD_TOK 38
#define YY_EMPTY_TOK 39
#define YY_ERR_TOK_VAL 256
#define YY_HAS_MULTI_START 0
#define YY_N_TESTS 0
#define YY_MAX_TOK_VAL 276
#define YY_MAX_RULE_LEN 5
#define YY_N_NON_TERMS 33
#define YY_N_RHS 231
#define YY_N_RULES 79
#define YY_N_TOKS 40
#define YY_BAD_BASE 2
#define YY_N_CHECK 382
#define YY_N_NEXT 349
#define YY_N_STATES 110
#define YY_TESTS_SIZE 0
#define YY_CMD_LINE_DEBUG 0
#define YY_PREFIX yy
#define YY_IS_PURE 0
#define YY_HAS_LOC 0
#define YY_IS_TYPED 1
#define YY_ZYACC_MAJOR_VERSION 1
#define YY_ZYACC_MINOR_VERSION 02
#line 145 "parse.c"

/*                          TOKEN DEFINITIONS.                          */

#define ACT_TOK 257
#define CHAR_TOK 258
#define COLON_BEGIN_TOK 259
#define COLON_END_TOK 260
#define EOF_PAT_TOK 261
#define ID_TOK 262
#define LEX_DIR_TOK 263
#define MACRO_TOK 264
#define NEXT_ACT_TOK 265
#define NUM_TOK 266
#define OPTION_VAL_TOK 267
#define OPTION_TOK 268
#define SEC_TOK 269
#define SS_ID_TOK 270
#define STARTX_TOK 271
#define START_TOK 272
#define ARRAY_OPTION_TOK 273
#define POINTER_OPTION_TOK 274
#line 167 "parse.c"

/*	USER DEFINITIONS FROM SECTION 1 OF YACC FILE.			*/

#line 20 "parse.y"


#include "chrclass.h"
#include "options.h"
#include "patterns.h"
#include "scan.h"
#include "zparse.h"

#include "area.h"
#include "error.h"
#include "iset.h"

#include <ctype.h>

static struct {
  Boolean isExclusive;		/* TRUE if current ss-decl is exclusive. */
} globals;
#define G globals

typedef enum {
  EOF_RE,			/* RE for <<EOF>> pattern. */
  INTRA_TOK_RE,			/* RE for intra-token pattern. */
  START_RE,			/* RE with `^' pattern. */
  SIMPLE_RE			/* Not one of the above. */
} REType;
  
VOID yyerror PROTO((Char *msg));

#define yylex scan


#line 203 "parse.c"

/*                      SEMANTIC TYPE DEFINITION YYSTYPE.               */

typedef union {
#line 52 "parse.y"
 
  Index2 id;		/* ID index for macros and start-states. */
  UInt ch;		/* For single-chars in patterns. */
  Index2 actN;		/* Action number. */
  Pattern pat;
  Index2 lineN;		/* Line number in source file. */
  struct {
    ISet ss;		/* Set of start-states used in pattern. */
    Index2 lineN;	/* Line # at which start-states begin. */
  } ssLineN;
  ISet ss;
  VOIDP rawClass;
  Index class;
  UInt num;		/* Numbers for repetition pattern. */
  struct {
    Index2 lo, hi;	/* Inclusive boundaries of character range. */
  } charRange;
  struct {
    Index2 lo, hi;	/* Inclusive boundaries. */
  } repeatRange;	/* Range used for repetition pattern. */
  ConstString text;	/* Options text. */

} YYSTYPE;

#line 233 "parse.c"

/*           DEFINE TYPES WHICH DEPEND ON PARSER PARAMETERS.            */


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


/* typedef YYRHS to represent values in 0, ..., (YY_N_RHS - 1). */
#if ((YY_N_RHS - 1) <= YY_UCHAR_MAX)
typedef YYUChar YYRHS;
#elif ((YY_N_RHS - 1) <= YY_USHRT_MAX)
typedef YYUShrt YYRHS;
#else
  #error Type YYRHS cannot be represented.
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


/* Combine YY_CMD_LINE_DEBUG and YYDEBUG into YYDEBUG. */
#if YY_CMD_LINE_DEBUG == 1
#undef YYDEBUG
#define YYDEBUG 1
#endif

#if YYDEBUG

#include <stdio.h>

typedef enum {
  YY_TOKEN_SYM= 0,		/* A token symbol. */
  YY_NON_TERM_SYM= 1,		/* A non-terminal symbol. */
  YY_RULE_SYM= 2		/* Rule symbol --- used only rule end. */
} YYSymType;

#define YY_SYM_TYPE_BIT 2	/* Min. # of bits for YYSymType. */

#define YY_SYM_TYPE(sym)						\
  ((YYSymType) ((sym) & ((1L << YY_SYM_TYPE_BIT) - 1)))
#define YY_SYM_NUM(sym)		((sym) >> YY_SYM_TYPE_BIT)
#define YY_MAKE_SYM(symType, symNum)	 				\
  ((((symNum) << YY_SYM_TYPE_BIT)) | (symType))

#define YY_MAX_SYM \
((( /* max(YY_N_NON_TERMS, YY_N_TOKS, YY_N_RULES) */ \
    (YY_N_NON_TERMS > YY_N_TOKS \
     ? (YY_N_NON_TERMS > YY_N_RULES ? YY_N_NON_TERMS : YY_N_RULES) \
     : (YY_N_TOKS > YY_N_RULES ? YY_N_TOKS : YY_N_RULES)) \
   - 1) \
  << YY_SYM_TYPE_BIT) \
 | ((1 << YY_SYM_TYPE_BIT) - 1))



/* typedef YYSym to represent values in 0, ..., YY_MAX_SYM. */
#if (YY_MAX_SYM <= YY_UCHAR_MAX)
typedef YYUChar YYSym;
#elif (YY_MAX_SYM <= YY_USHRT_MAX)
typedef YYUShrt YYSym;
#else
  #error Type YYSym cannot be represented.
#endif


#endif /* ifdef YYDEBUG */

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

#line 416 "parse.c"


/*                           GRAMMAR TABLES.                            */

/*

#if YYDEBUG					#Grammar info. for debugging.

static char *yyTokenNames[YY_N_TOKS];		#Token names. 
static char *yyNonTermNames[YY_N_NONTERMS];	#NonTerminal names.
static YYRHS yyRule[YY_N_RULES];		#Start of each rule in yySyms.
static YYSym yySyms[YY_N_RHS];			#Rule right-hand sides.
static YYSym yyAccess[YY_N_STATES]		#Access symbols for each state.

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



*/

#if YYDEBUG

static char *yyTokenNames[40]= {
  "error", "<EOF>", "'<'", "'+'", 
  "ACT_TOK", "CHAR_TOK", "COLON_BEGIN_TOK", "COLON_END_TOK", 
  "EOF_PAT_TOK", "ID_TOK", "LEX_DIR_TOK", "MACRO_TOK", 
  "NEXT_ACT_TOK", "NUM_TOK", "OPTION_VAL_TOK", "OPTION_TOK", 
  "SEC_TOK", "SS_ID_TOK", "STARTX_TOK", "START_TOK", 
  "ARRAY_OPTION_TOK", "POINTER_OPTION_TOK", "'^'", "'$'", 
  "'/'", "'>'", "','", "'|'", 
  "'*'", "'?'", "'('", "')'", 
  "'.'", "'['", "']'", "'-'", 
  "'{'", "'}'", "$yyBadTok", "$yyEmptyTok"
};
static char *yyNonTermNames[33]= {
  "$S", "baseRegExp", "catRegExp", "classElement", 
  "classElements", "ctypeClass", "postRegExp", "numRange", 
  "regExp", "optRightContext", "optSSList", "rightContext", 
  "ssUseList", "lexProgram", "section1", "YY_15", 
  "section2", "options", "restSection1", "optionLine", 
  "section1Line", "startDec", "def", "lexDir", 
  "YY_24", "ssDefList", "YY_26", "YY_27", 
  "sec2Patterns", "actPatterns", "lastActPattern", "intermediateActPattern", 
  "$Err"
};
static YYRHS yyRule[79]= {
      0,     3,     8,     9,    12,    14,    15,    18,    21,    23, 
     25,    27,    29,    32,    34,    36,    38,    40,    42,    45, 
     49,    50,    54,    55,    56,    59,    63,    64,    66,    69, 
     70,    73,    75,    78,    82,    84,    89,    95,    99,   104, 
    108,   113,   119,   123,   128,   132,   133,   135,   137,   140, 
    141,   145,   147,   151,   155,   157,   160,   162,   165,   168, 
    171,   174,   176,   180,   182,   184,   186,   190,   195,   197, 
    199,   202,   205,   207,   211,   215,   219,   224,   230
};
static YYSym yySyms[231]= {
     53,     4,     2,    57,    64,    61,    65,     6,    10,    69, 
     73,    14,    69,    18,    22,    69,    77,    26,    60,    56, 
     30,    60,    34,    80,    38,    84,    42,    81,    46,    73, 
     81,    50,    85,    54,    89,    58,    93,    62,     0,    66, 
     40,    70,    40,    56,    74,    76,    97,   101,    78,    82, 
     72,   105,   101,    86,    90,    94,   101,    68,    98,    36, 
    109,    33,   102,   106,    36,   110,    16,   113,   114,   118, 
    113,   117,   122,   121,   126,   125,   117,   130,    12,    33, 
     16,   134,     0,   138,    41,    33,    37,    48,   142,    41, 
     88,    33,    37,    48,   146,    41,    45,    48,   150,    41, 
     88,    45,    48,   154,    41,    32,    48,   158,    41,    33, 
     37,    16,   162,    41,    88,    33,    37,    16,   166,    41, 
     45,    16,   170,    41,    88,    45,    16,   174,    41,    32, 
     16,   178,   182,    45,   186,    92,   190,    96,    33,   194, 
    198,     8,    49,   100,   202,    68,   206,    49,   104,    68, 
    210,    33,   108,     9,   214,     9,   218,     9,    25,   222, 
     25,   226,    25,   112,   230,    25,   116,   234,    25,    12, 
    238,    25,    29,   242,     5,   246,   120,    33,   124,   250, 
    128,   254,    20,   258,    44,   262,   132,    17,   136,   266, 
    132,    88,    17,   136,   270,    13,   274,    21,   278,    17, 
     13,   282,    17,    21,   286,    20,   290,    20,   140,    20, 
    294,    24,    36,    28,   298,   144,    52,   148,   302,   144, 
     52,   104,   148,   306,   144,    52,   104,    52,   148,   310, 
    314
};
static YYSym yyAccess[110]= {
      1,    53,    57,    69,     4,    64,    60,    80,    84,     0, 
     76,    72,    36,    40,    73,    77,    81,    85,    89,    93, 
     61,    56,    97,   105,   109,    56,    81,    16,    65,   101, 
    101,   120,   128,    20,    44,   132,    33,     9,    25,     5, 
    113,    68,    33,    88,    20,    24,    17,    13,    21,   108, 
     25,   112,   116,    12,   144,    29,     8,    12,     0,   117, 
    121,   125,    41,   124,    17,   140,    36,   136,    13,    21, 
      9,    52,    68,    49,    33,   117,    88,    32,    92,    96, 
     33,    45,   136,    20,    28,   148,   104,   100,   104,    16, 
     33,    45,    16,    48,    33,    37,    45,    16,    48,   148, 
     52,    68,    37,    16,    48,    16,    48,   148,    16,    48
};
#endif /* #if YYDEBUG */

static YYTok yyTranslate[277]= {
      1,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    23,    38,    38,    38, 
     30,    31,    28,     3,    26,    35,    32,    24,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
      2,    38,    25,    29,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    33,    38,    34,    22,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    36,    27,    37,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,    38,    38,    38,    38, 
     38,    38,    38,    38,    38,    38,     0,     4,     5,     6, 
      7,     8,     9,    10,    11,    12,    13,    14,    15,    16, 
     17,    18,    19,    20,    21,    38,    39
};
static YYLen yyLength[79]= {
      2,     4,     0,     2,     1,     0,     2,     2,     1,     1, 
      1,     1,     2,     1,     1,     1,     1,     1,     2,     3, 
      0,     3,     0,     0,     2,     3,     0,     1,     2,     0, 
      2,     1,     2,     3,     1,     4,     5,     3,     4,     3, 
      4,     5,     3,     4,     3,     0,     1,     1,     2,     0, 
      3,     1,     3,     3,     1,     2,     1,     2,     2,     2, 
      2,     1,     3,     1,     1,     1,     3,     4,     1,     1, 
      2,     2,     1,     3,     3,     3,     4,     5,     0
};
static YYNonTerm yyLHS[79]= {
      0,    13,    15,    14,    14,    17,    17,    19,    19,    19, 
     19,    18,    18,    20,    20,    20,    20,    23,    23,    21, 
     24,    21,    26,    25,    25,    22,    27,    22,    16,    28, 
     28,    29,    29,    29,    29,    31,    31,    31,    31,    31, 
     30,    30,    30,    30,    30,     9,     9,    11,    11,    10, 
     10,    12,    12,     8,     8,     2,     2,     6,     6,     6, 
      6,     6,     1,     1,     1,     1,     1,     1,     4,     4, 
      4,     4,     3,     3,     5,     7,     7,     7,    32
};
static YYBase yyActBase[110]= {
      2,     7,     6,   243,     2,     2,   226,     2,     2,     2, 
      2,     2,   171,   256,   282,     2,     2,     2,     2,     2, 
     11,     2,     2,     2,    64,     2,     2,     2,     2,   267, 
    271,    64,     2,     2,     2,    54,   212,    68,     0,     2, 
    136,     2,    97,    15,    20,    31,     1,     2,     2,    64, 
     34,     2,     2,     2,    36,     2,    30,    64,     2,     2, 
      2,   140,   183,     2,     8,    83,    76,     2,     2,     2, 
    102,    90,     2,   124,    47,     2,   187,    77,     2,    64, 
     81,    78,     2,     2,     2,     2,     4,     2,    86,     2, 
     81,   105,     2,     2,    44,   111,     2,     2,     2,     2, 
     73,     2,   118,     2,     2,     2,     2,     2,     2,     2
};
static YYRuleN yyActDef[110]= {
      5,    78,    78,    78,     0,     2,     8,     9,    10,    16, 
     20,    22,    27,    17,    78,     6,    11,    13,    14,    15, 
     78,     7,    23,    23,    78,    18,    12,    29,     1,    19, 
     21,    78,    63,    64,    65,    78,    25,    54,    56,    61, 
     78,    24,    78,    78,    72,    78,    78,    68,    69,    78, 
     55,    57,    58,    59,    78,    60,    78,    78,    34,    30, 
     31,    78,    78,    62,    78,    78,    78,    66,    70,    71, 
     53,    78,    51,    78,    78,    32,    78,    78,    47,    78, 
     45,    78,    67,    73,    74,    75,    78,    50,    78,    33, 
     45,    78,    44,    39,    48,    78,    46,    42,    37,    76, 
     78,    52,    78,    43,    38,    40,    35,    77,    41,    36
};
static YYBase yyGotoBase[33]= {
      2,     2,   106,   122,    96,   142,    82,     2,   272,    66, 
      2,    91,     2,     2,     2,     2,     2,     2,     2,     2, 
    151,     2,     2,     2,     2,   134,     2,     2,     2,   121, 
      2,     2,     2
};
static YYState yyGotoDef[33]= {
      0,    39,    37,    68,    64,    69,    38,    55,    94,   102, 
     62,    96,    73,     1,     2,    20,    28,     3,    14,    15, 
     26,    17,    18,    19,    22,    30,    23,    24,    40,    75, 
     60,    61,     0
};
static YYAct yyNext[349]= {
    166,     0,     0,    53,   166,   166,    44,    45,     4,   166, 
    166,   166,   166,    44,    45,    27,   166,   100,   166,   166, 
     44,    45,     5,   166,   166,   182,   182,   166,    51,    52, 
    166,   166,   166,   166,   165,    67,    54,    53,   165,   165, 
     66,    99,    82,   165,   165,   165,   165,    72,   158,    71, 
    165,    89,   165,   165,   182,    65,   158,   165,   165,    44, 
     45,   165,    51,    52,   165,   165,   165,   165,   164,    33, 
     54,    49,   164,    33,    49,    34,    43,   164,   164,    34, 
    164,    92,    97,    84,   164,   155,   164,   164,    83,    93, 
     98,   164,   164,   155,    31,   164,    32,    35,    31,   164, 
     32,    35,   163,   101,    78,    79,   163,    33,    49,   103, 
    107,   163,   163,    34,   163,   105,    86,   104,   163,    50, 
    163,   163,   108,   106,    49,   163,   163,    85,    63,   163, 
    109,    46,    31,   163,    32,    35,    58,   138,    56,    57, 
     58,   159,    56,    57,   159,   159,    95,   159,   159,    87, 
     88,   159,    50,    81,    16,    70,    29,    47,   159,   159, 
    159,    59,   159,   159,   159,    47,   159,    91,   159,   159, 
    159,   137,   159,   159,     0,     0,   136,    48,     0,     0, 
    137,   137,   136,     0,     0,    48,     0,   137,    33,   137, 
    137,    77,    33,     0,    34,     0,     0,     0,    34,     0, 
      0,   136,     0,   136,   136,    76,    78,    79,     0,     0, 
     78,    79,   135,    31,     0,    32,    35,    31,     0,    32, 
     35,   135,   135,     0,     0,     0,   118,     0,   135,     0, 
    135,   135,     0,     0,     0,   118,   118,     0,     0,    49, 
     21,   118,   118,     9,   118,   118,   118,   118,     0,     0, 
      0,     0,    12,    13,     0,     0,   127,     0,     6,   114, 
      0,    11,    10,     7,     8,   127,   127,   129,     0,     0, 
     25,   131,   127,     0,   127,   127,   129,   129,     0,     0, 
    131,   131,     9,   129,    41,   129,   129,   131,    41,   131, 
    131,    12,    13,     0,     0,     0,    36,     0,   113,     0, 
     11,    10,     0,    42,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,    74, 
      0,     0,     0,     0,    80,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,    90
};
static YYCheck yyCheck[382]= {
      0,   110,   110,     3,     4,     5,     5,     6,     1,     9, 
     10,    11,    12,     5,     6,     4,    16,    13,    18,    19, 
      5,     6,    16,    23,    24,     5,     6,    27,    28,    29, 
     30,    31,    32,    33,     0,    34,    36,     3,     4,     5, 
      9,    37,    34,     9,    10,    11,    12,    17,     4,    13, 
     16,     4,    18,    19,    34,    35,    12,    23,    24,     5, 
      6,    27,    28,    29,    30,    31,    32,    33,     0,     5, 
     36,    27,     4,     5,    27,    11,    22,     9,    10,    11, 
     12,     4,     4,     7,    16,     4,    18,    19,     5,    12, 
     12,    23,    24,    12,    30,    27,    32,    33,    30,    31, 
     32,    33,     0,    17,    23,    24,     4,     5,    27,     4, 
     37,     9,    10,    11,    12,     4,    26,    12,    16,    37, 
     18,    19,     4,    12,    27,    23,    24,    37,    31,    27, 
     12,    35,    30,    31,    32,    33,     0,     1,     2,     3, 
      0,     5,     2,     3,     8,     5,    80,    11,     8,    25, 
     26,    11,    70,    62,     3,    49,    22,    35,    22,    23, 
     24,    40,    22,    23,    24,    43,    30,    76,    32,    33, 
     30,     0,    32,    33,   110,   110,     5,    35,   110,   110, 
      9,    10,    11,   110,   110,    43,   110,    16,     5,    18, 
     19,     8,     5,   110,    11,   110,   110,   110,    11,   110, 
    110,    30,   110,    32,    33,    22,    23,    24,   110,   110, 
     23,    24,     0,    30,   110,    32,    33,    30,   110,    32, 
     33,     9,    10,   110,   110,   110,     0,   110,    16,   110, 
     18,    19,   110,   110,   110,     9,    10,   110,   110,    27, 
     14,    15,    16,     0,    18,    19,    20,    21,   110,   110, 
    110,   110,     9,    10,   110,   110,     0,   110,    15,    16, 
    110,    18,    19,    20,    21,     9,    10,     0,   110,   110, 
     14,     0,    16,   110,    18,    19,     9,    10,   110,   110, 
      9,    10,     0,    16,    17,    18,    19,    16,    17,    18, 
     19,     9,    10,   110,   110,   110,    24,   110,    16,   110, 
     18,    19,   110,    31,   110,   110,   110,   110,   110,   110, 
    110,   110,   110,   110,   110,   110,   110,   110,   110,   110, 
    110,   110,   110,   110,   110,   110,   110,   110,   110,    57, 
    110,   110,   110,   110,    62,   110,   110,   110,   110,   110, 
    110,   110,   110,   110,   110,   110,   110,   110,    76,   110, 
    110,   110,   110,   110,   110,   110,   110,   110,   110,   110, 
    110,   110,   110,   110,   110,   110,   110,   110,   110,   110, 
    110,   110,   110,   110,   110,   110,   110,   110,   110,   110, 
    110,   110
};
#line 693 "parse.c"

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
#ifndef YY_LOC
/* Default struct to maintain location information. */
#define YY_LOC 								\
  struct {								\
    int first_line;							\
    int last_line;							\
    int first_column;							\
    int last_column;							\
  }
#endif

typedef YY_LOC YYLoc;

#endif /* if (YY_HAS_LOC) */

typedef YYSTYPE YYSType;	/* Type for semantic values. */

typedef struct {		/* Stack type. */
  YYState state;		/* State stored in stack entry. */
  YYSType outVals;		/* Semantic value. */
#if (YY_HAS_IN_ATTRIBS)
  YYIn inVals;
#endif
#if (YY_HAS_LOC)
  YYLoc loc;			/* Location information. */
#endif
} YYStk;

#if YY_IS_TYPED
#define YY_NUM_VAR(offset, type)	(yySP[offset].outVals.type)
#else
#define YY_NUM_VAR(offset, type)	(yySP[offset].outVals)
#endif

#define YY_IN_VAR(offset, selector)	(yySP[offset].inVals.selector)
#define YY_OUT_VAR(offset, selector)	(yySP[offset].outVals.selector)
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
#if YYDEBUG
int YY_DEBUG= 0;		/* Flag to turn debug messages on/off. 	*/
#endif
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
  YY_STK_LOC(rhsIndex - ruleLen - 1)

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
#if YY_PROTO
static YYUInt yyGrowStk (YYStk **mainBase, YYStk **mainSP, YYUInt newSize)
#else
static YYUInt yyGrowStk (mainBase, mainSP, newSize)
  YYStk **mainBase;
  YYStk **mainSP;
  YYUInt newSize;
#endif /* #if YY_PROTO */
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
    return newSize;
  }
}


/*                         DEBUG HELPER ROUTINES.                       */

#if YYDEBUG



#if YY_PROTO
static YY_CONST char *yySymName (YYUInt sym) 
#else
static YY_CONST char *yySymName (sym) 
  YYUInt sym;
#endif /* #if YY_PROTO */
{
  return ((YY_SYM_TYPE(sym) == YY_NON_TERM_SYM) 
	  ? yyNonTermNames 
	  : yyTokenNames)
	  [YY_SYM_NUM(sym)];	
}

#if YY_PROTO
static YY_VOID yyPrintRule (YYUInt ruleN) 
#else
static YY_VOID yyPrintRule (ruleN) 
  YYUInt ruleN;
#endif /* #if YY_PROTO */
{
  unsigned i;
  fprintf(stderr, "%s: ", yyNonTermNames[yyLHS[ruleN]]);
  for (i= yyRule[ruleN]; YY_SYM_TYPE(yySyms[i]) != YY_RULE_SYM; i++) {
    fprintf(stderr, "%s ",  yySymName(yySyms[i]));
  }
}

#endif /* #if YYDEBUG */


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
  YYStk *yyStk= (YYStk *)malloc(YY_STK_INC * sizeof(YYStk)); /* Stack base. */	
  YYStk *yySP= yyStk;					/* Main stk. ptr. */
  YYUInt yyStkSize= YY_STK_INC;	/* Current size of main & location stks. */
#if YY_HAS_MULTI_START
  YYState yyState= YY_START;	/* Current parser state. */
#else
  YYState yyState= 0;		/* Current parser state. */
#endif
  YYTok yyTok= YYEMPTY;		/* Translated value of current lookahead. */
  int yyCharSave= 0;		/* Saved value of yyChar in error processing. */
  YY_CONST YYTok yyErrTok= 	/* Token error. */
    YY_TRANSLATE(YY_ERR_TOK_VAL);
  YYUChar yyErrShift= 0;	/* # of tokens to be shifted when error. */
  YYUChar yyRetVal;		/* Value to be returned by yyparse(). */
#if YY_IS_PURE
  YYSType YY_LVAL;		/* Lexical semantic value. */
  int YY_CHAR;			/* Current lookahead token (actual value). */
  int YY_NERRS= 0;		/* Number of errors. */
#if (YY_HAS_LOC)
  YYLoc yylloc;			/* Lexical location value. */
#endif
#endif
  /* Local definitions from user. */
#line 989 "parse.c"
  /* Confirm initial stack allocation ok. */
  if (!yyStk) 
    YY_ERROR("Out of memory.");	/* Initial stk. allocation failed. */

  yySP->state= yyState; yySP++;		/* Push initial state. */

  while (1) {				/* Terminate only via return. */
    YYUInt yyRuleN;			/* Rule # used for reduction. */

    assert(yyState == yySP[-1].state);	/* yyState mirrors state on TOS. */

    /* yyTok & YY_CHAR must agree. */
    assert(yyTok == YYEMPTY || yyTok == YY_TRANSLATE(YY_CHAR)); 

#if YYDEBUG
    if (YY_DEBUG != 0) {
      YYStk *p;	/* Spew out stack. */
      fprintf(stderr, "[ ");
      for (p= (YY_DEBUG > 0 || yySP + YY_DEBUG < yyStk) 
              ? yyStk 
	      : yySP + YY_DEBUG;
	   p < yySP; p++) {
	YY_CONST YYUInt s= p->state;
	fprintf(stderr, "%d/%s ", s, yySymName(yyAccess[s]));
      }
      fprintf(stderr, "] ");
    }
#endif
    /* Ensure space for at least 1 more stack entry. */
    YY_STK_CHK(yyStk, yySP, yyStkSize, YY_STK_INC);
#if YY_HAS_IN_ATTRIBS
    switch (yyState) { /* Don't check for non-empty reduction. */
#line 1022 "parse.c"
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
#if YYDEBUG
    if (YY_DEBUG != 0) {
      fprintf(stderr, "%s: SHIFT %d\n", yyTokenNames[yyTok], yyState);
    }
#endif
    /* Push state on stack. */
    yySP->state= yyState; yySP->outVals= YY_LVAL; 
#if (YY_HAS_LOC)
    yySP->loc= yylloc;
#endif
    yySP++;

    if (yyErrShift) {	/* Error processing in shift action. */
      if (yyTok == yyErrTok) { 	/* We successfully shifted error. */
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
#line 1090 "parse.c"
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

#if YYDEBUG
      if (YY_DEBUG != 0) {
        fprintf(stderr, "%s: ", yyTokenNames[yyTok]);
        if (yyRuleN == YY_ERR_RULE || yyTok == yyErrTok) {
	  fprintf(stderr, "ERROR");
        }
        else if (yyRuleN == YY_ACCEPT_RULE) {
	  fprintf(stderr, "ACCEPT." );
        }
        else {
	  fprintf(stderr, "REDUCE %d ", yyRuleN);
	  yyPrintRule(yyRuleN); 
        }
        fprintf(stderr, "\n");
      }
#endif
      if (yyTok == yyErrTok) goto yyFail; /* error triggered a default redn. */
      switch (yyRuleN) {	/* Perform relevant user or default action. */
	/* Use yySP->outVals as temporary to store new semantic value. */
	case YY_ACCEPT_RULE:	/* Accepting rule. */
	  YYACCEPT; break;
	/* User actions go here. */
	case 2:
#line 172 "parse.y"
{ beginSec2Patterns(); }
	break;
	case 1:
#line 172 "parse.y"
{ endSec2Patterns(); }
	break;
	case 7:
#line 183 "parse.y"
{ setOptions(YY_NUM_VAR(-1, text)); }
	break;
	case 9:
#line 185 "parse.y"
{ setOptions("--array"); }
	break;
	case 10:
#line 186 "parse.y"
{ setOptions("--array=0"); }
	break;
	case 20:
#line 203 "parse.y"
{ G.isExclusive= FALSE; }
	break;
	case 22:
#line 204 "parse.y"
{ G.isExclusive= TRUE; }
	break;
	case 24:
#line 208 "parse.y"
{ defStartState(YY_NUM_VAR(-1, id), G.isExclusive, lineN()); }
	break;
	case 26:
#line 211 "parse.y"
{ /*$<lineN>$= lineN();*/ }
	break;
	case 25:
#line 211 "parse.y"
{ defMacro(YY_NUM_VAR(-3, id), YY_NUM_VAR(-1, pat), lineN()); }
	break;
	case 27:
#line 212 "parse.y"
{ defMacro(YY_NUM_VAR(-1, id), NIL, lineN()); }
	break;
	case 33:
#line 224 "parse.y"
{ addIntraTokPat(YY_NUM_VAR(-3, lineN), YY_NUM_VAR(-2, pat), YY_NUM_VAR(-1, actN)); }
	break;
	case 35:
#line 229 "parse.y"
{ addRegularPat(FALSE, RELEASE_ISET(YY_NUM_VAR(-4, ssLineN).ss), YY_NUM_VAR(-4, ssLineN).lineN, YY_NUM_VAR(-3, pat), YY_NUM_VAR(-2, pat), YY_NUM_VAR(-1, actN)); }
	break;
	case 36:
#line 231 "parse.y"
{ addRegularPat(TRUE, RELEASE_ISET(YY_NUM_VAR(-5, ssLineN).ss), YY_NUM_VAR(-5, ssLineN).lineN, YY_NUM_VAR(-3, pat), YY_NUM_VAR(-2, pat), YY_NUM_VAR(-1, actN)); }
	break;
	case 37:
#line 233 "parse.y"
{ addRegularPat(FALSE, RELEASE_ISET(YY_NUM_VAR(-3, ssLineN).ss), YY_NUM_VAR(-3, ssLineN).lineN, NIL, YY_NUM_VAR(-2, pat), YY_NUM_VAR(-1, actN)); }
	break;
	case 38:
#line 235 "parse.y"
{ addRegularPat(TRUE, RELEASE_ISET(YY_NUM_VAR(-4, ssLineN).ss), YY_NUM_VAR(-4, ssLineN).lineN, NIL, YY_NUM_VAR(-2, pat), YY_NUM_VAR(-1, actN)); }
	break;
	case 39:
#line 237 "parse.y"
{ addEOFPat(RELEASE_ISET(YY_NUM_VAR(-3, ssLineN).ss), YY_NUM_VAR(-3, ssLineN).lineN, YY_NUM_VAR(-1, actN)); }
	break;
	case 40:
#line 241 "parse.y"
{ addRegularPat(FALSE, RELEASE_ISET(YY_NUM_VAR(-4, ssLineN).ss), YY_NUM_VAR(-4, ssLineN).lineN, YY_NUM_VAR(-3, pat), YY_NUM_VAR(-2, pat), YY_NUM_VAR(-1, actN)); }
	break;
	case 41:
#line 243 "parse.y"
{ addRegularPat(TRUE, RELEASE_ISET(YY_NUM_VAR(-5, ssLineN).ss), YY_NUM_VAR(-5, ssLineN).lineN, YY_NUM_VAR(-3, pat), YY_NUM_VAR(-2, pat), YY_NUM_VAR(-1, actN)); }
	break;
	case 42:
#line 245 "parse.y"
{ addRegularPat(FALSE, RELEASE_ISET(YY_NUM_VAR(-3, ssLineN).ss), YY_NUM_VAR(-3, ssLineN).lineN, NIL, YY_NUM_VAR(-2, pat), YY_NUM_VAR(-1, actN)); }
	break;
	case 43:
#line 247 "parse.y"
{ addRegularPat(TRUE, RELEASE_ISET(YY_NUM_VAR(-4, ssLineN).ss), YY_NUM_VAR(-4, ssLineN).lineN, NIL, YY_NUM_VAR(-2, pat), YY_NUM_VAR(-1, actN)); }
	break;
	case 44:
#line 249 "parse.y"
{ addEOFPat(RELEASE_ISET(YY_NUM_VAR(-3, ssLineN).ss), YY_NUM_VAR(-3, ssLineN).lineN, YY_NUM_VAR(-1, actN)); }
	break;
	case 45:
#line 252 "parse.y"
{ YY_NUM_VAR(0, pat)= NIL; }
	break;
	case 47:
#line 256 "parse.y"
{ YY_NUM_VAR(0, pat)= classPat(rawClassToClass(
				addRawClass(newRawClass(), '\n', '\n'))); 
                        }
	break;
	case 48:
#line 259 "parse.y"
{ YY_NUM_VAR(0, pat)= YY_NUM_VAR(-1, pat); }
	break;
	case 49:
#line 262 "parse.y"
{ YY_NUM_VAR(0, ssLineN).ss= BAD_ISET; YY_NUM_VAR(0, ssLineN).lineN= lineN(); }
	break;
	case 50:
#line 263 "parse.y"
{ YY_NUM_VAR(0, ssLineN).ss= YY_NUM_VAR(-2, ss); YY_NUM_VAR(0, ssLineN).lineN= YY_NUM_VAR(-3, lineN); }
	break;
	case 51:
#line 266 "parse.y"
{  YY_NUM_VAR(0, ss)= unaryISet(getStartState(YY_NUM_VAR(-1, id))); }
	break;
	case 52:
#line 267 "parse.y"
{  YY_NUM_VAR(0, ss)= addISet(YY_NUM_VAR(-3, ss), getStartState(YY_NUM_VAR(-1, id))); }
	break;
	case 53:
#line 270 "parse.y"
{ YY_NUM_VAR(0, pat)= altPat(YY_NUM_VAR(-3, pat), YY_NUM_VAR(-1, pat)); }
	break;
	case 55:
#line 274 "parse.y"
{ YY_NUM_VAR(0, pat)= catPat(YY_NUM_VAR(-2, pat), YY_NUM_VAR(-1, pat)); }
	break;
	case 57:
#line 278 "parse.y"
{ YY_NUM_VAR(0, pat)= repeatPat(YY_NUM_VAR(-2, pat), 0, NIL); }
	break;
	case 58:
#line 279 "parse.y"
{ YY_NUM_VAR(0, pat)= repeatPat(YY_NUM_VAR(-2, pat), 0, 1); }
	break;
	case 59:
#line 280 "parse.y"
{ YY_NUM_VAR(0, pat)= repeatPat(YY_NUM_VAR(-2, pat), 1, NIL); }
	break;
	case 60:
#line 281 "parse.y"
{ YY_NUM_VAR(0, pat)= repeatPat(YY_NUM_VAR(-2, pat), YY_NUM_VAR(-1, repeatRange).lo, YY_NUM_VAR(-1, repeatRange).hi); }
	break;
	case 62:
#line 285 "parse.y"
{ YY_NUM_VAR(0, pat)= YY_NUM_VAR(-2, pat); }
	break;
	case 63:
#line 286 "parse.y"
{ YY_NUM_VAR(0, pat)= classPat(dotClass()); }
	break;
	case 64:
#line 287 "parse.y"
{ YY_NUM_VAR(0, pat)= classPat(rawClassToClass(
                                        addRawClass(newRawClass(), YY_NUM_VAR(-1, ch), YY_NUM_VAR(-1, ch)))); 
                                }
	break;
	case 65:
#line 290 "parse.y"
{ YY_NUM_VAR(0, pat)= macroPat(YY_NUM_VAR(-1, id)); }
	break;
	case 66:
#line 291 "parse.y"
{ YY_NUM_VAR(0, pat)= classPat(rawClassToClass(YY_NUM_VAR(-2, rawClass))); }
	break;
	case 67:
#line 292 "parse.y"
{ YY_NUM_VAR(0, pat)= classPat(negClass(rawClassToClass(YY_NUM_VAR(-2, rawClass)))); }
	break;
	case 68:
#line 295 "parse.y"
{ YY_NUM_VAR(0, rawClass)= addRawClass(newRawClass(), 
						  YY_NUM_VAR(-1, charRange).lo, YY_NUM_VAR(-1, charRange).hi); 
                                }
	break;
	case 69:
#line 298 "parse.y"
{ YY_NUM_VAR(0, rawClass)= ctypeAddToRawClass(newRawClass(), YY_NUM_VAR(-1, id)); }
	break;
	case 70:
#line 299 "parse.y"
{ YY_NUM_VAR(0, rawClass)= addRawClass(YY_NUM_VAR(-2, rawClass), YY_NUM_VAR(-1, charRange).lo, YY_NUM_VAR(-1, charRange).hi); }
	break;
	case 71:
#line 300 "parse.y"
{ YY_NUM_VAR(0, rawClass)= ctypeAddToRawClass(YY_NUM_VAR(-2, rawClass), YY_NUM_VAR(-1, id)); }
	break;
	case 72:
#line 303 "parse.y"
{ YY_NUM_VAR(0, charRange).lo= YY_NUM_VAR(0, charRange).hi= YY_NUM_VAR(-1, ch); }
	break;
	case 73:
#line 304 "parse.y"
{ YY_NUM_VAR(0, charRange).lo= YY_NUM_VAR(-3, ch); YY_NUM_VAR(0, charRange).hi= YY_NUM_VAR(-1, ch); }
	break;
	case 74:
#line 308 "parse.y"
{ YY_NUM_VAR(0, id)= YY_NUM_VAR(-2, id); }
	break;
	case 75:
#line 311 "parse.y"
{ YY_NUM_VAR(0, repeatRange).lo= YY_NUM_VAR(0, repeatRange).hi= YY_NUM_VAR(-2, num); }
	break;
	case 76:
#line 312 "parse.y"
{ YY_NUM_VAR(0, repeatRange).lo= YY_NUM_VAR(-3, num); YY_NUM_VAR(0, repeatRange).hi= NIL; }
	break;
	case 77:
#line 313 "parse.y"
{ YY_NUM_VAR(0, repeatRange).lo= YY_NUM_VAR(-4, num); YY_NUM_VAR(0, repeatRange).hi= YY_NUM_VAR(-2, num); }
	break;
#line 1339 "parse.c"
	case YY_ERR_RULE:	/* "Rule" used to break to a parse error. */
	  goto yyError;
	default:		/* Perform default action. */
	  yyDefault= 1;		/* Set flag to indicate no semantic copy. */
	  break;
      }
      {	YYLen yyLen= yyLength[yyRuleN];	/* Length of reducing rule. */
      	YYNonTerm yyN= yyLHS[yyRuleN];	/* LHS nonTerm #. */
      	YYStk *yyReduceP= yySP - yyLen;	/* Point to stack entry for rule LHS. */
#if YY_HAS_LOC
	YY_UPDATE_LOC(yyLen, yySP->loc); /* Use yySP->loc as temp. entry. */
#endif
        /* Update semantic value and location info. for LHS stk. location. */
        if (yyLen > 0) { /* Stack location for rule LHS not at yySP. */
#if YY_HAS_LOC
	  yyReduceP->loc= yySP->loc; 	/* Copy tmp. loc. entry to loc. stk. */
#endif
	  if (!yyDefault) {		/* Semantic copy definitely required. */
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
    else if (yyTok == yyErrTok) { /* We need to pop current state. */
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

#line 316 "parse.y"


/* Parser initialization. */
VOID 
initParse()
{
  VOID_RET();
}

/* Syntax error routine. */
VOID 
yyerror(s)
  Char *s;
{
  ConstString p;
  for (p= yytext; isspace(*p); p++) ;
  if (*p == '\0') { /* yytext is empty or contains only whitespace. */
    error("parse error");
  }
  else {
    error("parse error before `%s'.", yytext);
  }
  VOID_RET();
}


#line 1425 "parse.c"
