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

