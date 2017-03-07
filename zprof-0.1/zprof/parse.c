/* Automatically generated from parse.y by zyacc version 1.03
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

#define YY_HAS_IN_ATTRIBS 1
#define YY_BAD_TOK 83
#define YY_EMPTY_TOK 84
#define YY_ERR_TOK 0
#define YY_ERR_TOK_VAL 256
#define YY_HAS_MULTI_START 0
#define YY_N_TESTS 2
#define YY_MAX_TOK_VAL 315
#define YY_MAX_RULE_LEN 9
#define YY_N_NON_TERMS 158
#define YY_N_TERMS 85
#define YY_N_RHS 922
#define YY_N_RULES 315
#define YY_N_TOKS 85
#define YY_BAD_BASE 7
#define YY_N_CHECK 3348
#define YY_N_NEXT 3331
#define YY_N_STATES 477
#define YY_TESTS_SIZE 18
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

#define AUTO_TOK 257
#define BREAK_TOK 258
#define CASE_TOK 259
#define CHAR_TOK 260
#define CONST_TOK 261
#define CONTINUE_TOK 262
#define DEFAULT_TOK 263
#define DO_TOK 264
#define DOUBLE_TOK 265
#define ELSE_TOK 266
#define ENUM_TOK 267
#define EXTERN_TOK 268
#define FLOAT_TOK 269
#define FOR_TOK 270
#define GOTO_TOK 271
#define IF_TOK 272
#define INT_TOK 273
#define LONG_TOK 274
#define REGISTER_TOK 275
#define RETURN_TOK 276
#define SHORT_TOK 277
#define SIGNED_TOK 278
#define SIZEOF_TOK 279
#define STATIC_TOK 280
#define STRUCT_TOK 281
#define SWITCH_TOK 282
#define TYPEDEF_TOK 283
#define UNION_TOK 284
#define UNSIGNED_TOK 285
#define VOID_TOK 286
#define VOLATILE_TOK 287
#define WHILE_TOK 288
#define ADD_ASSGN_TOK 289
#define AND_ASSGN_TOK 290
#define BOOL_AND_TOK 291
#define BOOL_OR_TOK 292
#define DEC_TOK 293
#define DEREF_TOK 294
#define DIV_ASSGN_TOK 295
#define DOT_DOT_TOK 296
#define EQ_TOK 297
#define GE_TOK 298
#define INC_TOK 299
#define LE_TOK 300
#define LSH_ASSGN_TOK 301
#define LSH_TOK 302
#define MOD_ASSGN_TOK 303
#define MULT_ASSGN_TOK 304
#define NE_TOK 305
#define OR_ASSGN_TOK 306
#define RSH_ASSGN_TOK 307
#define RSH_TOK 308
#define SUB_ASSGN_TOK 309
#define XOR_ASSGN_TOK 310
#define ID_TOK 311
#define NUM_TOK 312
#define STRING_TOK 313
#line 426 "parse.c"

/*	USER DEFINITIONS FROM SECTION 1 OF YACC FILE.			*/

#line 15 "parse.y"


#include "options.h"
#include "out.h"
#include "scan.h"

#include "area.h"
#include "error.h"
#include "hashtab.h"
#include "ids.h"
#include "parseopt.h"

#define YY_LEX scan

static void dclTypedef PROTO((Index id, BooleanX isTypedef));
static Boolean isTypedef PROTO((VOID_ARGS));
static VOID beginScope PROTO((VOID_ARGS));
static VOID endScope PROTO((VOID_ARGS));
static Int helpOptFn 
  PROTO((Int id, VOIDP checkP, VOIDP valP, ConstString argP));
static Int typedefOptFn 
  PROTO((Int id, VOIDP checkP, VOIDP valP, ConstString argP));
static Int versionOptFn 
  PROTO((Int id, VOIDP checkP, VOIDP valP, ConstString argP));


static struct {
  Boolean seenType;
} globs;
#define G globs

#define OUT(t)		putOut(&t)

enum {
  TYPEDEF_DCL,		/* declaration of a typedef */
  ID_DCL,		/* declaration of other ID in typedef namespace */
  OTHER_DCL,		/* declaration of other ID in other namespace */
};

#if YY_ZYACC_DEBUG

static YY_VOID tokSemFn PROTO((FILE *out, int tokNum, VOIDP p));

#define YY_SEM_FN tokSemFn

#endif /* if YY_ZYACC_DEBUG */


#line 479 "parse.c"

/*                      SEMANTIC TYPE DEFINITIONS.	               */

/* Definitions for:
 * YYSTYPE:	%union declaration + types of all terminals.
 * YYOut:	union of all synthesized attribute classes, including YYSTYPE. 
 * YYIn:	union of all inherited attribute classes.
 */

typedef union {
  struct {
    TokSem t;
  } tok;
} YYSTYPE;

typedef union {
  YYSTYPE yySType;
  struct {
    CounterType counterZ;
  } yy_compound_statement;
  struct {
    Index dclType;
  } yy_declaration_specifiers;
  struct {
    Index dclType;
  } yy_declaration_specifiers_x;
  struct {
    Index dclType;
  } yy_storage_class_specifier;
  struct {
    Index id;
  } yy_ID;
  struct {
    CounterType counterZ;
  } yy_statement;
  struct {
    CounterType counterZ;
  } yy_labeled_statement;
  struct {
    CounterType counterZ;
  } yy_selection_statement;
  struct {
    CounterType counterZ;
  } yy_iteration_statement;
  struct {
    CounterType counterZ;
  } yy_statement_list;
  struct {
    CounterType counterZ;
  } yy_needs_counter_statement;
} YYOut;
typedef struct {
  union {
    /* compound_statement.counter0 */
    CounterType yyT_0;
  } yyC_0;
  union {
    /* declarator.dclType init_declarator_list.dclType 
     * direct_declarator.dclType init_declarator.dclType 
     */
    Index yyT_1;
    /* statement.counter0 statement_list.counter0 counter_end.counter0 
     * counter_begin.counter0 
     */
    CounterType yyT_0;
  } yyC_1;
} YYIn;
#line 547 "parse.c"




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

#line 583 "parse.c"


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

static YY_CONST char *yyTermNames[85]= {
  "error", "<EOF>", "'auto'", "'break'", 
  "'case'", "'char'", "'const'", "'continue'", 
  "'default'", "'do'", "'double'", "'else'", 
  "'enum'", "'extern'", "'float'", "'for'", 
  "'goto'", "'if'", "'int'", "'long'", 
  "'register'", "'return'", "'short'", "'signed'", 
  "'sizeof'", "'static'", "'struct'", "'switch'", 
  "'typedef'", "'union'", "'unsigned'", "'void'", 
  "'volatile'", "'while'", "'+='", "'&='", 
  "'&&'", "'||'", "'--'", "'->'", 
  "'/='", "'...'", "'=='", "'>='", 
  "'++'", "'<='", "'<<='", "'<<'", 
  "'%='", "'*='", "'!='", "'|='", 
  "'>>='", "'>>'", "'-='", "'^='", 
  "';'", "'{'", "'}'", "'['", 
  "']'", "'('", "')'", "','", 
  "':'", "'='", "'*'", "'?'", 
  "'|'", "'^'", "'&'", "'<'", 
  "'>'", "'+'", "'-'", "'/'", 
  "'%'", "'~'", "'!'", "'.'", 
  "ID_TOK", "NUM_TOK", "STRING_TOK", "$yyBadTok", 
  "$yyEmptyTok"
};
static YY_CONST char *yyNonTermNames[158]= {
  "$S", "translation_unit", "external_declaration", "function_definition", 
  "declaration", "fnDeclarator", "compound_statement", "declaration_list", 
  "declaration_specifiers", "declarator", "SEMI", "init_declarator_list", 
  "declaration_specifiers_x", "storage_class_specifier", "type_specifier", "type_qualifier", 
  "TYPEDEF", "EXTERN", "STATIC", "AUTO", 
  "REGISTER", "CHAR", "SHORT", "INT", 
  "LONG", "SIGNED", "UNSIGNED", "FLOAT", 
  "DOUBLE", "VOID", "struct_or_union_specifier", "enum_specifier", 
  "typedef_name", "CONST", "VOLATILE", "struct_or_union", 
  "ID", "LBRACE", "struct_declaration_list", "RBRACE", 
  "STRUCT", "UNION", "struct_declaration", "init_declarator", 
  "COMMA", "EQ", "initializer", "specifier_qualifier_list", 
  "struct_declarator_list", "specifier_qualifier_list_x", "struct_declarator", "fieldDeclarator", 
  "COLON", "constant_expr", "ENUM", "enumerator_list", 
  "enumerator", "direct_declarator", "pointer", "LPAREN", 
  "RPAREN", "LBRACKET", "RBRACKET", "parameter_type_list", 
  "identifier_list", "STAR", "type_qualifier_list", "parameter_list", 
  "DOT_DOT_DOT", "parameter_declaration", "abstract_declarator", "assignment_expr", 
  "initializer_list", "type_name", "direct_abstract_declarator", "YYTest_1", 
  "statement", "counter_begin", "labeled_statement", "counter_end", 
  "expression_statement", "selection_statement", "iteration_statement", "jump_statement", 
  "CASE", "DEFAULT", "expr", "lbrace", 
  "rbrace", "statement_list", "IF", "needs_counter_statement", 
  "ELSE", "SWITCH", "WHILE", "DO", 
  "FOR", "GOTO", "CONTINUE", "BREAK", 
  "RETURN", "conditional_expr", "unary_expr", "assignment_operator", 
  "STAR_EQ", "SLASH_EQ", "PERCENT_EQ", "PLUS_EQ", 
  "MINUS_EQ", "LT_LT_EQ", "GT_GT_EQ", "AMPERSAND_EQ", 
  "CARAT_EQ", "BAR_EQ", "logical_or_expr", "QUESTION", 
  "logical_and_expr", "BAR_BAR", "inclusive_or_expr", "AMPERSAND_AMPERSAND", 
  "exclusive_or_expr", "BAR", "and_expr", "CARAT", 
  "equality_expr", "AMPERSAND", "relational_expr", "EQ_EQ", 
  "BANG_EQ", "shift_expr", "LT", "GT", 
  "LT_EQ", "GT_EQ", "additive_expr", "LT_LT", 
  "GT_GT", "multiplicative_expr", "PLUS", "MINUS", 
  "cast_expr", "SLASH", "PERCENT", "postfix_expr", 
  "PLUS_PLUS", "MINUS_MINUS", "unary_operator", "SIZEOF", 
  "TILDE", "BANG", "primary_expr", "argument_expr_list", 
  "DOT", "MINUS_GT", "constant", "STRING", 
  "NUM", "$Err"
};
static YY_CONST YYXRHS yyRHS[315]= {
      0,     3,     5,     8,    10,    12,    14,    17,    21,    25, 
     30,    32,    35,    39,    41,    44,    46,    48,    51,    53, 
     56,    58,    61,    63,    65,    67,    69,    71,    73,    75, 
     77,    79,    81,    83,    85,    87,    89,    91,    93,    95, 
     97,    99,   105,   110,   113,   115,   117,   119,   122,   124, 
    128,   130,   134,   138,   140,   142,   145,   147,   150,   152, 
    156,   158,   161,   165,   167,   172,   178,   181,   183,   187, 
    189,   193,   195,   198,   200,   204,   208,   213,   218,   222, 
    227,   229,   232,   235,   239,   241,   244,   246,   250,   252, 
    256,   259,   261,   264,   266,   270,   272,   276,   281,   283, 
    287,   289,   292,   294,   296,   299,   303,   306,   310,   314, 
    319,   322,   326,   330,   335,   338,   339,   343,   347,   349, 
    353,   357,   361,   365,   370,   374,   376,   379,   383,   387, 
    391,   396,   398,   400,   402,   405,   411,   419,   425,   427, 
    433,   441,   448,   456,   464,   473,   481,   490,   499,   509, 
    513,   516,   519,   522,   526,   528,   532,   534,   536,   540, 
    542,   544,   546,   548,   550,   552,   554,   556,   558,   560, 
    562,   564,   574,   576,   578,   582,   584,   588,   590,   594, 
    596,   600,   602,   606,   608,   612,   616,   618,   622,   626, 
    630,   634,   636,   640,   644,   646,   650,   654,   656,   660, 
    664,   668,   670,   675,   677,   680,   683,   686,   689,   694, 
    696,   698,   700,   702,   704,   706,   708,   713,   717,   722, 
    726,   730,   733,   736,   738,   740,   742,   746,   748,   752, 
    754,   756,   758,   760,   762,   764,   766,   768,   770,   772, 
    774,   776,   778,   780,   782,   784,   786,   788,   790,   792, 
    794,   796,   798,   800,   802,   804,   806,   808,   810,   812, 
    814,   816,   818,   820,   822,   824,   826,   828,   830,   832, 
    834,   836,   838,   840,   842,   844,   846,   848,   850,   852, 
    854,   856,   858,   860,   862,   864,   866,   868,   870,   872, 
    874,   876,   878,   880,   882,   884,   886,   888,   890,   892, 
    894,   896,   898,   901,   903,   905,   907,   909,   911,   913, 
    915,   917,   919,   920,   921
};
static YY_CONST YYXNonTerm yyLHS1[315]= {
      0,     1,     1,     2,     2,     2,     3,     3,     3,     3, 
      5,     4,     4,     7,     7,     8,    12,    12,    12,    12, 
     12,    12,    13,    13,    13,    13,    13,    14,    14,    14, 
     14,    14,    14,    14,    14,    14,    14,    14,    14,    15, 
     15,    30,    30,    30,    35,    35,    38,    38,    11,    11, 
     43,    43,    42,    47,    49,    49,    49,    49,    48,    48, 
     50,    50,    50,    51,    31,    31,    31,    55,    55,    56, 
     56,     9,     9,    57,    57,    57,    57,    57,    57,    57, 
     58,    58,    58,    58,    66,    66,    63,    63,    67,    67, 
     69,    69,    69,    64,    64,    46,    46,    46,    72,    72, 
     73,    73,    70,    70,    70,    74,    74,    74,    74,    74, 
     74,    74,    74,    74,    32,    75,    76,    76,    76,    76, 
     76,    76,    78,    78,    78,    80,    80,     6,     6,     6, 
      6,    87,    88,    89,    89,    81,    81,    81,    91,    82, 
     82,    82,    82,    82,    82,    82,    82,    82,    82,    83, 
     83,    83,    83,    83,    86,    86,    86,    71,    71,   103, 
    103,   103,   103,   103,   103,   103,   103,   103,   103,   103, 
    101,   101,    53,   114,   114,   116,   116,   118,   118,   120, 
    120,   122,   122,   124,   124,   124,   126,   126,   126,   126, 
    126,   129,   129,   129,   134,   134,   134,   137,   137,   137, 
    137,   140,   140,   102,   102,   102,   102,   102,   102,   146, 
    146,   146,   146,   146,   146,   143,   143,   143,   143,   143, 
    143,   143,   143,   150,   150,   150,   150,   151,   151,   154, 
    125,   119,   111,    19,   149,   128,   121,   117,   113,    99, 
    123,   112,    84,    21,    52,    44,    33,    98,    85,    95, 
    152,    68,    28,    92,    54,    45,   127,    17,    27,    96, 
     97,   131,   133,   136,   110,    36,    90,    23,    37,    61, 
     24,    59,   130,   132,   135,   109,   139,   108,   153,   145, 
    156,   142,   106,   138,   107,   144,   115,    39,    62,    20, 
    100,    60,    10,    22,    25,   147,   141,   105,    65,   104, 
     18,   155,   155,    40,    93,   148,    16,    41,    26,    29, 
     34,    94,    77,    79,   157
};
static YY_CONST YYXSym yySyms[922]= {
      5,     4,     2,     9,     6,     5,     9,    10,    13,    14, 
     17,    18,     0,    22,    21,    25,    26,    21,    29,    25, 
     30,    33,    37,    25,    34,    33,    37,    29,    25,    38, 
     37,    42,    33,    41,    46,    33,    45,    41,    50,    17, 
     54,    29,    17,    58,    49,    62,    53,    66,    53,    49, 
     70,    57,    74,    57,    49,    78,    61,    82,    61,    49, 
     86,    65,    90,    69,    94,    73,    98,    77,   102,    81, 
    106,    85,   110,    89,   114,    93,   118,    97,   122,   101, 
    126,   105,   130,   109,   134,   113,   138,   117,   142,   121, 
    146,   125,   150,   129,   154,   133,   158,   137,   162,   141, 
    145,   149,   153,   157,   166,   141,   149,   153,   157,   170, 
    141,   145,   174,   161,   178,   165,   182,   169,   186,   153, 
    169,   190,   173,   194,    45,   177,   173,   198,    37,   202, 
     37,   181,   185,   206,   189,   193,    41,   210,   197,   214, 
     57,   218,    57,   197,   222,    61,   226,    61,   197,   230, 
    201,   234,   193,   177,   201,   238,   205,   242,   209,   213, 
    246,   205,   209,   213,   250,    37,   254,   217,   149,   221, 
    157,   258,   217,   145,   149,   221,   157,   262,   217,   145, 
    266,   225,   270,   221,   177,   225,   274,   145,   278,   145, 
    181,   213,   282,   229,   286,   233,   229,   290,   145,   294, 
    237,    37,   241,   298,   229,   245,   249,   302,   229,   245, 
    213,   249,   306,   229,   237,   253,   241,   310,   229,   237, 
    241,   314,   229,   237,   257,   241,   318,   261,   322,   261, 
    265,   326,   261,   233,   330,   261,   265,   233,   334,    61, 
    338,   265,    61,   342,   269,   346,   269,   177,   273,   350, 
    277,   354,   269,   177,   277,   358,    33,    37,   362,    33, 
    366,    33,   281,   370,   145,   374,   257,   177,   145,   378, 
    285,   382,   149,   289,   157,   386,   149,   289,   177,   157, 
    390,   185,   394,   289,   177,   185,   398,   189,   402,   189, 
    281,   406,   233,   410,   297,   414,   233,   297,   418,   237, 
    281,   241,   422,   245,   249,   426,   245,   213,   249,   430, 
    297,   245,   249,   434,   297,   245,   213,   249,   438,   237, 
    241,   442,   237,   253,   241,   446,   297,   237,   241,   450, 
    297,   237,   253,   241,   454,   301,   145,   458,   462,   309, 
    313,   317,   466,   309,   321,   317,   470,    25,   474,   309, 
    325,   317,   478,   309,   329,   317,   482,   309,   333,   317, 
    486,   145,   209,   305,   490,   337,   213,   209,   305,   494, 
    341,   209,   305,   498,    41,   502,   345,    41,   506,   349, 
    309,   353,   510,   349,   357,   353,   514,   349,    29,   353, 
    518,   349,    29,   357,   353,   522,   149,   526,   157,   530, 
    305,   534,   357,   305,   538,   361,   237,   345,   241,   365, 
    542,   361,   237,   345,   241,   365,   369,   305,   546,   373, 
    237,   345,   241,   305,   550,   305,   554,   377,   237,   345, 
    241,   305,   558,   381,   305,   377,   237,   345,   241,    41, 
    562,   385,   237,    41,    41,   241,   305,   566,   385,   237, 
     41,    41,   345,   241,   305,   570,   385,   237,    41,   345, 
     41,   241,   305,   574,   385,   237,    41,   345,    41,   345, 
    241,   305,   578,   385,   237,   345,    41,    41,   241,   305, 
    582,   385,   237,   345,    41,    41,   345,   241,   305,   586, 
    385,   237,   345,    41,   345,    41,   241,   305,   590,   385, 
    237,   345,    41,   345,    41,   345,   241,   305,   594,   389, 
    145,    41,   598,   393,    41,   602,   397,    41,   606,   401, 
     41,   610,   401,   345,    41,   614,   285,   618,   345,   177, 
    285,   622,     0,   626,   405,   630,   409,   413,   285,   634, 
    181,   638,   417,   642,   421,   646,   425,   650,   429,   654, 
    433,   658,   437,   662,   441,   666,   445,   670,   449,   674, 
    453,   678,   457,   682,   457,   461,   309,   457,   317,   209, 
    309,   405,   317,   686,   405,   690,   465,   694,   457,   469, 
    465,   698,   473,   702,   465,   477,   473,   706,   481,   710, 
    473,   485,   481,   714,   489,   718,   481,   493,   489,   722, 
    497,   726,   489,   501,   497,   730,   505,   734,   497,   509, 
    505,   738,   497,   513,   505,   742,   517,   746,   505,   521, 
    517,   750,   505,   525,   517,   754,   505,   529,   517,   758, 
    505,   533,   517,   762,   537,   766,   517,   541,   537,   770, 
    517,   545,   537,   774,   549,   778,   537,   553,   549,   782, 
    537,   557,   549,   786,   561,   790,   549,   261,   561,   794, 
    549,   565,   561,   798,   549,   569,   561,   802,   409,   806, 
    237,   293,   241,   561,   810,   573,   814,   577,   409,   818, 
    581,   409,   822,   585,   561,   826,   589,   409,   830,   589, 
    237,   293,   241,   834,   501,   838,   261,   842,   553,   846, 
    557,   850,   593,   854,   597,   858,   601,   862,   573,   245, 
    345,   249,   866,   573,   237,   241,   870,   573,   237,   605, 
    241,   874,   573,   609,   145,   878,   573,   613,   145,   882, 
    573,   577,   886,   573,   581,   890,   145,   894,   617,   898, 
    621,   902,   237,   345,   241,   906,   285,   910,   605,   177, 
    285,   914,   625,   918,   280,   922,   144,   926,   140,   930, 
      8,   934,   312,   938,   200,   942,   272,   946,   148,   950, 
    204,   954,    12,   958,   276,   962,   220,   966,    16,   970, 
     20,   974,   256,   978,   252,   982,    24,   986,    28,   990, 
     32,   994,    36,   998,   316,  1002,   164,  1006,    40,  1010, 
     44,  1014,    48,  1018,   260,  1022,   168,  1026,    52,  1030, 
     56,  1034,    60,  1038,    64,  1042,   288,  1046,   172,  1050, 
    212,  1054,   208,  1058,   320,  1062,    68,  1066,    72,  1070, 
    228,  1074,   236,  1078,    76,  1082,   244,  1086,   284,  1090, 
    180,  1094,   188,  1098,   184,  1102,   296,  1106,   216,  1110, 
    156,  1114,   152,  1118,   324,  1122,   304,  1126,   192,  1130, 
    292,  1134,   136,  1138,   176,  1142,   268,  1146,   232,  1150, 
    240,  1154,    80,  1158,    84,  1162,   248,  1166,   224,  1170, 
     88,  1174,    92,  1178,    96,  1182,   300,  1186,   160,  1190, 
    264,  1194,   196,  1198,   100,  1202,   328,  1206,   621,   328, 
   1210,   104,  1214,   108,  1218,   308,  1222,   112,  1226,   116, 
   1230,   120,  1234,   124,  1238,   128,  1242,   132,  1246,  1250, 
   1254,  1258
};
static YY_CONST YYXSym yyAccess[477]= {
      1,     0,   112,    52,   100,     8,    80,    20,    88,    72, 
     76,    92,   120,    56,    40,   124,    24,   128,   320,   104, 
    116,    48,   244,   264,     5,     9,    13,    17,    21,    33, 
     37,    49,   229,   233,    53,    57,    61,    65,    69,    73, 
     77,    81,    85,    89,    93,    97,   101,   105,   109,   113, 
    117,   121,   125,   129,   133,   137,   141,   217,   301,   161, 
    165,   145,   237,   261,     4,     9,   228,    25,    29,    33, 
    349,    17,   149,   224,    37,    41,    45,   173,   236,   245, 
    237,   229,    49,    49,    49,   145,   149,   149,   145,   145, 
     37,   265,   233,    61,    25,    17,    37,   309,   357,    29, 
     25,   305,   260,    25,    29,   181,   252,    41,   177,   240, 
    280,   292,   296,   176,   152,    96,   308,   312,   328,   324, 
    249,   213,   405,   457,   573,   577,   581,   585,   589,   465, 
    473,   481,   489,   497,   505,   517,   537,   549,   561,   409, 
    237,   601,   501,   261,   553,   557,   593,   597,   145,   617, 
    621,   625,   248,   253,   241,   257,   269,   145,   277,    33, 
    149,   153,   169,   189,   197,    57,    61,   221,   225,   145, 
    149,   241,   233,    61,   232,    16,    32,     0,    68,   108, 
    132,    36,    60,    64,    28,    12,    84,   353,   313,   321, 
    325,   329,   333,   405,   409,   145,   337,   341,    41,   345, 
    361,   373,   377,   381,   385,   389,   393,   397,   401,   285, 
    157,   353,   305,   309,   353,   357,    25,   185,   285,   149, 
    173,   249,   268,   148,   461,   469,   316,   156,   245,   237, 
    609,   613,   577,   581,   409,   237,   409,   561,   409,   237, 
    144,   477,   272,   485,   276,   493,   501,   168,   200,   509, 
    513,   284,   288,   180,   172,   521,   525,   529,   533,   188, 
    212,   541,   545,   553,   557,   300,   304,   261,   565,   569, 
    293,   345,   189,   328,   241,   241,   177,   177,    37,   281, 
    233,   237,   297,   245,   153,   157,   169,   256,   193,   201, 
    205,   209,    37,   197,   197,   157,   177,   181,   221,   317, 
    317,   317,   317,   317,   196,   160,   192,   136,   216,   184, 
    208,   140,   220,   204,   413,   181,   417,   421,   425,   429, 
    433,   437,   441,   445,   449,   453,   209,   213,   209,    41, 
    177,   237,   237,   237,   305,   237,   145,    41,    41,    41, 
    345,   353,   289,   185,   309,   465,   345,   241,   605,   285, 
    145,   145,   293,   473,   481,   489,   497,   505,   505,   517, 
    517,   517,   517,   537,   537,   549,   549,   561,   561,   561, 
    241,   241,   281,   233,   237,   145,   164,   273,   277,   297, 
    281,   241,   253,   245,   237,   249,   213,   157,    41,   177, 
    209,   213,   225,   213,   157,   285,   305,   209,   305,   285, 
    345,   345,   345,   377,    41,   345,    41,    41,   157,   177, 
    457,   249,   241,   177,   241,   561,   241,   241,   249,   213, 
    241,   253,   249,   201,   213,   305,   241,   241,   241,   237, 
     41,   345,    41,   157,   185,   317,   285,   249,   241,   365, 
    305,   305,   305,   345,   241,   345,    41,    41,   345,   209, 
     44,   369,   241,   305,   241,   241,   345,   241,   345,    41, 
    309,   305,    41,   305,   305,   241,   305,   241,   241,   345, 
    405,   305,   305,   305,   241,   317,   305
};
static YY_CONST YYLineNum yyLineNums[315]= {
      0,   142,   143,   147,   148,   149,   153,   154,   156,   158, 
    162,   166,   167,   171,   172,   177,   181,   184,   189,   190, 
    192,   193,   198,   200,   202,   204,   206,   211,   213,   215, 
    217,   219,   221,   223,   225,   227,   229,   231,   233,   237, 
    238,   242,   243,   244,   248,   249,   253,   254,   258,   259, 
    263,   264,   268,   273,   277,   278,   279,   280,   284,   285, 
    289,   290,   291,   295,   299,   300,   301,   305,   306,   310, 
    311,   315,   316,   321,   323,   324,   325,   326,   327,   328, 
    332,   333,   334,   335,   339,   340,   344,   345,   349,   350, 
    355,   356,   357,   361,   362,   366,   367,   368,   372,   373, 
    377,   378,   382,   383,   384,   388,   389,   390,   391,   392, 
    393,   394,   395,   396,   401,   400,   413,   415,   417,   420, 
    423,   425,   429,   430,   431,   435,   436,   441,   442,   444, 
    447,   452,   457,   461,   463,   467,   470,   471,   475,   479, 
    480,   481,   482,   483,   484,   485,   486,   487,   489,   493, 
    494,   495,   496,   497,   504,   505,   506,   510,   511,   515, 
    516,   517,   518,   519,   520,   521,   522,   523,   524,   525, 
    529,   534,   538,   542,   543,   547,   548,   552,   553,   557, 
    558,   562,   563,   567,   568,   569,   573,   574,   575,   576, 
    577,   581,   582,   583,   587,   588,   589,   593,   594,   595, 
    596,   600,   601,   605,   606,   607,   608,   609,   610,   614, 
    615,   616,   617,   618,   619,   623,   624,   625,   626,   627, 
    628,   629,   630,   634,   635,   636,   637,   641,   642,   646, 
    654,   659,   664,   669,   674,   679,   684,   689,   694,   699, 
    704,   709,   714,   719,   724,   729,   734,   739,   744,   749, 
    754,   759,   764,   769,   774,   779,   784,   789,   794,   799, 
    804,   809,   814,   819,   824,   829,   838,   843,   848,   853, 
    858,   863,   868,   873,   878,   883,   888,   893,   898,   903, 
    908,   913,   918,   923,   928,   933,   938,   943,   948,   953, 
    958,   963,   968,   973,   978,   983,   988,   993,   998,  1003, 
   1008,  1013,  1015,  1020,  1025,  1030,  1035,  1040,  1045,  1050, 
   1055,  1060,  1066,  1071,  1072
};
#endif /* #if YY_ZYACC_DEBUG */

static YYTok yyTranslate[316]= {
      1,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    78,    83,    83,    83,    76,    70,    83, 
     61,    62,    66,    73,    63,    74,    79,    75,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    64,    56, 
     71,    65,    72,    67,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    59,    83,    60,    69,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    57,    68,    58,    77,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,    83,    83,    83,    83, 
     83,    83,    83,    83,    83,    83,     0,     2,     3,     4, 
      5,     6,     7,     8,     9,    10,    11,    12,    13,    14, 
     15,    16,    17,    18,    19,    20,    21,    22,    23,    24, 
     25,    26,    27,    28,    29,    30,    31,    32,    33,    34, 
     35,    36,    37,    38,    39,    40,    41,    42,    43,    44, 
     45,    46,    47,    48,    49,    50,    51,    52,    53,    54, 
     55,    80,    81,    82,    83,    84
};
static YYLen yyLength[315]= {
      2,     1,     2,     1,     1,     1,     2,     3,     3,     4, 
      1,     2,     3,     1,     2,     1,     1,     2,     1,     2, 
      1,     2,     1,     1,     1,     1,     1,     1,     1,     1, 
      1,     1,     1,     1,     1,     1,     1,     1,     1,     1, 
      1,     5,     4,     2,     1,     1,     1,     2,     1,     3, 
      1,     3,     3,     1,     1,     2,     1,     2,     1,     3, 
      1,     2,     3,     1,     4,     5,     2,     1,     3,     1, 
      3,     1,     2,     1,     3,     3,     4,     4,     3,     4, 
      1,     2,     2,     3,     1,     2,     1,     3,     1,     3, 
      2,     1,     2,     1,     3,     1,     3,     4,     1,     3, 
      1,     2,     1,     1,     2,     3,     2,     3,     3,     4, 
      2,     3,     3,     4,     2,     0,     3,     3,     1,     3, 
      3,     3,     3,     4,     3,     1,     2,     3,     3,     3, 
      4,     1,     1,     1,     2,     5,     7,     5,     1,     5, 
      7,     6,     7,     7,     8,     7,     8,     8,     9,     3, 
      2,     2,     2,     3,     1,     3,     1,     1,     3,     1, 
      1,     1,     1,     1,     1,     1,     1,     1,     1,     1, 
      1,     9,     1,     1,     3,     1,     3,     1,     3,     1, 
      3,     1,     3,     1,     3,     3,     1,     3,     3,     3, 
      3,     1,     3,     3,     1,     3,     3,     1,     3,     3, 
      3,     1,     4,     1,     2,     2,     2,     2,     4,     1, 
      1,     1,     1,     1,     1,     1,     4,     3,     4,     3, 
      3,     2,     2,     1,     1,     1,     3,     1,     3,     1, 
      1,     1,     1,     1,     1,     1,     1,     1,     1,     1, 
      1,     1,     1,     1,     1,     1,     1,     1,     1,     1, 
      1,     1,     1,     1,     1,     1,     1,     1,     1,     1, 
      1,     1,     1,     1,     1,     1,     1,     1,     1,     1, 
      1,     1,     1,     1,     1,     1,     1,     1,     1,     1, 
      1,     1,     1,     1,     1,     1,     1,     1,     1,     1, 
      1,     1,     1,     1,     1,     1,     1,     1,     1,     1, 
      1,     1,     2,     1,     1,     1,     1,     1,     1,     1, 
      1,     1,     0,     0,     0
};
static YYNonTerm yyLHS[315]= {
      0,     1,     1,     2,     2,     2,     3,     3,     3,     3, 
      5,     4,     4,     7,     7,     8,    12,    12,    12,    12, 
     12,    12,    13,    13,    13,    13,    13,    14,    14,    14, 
     14,    14,    14,    14,    14,    14,    14,    14,    14,    15, 
     15,    30,    30,    30,    35,    35,    38,    38,    11,    11, 
     43,    43,    42,    47,    49,    49,    49,    49,    48,    48, 
     50,    50,    50,    51,    31,    31,    31,    55,    55,    56, 
     56,     9,     9,    57,    57,    57,    57,    57,    57,    57, 
     58,    58,    58,    58,    66,    66,    63,    63,    67,    67, 
     69,    69,    69,    64,    64,    46,    46,    46,    72,    72, 
     73,    73,    70,    70,    70,    74,    74,    74,    74,    74, 
     74,    74,    74,    74,    32,    75,    76,    76,    76,    76, 
     76,    76,    78,    78,    78,    80,    80,     6,     6,     6, 
      6,    87,    88,    89,    89,    81,    81,    81,    91,    82, 
     82,    82,    82,    82,    82,    82,    82,    82,    82,    83, 
     83,    83,    83,    83,    86,    86,    86,    71,    71,   103, 
    103,   103,   103,   103,   103,   103,   103,   103,   103,   103, 
    101,   101,    53,   114,   114,   116,   116,   118,   118,   120, 
    120,   122,   122,   124,   124,   124,   126,   126,   126,   126, 
    126,   129,   129,   129,   134,   134,   134,   137,   137,   137, 
    137,   140,   140,   102,   102,   102,   102,   102,   102,   146, 
    146,   146,   146,   146,   146,   143,   143,   143,   143,   143, 
    143,   143,   143,   150,   150,   150,   150,   151,   151,   154, 
    125,   119,   111,    19,   149,   128,   121,   117,   113,    99, 
    123,   112,    84,    21,    52,    44,    33,    98,    85,    95, 
    152,    68,    28,    92,    54,    45,   127,    17,    27,    96, 
     97,   131,   133,   136,   110,    36,    90,    23,    37,    61, 
     24,    59,   130,   132,   135,   109,   139,   108,   153,   145, 
    156,   142,   106,   138,   107,   144,   115,    39,    62,    20, 
    100,    60,    10,    22,    25,   147,   141,   105,    65,   104, 
     18,   155,   155,    40,    93,   148,    16,    41,    26,    29, 
     34,    94,    77,    79,   157
};
static YYBase yyActBase[477]= {
   1043,     7,     7,     7,     7,     7,     7,     7,     7,     7, 
      7,     7,     7,     7,     7,     7,     7,     7,     7,     7, 
      7,     7,     7,     7,   998,     7,     7,     7,  1713,    62, 
      7,     7,  1088,     4,  1342,  1405,  1468,     7,     7,     7, 
      7,     7,     7,     7,     7,     7,     7,     7,     7,     7, 
      7,     7,     7,     7,     7,     7,    14,    14,    39,     7, 
      7,     7,   123,   139,     7,     7,     7,     7,  1713,    62, 
      0,     7,     7,     7,  1531,     7,    16,     7,     7,  1061, 
   1746,  1152,     7,     7,     7,  1216,  2033,    39,  1279,     7, 
     68,   222,     7,     7,     7,     7,   215,   739,   249,    83, 
      7,     7,     7,     7,  1784,  2034,     7,     7,   123,     7, 
      7,     7,     7,     7,     7,     7,     7,     7,     7,     7, 
      7,    78,     7,   151,    93,  2079,  2079,  2079,  2079,   977, 
    477,   722,   394,  2705,  2594,  2409,  2292,  2169,     7,     7, 
    328,     7,     7,     7,     7,     7,     7,     7,     7,     7, 
    147,     7,     7,    86,     7,    71,    73,     7,     7,    63, 
   2033,  2003,     7,   231,     7,  1877,  1940,     6,     7,   172, 
     70,     7,     7,     7,     7,     7,     7,     7,     7,     7, 
      7,     7,     7,     7,     7,     7,     7,     7,     7,     7, 
      7,     7,     7,     7,  2128,   230,  2079,    87,     7,    16, 
     94,    94,    94,   411,    94,    79,     3,     3,   853,     7, 
      7,     7,     7,   822,     7,   494,     7,     7,     7,  2034, 
      7,     7,     7,     7,     7,  2079,     7,     7,   915,  1560, 
     82,    82,     7,     7,     7,   915,     7,     7,     7,   573, 
      7,  2079,     7,  2079,     7,  2079,  2079,     7,     7,  2079, 
   2079,     7,     7,     7,     7,  2079,  2079,  2079,  2079,     7, 
      7,  2079,  2079,  2079,  2079,     7,     7,  2079,  2079,  2079, 
     96,    84,     1,     7,     7,     7,    88,  1817,     7,     7, 
    117,  1587,   133,  2061,  2003,     7,     7,     7,    16,     7, 
     12,  2079,     7,     7,     7,     7,    88,  2079,     6,     7, 
      7,     7,     7,     7,     7,     7,     7,     7,     7,     7, 
      7,     7,     7,     7,  2079,     7,     7,     7,     7,     7, 
      7,     7,     7,     7,     7,     7,   656,   103,   656,     7, 
   2079,   915,   915,   915,     2,   853,   156,     7,     7,     7, 
     16,     7,     6,     7,  2079,  1258,    57,     7,    84,     7, 
      7,     7,   154,   911,   805,   639,  2720,  2631,  2668,  2446, 
   2483,  2520,  2557,  2331,  2370,  2210,  2251,     7,     7,     7, 
   2079,     7,     7,   200,  1650,     7,     7,     7,     7,   237, 
    154,     7,   154,  2061,  1849,     7,   157,     7,     7,   231, 
   2079,     7,     7,     7,     7,     7,     7,   656,     7,     7, 
     84,    84,    84,   159,   853,    16,     7,     7,     7,  2016, 
   1130,     7,     7,  2079,     7,     7,     7,     7,     7,   161, 
      7,   163,     7,     7,     7,     7,   656,   656,   656,   915, 
    301,    16,   884,     7,     7,   167,     7,     7,     7,   166, 
      7,     7,     7,    84,   656,    84,   546,   546,    16,     7, 
      7,   656,   177,     7,   656,   656,    84,   656,    84,   546, 
   2079,     7,     7,     7,     7,   656,     7,   656,   656,    84, 
      7,     7,     7,     7,   656,     7,     7
};
static YYRuleN yyActDef[477]= {
    314,     5,   306,   257,   300,   233,   289,   243,   293,   267, 
    270,   294,   308,   258,   252,   309,   246,   310,   265,   303, 
    307,   254,   271,   298,   314,     1,     3,     4,   314,   314, 
     10,    15,    71,   314,    16,    18,    20,    22,    23,    24, 
     25,    26,    27,    28,    29,    30,    31,    32,    33,    34, 
     35,    36,    37,    38,    39,    40,   314,   314,   314,    44, 
     45,    73,   314,    80,     0,     2,   268,     6,   314,   314, 
    312,    13,   131,   292,    50,    11,   314,    48,   269,   314, 
    314,    72,    17,    19,    21,    43,   314,   314,    66,   114, 
    314,    81,    82,    84,     7,    14,    50,   314,   312,   312, 
    118,   133,   255,     8,   314,   314,   245,    12,   314,   288, 
    230,   283,   276,   285,   279,   295,   305,   234,   301,   280, 
     75,   314,   172,   170,   203,   314,   314,   314,   314,   173, 
    175,   177,   179,   181,   183,   186,   191,   194,   197,   201, 
    314,   215,   209,   210,   211,   212,   213,   214,   223,   224, 
    225,   229,   291,   314,    78,   314,    86,    93,    88,    91, 
    314,   314,    46,   314,    53,    54,    56,   314,    67,    69, 
    314,    74,    83,    85,   287,   242,   248,   156,   266,   304, 
    311,   249,   259,   260,   247,   239,   290,   127,   313,   313, 
    313,   313,   313,   157,   201,   223,   314,   314,   125,   314, 
    314,   314,   314,   312,   314,   314,   314,   314,   314,   154, 
    132,   128,   134,   314,   129,   312,     9,    51,    95,   314, 
     49,    76,   286,   237,   312,   314,   250,   278,   314,   314, 
    314,   314,   221,   222,   204,   314,   205,   206,   207,   314, 
    231,   314,   236,   314,   240,   314,   314,   256,   235,   314, 
    314,   272,   261,   273,   262,   314,   314,   314,   314,   274, 
    263,   314,   314,   314,   314,   296,   281,   314,   314,   314, 
    314,   314,   100,   302,    77,    79,   314,   314,    90,    92, 
    102,   314,   103,   314,   314,    42,    47,   244,   314,    58, 
     60,   314,    63,    55,    57,    64,   314,   314,   314,   116, 
    117,   119,   120,   121,   299,   297,   282,   284,   277,   275, 
    264,   232,   241,   238,   314,   159,   160,   161,   162,   163, 
    164,   165,   166,   167,   168,   169,   312,   314,   312,   126, 
    314,   314,   314,   314,   314,   314,   314,   150,   151,   152, 
    314,   130,   314,    98,   314,   174,   314,   217,   314,   227, 
    219,   220,   314,   176,   178,   180,   182,   184,   185,   187, 
    188,   189,   190,   192,   193,   195,   196,   198,   199,   200, 
    314,   226,   101,   102,   314,    94,   251,    87,    89,   104, 
    314,   110,   314,   314,   314,   106,   314,    41,    52,   314, 
    314,    61,    68,    70,    65,   158,   122,   312,   124,   155, 
    314,   314,   314,   314,   314,   314,   149,   153,    96,   314, 
    313,   216,   218,   314,   208,   202,   105,   111,   108,   314, 
    112,   314,   107,    59,    62,   123,   312,   312,   312,   314, 
    314,   314,   314,    97,    99,   314,   228,   109,   113,   135, 
    138,   137,   139,   314,   312,   314,   314,   314,   314,   312, 
    253,   312,   314,   141,   312,   312,   314,   312,   314,   314, 
    314,   136,   140,   142,   143,   312,   145,   312,   312,   314, 
    313,   144,   146,   147,   312,   171,   148
};
static YYBase yyGotoBase[158]= {
      7,     7,    11,     7,   614,     7,   325,   247,   312,  1288, 
   2603,     7,     5,     7,   641,  2714,     7,     7,     7,     7, 
      7,     7,     7,     7,     7,     7,     7,     7,     7,     7, 
      7,     7,     7,     7,     7,     7,  2801,   390,   174,   427, 
      7,     7,   181,   142,   564,   155,   136,   105,     7,   148, 
    116,     7,   190,   527,     7,   199,     8,   239,   227,  2791, 
   2674,   392,   358,   158,     7,  2821,     7,     7,     7,    31, 
     92,   438,     7,   176,    23,     7,  2467,   164,     7,   251, 
      7,     7,     7,     7,     7,     7,  2792,     7,   271,   262, 
      7,     7,     7,     7,     9,     7,     7,     7,     7,     7, 
      7,   338,  2871,     7,     7,     7,     7,     7,     7,     7, 
      7,     7,     7,     7,    17,     7,   112,     7,   107,     7, 
    113,     7,    90,     7,   134,   110,   127,     7,     7,   267, 
      7,     7,     7,     7,    59,     7,     7,   140,   119,   141, 
    506,     7,     7,     7,   241,   260,     7,     7,     7,     7, 
      7,     7,     7,     7,     7,     7,     7,     7
};
static YYState yyGotoDef[158]= {
      0,    24,    65,    26,    95,    28,   100,   104,    69,   292, 
    198,    76,    31,    34,    35,    36,    37,    38,    39,    40, 
     41,    42,    43,    44,    45,    46,    47,    48,    49,    50, 
     51,    52,    53,    54,    55,    56,   148,    72,   284,   210, 
     59,    60,   286,    77,   330,   105,   434,   163,   288,   164, 
    423,   290,   291,   424,    57,   298,   168,    32,    33,   140, 
    381,   283,   437,   382,   155,   143,    91,   156,   377,   158, 
    380,   209,   342,   352,   282,    58,   212,   213,   188,   475, 
    189,   190,   191,   192,   196,   197,   271,    70,   341,   215, 
    200,   439,   451,   201,   202,   203,   204,   205,   206,   207, 
    208,   193,   139,   314,   316,   317,   318,   319,   320,   321, 
    322,   323,   324,   325,   123,   224,   129,   225,   130,   241, 
    131,   243,   132,   245,   133,   142,   134,   249,   250,   135, 
    255,   256,   257,   258,   136,   261,   262,   137,   144,   145, 
    138,   268,   269,   124,   125,   126,   127,   128,   146,   147, 
    141,   348,   230,   231,   149,   150,   151,     0
};
static YYAct yyNext[3331]= {
    789,     0,     5,   789,   789,     7,    16,   789,   789,   789, 
     14,    25,    21,     3,    13,   789,   789,   789,     9,    10, 
      6,   789,     8,    11,   789,     4,    19,   789,     2,    20, 
     12,    15,    17,   789,     0,   180,     0,     0,   789,    82, 
     83,    84,     0,     0,   789,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,   789,    66,   789,    73, 
     78,   789,    22,   577,   174,    22,   789,    23,   537,   106, 
    789,    66,    73,   789,   789,   537,   287,   789,   789,   106, 
    802,   789,   789,   789,    18,     5,   789,   789,     7,    16, 
    789,   789,   789,    14,    18,    21,     3,    13,   789,   789, 
    789,     9,    10,     6,   789,     8,    11,   789,     4,    19, 
    789,     2,    20,    12,    15,    17,   789,   109,    73,    18, 
    106,   789,    78,    22,    22,   568,   568,   789,    23,    23, 
    152,   114,   227,   152,   106,   563,   106,   113,   109,   789, 
     66,   174,    18,    18,   789,    16,   152,   106,   152,   789, 
     18,   287,    78,   789,    22,    22,   789,   789,   152,    18, 
    789,   789,    18,   804,   789,   789,   612,   287,    18,   612, 
    612,    17,   226,   612,   612,   612,    78,   450,    22,   579, 
    579,   612,   612,   612,    22,   702,   702,   612,   223,    23, 
    612,   702,    78,   612,    22,   580,   580,    18,   557,   612, 
    557,   557,   557,    18,   612,    23,   702,   647,   702,   647, 
    612,   647,    73,   647,   647,   647,   152,   109,   222,   557, 
     22,   109,   612,   612,   612,   152,   702,   612,    16,   273, 
    546,   287,   612,    73,    97,   546,   612,   102,   153,   612, 
    612,   217,   246,   612,   612,   272,   612,   612,   612,   789, 
    220,   279,   789,   789,    17,   263,   789,   789,   789,    78, 
    161,    22,   579,   579,   789,   789,   789,   162,   700,   700, 
    789,   527,    81,   789,   700,    68,   789,   264,   527,   289, 
    102,   558,   789,   558,   558,   558,   167,   789,    23,   700, 
     92,   700,    22,   789,   287,   287,    78,    23,    22,   581, 
    581,   177,   558,   379,   392,   789,    66,   174,   378,   700, 
    789,    18,    29,   293,   294,   789,   270,    99,   172,   789, 
    363,   364,   789,   789,   297,   115,   789,   789,   177,   789, 
    789,   789,    98,     7,    16,   355,    29,   345,    14,   114, 
     21,   162,    13,   403,   272,   113,     9,    10,   353,   315, 
      8,    11,   115,    67,    19,   343,   354,    20,    12,    15, 
     17,   410,    22,   152,   372,   232,   114,    23,   187,   211, 
    214,   110,   113,     0,   111,   112,   357,   358,   116,   117, 
    356,    18,   119,   118,   233,   326,   280,   328,   344,    22, 
      0,     0,   159,    94,    23,     0,   379,     0,   110,   103, 
      0,   111,   112,   365,   366,   116,   117,     0,   792,   119, 
    118,   789,     0,     0,   789,   789,     0,   122,   789,   789, 
    789,     0,     0,     0,    79,     0,   789,   789,   789,   216, 
    656,   656,   789,     0,     0,   789,     0,   120,   789,   299, 
    300,   301,   302,   303,   789,     0,    86,    87,     0,   789, 
    656,     0,   656,     0,   656,   789,   656,   656,   656,     0, 
      0,   656,   656,   656,   110,   246,     0,   789,    66,   789, 
      0,     0,   789,    79,     0,   160,     0,   789,   170,   221, 
    390,   789,   263,   263,   789,   789,     0,     0,   789,   789, 
      0,   789,   789,   789,   789,   219,     0,   789,   789,   373, 
      0,   789,   789,   789,   264,   264,     0,     0,   280,   789, 
    789,   789,     0,   652,   652,   789,   228,   397,   789,    81, 
      0,   789,   359,   360,   361,   362,     0,   789,     0,     0, 
      0,     0,   789,   652,   122,   652,     0,   652,   789,   652, 
    652,   652,   421,   218,   652,   242,   177,     0,     0,     0, 
    789,    66,   174,     0,     0,   789,     0,     0,     0,     0, 
    789,     0,     0,     0,   789,     0,     0,   789,   789,     0, 
    115,   789,   789,   177,   789,   789,   789,     0,     7,    16, 
      0,     0,     0,    14,   114,    21,     0,    13,   285,   159, 
    113,     9,    10,   159,   295,     8,    11,   115,     0,    19, 
      0,   373,    20,    12,    15,    17,   121,    22,   152,   219, 
      0,   114,    23,   460,    27,     0,   110,   113,     0,   111, 
    112,   122,     0,   116,   117,   449,    18,   119,   118,   122, 
      0,     0,     0,   237,    22,   122,     0,     0,    27,    23, 
    108,   385,    71,   110,     0,     0,   111,   112,     0,     0, 
    116,   117,     0,   792,   119,   118,   789,   218,     0,   789, 
    789,   435,     0,   789,   789,   789,     0,   349,     0,     0, 
      0,   789,   789,   789,   383,   657,   657,   789,     0,     0, 
    789,     0,     0,   789,    71,     0,   159,     0,    71,   789, 
      0,     0,     0,     0,   789,   657,   159,   657,     0,   657, 
    789,   657,   657,   657,   411,     0,   657,   657,   657,   110, 
      0,   387,   789,    66,   789,     0,     0,   789,     0,   276, 
    277,   122,   789,   327,     0,   394,   789,   165,   122,   789, 
    789,   296,     0,   789,   789,     0,   789,   789,   789,   177, 
      0,   418,   185,   175,   422,     0,   184,   176,   181,     0, 
      0,     0,   395,     0,   182,   183,   178,     0,   654,   654, 
    186,     0,     0,   115,     0,     0,   179,     0,   399,   408, 
      0,   383,   180,   367,   368,   369,     0,   114,   654,     0, 
    654,   165,   654,   113,   654,   654,   654,     0,     0,   654, 
    654,   244,     0,     0,     0,    73,     0,   174,   470,   219, 
     22,   165,   165,     0,     0,    23,   165,   165,     0,   110, 
    386,     0,   111,   112,     0,     0,   116,   117,   391,    18, 
    119,   118,   177,     0,   393,   185,   175,     0,     0,   184, 
    176,   181,     0,     0,     0,     0,   433,   182,   183,   178, 
      0,   655,   655,   186,     0,     0,   115,   218,     0,   179, 
      0,   436,   389,   177,     0,   180,     0,     0,     0,     0, 
    114,   655,   296,   655,     0,   655,   113,   655,   655,   655, 
      0,     0,   655,   655,   244,     0,   415,   115,    73,     0, 
    165,     0,     0,    22,   177,     0,     0,     0,    23,     0, 
      0,   114,   110,     0,     0,   111,   112,   113,     0,   116, 
    117,     0,    18,   119,   118,     0,   409,     0,   115,    73, 
    419,     0,   413,     0,    22,   177,     0,     0,     0,    23, 
      0,     0,   114,   110,     0,   165,   111,   112,   113,     0, 
    116,   117,     0,    18,   119,   118,     0,     0,     0,   115, 
     73,     0,     0,     0,     0,    22,     0,   653,   653,     0, 
     23,     0,     0,   114,   110,     0,     0,   111,   112,   113, 
      0,   116,   117,     0,    18,   119,   118,   653,     0,   653, 
      0,   653,     0,   653,   653,   653,    22,     0,   653,   242, 
      0,    23,     0,     0,     0,   110,     0,     0,   111,   112, 
      0,     0,   116,   117,     0,    18,   119,   118,     1,    64, 
      5,     0,     0,     7,    16,     0,     0,     0,    14,     0, 
     21,     3,    13,   240,   650,     0,     9,    10,     6,     0, 
      8,    11,     0,     4,    19,     0,     2,    20,    12,    15, 
     17,     0,     0,   650,     0,   650,     0,   650,     0,   650, 
    650,   650,     0,     1,   650,     5,     0,     0,     7,    16, 
      0,     0,     0,    14,     0,    21,     3,    13,     0,    22, 
      0,     9,    10,     6,    23,     8,    11,     0,     4,    19, 
      0,     2,    20,    12,    15,    17,     0,     0,   792,     0, 
      0,     0,     0,     0,     0,   115,     0,     0,     0,     0, 
    548,     0,     0,   548,   548,     0,     0,     0,   548,   114, 
    548,   548,   548,     0,    22,   113,   548,   548,   548,    23, 
    548,   548,     0,   548,   548,     0,   548,   548,   548,   548, 
    548,   109,    22,   792,     0,     0,     0,    23,     0,     0, 
      0,   110,     0,     0,   111,   112,     0,     0,   116,   117, 
      0,    18,   119,   118,   548,   548,     0,    78,     0,    22, 
    548,   548,   548,   548,   549,     0,     0,   549,   549,     0, 
      0,     0,   549,     0,   549,   549,   549,   223,   548,     0, 
    549,   549,   549,     0,   549,   549,     0,   549,   549,     0, 
    549,   549,   549,   549,   549,     0,   790,     0,   790,     0, 
    790,     0,   790,   790,   790,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,   549,   549, 
      0,    78,     0,    22,   549,   549,   549,   549,   520,     0, 
      0,   520,   520,     0,     0,     0,   520,     0,   520,   520, 
    520,     0,   549,     0,   520,   520,   520,     0,   520,   520, 
      0,   520,   520,     0,   520,   520,   520,   520,   520,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,   520,    66,     0,   520,     0,   520,   520,   520, 
    520,   543,   520,     0,   543,   543,     0,     0,    30,   543, 
      0,   543,   543,   543,   240,   651,   520,   543,   543,   543, 
      0,   543,   543,     0,   543,   543,     0,   543,   543,   543, 
    543,   543,    30,     0,   651,     0,   651,    74,   651,     0, 
    651,   651,   651,     0,     0,   651,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,   543,    66,     0,   543,     0, 
    543,   543,   543,   543,     5,   543,     0,     7,    16,     0, 
     90,     0,    14,     0,    21,     3,    13,    96,     0,   543, 
      9,    10,     6,     0,     8,    11,     0,     4,    19,     0, 
      2,    20,    12,    15,    17,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,    96,     0,   493,     0, 
      0,   493,     0,   493,   493,   493,     0,     5,   493,     0, 
      7,    16,     0,     0,     0,    14,     0,    21,     3,    13, 
      0,     0,   796,     9,    10,     6,     0,     8,    11,     0, 
      4,    19,     0,     2,    20,    12,    15,    17,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,   278,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,   495,     0,     0,   495,     0,   495,   495,   495,     0, 
      5,   495,     0,     7,    16,     0,     0,     0,    14,     0, 
     21,     3,    13,     0,     0,   798,     9,    10,     6,     0, 
      8,    11,     0,     4,    19,     0,     2,    20,    12,    15, 
     17,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,   497,     0,     0,   497,     0,   497, 
    497,   497,     0,     5,   497,     0,     7,    16,     0,     0, 
      0,    14,     0,    21,     3,    13,     0,     0,   800,     9, 
     10,     6,     0,     8,    11,     0,     4,    19,     0,     2, 
     20,    12,    15,    17,     0,     0,     0,     0,     0,    90, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,   115,     0,     0,   527,    66,     5, 
      0,     0,     7,    16,   527,     0,   102,    14,   114,    21, 
      3,    13,     0,     0,   113,     9,    10,     6,     0,     8, 
     11,   794,     4,    19,     0,     2,    20,    12,    15,    17, 
      0,    22,   152,     0,     0,     0,    23,     0,     0,     0, 
    110,     0,     0,   111,   112,     0,     0,   116,   117,     0, 
     18,   119,   118,     0,     0,     0,    78,     0,    22,   152, 
      0,     0,     5,    23,     0,     7,    16,     0,     0,     0, 
     14,     0,    21,     3,    13,     0,     0,   792,     9,    10, 
      6,     0,     8,    11,     0,     4,    19,     0,     2,    20, 
     12,    15,    17,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,    78, 
      0,    22,   152,     0,     0,     5,    23,     0,     7,    16, 
      0,     0,     0,    14,     0,    21,     3,    13,     0,     0, 
    794,     9,    10,     6,     0,     8,    11,     0,     4,    19, 
      0,     2,    20,    12,    15,    17,     0,     0,     5,     0, 
      0,     7,    16,     0,     0,     0,    14,     0,    21,     3, 
     13,     0,     0,     0,     9,    10,     6,     0,     8,    11, 
     66,     4,    19,     0,     2,    20,    12,    15,    17,     0, 
      0,     0,     0,     0,     0,     0,     5,     0,     0,     7, 
     16,     0,     0,   794,    14,     0,    21,     3,    13,     0, 
      0,     0,     9,    10,     6,     0,     8,    11,   152,     4, 
     19,     0,     2,    20,    12,    15,    17,     0,     0,     5, 
      0,     0,     7,    16,     0,     0,   792,    14,     0,    21, 
      3,    13,     0,     0,     0,     9,    10,     6,     0,     8, 
     11,    66,     4,    19,     0,     2,    20,    12,    15,    17, 
      0,     5,     0,     0,     7,    16,     0,     0,   376,    14, 
      0,    21,     3,    13,   794,     0,     0,     9,    10,     6, 
      0,     8,    11,     0,     4,    19,     0,     2,    20,    12, 
     15,    17,     7,    16,     0,     0,     0,    14,     0,    21, 
      0,    13,     0,     0,     0,     9,    10,   794,     0,     8, 
     11,     0,     0,    19,     0,     0,    20,    12,    15,    17, 
      0,   152,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,   794, 
      0,     0,     0,     0,     0,     0,   531,     0,   531,   531, 
      0,   531,     0,   531,     0,     7,    16,     0,     0,     0, 
     14,     0,    21,     0,    13,     0,     0,   806,     9,    10, 
      0,     0,     8,    11,     0,     0,    19,     0,     0,    20, 
     12,    15,    17,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,   533, 
      0,   533,   533,     0,   533,     0,   533,     0,     7,    16, 
      0,     0,     0,    14,     0,    21,     0,    13,     0,     0, 
    808,     9,    10,     0,     0,     8,    11,     0,     0,    19, 
      0,     0,    20,    12,    15,    17,     0,     0,     7,    16, 
    115,     0,     0,    14,     0,    21,     0,    13,     0,     0, 
      0,     9,    10,     0,   114,     8,    11,     0,   115,    19, 
    113,   174,    20,    12,    15,    17,     0,     0,     0,     0, 
      0,     0,   114,    66,   174,     0,     0,    22,   113,     0, 
      0,     0,    23,   794,     0,   115,   110,     0,     0,   111, 
    112,    66,     0,   116,   117,    22,    18,   119,   118,   114, 
     23,     0,     0,   115,   110,   113,     0,   111,   112,     0, 
      0,   116,   117,   794,    18,   119,   118,   114,     0,     0, 
      0,   109,    22,   113,     0,     0,     0,    23,     0,     0, 
      0,   110,     0,     0,   111,   112,     0,     0,   116,   117, 
     22,    18,   119,   118,     0,    23,     0,     0,     0,   110, 
      0,     0,   111,   112,     0,     0,   116,   117,     0,    18, 
    119,   118,   307,   311,   678,   678,     0,     0,   305,     0, 
    678,   678,     0,   678,   309,   678,   306,   304,   678,   313, 
    310,   678,   308,   312,   678,     0,   678,     0,   678,     0, 
    678,   678,     0,   102,   678,   678,   678,   678,   678,   678, 
    678,   678,   678,   678,   678,   671,   671,     0,     0,     0, 
      0,   671,   671,     0,   671,     0,   671,     0,     0,   671, 
      0,     0,   671,     0,     0,   671,     0,   671,     0,   671, 
      0,   671,   671,   671,     0,    23,   671,   671,   671,   671, 
    671,   671,   671,   671,   265,   266,   672,   672,     0,     0, 
      0,     0,   672,   672,     0,   672,     0,   672,     0,     0, 
    672,     0,     0,   672,     0,     0,   672,     0,   672,     0, 
    672,     0,   672,   672,   672,     0,    23,   672,   672,   672, 
    672,   672,   672,   672,   672,   265,   266,   673,   673,     0, 
      0,     0,     0,   673,   673,     0,   673,     0,   673,     0, 
      0,   673,     0,     0,   673,     0,     0,   673,     0,   673, 
      0,   673,     0,   673,   673,   673,     0,    23,   673,   673, 
    673,   673,   673,   673,   673,   673,   265,   266,   668,   668, 
      0,     0,     0,     0,   668,   668,     0,   668,     0,   668, 
      0,     0,   668,     0,     0,   668,     0,     0,   668,     0, 
    668,     0,   668,     0,   668,   668,   668,     0,     0,   668, 
    668,   668,   668,   668,   668,   111,   112,   669,   669,     0, 
      0,     0,     0,   669,   669,     0,   669,     0,   669,     0, 
      0,   669,     0,     0,   669,     0,     0,   669,     0,   669, 
      0,   669,     0,   669,   669,   669,     0,     0,   669,   669, 
    669,   669,   669,   669,   111,   112,   670,   670,     0,     0, 
      0,     0,   670,   670,     0,   670,     0,   670,     0,     0, 
    670,     0,     0,   670,     0,     0,   670,     0,   670,     0, 
    670,     0,   670,   670,   670,     0,     0,   670,   670,   670, 
    670,   670,   670,   111,   112,   663,   663,     0,     0,     0, 
      0,   663,   663,     0,   663,     0,   259,     0,     0,   663, 
      0,     0,   260,     0,     0,   663,     0,   663,     0,   663, 
      0,   663,   663,   663,     0,     0,   663,   663,   663,   663, 
    663,   663,   664,   664,     0,     0,     0,     0,   664,   664, 
      0,   664,     0,   259,     0,     0,   664,     0,     0,   260, 
      0,     0,   664,     0,   664,     0,   664,     0,   664,   664, 
    664,     0,     0,   664,   664,   664,   664,   664,   664,   665, 
    665,     0,     0,     0,     0,   665,   665,     0,   665,     0, 
    259,     0,     0,   665,     0,     0,   260,   101,     0,   665, 
      0,   665,     0,   665,     0,   665,   665,   665,     0,     0, 
    665,   665,   665,   665,   665,   665,   666,   666,     0,     0, 
      0,     0,   666,   666,     0,   666,   101,   259,     0,     0, 
    666,     0,     0,   260,     0,     0,   666,     0,   666,     0, 
    666,     0,   666,   666,   666,     0,     0,   666,   666,   666, 
    666,   666,   666,   667,   667,     0,     0,     0,     0,   667, 
    667,     0,   667,     0,   259,     0,     0,   667,     0,     0, 
    260,     0,     0,   667,     0,   667,     0,   667,     0,   667, 
    667,   667,     0,     0,   667,   667,   667,   667,   667,   667, 
    660,   660,    75,     0,     0,     0,   660,   254,     0,   253, 
      0,     0,     0,     0,   660,     0,     0,     0,     0,     0, 
    660,     0,   660,     0,   660,     0,   660,   660,   660,     0, 
      0,   660,   660,   660,   660,   251,   252,   661,   661,     0, 
    334,     0,    75,   661,   254,     0,   253,     0,     0,   107, 
      0,   661,     0,     0,     0,     0,     0,   661,     0,   661, 
      0,   661,     0,   661,   661,   661,     0,     0,   661,   661, 
    661,   661,   251,   252,   662,   662,     0,     0,     0,     0, 
    662,   254,     0,   253,     0,     0,     0,     0,   662,     0, 
      0,     0,     0,     0,   662,     0,   662,     0,   662,     0, 
    662,   662,   662,     0,     0,   662,   662,   662,   662,   251, 
    252,   658,   658,     0,     0,     0,     0,   247,     0,     0, 
      0,     0,     0,     0,   154,   248,   659,   659,     0,     0, 
      0,   658,   247,   658,   171,   658,     0,   658,   658,   658, 
    248,     0,   658,   658,   658,   658,   659,    93,   659,     0, 
    659,     0,   659,   659,   659,     0,     0,   659,   659,   659, 
    659,    62,     0,   396,     0,   398,     0,     0,     0,     0, 
    166,    61,   329,     0,     0,   173,     0,     0,     0,   337, 
    338,   339,     0,     0,     0,    62,     0,     0,     0,     0, 
     62,    63,     0,    80,    62,    61,     0,   274,     0,   275, 
     61,     0,     0,     0,    61,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,    63,     0,     0,     0,     0, 
     63,     0,     0,    62,   166,     0,     0,    85,    88,    89, 
     62,     0,     0,    61,   425,     0,     0,     0,     0,     0, 
     61,     0,    80,     0,   166,   166,     0,     0,     0,   166, 
    166,   157,     0,    63,    63,     0,     0,     0,   169,   199, 
     63,   388,     0,   440,   441,   442,     0,     0,   195,    62, 
      0,     0,     0,   347,     0,     0,     0,     0,     0,    61, 
      0,   453,    63,     0,     0,   229,   235,   235,   461,   239, 
      0,   463,   464,     0,   466,     0,     0,     0,     0,    63, 
      0,     0,   471,     0,   472,   473,     0,     0,   404,   406, 
      0,   476,     0,   407,   370,   371,     0,     0,     0,     0, 
    281,     0,     0,   166,    62,     0,     0,     0,   267,     0, 
     61,     0,     0,     0,    61,     0,     0,     0,   194,     0, 
      0,   169,     0,     0,     0,     0,   194,     0,     0,     0, 
     63,     0,     0,     0,    63,     0,     0,     0,     0,     0, 
      0,   331,   332,   333,     0,   335,   234,   236,   166,   238, 
    340,     0,     0,     0,     0,   199,   336,   430,   432,     0, 
      0,   194,     0,     0,   195,     0,     0,     0,     0,     0, 
    346,     0,   412,     0,     0,     0,   414,     0,     0,     0, 
      0,   350,   351,     0,   446,   447,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,   459,     0,     0,   416,   462,   417,     0,   420,     0, 
      0,     0,     0,   374,     0,     0,     0,     0,     0,     0, 
      0,   281,   281,   384,   426,   427,   428,   375,     0,   194, 
      0,    61,    61,     0,   194,     0,     0,     0,     0,     0, 
    194,     0,     0,    63,     0,   438,     0,   169,     0,   194, 
    194,     0,    63,     0,   444,     0,   194,     0,     0,     0, 
    194,     0,     0,     0,     0,     0,     0,   452,     0,   454, 
    455,   457,     0,   400,   401,   402,     0,   405,     0,     0, 
    465,     0,   467,   468,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,   474,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,   374,   374,     0,     0,     0,     0, 
    384,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
     62,     0,     0,     0,     0,   194,   267,   267,     0,     0, 
     61,     0,     0,     0,   429,    63,   431,     0,     0,     0, 
      0,   194,   194,   194,   194,     0,   194,     0,     0,     0, 
     63,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,   443,   445,     0,   448,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,   456,   458, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,   469,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,   194,     0,     0,     0,     0, 
    194,     0,     0,     0,   194,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
    194,   194,     0,   194,     0,     0,     0,     0,     0,     0, 
      0,     0,     0,     0,     0,     0,     0,   194,   194,     0, 
      0,     0,     0,     0,     0,     0,     0,     0,     0,     0, 
    194
};
static YYCheck yyCheck[3348]= {
      0,   477,     2,     3,     4,     5,     6,     7,     8,     9, 
     10,     0,    12,    13,    14,    15,    16,    17,    18,    19, 
     20,    21,    22,    23,    24,    25,    26,    27,    28,    29, 
     30,    31,    32,    33,   477,    33,   477,   477,    38,    34, 
     35,    36,   477,   477,    44,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,    56,    57,    58,    56, 
     59,    61,    61,    62,    58,    61,    66,    66,    56,    63, 
     70,    57,    56,    73,    74,    63,    64,    77,    78,    63, 
     80,    81,    82,     0,    80,     2,     3,     4,     5,     6, 
      7,     8,     9,    10,    80,    12,    13,    14,    15,    16, 
     17,    18,    19,    20,    21,    22,    23,    24,    25,    26, 
     27,    28,    29,    30,    31,    32,    33,    60,    56,    80, 
     63,    38,    59,    61,    61,    62,    63,    44,    66,    66, 
     62,    38,    39,    62,    63,    62,    63,    44,    60,    56, 
     57,    58,    80,    80,    61,     6,    62,    63,    62,    66, 
     80,    64,    59,    70,    61,    61,    73,    74,    62,    80, 
     77,    78,    80,    80,    81,    82,     0,    64,    80,     3, 
      4,    32,    79,     7,     8,     9,    59,    11,    61,    62, 
     63,    15,    16,    17,    61,    38,    39,    21,    37,    66, 
     24,    44,    59,    27,    61,    62,    63,    80,    59,    33, 
     61,    62,    63,    80,    38,    66,    59,    56,    61,    58, 
     44,    60,    56,    62,    63,    64,    62,    60,    67,    80, 
     61,    60,    56,    57,    58,    62,    79,    61,     6,    82, 
     58,    64,    66,    56,    70,    63,    70,    65,    80,    73, 
     74,   105,   132,    77,    78,   140,    80,    81,    82,     0, 
    108,   159,     3,     4,    32,   136,     7,     8,     9,    59, 
     86,    61,    62,    63,    15,    16,    17,    86,    38,    39, 
     21,    56,    33,    24,    44,    28,    27,   136,    63,   163, 
     65,    59,    33,    61,    62,    63,    87,    38,    66,    59, 
     63,    61,    61,    44,    64,    64,    59,    66,    61,    62, 
     63,     0,    80,   280,   296,    56,    57,    58,   277,    79, 
     61,    80,     0,   165,   166,    66,   140,    70,    91,    70, 
    261,   262,    73,    74,   169,    24,    77,    78,     0,    80, 
     81,    82,    70,     5,     6,   245,    24,   225,    10,    38, 
     12,   160,    14,   334,   239,    44,    18,    19,   241,   194, 
     22,    23,    24,    28,    26,   219,   243,    29,    30,    31, 
     32,   344,    61,    62,   272,   124,    38,    66,    97,    98, 
     99,    70,    44,   477,    73,    74,   249,   250,    77,    78, 
    246,    80,    81,    82,   124,   195,   159,   197,   224,    61, 
    477,   477,    80,    68,    66,   477,   373,   477,    70,    74, 
    477,    73,    74,   263,   264,    77,    78,   477,    80,    81, 
     82,     0,   477,   477,     3,     4,   477,    79,     7,     8, 
      9,   477,   477,   477,    32,   477,    15,    16,    17,   104, 
     36,    37,    21,   477,   477,    24,   477,    79,    27,   188, 
    189,   190,   191,   192,    33,   477,    56,    57,   477,    38, 
     56,   477,    58,   477,    60,    44,    62,    63,    64,   477, 
    477,    67,    68,    69,    70,   355,   477,    56,    57,    58, 
    477,   477,    61,    81,   477,    85,   477,    66,    88,   121, 
    290,    70,   363,   364,    73,    74,   477,   477,    77,    78, 
    477,    80,    81,    82,     0,   105,   477,     3,     4,   272, 
    477,     7,     8,     9,   363,   364,   477,   477,   281,    15, 
     16,    17,   477,    36,    37,    21,   124,   327,    24,   280, 
    477,    27,   255,   256,   257,   258,   477,    33,   477,   477, 
    477,   477,    38,    56,   196,    58,   477,    60,    44,    62, 
     63,    64,   384,   105,    67,    68,     0,   477,   477,   477, 
     56,    57,    58,   477,   477,    61,   477,   477,   477,   477, 
     66,   477,   477,   477,    70,   477,   477,    73,    74,   477, 
     24,    77,    78,     0,    80,    81,    82,   477,     5,     6, 
    477,   477,   477,    10,    38,    12,   477,    14,   161,   277, 
     44,    18,    19,   281,   167,    22,    23,    24,   477,    26, 
    477,   374,    29,    30,    31,    32,    79,    61,    62,   219, 
    477,    38,    66,   449,     0,   477,    70,    44,   477,    73, 
     74,   283,   477,    77,    78,   435,    80,    81,    82,   291, 
    477,   477,   477,   127,    61,   297,   477,   477,    24,    66, 
     76,   283,    28,    70,   477,   477,    73,    74,   477,   477, 
     77,    78,   477,    80,    81,    82,     0,   219,   477,     3, 
      4,   410,   477,     7,     8,     9,   477,   229,   477,   477, 
    477,    15,    16,    17,   282,    36,    37,    21,   477,   477, 
     24,   477,   477,    27,    70,   477,   374,   477,    74,    33, 
    477,   477,   477,   477,    38,    56,   384,    58,   477,    60, 
     44,    62,    63,    64,   346,   477,    67,    68,    69,    70, 
    477,   284,    56,    57,    58,   477,   477,    61,   477,   155, 
    156,   383,    66,   196,   477,   298,    70,    86,   390,    73, 
     74,   167,   477,    77,    78,   477,    80,    81,    82,     0, 
    477,   383,     3,     4,   386,   477,     7,     8,     9,   477, 
    477,   477,   314,   477,    15,    16,    17,   477,    36,    37, 
     21,   477,   477,    24,   477,   477,    27,   477,   330,   342, 
    477,   379,    33,   267,   268,   269,   477,    38,    56,   477, 
     58,   140,    60,    44,    62,    63,    64,   477,   477,    67, 
     68,    69,   477,   477,   477,    56,   477,    58,   460,   409, 
     61,   160,   161,   477,   477,    66,   165,   166,   477,    70, 
    283,   477,    73,    74,   477,   477,    77,    78,   291,    80, 
     81,    82,     0,   477,   297,     3,     4,   477,   477,     7, 
      8,     9,   477,   477,   477,   477,   409,    15,    16,    17, 
    477,    36,    37,    21,   477,   477,    24,   409,   477,    27, 
    477,   413,   288,     0,   477,    33,   477,   477,   477,   477, 
     38,    56,   298,    58,   477,    60,    44,    62,    63,    64, 
    477,   477,    67,    68,    69,   477,   370,    24,    56,   477, 
    239,   477,   477,    61,     0,   477,   477,   477,    66,   477, 
    477,    38,    70,   477,   477,    73,    74,    44,   477,    77, 
     78,   477,    80,    81,    82,   477,   342,   477,    24,    56, 
    383,   477,   348,   477,    61,     0,   477,   477,   477,    66, 
    477,   477,    38,    70,   477,   284,    73,    74,    44,   477, 
     77,    78,   477,    80,    81,    82,   477,   477,   477,    24, 
     56,   477,   477,   477,   477,    61,   477,    36,    37,   477, 
     66,   477,   477,    38,    70,   477,   477,    73,    74,    44, 
    477,    77,    78,   477,    80,    81,    82,    56,   477,    58, 
    477,    60,   477,    62,    63,    64,    61,   477,    67,    68, 
    477,    66,   477,   477,   477,    70,   477,   477,    73,    74, 
    477,   477,    77,    78,   477,    80,    81,    82,     0,     1, 
      2,   477,   477,     5,     6,   477,   477,   477,    10,   477, 
     12,    13,    14,    36,    37,   477,    18,    19,    20,   477, 
     22,    23,   477,    25,    26,   477,    28,    29,    30,    31, 
     32,   477,   477,    56,   477,    58,   477,    60,   477,    62, 
     63,    64,   477,     0,    67,     2,   477,   477,     5,     6, 
    477,   477,   477,    10,   477,    12,    13,    14,   477,    61, 
    477,    18,    19,    20,    66,    22,    23,   477,    25,    26, 
    477,    28,    29,    30,    31,    32,   477,   477,    80,   477, 
    477,   477,   477,   477,   477,    24,   477,   477,   477,   477, 
      2,   477,   477,     5,     6,   477,   477,   477,    10,    38, 
     12,    13,    14,   477,    61,    44,    18,    19,    20,    66, 
     22,    23,   477,    25,    26,   477,    28,    29,    30,    31, 
     32,    60,    61,    80,   477,   477,   477,    66,   477,   477, 
    477,    70,   477,   477,    73,    74,   477,   477,    77,    78, 
    477,    80,    81,    82,    56,    57,   477,    59,   477,    61, 
     62,    63,    64,    65,     2,   477,   477,     5,     6,   477, 
    477,   477,    10,   477,    12,    13,    14,    37,    80,   477, 
     18,    19,    20,   477,    22,    23,   477,    25,    26,   477, 
     28,    29,    30,    31,    32,   477,    56,   477,    58,   477, 
     60,   477,    62,    63,    64,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,    56,    57, 
    477,    59,   477,    61,    62,    63,    64,    65,     2,   477, 
    477,     5,     6,   477,   477,   477,    10,   477,    12,    13, 
     14,   477,    80,   477,    18,    19,    20,   477,    22,    23, 
    477,    25,    26,   477,    28,    29,    30,    31,    32,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,    56,    57,   477,    59,   477,    61,    62,    63, 
     64,     2,    66,   477,     5,     6,   477,   477,     0,    10, 
    477,    12,    13,    14,    36,    37,    80,    18,    19,    20, 
    477,    22,    23,   477,    25,    26,   477,    28,    29,    30, 
     31,    32,    24,   477,    56,   477,    58,    29,    60,   477, 
     62,    63,    64,   477,   477,    67,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,    56,    57,   477,    59,   477, 
     61,    62,    63,    64,     2,    66,   477,     5,     6,   477, 
     62,   477,    10,   477,    12,    13,    14,    69,   477,    80, 
     18,    19,    20,   477,    22,    23,   477,    25,    26,   477, 
     28,    29,    30,    31,    32,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   108,   477,    56,   477, 
    477,    59,   477,    61,    62,    63,   477,     2,    66,   477, 
      5,     6,   477,   477,   477,    10,   477,    12,    13,    14, 
    477,   477,    80,    18,    19,    20,   477,    22,    23,   477, 
     25,    26,   477,    28,    29,    30,    31,    32,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   159,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,    56,   477,   477,    59,   477,    61,    62,    63,   477, 
      2,    66,   477,     5,     6,   477,   477,   477,    10,   477, 
     12,    13,    14,   477,   477,    80,    18,    19,    20,   477, 
     22,    23,   477,    25,    26,   477,    28,    29,    30,    31, 
     32,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,    56,   477,   477,    59,   477,    61, 
     62,    63,   477,     2,    66,   477,     5,     6,   477,   477, 
    477,    10,   477,    12,    13,    14,   477,   477,    80,    18, 
     19,    20,   477,    22,    23,   477,    25,    26,   477,    28, 
     29,    30,    31,    32,   477,   477,   477,   477,   477,   281, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,    24,   477,   477,    56,    57,     2, 
    477,   477,     5,     6,    63,   477,    65,    10,    38,    12, 
     13,    14,   477,   477,    44,    18,    19,    20,   477,    22, 
     23,    80,    25,    26,   477,    28,    29,    30,    31,    32, 
    477,    61,    62,   477,   477,   477,    66,   477,   477,   477, 
     70,   477,   477,    73,    74,   477,   477,    77,    78,   477, 
     80,    81,    82,   477,   477,   477,    59,   477,    61,    62, 
    477,   477,     2,    66,   477,     5,     6,   477,   477,   477, 
     10,   477,    12,    13,    14,   477,   477,    80,    18,    19, 
     20,   477,    22,    23,   477,    25,    26,   477,    28,    29, 
     30,    31,    32,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,    59, 
    477,    61,    62,   477,   477,     2,    66,   477,     5,     6, 
    477,   477,   477,    10,   477,    12,    13,    14,   477,   477, 
     80,    18,    19,    20,   477,    22,    23,   477,    25,    26, 
    477,    28,    29,    30,    31,    32,   477,   477,     2,   477, 
    477,     5,     6,   477,   477,   477,    10,   477,    12,    13, 
     14,   477,   477,   477,    18,    19,    20,   477,    22,    23, 
     57,    25,    26,   477,    28,    29,    30,    31,    32,   477, 
    477,   477,   477,   477,   477,   477,     2,   477,   477,     5, 
      6,   477,   477,    80,    10,   477,    12,    13,    14,   477, 
    477,   477,    18,    19,    20,   477,    22,    23,    62,    25, 
     26,   477,    28,    29,    30,    31,    32,   477,   477,     2, 
    477,   477,     5,     6,   477,   477,    80,    10,   477,    12, 
     13,    14,   477,   477,   477,    18,    19,    20,   477,    22, 
     23,    57,    25,    26,   477,    28,    29,    30,    31,    32, 
    477,     2,   477,   477,     5,     6,   477,   477,    41,    10, 
    477,    12,    13,    14,    80,   477,   477,    18,    19,    20, 
    477,    22,    23,   477,    25,    26,   477,    28,    29,    30, 
     31,    32,     5,     6,   477,   477,   477,    10,   477,    12, 
    477,    14,   477,   477,   477,    18,    19,    80,   477,    22, 
     23,   477,   477,    26,   477,   477,    29,    30,    31,    32, 
    477,    62,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,    80, 
    477,   477,   477,   477,   477,   477,    59,   477,    61,    62, 
    477,    64,   477,    66,   477,     5,     6,   477,   477,   477, 
     10,   477,    12,   477,    14,   477,   477,    80,    18,    19, 
    477,   477,    22,    23,   477,   477,    26,   477,   477,    29, 
     30,    31,    32,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,    59, 
    477,    61,    62,   477,    64,   477,    66,   477,     5,     6, 
    477,   477,   477,    10,   477,    12,   477,    14,   477,   477, 
     80,    18,    19,   477,   477,    22,    23,   477,   477,    26, 
    477,   477,    29,    30,    31,    32,   477,   477,     5,     6, 
     24,   477,   477,    10,   477,    12,   477,    14,   477,   477, 
    477,    18,    19,   477,    38,    22,    23,   477,    24,    26, 
     44,    58,    29,    30,    31,    32,   477,   477,   477,   477, 
    477,   477,    38,    57,    58,   477,   477,    61,    44,   477, 
    477,   477,    66,    80,   477,    24,    70,   477,   477,    73, 
     74,    57,   477,    77,    78,    61,    80,    81,    82,    38, 
     66,   477,   477,    24,    70,    44,   477,    73,    74,   477, 
    477,    77,    78,    80,    80,    81,    82,    38,   477,   477, 
    477,    60,    61,    44,   477,   477,   477,    66,   477,   477, 
    477,    70,   477,   477,    73,    74,   477,   477,    77,    78, 
     61,    80,    81,    82,   477,    66,   477,   477,   477,    70, 
    477,   477,    73,    74,   477,   477,    77,    78,   477,    80, 
     81,    82,    34,    35,    36,    37,   477,   477,    40,   477, 
     42,    43,   477,    45,    46,    47,    48,    49,    50,    51, 
     52,    53,    54,    55,    56,   477,    58,   477,    60,   477, 
     62,    63,   477,    65,    66,    67,    68,    69,    70,    71, 
     72,    73,    74,    75,    76,    36,    37,   477,   477,   477, 
    477,    42,    43,   477,    45,   477,    47,   477,   477,    50, 
    477,   477,    53,   477,   477,    56,   477,    58,   477,    60, 
    477,    62,    63,    64,   477,    66,    67,    68,    69,    70, 
     71,    72,    73,    74,    75,    76,    36,    37,   477,   477, 
    477,   477,    42,    43,   477,    45,   477,    47,   477,   477, 
     50,   477,   477,    53,   477,   477,    56,   477,    58,   477, 
     60,   477,    62,    63,    64,   477,    66,    67,    68,    69, 
     70,    71,    72,    73,    74,    75,    76,    36,    37,   477, 
    477,   477,   477,    42,    43,   477,    45,   477,    47,   477, 
    477,    50,   477,   477,    53,   477,   477,    56,   477,    58, 
    477,    60,   477,    62,    63,    64,   477,    66,    67,    68, 
     69,    70,    71,    72,    73,    74,    75,    76,    36,    37, 
    477,   477,   477,   477,    42,    43,   477,    45,   477,    47, 
    477,   477,    50,   477,   477,    53,   477,   477,    56,   477, 
     58,   477,    60,   477,    62,    63,    64,   477,   477,    67, 
     68,    69,    70,    71,    72,    73,    74,    36,    37,   477, 
    477,   477,   477,    42,    43,   477,    45,   477,    47,   477, 
    477,    50,   477,   477,    53,   477,   477,    56,   477,    58, 
    477,    60,   477,    62,    63,    64,   477,   477,    67,    68, 
     69,    70,    71,    72,    73,    74,    36,    37,   477,   477, 
    477,   477,    42,    43,   477,    45,   477,    47,   477,   477, 
     50,   477,   477,    53,   477,   477,    56,   477,    58,   477, 
     60,   477,    62,    63,    64,   477,   477,    67,    68,    69, 
     70,    71,    72,    73,    74,    36,    37,   477,   477,   477, 
    477,    42,    43,   477,    45,   477,    47,   477,   477,    50, 
    477,   477,    53,   477,   477,    56,   477,    58,   477,    60, 
    477,    62,    63,    64,   477,   477,    67,    68,    69,    70, 
     71,    72,    36,    37,   477,   477,   477,   477,    42,    43, 
    477,    45,   477,    47,   477,   477,    50,   477,   477,    53, 
    477,   477,    56,   477,    58,   477,    60,   477,    62,    63, 
     64,   477,   477,    67,    68,    69,    70,    71,    72,    36, 
     37,   477,   477,   477,   477,    42,    43,   477,    45,   477, 
     47,   477,   477,    50,   477,   477,    53,    70,   477,    56, 
    477,    58,   477,    60,   477,    62,    63,    64,   477,   477, 
     67,    68,    69,    70,    71,    72,    36,    37,   477,   477, 
    477,   477,    42,    43,   477,    45,    99,    47,   477,   477, 
     50,   477,   477,    53,   477,   477,    56,   477,    58,   477, 
     60,   477,    62,    63,    64,   477,   477,    67,    68,    69, 
     70,    71,    72,    36,    37,   477,   477,   477,   477,    42, 
     43,   477,    45,   477,    47,   477,   477,    50,   477,   477, 
     53,   477,   477,    56,   477,    58,   477,    60,   477,    62, 
     63,    64,   477,   477,    67,    68,    69,    70,    71,    72, 
     36,    37,    29,   477,   477,   477,    42,    43,   477,    45, 
    477,   477,   477,   477,    50,   477,   477,   477,   477,   477, 
     56,   477,    58,   477,    60,   477,    62,    63,    64,   477, 
    477,    67,    68,    69,    70,    71,    72,    36,    37,   477, 
    203,   477,    69,    42,    43,   477,    45,   477,   477,    76, 
    477,    50,   477,   477,   477,   477,   477,    56,   477,    58, 
    477,    60,   477,    62,    63,    64,   477,   477,    67,    68, 
     69,    70,    71,    72,    36,    37,   477,   477,   477,   477, 
     42,    43,   477,    45,   477,   477,   477,   477,    50,   477, 
    477,   477,   477,   477,    56,   477,    58,   477,    60,   477, 
     62,    63,    64,   477,   477,    67,    68,    69,    70,    71, 
     72,    36,    37,   477,   477,   477,   477,    42,   477,   477, 
    477,   477,   477,   477,    80,    50,    36,    37,   477,   477, 
    477,    56,    42,    58,    90,    60,   477,    62,    63,    64, 
     50,   477,    67,    68,    69,    70,    56,    63,    58,   477, 
     60,   477,    62,    63,    64,   477,   477,    67,    68,    69, 
     70,     0,   477,   326,   477,   328,   477,   477,   477,   477, 
     86,     0,   199,   477,   477,    91,   477,   477,   477,   206, 
    207,   208,   477,   477,   477,    24,   477,   477,   477,   477, 
     29,     0,   477,    32,    33,    24,   477,   153,   477,   155, 
     29,   477,   477,   477,    33,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,    24,   477,   477,   477,   477, 
     29,   477,   477,    62,   140,   477,   477,    56,    57,    58, 
     69,   477,   477,    62,   397,   477,   477,   477,   477,   477, 
     69,   477,    81,   477,   160,   161,   477,   477,   477,   165, 
    166,    80,   477,    62,    63,   477,   477,   477,    87,    97, 
     69,   288,   477,   426,   427,   428,   477,   477,    97,   108, 
    477,   477,   477,   229,   477,   477,   477,   477,   477,   108, 
    477,   444,    91,   477,   477,   124,   125,   126,   451,   128, 
    477,   454,   455,   477,   457,   477,   477,   477,   477,   108, 
    477,   477,   465,   477,   467,   468,   477,   477,   335,   336, 
    477,   474,   477,   340,   270,   271,   477,   477,   477,   477, 
    159,   477,   477,   239,   163,   477,   477,   477,   137,   477, 
    159,   477,   477,   477,   163,   477,   477,   477,    97,   477, 
    477,   170,   477,   477,   477,   477,   105,   477,   477,   477, 
    159,   477,   477,   477,   163,   477,   477,   477,   477,   477, 
    477,   200,   201,   202,   477,   204,   125,   126,   284,   128, 
    208,   477,   477,   477,   477,   213,   205,   404,   405,   477, 
    477,   140,   477,   477,   213,   477,   477,   477,   477,   477, 
    228,   477,   348,   477,   477,   477,   352,   477,   477,   477, 
    477,   230,   231,   477,   431,   432,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   448,   477,   477,   380,   452,   382,   477,   384,   477, 
    477,   477,   477,   272,   477,   477,   477,   477,   477,   477, 
    477,   280,   281,   282,   400,   401,   402,   276,   477,   208, 
    477,   280,   281,   477,   213,   477,   477,   477,   477,   477, 
    219,   477,   477,   272,   477,   421,   477,   296,   477,   228, 
    229,   477,   281,   477,   430,   477,   235,   477,   477,   477, 
    239,   477,   477,   477,   477,   477,   477,   443,   477,   445, 
    446,   447,   477,   331,   332,   333,   477,   335,   477,   477, 
    456,   477,   458,   459,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   469,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   373,   374,   477,   477,   477,   477, 
    379,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    389,   477,   477,   477,   477,   314,   365,   366,   477,   477, 
    389,   477,   477,   477,   403,   374,   404,   477,   477,   477, 
    477,   330,   331,   332,   333,   477,   335,   477,   477,   477, 
    389,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   429,   430,   477,   432,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   446,   447, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   459,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   404,   477,   477,   477,   477, 
    409,   477,   477,   477,   413,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    429,   430,   477,   432,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   446,   447,   477, 
    477,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    459,   477,   477,   477,   477,   477,   477,   477,   477,   477, 
    477,   477,   477,   477,   477,   477,   477,   477
};
static YYTest yyTests[18]= {
      1,     0,     1,     0,     1,     0,     1,     0,     1,     0, 
      1,     0,     1,     0,     1,     0,     1,     0
};
static YYAct yyTestActs[18]= {
    592,    18,   592,   791,   592,   493,   592,   495,   592,   497, 
    592,   789,   592,   789,   592,   531,   592,   533
};
#line 1856 "parse.c"

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
    yyDebugData.stkBase= *mainBase;
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
#line 2163 "parse.c"
  /* Confirm initial stack allocation ok. */
  if (!yyStk) {
    YY_ERROR("Out of memory.");	/* Initial stk. allocation failed. */
  }
#if YY_ZYACC_DEBUG
  yyDebugData.stkBase= yyStk;
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
	case 0:
YY_IN_VAR(-1, yyC_1.yyT_1)= ID_DCL;
	break;
	case 24:
YY_IN_VAR(-1, yyC_1.yyT_1)= ID_DCL;
	break;
	case 28:
YY_IN_VAR(-1, yyC_0.yyT_0)= FN_COUNTER;
	break;
	case 29:
YY_IN_VAR(-1, yyC_1.yyT_1)= YY_NON_TERM_VAR(-1, yy_declaration_specifiers.dclType);
	break;
	case 33:
YY_IN_VAR(-1, yyC_1.yyT_1)= YY_IN_VAR(-2, yyC_1.yyT_1);
	break;
	case 62:
YY_IN_VAR(-1, yyC_1.yyT_1)= YY_IN_VAR(-2, yyC_1.yyT_1);
	break;
	case 68:
YY_IN_VAR(-1, yyC_0.yyT_0)= FN_COUNTER;
	break;
	case 69:
YY_IN_VAR(-1, yyC_1.yyT_1)= YY_NON_TERM_VAR(-1, yy_declaration_specifiers.dclType);
	break;
	case 70:
YY_IN_VAR(-1, yyC_1.yyT_0)= YY_IN_VAR(-2, yyC_0.yyT_0);
YY_IN_VAR(-1, yyC_0.yyT_0)= (
YY_IN_VAR(-2, yyC_0.yyT_0)) == STMT_COUNTER ? COMPOUND_COUNTER : (
YY_IN_VAR(-2, yyC_0.yyT_0));
	break;
	case 74:
YY_IN_VAR(-1, yyC_0.yyT_0)= FN_COUNTER;
	break;
	case 98:
YY_IN_VAR(-1, yyC_1.yyT_0)= YY_NON_TERM_VAR(-1, yy_statement_list.counterZ);
YY_IN_VAR(-1, yyC_0.yyT_0)= (
YY_NON_TERM_VAR(-1, yy_statement_list.counterZ)) == STMT_COUNTER ? COMPOUND_COUNTER : (
YY_NON_TERM_VAR(-1, yy_statement_list.counterZ));
	break;
	case 99:
YY_IN_VAR(-1, yyC_1.yyT_0)= YY_IN_VAR(-3, yyC_0.yyT_0);
YY_IN_VAR(-1, yyC_0.yyT_0)= (
YY_IN_VAR(-3, yyC_0.yyT_0)) == STMT_COUNTER ? COMPOUND_COUNTER : (
YY_IN_VAR(-3, yyC_0.yyT_0));
	break;
	case 104:
YY_IN_VAR(-1, yyC_0.yyT_0)= FN_COUNTER;
	break;
	case 108:
YY_IN_VAR(-1, yyC_1.yyT_1)= YY_IN_VAR(-3, yyC_1.yyT_1);
	break;
	case 159:
YY_IN_VAR(-1, yyC_1.yyT_1)= YY_NON_TERM_VAR(-1, yy_declaration_specifiers.dclType);
	break;
	case 163:
YY_IN_VAR(-1, yyC_1.yyT_1)= OTHER_DCL;
	break;
	case 188:
YY_IN_VAR(-1, yyC_1.yyT_0)= YY_IN_VAR(-3, yyC_1.yyT_0);
	break;
	case 189:
YY_IN_VAR(-1, yyC_1.yyT_0)= YY_IN_VAR(-3, yyC_1.yyT_0);
	break;
	case 190:
YY_IN_VAR(-1, yyC_1.yyT_0)= YY_IN_VAR(-3, yyC_1.yyT_0);
	break;
	case 191:
YY_IN_VAR(-1, yyC_1.yyT_0)= YY_IN_VAR(-3, yyC_1.yyT_0);
	break;
	case 192:
YY_IN_VAR(-1, yyC_1.yyT_0)= YY_IN_VAR(-3, yyC_1.yyT_0);
	break;
	case 203:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 215:
YY_IN_VAR(-1, yyC_1.yyT_0)= YY_NON_TERM_VAR(-1, yy_statement_list.counterZ);
YY_IN_VAR(-1, yyC_0.yyT_0)= (
YY_NON_TERM_VAR(-1, yy_statement_list.counterZ)) == STMT_COUNTER ? COMPOUND_COUNTER : (
YY_NON_TERM_VAR(-1, yy_statement_list.counterZ));
	break;
	case 224:
YY_IN_VAR(-1, yyC_1.yyT_0)= COND_COUNTER;
	break;
	case 280:
YY_IN_VAR(-1, yyC_1.yyT_1)= YY_IN_VAR(-2, yyC_1.yyT_1);
	break;
	case 281:
YY_IN_VAR(-1, yyC_1.yyT_1)= YY_IN_VAR(-2, yyC_1.yyT_1);
	break;
	case 326:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 328:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 389:
YY_IN_VAR(-1, yyC_1.yyT_1)= OTHER_DCL;
	break;
	case 397:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 410:
YY_IN_VAR(-1, yyC_1.yyT_0)= COND_COUNTER;
	break;
	case 426:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 427:
YY_IN_VAR(-1, yyC_1.yyT_0)= NO_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
NO_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
NO_COUNTER);
	break;
	case 428:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 444:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 449:
YY_IN_VAR(-1, yyC_1.yyT_0)= COND_COUNTER;
	break;
	case 451:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 454:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 455:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 457:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 465:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 467:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 468:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
	case 470:
YY_IN_VAR(-1, yyC_1.yyT_0)= COND_COUNTER;
	break;
	case 474:
YY_IN_VAR(-1, yyC_1.yyT_0)= STMT_COUNTER;
YY_IN_VAR(-1, yyC_0.yyT_0)= (
STMT_COUNTER) == STMT_COUNTER ? COMPOUND_COUNTER : (
STMT_COUNTER);
	break;
#line 2386 "parse.c"
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
	  case 1:
	    yyResult= !G.seenType && isTypedef();
	  break;
#line 2458 "parse.c"
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
	case 15:
#define yyYYdclType YY_NON_TERM_VAR(-1, yy_declaration_specifiers_x.dclType)
#line 177 "parse.y"
{ G.seenType= FALSE; }
YY_NON_TERM_VAR(0, yy_declaration_specifiers.dclType)= yyYYdclType;
#undef yyYYdclType
	break;
	case 16:
#define yyYYdclType YY_NON_TERM_VAR(-1, yy_storage_class_specifier.dclType)
YY_NON_TERM_VAR(0, yy_declaration_specifiers_x.dclType)= yyYYdclType;
#undef yyYYdclType
	break;
	case 17:
#define yyYYdclType YY_NON_TERM_VAR(0, yy_declaration_specifiers_x.dclType)
#define yyYYdclType1 YY_NON_TERM_VAR(-2, yy_storage_class_specifier.dclType)
#define yyYYdclType2 YY_NON_TERM_VAR(-1, yy_declaration_specifiers_x.dclType)
#line 184 "parse.y"
{ yyYYdclType= (yyYYdclType1 == TYPEDEF_DCL || yyYYdclType2 == TYPEDEF_DCL)
                    ? TYPEDEF_DCL
                    : ID_DCL; 
	}
#undef yyYYdclType
#undef yyYYdclType1
#undef yyYYdclType2
	break;
	case 18:
#define yyYYdclType YY_NON_TERM_VAR(0, yy_declaration_specifiers_x.dclType)
#line 189 "parse.y"
{ yyYYdclType= ID_DCL; }
#undef yyYYdclType
	break;
	case 19:
#define yyYYdclType YY_NON_TERM_VAR(-1, yy_declaration_specifiers_x.dclType)
YY_NON_TERM_VAR(0, yy_declaration_specifiers_x.dclType)= yyYYdclType;
#undef yyYYdclType
	break;
	case 20:
#define yyYYdclType YY_NON_TERM_VAR(0, yy_declaration_specifiers_x.dclType)
#line 192 "parse.y"
{ yyYYdclType= ID_DCL; }
#undef yyYYdclType
	break;
	case 21:
#define yyYYdclType YY_NON_TERM_VAR(-1, yy_declaration_specifiers_x.dclType)
YY_NON_TERM_VAR(0, yy_declaration_specifiers_x.dclType)= yyYYdclType;
#undef yyYYdclType
	break;
	case 22:
#define yyYYdclType YY_NON_TERM_VAR(0, yy_storage_class_specifier.dclType)
#line 198 "parse.y"
{ yyYYdclType= TYPEDEF_DCL; }
#undef yyYYdclType
	break;
	case 23:
#define yyYYdclType YY_NON_TERM_VAR(0, yy_storage_class_specifier.dclType)
#line 200 "parse.y"
{ yyYYdclType= ID_DCL; }
#undef yyYYdclType
	break;
	case 24:
#define yyYYdclType YY_NON_TERM_VAR(0, yy_storage_class_specifier.dclType)
#line 202 "parse.y"
{ yyYYdclType= ID_DCL; }
#undef yyYYdclType
	break;
	case 25:
#define yyYYdclType YY_NON_TERM_VAR(0, yy_storage_class_specifier.dclType)
#line 204 "parse.y"
{ yyYYdclType= ID_DCL; }
#undef yyYYdclType
	break;
	case 26:
#define yyYYdclType YY_NON_TERM_VAR(0, yy_storage_class_specifier.dclType)
#line 206 "parse.y"
{ yyYYdclType= ID_DCL; }
#undef yyYYdclType
	break;
	case 27:
#line 211 "parse.y"
{ G.seenType= TRUE; }
	break;
	case 28:
#line 213 "parse.y"
{ G.seenType= TRUE; }
	break;
	case 29:
#line 215 "parse.y"
{ G.seenType= TRUE; }
	break;
	case 30:
#line 217 "parse.y"
{ G.seenType= TRUE; }
	break;
	case 31:
#line 219 "parse.y"
{ G.seenType= TRUE; }
	break;
	case 32:
#line 221 "parse.y"
{ G.seenType= TRUE; }
	break;
	case 33:
#line 223 "parse.y"
{ G.seenType= TRUE; }
	break;
	case 34:
#line 225 "parse.y"
{ G.seenType= TRUE; }
	break;
	case 35:
#line 227 "parse.y"
{ G.seenType= TRUE; }
	break;
	case 36:
#line 229 "parse.y"
{ G.seenType= TRUE; }
	break;
	case 37:
#line 231 "parse.y"
{ G.seenType= TRUE; }
	break;
	case 38:
#line 233 "parse.y"
{ G.seenType= TRUE; }
	break;
	case 53:
#line 273 "parse.y"
{ G.seenType= FALSE; }
	break;
	case 73:
#define yyYYdclType YY_IN_VAR(-2, yyC_1.yyT_1)
#define yyYYid YY_NON_TERM_VAR(-1, yy_ID.id)
#line 321 "parse.y"
{ if (yyYYdclType != OTHER_DCL) dclTypedef(yyYYid, yyYYdclType == TYPEDEF_DCL);
	}
#undef yyYYdclType
#undef yyYYid
	break;
	case 114:
#line 401 "parse.y"
{ 
#if  TEST_TYPEDEF
         printf("*"); 
#endif
       }
	break;
	case 116:
#define yyYYcounterZ YY_NON_TERM_VAR(-2, yy_labeled_statement.counterZ)
YY_NON_TERM_VAR(0, yy_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 117:
#define yyYYcounterZ YY_NON_TERM_VAR(0, yy_statement.counterZ)
#line 415 "parse.y"
{ yyYYcounterZ= NO_COUNTER; }
#undef yyYYcounterZ
	break;
	case 118:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_compound_statement.counterZ)
YY_NON_TERM_VAR(0, yy_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 119:
#define yyYYcounterZ YY_NON_TERM_VAR(0, yy_statement.counterZ)
#line 420 "parse.y"
{ yyYYcounterZ= STMT_COUNTER; }
#undef yyYYcounterZ
	break;
	case 120:
#define yyYYcounterZ YY_NON_TERM_VAR(0, yy_statement.counterZ)
#line 423 "parse.y"
{ yyYYcounterZ= STMT_COUNTER; }
#undef yyYYcounterZ
	break;
	case 121:
#define yyYYcounterZ YY_NON_TERM_VAR(0, yy_statement.counterZ)
#line 425 "parse.y"
{ yyYYcounterZ= NO_COUNTER; }
#undef yyYYcounterZ
	break;
	case 122:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_labeled_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 123:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_labeled_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 124:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_labeled_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 127:
#define yyYYcounterZ YY_NON_TERM_VAR(0, yy_compound_statement.counterZ)
#line 441 "parse.y"
{ yyYYcounterZ= NO_COUNTER; }
#undef yyYYcounterZ
	break;
	case 128:
#define yyYYcounterZ YY_NON_TERM_VAR(-2, yy_statement_list.counterZ)
YY_NON_TERM_VAR(0, yy_compound_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 129:
#define yyYYcounterZ YY_NON_TERM_VAR(0, yy_compound_statement.counterZ)
#line 444 "parse.y"
{ yyYYcounterZ= NO_COUNTER; }
#undef yyYYcounterZ
	break;
	case 130:
#define yyYYcounterZ YY_NON_TERM_VAR(-2, yy_statement_list.counterZ)
YY_NON_TERM_VAR(0, yy_compound_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 131:
#line 452 "parse.y"
{ beginScope(); }
	break;
	case 132:
#line 457 "parse.y"
{ endScope(); }
	break;
	case 133:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_statement_list.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 134:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_statement_list.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 135:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_needs_counter_statement.counterZ)
YY_NON_TERM_VAR(0, yy_selection_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 136:
#define yyYYcounterZ YY_NON_TERM_VAR(0, yy_selection_statement.counterZ)
#define yyYYcounterT YY_NON_TERM_VAR(-3, yy_needs_counter_statement.counterZ)
#define yyYYcounterE YY_NON_TERM_VAR(-1, yy_statement.counterZ)
#line 470 "parse.y"
{ yyYYcounterZ= (yyYYcounterT == NO_COUNTER) ? yyYYcounterE : yyYYcounterT; }
#undef yyYYcounterZ
#undef yyYYcounterT
#undef yyYYcounterE
	break;
	case 137:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_selection_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 138:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_needs_counter_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 139:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_iteration_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 140:
#define yyYYcounterZ YY_NON_TERM_VAR(-6, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_iteration_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 141:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_iteration_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 142:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_iteration_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 143:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_iteration_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 144:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_iteration_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 145:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_iteration_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 146:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_iteration_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 147:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_iteration_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 148:
#define yyYYcounterZ YY_NON_TERM_VAR(-1, yy_statement.counterZ)
YY_NON_TERM_VAR(0, yy_iteration_statement.counterZ)= yyYYcounterZ;
#undef yyYYcounterZ
	break;
	case 230:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 654 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 231:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 659 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 232:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 664 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 233:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 669 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 234:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 674 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 235:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 679 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 236:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 684 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 237:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 689 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 238:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 694 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 239:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 699 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 240:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 704 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 241:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 709 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 242:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 714 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 243:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 719 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 244:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 724 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 245:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 729 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 246:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 734 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 247:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 739 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 248:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 744 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 249:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 749 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 250:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 754 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 251:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 759 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 252:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 764 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 253:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 769 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 254:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 774 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 255:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 779 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 256:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 784 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 257:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 789 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 258:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 794 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 259:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 799 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 260:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 804 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 261:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 809 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 262:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 814 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 263:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 819 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 264:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 824 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 265:
#define yyYYid YY_NON_TERM_VAR(0, yy_ID.id)
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 829 "parse.y"
{ yyYYid= yyYYt.id; OUT(yyYYt); 
#if TEST_TYPEDEF
	  printf(" %s", getIDString(yyYYid)); 
#endif
        }
#undef yyYYid
#undef yyYYt
	break;
	case 266:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 838 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 267:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 843 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 268:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 848 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 269:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 853 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 270:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 858 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 271:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 863 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 272:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 868 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 273:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 873 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 274:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 878 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 275:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 883 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 276:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 888 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 277:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 893 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 278:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 898 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 279:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 903 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 280:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 908 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 281:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 913 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 282:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 918 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 283:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 923 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 284:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 928 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 285:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 933 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 286:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 938 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 287:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 943 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 288:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 948 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 289:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 953 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 290:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 958 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 291:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 963 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 292:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 968 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 293:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 973 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 294:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 978 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 295:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 983 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 296:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 988 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 297:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 993 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 298:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 998 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 299:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 1003 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 300:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 1008 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 301:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 1013 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 302:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 1015 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 303:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 1020 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 304:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 1025 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 305:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 1030 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 306:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 1035 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 307:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 1040 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 308:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 1045 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 309:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 1050 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 310:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 1055 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 311:
#define yyYYt YY_TERM_VAR(-1, tok.t)
#line 1060 "parse.y"
{ OUT(yyYYt); }
#undef yyYYt
	break;
	case 312:
#define yyYYcounter0 YY_IN_VAR(-1, yyC_1.yyT_0)
#line 1066 "parse.y"
{ beginCounter(yyYYcounter0); }
#undef yyYYcounter0
	break;
	case 313:
#define yyYYcounter0 YY_IN_VAR(-1, yyC_1.yyT_0)
#line 1071 "parse.y"
{ endCounter(yyYYcounter0); }
#undef yyYYcounter0
	break;
#line 3317 "parse.c"
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

#line 1075 "parse.y"



/*				IDENTIFIERS				*/

 
static HashTab hashTab; 
typedef enum {
  TYPEDEF_ID= 1,
  NON_TYPEDEF_ID= 2
} IDType;

static VOID
initIDs()
{
  enum {
    INIT_HASH_SIZE= 256,
    MAX_LOAD_FACTOR= 4
  };
  HashOpts opts;
  opts.codeFnP= NULL; opts.equalFnP= NULL;
  opts.initSize= INIT_HASH_SIZE; opts.maxLoadFactor= MAX_LOAD_FACTOR;
  opts.isScoped= TRUE; opts.reorderOK= FALSE;
  hashTab= newHashTab(&opts);
}

static VOID
terminateIDs()
{
  delHashTab(hashTab);
}


static VOID 
dclTypedef(id, isTypedef)
  Index id;
  BooleanX isTypedef;
{
  Index val= isTypedef ? TYPEDEF_ID : NON_TYPEDEF_ID;
  putHashTab(hashTab, (HashKey)id, (HashVal)val);
  VOID_RET();
}

static Boolean 
isTypedef()
{
  CONST Index id= yylval.tok.t.id;
  HashVal v= getHashTab(hashTab, (HashKey)id);
  return v != NULL_HASH_VAL && ((Index)v) == TYPEDEF_ID;
}

static VOID
beginScope()
{
  beginScopeHashTab(hashTab);
}

static VOID
endScope()
{
  endScopeHashTab(hashTab);
}


/*			     MAIN PROGRAM				*/

static Options options;
CONST Options *CONST optionsP= &options;

OptInfo optTab[]= {
  /* LongOpt Name, ShortOpt Name, Argument Type, User ID, CheckP, ValP,
   * OptFn, Doc
   */

  OPT_ENTRY("cond", 'c', NO_OPT_FLAG, COND_OPT,	NULL,
	     (VOIDP) &options.isCond, yesNoOptFn,
            "\
--cond | -c          Profile subexpressions of a ?: conditional expression\n\
                     (default: `0').\n\
"
  ),

  OPT_ENTRY("help", 'h', NO_OPT_FLAG, HELP_OPT,	NULL, NULL,
	     helpOptFn,
            "\
--help | -h          Print summary of options and exit.\n\
"
  ),

  OPT_ENTRY("output", 'o', REQUIRED_OPT_FLAG, OUTPUT_OPT, NULL,
	    (VOIDP) &options.output, stringOptFn,
    "\
--output=OUTFILE |   Use OUTFILE as the name of the generated output\n\
 -o OUTFILE          C file (default: append `_BB').\n\
"
  ),

  OPT_ENTRY("prefix", 'p', REQUIRED_OPT_FLAG, PREFIX_OPT, NULL,
	    (VOIDP) &options.prefix, stringOptFn,
    "\
--prefix=PREFIX |    Use PREFIX as prefix of all generated names in\n\
 -p PREFIX           instrumented file (default: `_BB').\n\
"
  ),

  OPT_ENTRY("preprocess", 'P', OPTIONAL_OPT_FLAG, PREPROCESS_OPT, NULL,
	    (VOIDP) &options.isPreprocess, yesNoOptFn,
    "\
--preprocess | -P    Run preprocessor (given by environmental var CPP) on\n\
                     input file (default: `1').\n\
"
  ),

  OPT_ENTRY("silent", 's', NO_OPT_FLAG, SILENT_OPT, NULL,
	    (VOIDP) &options.isSilent, yesNoOptFn,
    "\
--silent | -s        Do not generate error messages (default: `0').\n\
"
  ),

  OPT_ENTRY(NULL, 'V', SYNONYM_OPT_FLAG, VERSION_OPT, NULL,
	    NULL, NULL, NULL
  ),

  OPT_ENTRY("version", 'v', NO_OPT_FLAG, VERSION_OPT, NULL,
	    NULL, versionOptFn,
    "\
--version | -v | -V  Print version information and exit.\n\
"
  ),

  OPT_ENTRY("typedef", 't', REQUIRED_OPT_FLAG, TYPEDEF_OPT, NULL,
	    NULL, typedefOptFn,
    "\
--typedef=ID |       Declare identifier ID to be a typedef in the global\n\
 -t ID               scope.\n\
"
  )

};


static Int 
helpOptFn(id, checkP, valP, argP)
  Int id; 
  VOIDP checkP;
  VOIDP valP;
  ConstString argP;
{
  allOptsHelp(NULL, "Usage: zprof [options] lex-file.", 
	      optTab, N_ELEMENTS(optTab));
  exit(0);
  return 0;
}

static Int 
typedefOptFn(id, checkP, valP, argP)
  Int id;
  VOIDP checkP;
  VOIDP valP;
  ConstString argP;
{
  dclTypedef(getID(argP, strlen(argP)), TRUE);
  return 0;
}

#ifndef VERSION
#define VERSION	"0.0"
#endif

static Int 
versionOptFn(id, checkP, valP, argP)
  Int id; 
  VOIDP checkP;
  VOIDP valP;
  ConstString argP;
{
  fprintf(stderr, "zprof version %s\n", VERSION);
  exit(0);
  return 0;
}

#ifndef SIGNATURE
#define SIGNATURE "Zerksis"
#endif

/* Insert signature string into executable to permit distinguishing it
 * from other executables of the same name.
 */
char uniqueSignature[]= SIGNATURE;

/* A little function to ensure that above signature doesn't get optimized
 * away by optimizing linkers.
 */
int 
ensureSig()
{
  int n= sizeof(uniqueSignature);
  int i;
  for (i= 0; i < n; i++) uniqueSignature[i]= (char)(2*i);
  return uniqueSignature[2];
}


int 
main PROTO((int argc, ConstString argv[]));

int
main(argc, argv)
  int argc;
  ConstString argv[]; 
{
  int n;
#if YY_ZYACC_DEBUG
  yydebug= 1;
#endif
  ensureSig();
  initOut();
  initScan();
  initIDs();
  options.prefix= "_BB"; options.isPreprocess= 1;
  n= parseOpt(NULL, argc, argv, optTab, N_ELEMENTS(optTab));
  if (n != argc - 1) {
    allOptsHelp(NULL, "Usage: zprof [options] file.", 
	        optTab, N_ELEMENTS(optTab));
    exit(1);
  }
  newFile(argv[n]);
  beginOutFile(argv[n]);
  if (yyparse() != 0) {
    fatal("could not recover from parse errors.");
  }
  putOut(&yylval.tok.t);
  endOutFile();
  terminateIDs();
  terminateScan();
  terminateOut();
  return 0;
}

#if YY_ZYACC_DEBUG

static YY_VOID
tokSemFn(out, tokNum, p) 
  FILE *out;
  int tokNum;
  VOIDP p;
{
  enum { MAX_TOK= 10 };
  YYSTYPE *yylvalP= p;
  CONST int len0= yylvalP->tok.t.len - yylvalP->tok.t.len1;
  CONST int len= (len0 < MAX_TOK) ? len0 : MAX_TOK;
  fprintf(out, "%.*s", len, yylvalP->tok.t.text + yylvalP->tok.t.len1);
}

#endif

#line 3634 "parse.c"
