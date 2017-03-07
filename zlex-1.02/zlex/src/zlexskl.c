/* Skeleton file automatically produced by gm4 -P from zlexskl.cm4. */

#define ZLEX_SCANNER 1

#if 0
#ifndef NDEBUG	
#define NDEBUG
#endif
#endif

#ifdef DEBUG_ZLEX 	/* Define to check run-time assertions. */
#undef NDEBUG
#endif

#include <assert.h>

/*			    C LIBRARY ROUTINES.				*/

#if __STDC__
#include <limits.h>
#include <stdlib.h>
#else
#if 0 
/* It is safest to not declare these here.  Most compilers should do the
 * right thing after producing warnings about missing declarations.
 */
extern exit();
extern int read();
#endif /* if 0 */
#endif /* #if __STDC__ */


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



/* Define max. value YY_UINT_MAX if YY_UINT_MAX undef. */
#ifndef YY_UINT_MAX
#ifdef UINT_MAX	
#define YY_UINT_MAX UINT_MAX		/* From <limits.h> */
#else
#define YY_UINT_MAX 65535		/* A "reasonable" value. */
#endif
#endif



/* Define max. value YY_ULONG_MAX if YY_ULONG_MAX undef. */
#ifndef YY_ULONG_MAX
#ifdef ULONG_MAX	
#define YY_ULONG_MAX ULONG_MAX		/* From <limits.h> */
#else
#define YY_ULONG_MAX 4294967295		/* A "reasonable" value. */
#endif
#endif



#ifndef YY_SHRT_MAX
#ifdef SHRT_MAX
#define YY_SHRT_MAX	SHRT_MAX
#else
#define YY_SHRT_MAX	32767
#endif
#endif

typedef unsigned short YYUShrt;
typedef unsigned int YYUInt;
typedef unsigned long YYULong;
typedef short YYShrt;
typedef int YYInt;

/* 			SCANNER PARAMETERS.				*/

/* Possibilities for compression. */
#define YY_NO_COMPRESS		0
#define YY_COMB_COMPRESS	1
#define YY_ITERATIVE_COMPRESS	2

/* Possibilities for table entries. */
#define YY_ADDRESS_TABLE	0
#define YY_DIFF_TABLE		1
#define YY_STATE_TABLE		2

/*

#Define following parameters:

#Parameters set by outDFA module.
YY_MAX_BASE:		Max. entry in yyBase[] array.
YY_N_COLS:		# of columns in next[] when COMPRESS == NO_COMPRESS.
YY_N_NEXT:		# of entries in next[].

#Parameters set in dfa module.
YY_N_INITIAL_STATES:	# of initial states (2*YY_N_START_STATES if ^ patterns).
YY_N_INTRA_STATES:	# of states with intra-tok actions and alternate states.
YY_N_NON_FINAL_STATES:	# of non-accepting states in DFA.
YY_N_SIG_STATES:	# of states in DFA having non-jam transitions.
YY_N_STATES:		Total number of scanner states (includes sentinel
                        state, but not jam state if --backup-optimize; 
			does not include sentinel state or jam state or 
			pseudo sentinel	states or pseudo jam states if 
			!backup-optimize).
YY_SENTINEL_STATE:	# of sentinel state.

#Parameters set in chrclass module.
YY_16_BIT:		1 iff scanner for 16 bit characters.
YY_CLASS_SEG_BIT:	The bit # (0-origin, little endian) defining a char seg.
YY_N_EQ_CLASSES:	eq. classes. (==  YY_CHAR_SET_SIZE if no eq. classes).
YY_MAX_EQ_BASE:		Max base value if eq-compression; 0 if not.
YY_MAX_EQ_CHECK:	Max check value if eq-compression; 0 if not.

#Parameters set in main module.
YY_ALIGN:		1 if tables should use natural int size.
YY_ARRAY:		1 if yytext is array.
YY_BACKUP_OPTIMIZE:	1 if scanner is being optimized for backup.
YY_CHAR_SET_SIZE:	Size of char-set (128 or 256).
YY_COMPRESS:		Value of YY_NO_COMPRESS, YY_COMB_COMPRESS or 
			YY_ITERATIVE_COMPRESS.
YY_DO_CODE		1 if directly executable scanner.
YY_DO_DEBUG:		1 if debugging requested via cmd-line; 0 if not.
YY_DO_REJECT:		1 if REJECT actions enabled.
YY_DO_STDIO:		1 if <stdio.h> fread() to be used for input.
YY_SENTINEL_CHAR:	Character used as sentinel.
YY_DEFAULT_ACTION:	0: echo; 1: error; 2: fatal; 3: ignore.
YY_TABLE_ENTRY:		Value of YY_ADDRESS_TABLE, YY_DIFF_TABLE or
			YY_STATE_TABLE.
YY_ZLEX_MAJOR_VERSION:	Major version # of zlex which generated scanner.
YY_ZLEX_MINOR_VERSION:	Minor version # of zlex which generated scanner.

#Parameters set in patterns module.
YY_DEFAULT_ACTN:	Default action number. 
YY_HAS_VAR_CONTEXTS:	True if there are variable-right-context patterns.
YY_HAS_INTRA_TOK_PAT:	True if there are intra-token patterns.
YY_LINENO_ACTN:		Number for yylineno action (0 if yylineno not used).
YY_N_START_STATES:	# of start-states (>= 1).
YY_NUM_RULES:		# of rules (== largest action # since 1-origin).
YY_START_OF_LINE:	1 if source file uses `^' pattern; 0 otherwise.

#Parameters set in scan module.
YY_SRC_NAME:		Name of source file.

*/

/* Scanner parameters. */
/*!*/


/*			    ZLEX LIBRARY INTERFACE			*/


/*

File:	 libzlex.h
Purpose: External interface to zlex library.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _LIBZLEX_H
#define _LIBZLEX_H

#include <stdio.h>

/* These are definitions exported by the zlex library. */


#if __STDC__

#ifndef YY_CONST
#define YY_CONST		const
#endif

#ifndef YY_VOID
#define YY_VOID			void
#endif

#ifndef YY_VOIDP
#define YY_VOIDP		void *
#endif

#ifndef YY_VOID_ARGS
#define YY_VOID_ARGS		void
#endif

#ifndef YY_PROTO
#define YY_PROTO(p)		p
#endif

#else	/* !defined(__STDC__) || __STDC__ == 0. */

#ifndef YY_CONST
#define YY_CONST
#endif

#ifndef YY_VOID
#define YY_VOID
#endif

#ifndef YY_VOIDP
#define YY_VOIDP		char *
#endif

#ifndef YY_VOID_ARGS
#define YY_VOID_ARGS
#endif

#ifndef YY_PROTO
#define YY_PROTO(p)		()
#endif

#endif /* ifdef __STDC__ */

#if YY_16_BIT
typedef unsigned short YYChar;
#else
typedef char YYChar;
#endif
typedef unsigned YYCharX;	/* For parameter passing. */

typedef int YYState;	 	/* Type for state not stored in table. */

typedef unsigned char YYUChar;
typedef YYUChar YYBool;		/* 0 == FALSE; 1 == TRUE. */
typedef unsigned YYBoolX;	/* For parameter passing. */

typedef unsigned long yy_size_t;

#ifndef BUFSIZ
#define BUFSIZ	1024
#endif

#define YY_BUF_SIZE	(8 * BUFSIZ)

#define YY_NULL 0

typedef YY_VOIDP YYDataHandle;	/* Opaque handle to scanner state. */
typedef YY_VOIDP YYBufHandle;	/* Opaque handle to scanner buffer. */

/* 			LIBRARY FUNCTIONS				*/

#if YY_16_BIT
#define yyCreateBuffer		yyCreateBuffer16
#define yyDeleteBuffer		yyDeleteBuffer16
#define yyFlushBuffer		yyFlushBuffer16
#define yySwitchToBuffer	yySwitchToBuffer16
#define yyMemBuffer		yyMemBuffer16
#define yyBytesBuffer		yyBytesBuffer16
#define yyStringBuffer		yyStringBuffer16
#define yyGet			yyGet16
#define yyLess			yyLess16
#define yyMore			yyMore16
#define yyUnput			yyUnput16
#define yyRestart		yyRestart16
#define yyPopState		yyPopState16
#define yyPushState		yyPushState16
#define yyTopState		yyTopState16
#endif /* if YY_16_BIT */

/* Buffer routines. */
YYBufHandle yyCreateBuffer YY_PROTO ((YYDataHandle d, FILE *f, yy_size_t s));
YY_VOID yyDeleteBuffer YY_PROTO ((YYDataHandle d, YYBufHandle b));
YY_VOID yyFlushBuffer YY_PROTO ((YYDataHandle d, YYBufHandle bP));
YY_VOID yySwitchToBuffer YY_PROTO ((YYDataHandle d, YYBufHandle b));

/* Memory Buffer routines. */
YYBufHandle yyMemBuffer YY_PROTO ((YYDataHandle d, YYChar *buf, yy_size_t len));
YYBufHandle yyBytesBuffer YY_PROTO ((YYDataHandle d, 
				     YY_CONST YYChar *bytes, int len));
YYBufHandle yyStringBuffer YY_PROTO ((YYDataHandle d, YY_CONST YYChar *str));

/* Action routines. */
int yyGet YY_PROTO ((YYDataHandle d));
YY_VOID yyLess YY_PROTO ((YYDataHandle d, int n));
YY_VOID yyMore YY_PROTO ((YYDataHandle d));
YY_VOID yyUnput YY_PROTO ((YYDataHandle d, YYCharX ch));

/* Restart routine. */
YY_VOID yyRestart YY_PROTO ((YYDataHandle d, FILE *fp));

/* Start-state stack routines. */
YY_VOID yyPopState YY_PROTO ((YYDataHandle d));
YY_VOID yyPushState YY_PROTO ((YYDataHandle d, YYState newState));
YYState yyTopState YY_PROTO ((YYDataHandle d));

#endif /* ifndef _LIBZLEX_H */

/*

File:	 libzlexs.h
Purpose: Skeleton interface to zlex library.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _LIBZLEXS_H
#define _LIBZLEXS_H

/* These are the definitions required in the parser from the skeleton,
 * but are not part of the external library interface.  Assumes external
 * library declarations have been made.
 */

typedef enum {				/* Record which options are set. */
  YY_LINENO_FLAG= 1,			/* yylineno to be computed. */
  YY_ARRAY_FLAG= YY_LINENO_FLAG * 2,	/* yytext is an array. */
  YY_BACKUP_OPTIMIZE_FLAG= 		/* Stack used for backing up. */
    YY_ARRAY_FLAG * 2
} YYFlags;

enum {
  YY_INIT_SENTINEL= 0x01	/* Sentinel for initialization. */
};

typedef struct {		/* Start-state stack. */
  YYState *base;		/* Points to base of stack. */
  yy_size_t size;		/* # of allocated entries. */
  unsigned index;		/* Index of next free location. */
} YYSSStack;

typedef yy_size_t (*YYReadFnP) YY_PROTO ((YYChar *buf, yy_size_t nRead));
typedef int (*YYWrapFnP) YY_PROTO ((YY_VOID_ARGS));

typedef struct {	/* Buffer structure. */	
  FILE *yyin;		/* Associated file. NULL if in-memory buffer. 	*/
  /* Buffer parameters. */
  YYChar *buf;		/* Allocated area. */
  YYChar *end;		/* Point 1 character beyond last char. in buffer. */
			/* end - buf == # of chars currently in buf.	*/
  yy_size_t size;	/* Total size of allocated area. 		*/

  yy_size_t nRead;	/* Total # of characters read from file, not 	*/
                        /* counting what is currently in buffer.	*/
  /* (end - buf + nRead) == total # of chars. read from file.		*/


  yy_size_t intraPos;	/* buf position just after last intra-token pattern. */

  YYChar *cp;		/* Point to next char. to be scanned. 		*/

  /* The following four fields are used only if --yylineno. */
  yy_size_t lastLinePos;/* File position AFTER last completely scanned '\n'. */
  yy_size_t firstNLPos;	/* Position AFTER 1st '\n' in current scan. */

  /* The following field is invalid when this buffer is active. 	*/
  /* When the buffer is active, its value can be found in yylineno. 	*/
  int lineno;		/* 1 + # of '\n's completely scanned. */

  YYChar lastCh;	/* Last character read from this buffer.	*/

  YYBool hasNL;		/* Non-zero if current scan hits a new '\n'. */

  YYUChar eofState;	/* 0= nonEOF; 1= seen EOF; 2= completed EOF. 	*/

} YYBuf;

typedef YYBuf *YYBufP;

/* Package up all static global data for convenient passing around. */
typedef struct {

  /* Pointers to extern variables available to user. */
  FILE **YY_CONST yyinP;	/* Points to yyin */
  FILE **YY_CONST yyoutP;	/* Points to yyout. */
  FILE **YY_CONST yyerrP;	/* Points to yyerr. */
  YYChar **YY_CONST yytextP;	/* Points to yytext or yytext0. */
  int *YY_CONST yylengP;	/* Points to yyleng. */
  int *YY_CONST yylinenoP;	/* Points to yylineno. */
  YYBufP *YY_CONST yyCurrentBufP;/* Points to YY_CURRENT_BUF. */

  YYChar *YY_CONST textArray;	/* Address of yytext array if YY_HAS_ARRAY. */

  YY_CONST YYBufP bufP;		/* Address of static buffer cache. */

  /* Pointers to user-defined call-back functions. */
  YY_CONST YYReadFnP read;	/* Routine to read into buf. */
  YY_CONST YYWrapFnP yywrap;	/* Wrap-up on end-of-file. */

  YYChar *saveCP;		/* Points to &yytext[yyleng]. */

  /* Sentinel information. */
  YYChar *sentinelP;		/* Points to sentinel char. */
  YYChar sentinelChar;	       	/* Character used as sentinel. */
  YYChar sentinelSave;		/* Char. originally at *sentinelP. */
  YYChar saveCh;		/* Character to be restored at next scan.*/

  YYUChar flags;		/* Or of reasons for sentinel. */
  
  /* Options: YY_LINENO_FLAG|YY_ARRAY_FLAG|YY_BACKUP_OPTIMIZE_FLAG  */
  YYUChar options;		/* Flags giving some scanner options. */

  /* yymore information. */
  YYUChar doMore;		/* Nonzero if outstanding yymore request. */
  YYChar *moreText;		/* Saved yytext. */
  yy_size_t moreLeng;		/* Saved yyleng. */


  /* Variables to save yytext. Used after a buffer is deleted. */
  YYChar *yyTextSave;		/* Point to save area. */
  yy_size_t yyTextSaveSize;	/* Current size of save area. */

  /* Stack of states used for backtracking. */
  YYState *stk;			/* Points to base of stack. */
  unsigned stkSize;		/* Max. # of entries in stk. */

  /* Start states. */
  YYState startState;		/* Current start-state. */
  YYSSStack *ssStkP;		/* Start-condition stack struct. */

} YYData; /* End typedef struct for scanner data. */

#if YY_16_BIT
#define yyTerminate		yyTerminate16
#define yyProcessSentinel	yyProcessSentinel16
#define yyReplace		yyReplace16
#define yyMakeAdjText		yyMakeAdjText16
#define yyDebugMsg		yyDebugMsg16
#define yyDefaultMsg		yyDefaultMsg16
#define yyEOFMsg		yyEOFMsg16
#endif /* if YY_16_BIT */

YY_VOID yyTerminate YY_PROTO ((YYData *dP));
YY_VOID yyProcessSentinel YY_PROTO ((YYData *dP, unsigned nStk));

YYChar *yyReplace YY_PROTO ((YYData *dP, yy_size_t len, 
			     YY_CONST YYChar *string, 
			     YYChar **intraTextP));

YYChar *yyMakeAdjText YY_PROTO ((YYData *dP));

YY_VOID yyDebugMsg YY_PROTO ((YY_CONST YYData *dP, char *srcFile, int n, 
			     YYChar *yyText, int debugFlag));
YY_VOID yyDefaultMsg YY_PROTO ((YY_CONST YYData *dP, 
				YYChar *yytext, int debugFlag));
YY_VOID yyEOFMsg YY_PROTO ((YY_CONST YYData *dP, int debugFlag));
YY_VOID yyZlexFatal YY_PROTO ((FILE *yyerr, YY_CONST char *msg));

/* Invariant macro used for assertions. */
#define YY_INV(dP)							\
  ( 									\
    /* cp within current buffer. */					\
    ( (dP)->bufP->buf <= (dP)->bufP->cp &&				\
      (dP)->bufP->cp <= (dP)->bufP->end + 1				\
    ) &&								\
    /* saveCP points to &yytext[yyleng]. */				\
    ( (dP)->saveCP == *(dP)->yytextP + *(dP)->yylengP )			\
  )

#endif /* ifndef _LIBZLEXS_H */



/* 	MACROS AND TYPEDEFS DEPENDING ON SCANNER PARAMETERS.		*/

/* We don't use #error, since some K&R compilers choke on it. */

#if YY_BACKUP_OPTIMIZE
/* State to go to when there is no legal transition from current state. */
#define YY_JAM_STATE (YY_N_STATES)	/* Largest state number. */
#else
/* This is only used as a place-holder to indicate a jam in addition to
 * YY_N_STATES pseudo-jam states.
 */
#define YY_JAM_STATE		(2*YY_N_STATES + YY_N_SIG_STATES)
#endif

/* Maximum state # which may be stored in tables. */
#define YY_MAX_STATE		YY_JAM_STATE

/* If not state table, will need two extra check values. */
#if YY_TABLE_ENTRY == YY_STATE_TABLE

#define YY_X_CHECK	0

#else

#define YY_X_CHECK	2

/* Index for default state in comb vector relative to start of state. */
#define YY_DEF_INDEX	YY_N_EQ_CLASSES

/* Index for state number in comb vector relative to start of state. */
#define YY_STATE_INDEX	(YY_N_EQ_CLASSES + 1)

#endif

#if YY_ALIGN

typedef YYUInt YYXState;
typedef YYUInt YYAct;
typedef YYUInt YYBase;
typedef YYUInt YYCheck;
typedef YYInt  YYDisp;
typedef YYInt  YYDef;
typedef YYUInt YYClass;
typedef YYUInt YYEqBase;
typedef YYUInt YYEqCheck;

#else

/* Let preprocessor figure out smallest type which can hold a state.
 * Used for holding states in tables.
 */

/* typedef YYXState to represent values in 0, ..., (YY_MAX_STATE). */
#if ((YY_MAX_STATE) <= YY_UCHAR_MAX)
typedef YYUChar YYXState;
#elif ((YY_MAX_STATE) <= YY_USHRT_MAX)
typedef YYUShrt YYXState;
#elif ((YY_MAX_STATE) <= YY_UINT_MAX)
typedef YYUInt YYXState;
#elif ((YY_MAX_STATE) <= YY_ULONG_MAX)
typedef YYULong YYXState;
#else
  #error Type YYXState cannot be represented.
#endif


/* Let preprocessor figure out type for YYAct. */

/* typedef YYAct to represent values in 0, ..., YY_NUM_RULES. */
#if (YY_NUM_RULES <= YY_UCHAR_MAX)
typedef YYUChar YYAct;
#elif (YY_NUM_RULES <= YY_USHRT_MAX)
typedef YYUShrt YYAct;
#elif (YY_NUM_RULES <= YY_UINT_MAX)
typedef YYUInt YYAct;
#elif (YY_NUM_RULES <= YY_ULONG_MAX)
typedef YYULong YYAct;
#else
  #error Type YYAct cannot be represented.
#endif


/* Let preprocessor figure out type for base[].	*/

/* typedef YYBase to represent values in 0, ..., YY_MAX_BASE. */
#if (YY_MAX_BASE <= YY_UCHAR_MAX)
typedef YYUChar YYBase;
#elif (YY_MAX_BASE <= YY_USHRT_MAX)
typedef YYUShrt YYBase;
#elif (YY_MAX_BASE <= YY_UINT_MAX)
typedef YYUInt YYBase;
#elif (YY_MAX_BASE <= YY_ULONG_MAX)
typedef YYULong YYBase;
#else
  #error Type YYBase cannot be represented.
#endif


/* Let preprocessor figure out type for check[]. */

/* typedef YYCheck to represent values in 0, ..., YY_N_EQ_CLASSES + YY_X_CHECK. */
#if (YY_N_EQ_CLASSES + YY_X_CHECK <= YY_UCHAR_MAX)
typedef YYUChar YYCheck;
#elif (YY_N_EQ_CLASSES + YY_X_CHECK <= YY_USHRT_MAX)
typedef YYUShrt YYCheck;
#elif (YY_N_EQ_CLASSES + YY_X_CHECK <= YY_UINT_MAX)
typedef YYUInt YYCheck;
#elif (YY_N_EQ_CLASSES + YY_X_CHECK <= YY_ULONG_MAX)
typedef YYULong YYCheck;
#else
  #error Type YYCheck cannot be represented.
#endif


/* Maximum value held in a defaults arrays. */
#if YY_COMPRESS == YY_ITERATIVE_COMPRESS
#define YY_MAX_DEF	(2 * YY_MAX_STATE)
#else
#define YY_MAX_DEF	(YY_MAX_STATE)
#endif
/* Let preprocessor figure out type for def[] array. */

/* typedef YYDef to represent values in 0, ..., YY_MAX_DEF. */
#if (YY_MAX_DEF <= YY_UCHAR_MAX)
typedef YYUChar YYDef;
#elif (YY_MAX_DEF <= YY_USHRT_MAX)
typedef YYUShrt YYDef;
#elif (YY_MAX_DEF <= YY_UINT_MAX)
typedef YYUInt YYDef;
#elif (YY_MAX_DEF <= YY_ULONG_MAX)
typedef YYULong YYDef;
#else
  #error Type YYDef cannot be represented.
#endif


/* Let preprocessor figure out type for next displacement used when
 * YY_TABLE_ENTRY is YY_DIFF_TABLE.  This is approximate as we 
 * assume that a short is 2 bytes.
 */
#if YY_TABLE_ENTRY == YY_DIFF_TABLE
#if YY_BACKUP_OPTIMIZE
#define YY_DUMMY_SIZE		(YY_N_STATES - YY_N_SIG_STATES)
#else 
#define YY_DUMMY_SIZE							\
  (YY_N_STATES - YY_N_SIG_STATES + YY_N_STATES + YY_N_SIG_STATES)
#endif
#ifndef YY_SHRT_SIZE
#define YY_SHRT_SIZE	2
#endif
#if YY_COMPRESS == YY_ITERATIVE_COMPRESS
#if (2*(YY_N_NEXT*YY_SHRT_SIZE + YY_DUMMY_SIZE) <= YY_SHRT_MAX)
typedef YYShrt YYDisp;
#else
typedef YYInt YYDisp;
#endif
#else /* YY_COMPRESS != YY_ITERATIVE_COMPRESS */
#if (YY_N_NEXT*YY_SHRT_SIZE + YY_DUMMY_SIZE <= YY_SHRT_MAX)
typedef YYShrt YYDisp;
#else
typedef YYInt YYDisp;
#endif
#endif /* else YY_COMPRESS != YY_ITERATIVE_COMPRESS */
#endif /* if YY_TABLE_ENTRY == YY_DIFF_TABLE */

/* Let preprocessor figure out type for eqClass[]. */

/* typedef YYClass to represent values in 0, ..., YY_N_EQ_CLASSES-1. */
#if (YY_N_EQ_CLASSES-1 <= YY_UCHAR_MAX)
typedef YYUChar YYClass;
#elif (YY_N_EQ_CLASSES-1 <= YY_USHRT_MAX)
typedef YYUShrt YYClass;
#elif (YY_N_EQ_CLASSES-1 <= YY_UINT_MAX)
typedef YYUInt YYClass;
#elif (YY_N_EQ_CLASSES-1 <= YY_ULONG_MAX)
typedef YYULong YYClass;
#else
  #error Type YYClass cannot be represented.
#endif


/* Let preprocessor figure out type for eqBase[]. */

/* typedef YYEqBase to represent values in 0, ..., YY_MAX_EQ_BASE. */
#if (YY_MAX_EQ_BASE <= YY_UCHAR_MAX)
typedef YYUChar YYEqBase;
#elif (YY_MAX_EQ_BASE <= YY_USHRT_MAX)
typedef YYUShrt YYEqBase;
#elif (YY_MAX_EQ_BASE <= YY_UINT_MAX)
typedef YYUInt YYEqBase;
#elif (YY_MAX_EQ_BASE <= YY_ULONG_MAX)
typedef YYULong YYEqBase;
#else
  #error Type YYEqBase cannot be represented.
#endif


/* Let preprocessor figure out type for eqCheck[]. */

/* typedef YYEqCheck to represent values in 0, ..., YY_MAX_EQ_CHECK. */
#if (YY_MAX_EQ_CHECK <= YY_UCHAR_MAX)
typedef YYUChar YYEqCheck;
#elif (YY_MAX_EQ_CHECK <= YY_USHRT_MAX)
typedef YYUShrt YYEqCheck;
#elif (YY_MAX_EQ_CHECK <= YY_UINT_MAX)
typedef YYUInt YYEqCheck;
#elif (YY_MAX_EQ_CHECK <= YY_ULONG_MAX)
typedef YYULong YYEqCheck;
#else
  #error Type YYEqCheck cannot be represented.
#endif


#endif /* else !YY_ALIGN */


/* 		PUBLIC MACROS NOT REDEFINABLE BY USER.			*/

/* Macro for entering a start-state. Yucky, but can't be helped because 
 * of lex. 
 */
#define BEGIN 	yyData.startState= 
#define YY_BEGIN(s)  yyData.startState= (s) 	/* Cleaner version. */

#if YY_DO_REJECT
/* Macro for rejecting current action. Can only be used within yylex(). */
#define REJECT		YY_REJECT
#endif

#define	YY_MORE yyMore(YY_DATA_P)
#define yymore() YY_MORE

#define YY_LESS(n) yyLess(YY_DATA_P, n)
#define yyless(n) YY_LESS(n)

#define YY_TERMINATE()							\
  do {									\
    yyTerminate(YY_DATA_P);						\
    return YY_EOF_OUT;							\
  } while (0)
#define yyterminate() YY_TERMINATE()

#define YY_CHAR_NUM (yyBuf.nRead + (YY_TEXT0 - yyBuf.buf))

#define YY_NEW_FILE yyrestart(YY_IN)

#define YY_RESTART(f)	yyRestart(YY_DATA_P, f)
#define yyrestart(f)	YY_RESTART(f)

#define YY_GET()	yyGet(YY_DATA_P)
#ifdef __cplusplus
#define yy_input()	YY_GET()
#else
#define input()		YY_GET()
#endif

#define YY_UNPUT(ch)	yyUnput(YY_DATA_P, ch)
#define unput(ch)	YY_UNPUT(ch)

#define YY_BUFFER_STATE YYBufHandle

#define YY_CREATE_BUFFER(f, s)	yyCreateBuffer(YY_DATA_P, f, s)
#define yy_create_buffer(f, s)	YY_CREATE_BUFFER(f, s)
#define YY_SWITCH_TO_BUFFER(b)	yySwitchToBuffer(YY_DATA_P, b)
#define yy_switch_to_buffer(b)	(YY_SWITCH_TO_BUFFER(b))
#define YY_DELETE_BUFFER(b)	yyDeleteBuffer(YY_DATA_P, b)
#define yy_delete_buffer(b)	YY_DELETE_BUFFER(b)

#define YY_FLUSH_BUFFER		yy_flush_buffer(YY_CURRENT_BUFFER)
#define yy_flush_buffer(bp)	yyFlushBuffer(YY_DATA_P, bp)

#define YY_STRING_BUFFER(str)		yyStringBuffer(YY_DATA_P, str)
#define yy_scan_string(str)		YY_STRING_BUFFER(str)
#define YY_BYTES_BUFFER(bytes, len)	yyBytesBuffer(YY_DATA_P, bytes, len)
#define yy_scan_bytes(bytes, len)	YY_BYTES_BUFFER(bytes, len)
#define YY_MEM_BUFFER(buf, len)		yyMemBuffer(YY_DATA_P, buf, len)
#define yy_scan_buffer(buf, len)	YY_MEM_BUFFER(buf, len)

#define YY_PUSH_STATE(s)	yyPushState(YY_DATA_P, s)
#define yy_push_state(s)	YY_PUSH_STATE(s)
#define YY_POP_STATE()		yyPopState(YY_DATA_P)
#define yy_pop_state()		YY_POP_STATE()
#define YY_TOP_STATE()		yyTopState(YY_DATA_P)
#define yy_top_state()		YY_TOP_STATE()

/* 			SECTION 1 DECLARATIONS.				*/

/*!*/

/* 		    LIBRARY TYPES & DECLARATIONS.			*/

#include <assert.h>
#include <stdio.h>

#ifndef BUFSIZ
#define BUFSIZ	4096
#endif

#if __STDC__
#include <stdlib.h>	/* For exit(). */
#endif



/* 		NAMES FOR EXTERN SCANNER OBJECTS.			*/

/* Default prefixed names provided, if macros not defined in section 1.	*/
/*!*/


/*    DEFAULT MACRO DEFINITIONS IF NOT BEEN DEFINED IN SECTION 1.	*/

/* Prototype for yylex(). */
#ifndef YY_DECL
#define YY_DECL int YY_LEX(YY_VOID_ARGS)
#endif

/* Size of yytext when %array declaration used. */
#ifndef YYLMAX		
#define YYLMAX 8192	
#endif

#ifdef ECHO
#define YY_ECHO ECHO
#endif

#ifndef YY_ECHO
#define ECHO YY_ECHO
#define YY_ECHO fwrite(YY_TEXT, YY_LENG, 1, YY_OUT)
#endif

#ifndef YY_BREAK
#define YY_BREAK break
#endif

#ifndef YY_USER_ACTION
#define YY_USER_ACT
#else
#define YY_USER_ACT							\
  do { if (0 < yyAct && yyAct < YY_DEFAULT_ACTN) YY_USER_ACTION; } while (0)
#endif

#ifndef YY_USER_INIT
#define YY_USER_INIT
#endif

#ifndef YY_INPUT
#define YY_INPUT(buf, result, n)					\
  do {									\
    result= YY_READ(YY_IN, buf, (n));					\
    YY_READ_ERROR(result, YY_IN);					\
    if (result == 0) result= YY_EOF_IN; /* NOP when YY_EOF_IN == 0. */	\
  } while (0)
#endif

/* Combine YY_DO_DEBUG, YYDEBUG and YY_ZL_DEBUG (if def'd in declarations).*/
#if (YY_DO_DEBUG == 1)
#undef YY_ZL_DEBUG
#define YY_ZL_DEBUG
#endif
#ifdef YYDEBUG
#undef YY_ZL_DEBUG
#define YY_ZL_DEBUG
#endif

#ifndef YY_NULL
#define YY_NULL 0
#endif

#ifndef YY_EOF_IN
#define YY_EOF_IN	YY_NULL
#endif

#ifndef YY_EOF_OUT
#define YY_EOF_OUT	YY_NULL
#endif

#if YY_START_OF_LINE
#define YY_SET_BOL(v)	(yyBuf.lastCh= (v) ? '\n' : ' ')
#define yy_set_bol(v)	YY_SET_BOL(v)
#define YY_AT_BOL()	(yyBuf.lastCh == '\n')
#endif

#ifndef YY_FATAL
#define YY_FATAL(msg)	yyZlexFatal(YY_ERR, msg)
#endif

#if YY_DO_CODE		/* Generating a scanner with code-encoded state. */

#if YY_LABEL_VARS	/* Compiler allows vars to contain label addresses. */

#ifndef YY_LABEL_TYPEDEF /* How compiler declares vars containing labels. */
#ifdef __GNUC__
#define YY_LABEL_TYPEDEF(type)	typedef void *type
#endif
#endif

#ifndef YY_LABEL_ADDR	/* How compiler takes the address of a label. */
#ifdef __GNUC__
#define YY_LABEL_ADDR(label)	&&label
#endif
#endif

#endif /* if YY_LABEL_VARS */

#if YY_LABEL_VARS

YY_LABEL_TYPEDEF(YYLabP);
#define YY_SWITCH(tabName, var, def, cases)				\
  { static YYLabP tabName[]= { cases }; goto *tabName[var]; }
#define YY_CASE(val, label)	YY_LABEL_ADDR(label),
#define YY_DEFAULT_CASE(val, label) YY_CASE(val, label)

#else /* !YY_LABEL_VARS */

#define YY_SWITCH(tabName, var, def, cases)				\
  { switch (var) { cases default: goto def; } }
#define YY_CASE(val, label)	case val: goto label;
#define YY_DEFAULT_CASE(val, label)

#endif /* YY_LABEL_VARS */

#endif /* if YY_DO_CODE */

/*			START-STATE DEFINITIONS.			*/

/*!*/

/*		PRIVATE MACROS (SHOULD NOT BE USED BY USER)		*/

/* yytext which is usually in buffer. */
#if YY_ARRAY
#define YY_TEXT0		yytext0
#define YY_TEXT_ARRAY_INIT	YY_TEXT
#else
#define YY_TEXT0		YY_TEXT
#define YY_TEXT_ARRAY_INIT	NULL
#endif


#define YY_OPT_INIT \
  (   (YY_LINENO_ACTN ? YY_LINENO_FLAG : 0) \
    | (YY_ARRAY ? YY_ARRAY_FLAG : 0) \
    | (YY_BACKUP_OPTIMIZE ? YY_BACKUP_OPTIMIZE_FLAG : 0) \
  )

/* If cc initializer exp can't handle ?:, then define as follows (need to
 * add YY_BACKUP_OPTIMIZE case). 
 */
#if 0 
#if YY_ARRAY
#if YY_LINENO_ACTN
#define YY_OPT_INIT	(YY_LINENO_FLAG|YY_ARRAY_FLAG)
#else
#define	YY_OPT_INIT	(YY_ARRAY_FLAG)
#endif
#else /* !YY_ARRAY */
#if YY_LINENO_ACTN
#define YY_OPT_INIT	(YY_LINENO_FLAG)
#else
#define YY_OPT_INIT	0
#endif
#endif /* else !YY_ARRAY */
#endif /* #if 0 */

#if YY_DO_STDIO
#define YY_READ(f, b, n)	fread((b), sizeof(YYChar), (n), (f))
#define YY_READ_ERROR(result, f)					\
  do {									\
    if ((result) == 0 && ferror(f)) {					\
      YY_FATAL("Error reading input file.");				\
    }									\
  } while (0)
#else
extern int read();
/* Don't declare fileno() as it may be a macro. */
#define YY_READ(f, b, n)	read(fileno(f), (b), (n)*sizeof(YYChar))
#define YY_READ_ERROR(result, f)					\
  do {									\
    if ((result) < 0) {							\
      YY_FATAL("Error reading input file.");				\
    }									\
  } while (0)
#endif

#if (YY_DEFAULT_ACTION == 0)	/* echo */
#define YY_DEFAULT_ACT()						\
  do {									\
    YY_DEFAULT_MSG(); ECHO;						\
  } while (0)
#elif (YY_DEFAULT_ACTION == 1)	/* error */
#define YY_DEFAULT_ACT()						\
  do {									\
    YY_DEFAULT_MSG();							\
    fprintf(YY_ERR, "scanner jammed at character `%c'.\n", YY_TEXT[0]);	\
  } while (0)
#elif (YY_DEFAULT_ACTION == 2)	/* fatal */
#define YY_DEFAULT_ACT()						\
  do {									\
    char msg[80];							\
    YY_DEFAULT_MSG();							\
    sprintf(msg, "scanner jammed at character `%c'.\n", YY_TEXT[0]);	\
    YY_FATAL(msg);							\
  } while (0)
#else
#define YY_DEFAULT_ACT()	/* ignore */
#endif


/* Macro which decides whether we use a character directly or an eq. class. */
#if (YY_N_EQ_CLASSES == YY_CHAR_SET_SIZE)
#define YY_SET_EQ_CLASS(e, c) do { e= c; } while (0)
#elif (YY_MAX_EQ_BASE > 0) 
#define YY_SET_EQ_CLASS(e, c)						\
  do {									\
    YY_CONST YYUInt lo= (c) & ((1L << YY_CLASS_SEG_BIT) - 1);		\
    YY_CONST YYUInt hi= (c) >> YY_CLASS_SEG_BIT;			\
    YY_CONST YYUInt i= yyTab.u.s.eqBase[hi] + lo;			\
    e= (yyTab.u.s.eqCheck[i] == lo)					\
       ? yyTab.u.s.eqNext[i]						\
       : yyTab.u.s.eqDefault[hi];					\
  } while (0) 
#else
#define YY_SET_EQ_CLASS(e, c)						\
  do { e= yyTab.u.s.eqClass[c]; } while (0)
#endif

/* Macro which converts from YYChar to unsigned. Evaluates argument exactly
 * once.
 */
#if YY_16_BIT
#define YY_CHAR_TO_UNSIGNED(c)		((unsigned)(c))
#else
#define YY_CHAR_TO_UNSIGNED(c)		((unsigned)(unsigned char)(c))
#endif

#ifdef YY_ZL_DEBUG

#define YY_DEBUG_MSG(n)							\
  yyDebugMsg(&yyData, YY_SRC_NAME, n, YY_TEXT, YY_ZLEX_DEBUG)
#define YY_EOF_MSG() yyEOFMsg(&yyData, YY_ZLEX_DEBUG)
#define YY_DEFAULT_MSG() yyDefaultMsg(&yyData, YY_TEXT, YY_ZLEX_DEBUG)

#else

#define YY_DEBUG_MSG(n)
#define YY_EOF_MSG()
#define YY_DEFAULT_MSG()

#endif /* ifdef YY_DEBUG */

#ifdef YYTRACE	/* Trace DFA transitions. */

/* Since tracing will operate before the scanner has been initialized, 
 * when YY_ERR may be NULL, we check it using the following macro.
 */
#define YY_XERR		((YY_ERR) ? YY_ERR : stderr)

#define YY_TRACE_TRANSIT(s, c)	fprintf(YY_XERR, " %d/`%c'", (s), (c))
#define YY_TRACE_END(s)		fprintf(YY_XERR, " %d", s)
#define YY_TRACE_REJECT(s)	fprintf(YY_XERR, " ^%d", (s))
#define YY_TRACE_ACT(a)		fprintf(YY_XERR, " #%d\n", (a))

#else

#define YY_TRACE_TRANSIT(s, c)
#define YY_TRACE_END(s)
#define YY_TRACE_REJECT(s)
#define YY_TRACE_ACT(a)

#endif /* ifdef YYTRACE */

/* We need action numbers if YY_USER_ACTION is defined or YYTRACE is
 * defined.  The following clumsy code sets up YY_HAS_ACTION_NUMBERS 
 * without using the cpp `defined' operator. 
 */
#ifdef YYTRACE
#define YY_HAS_ACTION_NUMBERS 1
#else
#ifdef YY_USER_ACTION
#define YY_HAS_ACTION_NUMBERS 1
#else
#define YY_HAS_ACTION_NUMBERS 0
#endif
#endif

#if YY_HAS_ACTION_NUMBERS
#define YY_SET_ACT()							\
  do { yyAct= yyState < YY_N_STATES ? yyActs[yyState] : 0; } while (0)
#define yy_act	yyAct		/* For flex compatibility. */ 
#else
#define YY_SET_ACT()
#endif

/* Set up correct start-state at start of scanning. */
#if (YY_N_START_STATES == 1 && YY_START_OF_LINE == 0)
#define YY_START	0
#elif (YY_N_START_STATES == 1 && YY_START_OF_LINE == 1)
#define YY_START	(yyBuf.lastCh == '\n')
#elif (YY_N_START_STATES > 1 && YY_START_OF_LINE == 0)
#define YY_START	(yyData.startState)
#else
#define YY_START	(yyData.startState + (yyBuf.lastCh == '\n'))
#endif

/* For flex and AT&T compatibility. */
#define YYSTATE		YY_START


/* The following macros are used for initializing entries in the next[]
 * table.
 * YY_B(offset): 
 *		A macro used for initializing the base[] vector to point
 *		to next[offset] (using offset as an index if TABLE ==
 *		STATE_TABLE; using an address otherwise).
 * YY_N1(s0, s1): 
 *		A macro which represents the transition from significant 
 *		state s0 (which for TABLE != STATE_TABLE is represented 
 *		by its base value in next[]) to significant state s1 (which
 *		for TABLE != STATE_TABLE is represented by its base
 *		value in next[]). 
 * YY_N2(s0, s1): 
 *		A macro which represents the transition from significant 
 *		state s0 (which for TABLE != STATE_TABLE is represented 
 *		by its base value in next[]) to non-significant or
 *		pseudo-state s1.
 * YY_S(s):	
 *		A macro used for storing a state number when compressed 
 *		(stored at YY_STATE_INDEX offset from the base of the state).
 */
#if YY_TABLE_ENTRY == YY_ADDRESS_TABLE
#define YY_B(offset) (&yyTab.next[offset])
#define YY_N1(s0Base, s1Base) (&yyTab.next[s1Base])
#define YY_N2(s0Base, s1) ((YYNext *)(&yyTab.u.dummy[(s1) - YY_N_SIG_STATES]))
#define YY_S(s)	((YY_CONST YYNext *)(s))
#elif YY_TABLE_ENTRY == YY_DIFF_TABLE
/* By converting pointers to char* pointers, we can avoid an index*size
 * multiplication at runtime; also makes it possible for dummy[] to be
 * a char vector.  Disadvantage is that values may not fit within short.
 * The definition of YY_N2 uses array indices of 0 and makes explicit 
 * the address arithmetic.
 */
#define YY_B(offset) (&yyTab.next[offset])
#define YY_N1(s0Base, s1Base)						\
  ((s1Base - s0Base)*((int)sizeof(YYNext)))
#define YY_N2(s0Base, s1)						\
  ((&yyTab.u.dummy[0] - (char*)(&yyTab.next[0]))			\
   + ((s1) - YY_N_SIG_STATES)*((int)sizeof(char)) 			\
   - (s0Base)*((int)sizeof(YYNext)))
#define YY_S(s)	(s)
#else
#define YY_B(offset) (offset)
#define YY_N1(s0, s1) (s1)
#define YY_N2(s0, s1) (s1)
#define YY_S(s)	(s)
#endif

/* Decode next state tables.  9 possibilities for tables (3 compress 
 * options x 3 table-entry options).  Define following macros and
 * typedefs:
 *
 * YYTState:	A typedef used for states within transition loops.
 * YY_SIG_STATE_NUM(s)/YY_NON_SIG_STATE_NUM(s):  
 *		Convert from a significant/non-significant YYTState 
 *		to a normal numeric state.
 * YY_NUM_STATE_TO_T_STATE(s):
 *		Convert from  significant numeric state to a YYTState.
 * YY_SIG_LIMIT: A YYTState must be < this to be significant.
 * YY_D1(s0, def, isTerminal):
 *		A macro used for initializing the default for state s0
 *		(which for TABLE != STATE_TABLE is represented 
 *		by its base value in next[]) to default def (which
 *		for TABLE != STATE_TABLE is represented by its base
 *		value in next[]).  isTerminal is 1 iff the default
 *		is a terminal default which terminates the state
 *		transition iteration.
 * YY_D2(s0, def, isTerminal):
 *		A macro used for initializing the default for state s0
 *		(which for TABLE != STATE_TABLE is represented by its 
 *		base value in next[]) to non-significant or pseudo-state 
 *		default def. isTerminal is 1 iff the default is a terminal 
 *		default which terminates the state transition iteration.
 * YY_SET_NEXT_STATE(ss, c): Set YYTState ss to the next YYTState on
 *		character c.
 *
 * In addition, there are also typedefs for YYNext structures.
 */

#if (YY_COMPRESS == YY_NO_COMPRESS && YY_TABLE_ENTRY == YY_ADDRESS_TABLE)
typedef struct YYNext { YY_CONST struct YYNext *next; } YYNext;
typedef YY_CONST YYNext *YYTState;
#define YY_SIG_STATE_NUM(s)						\
  ((YYState)(((s) - yyTab.next)/YY_N_COLS))
#define YY_NON_SIG_STATE_NUM(s)						\
  (YY_N_SIG_STATES + (((char*)(s)) - yyTab.u.dummy))
#define YY_NUM_STATE_TO_T_STATE(s)	(&yyTab.next[(s)*YY_N_COLS])
#define YY_SIG_LIMIT							\
  (&yyTab.next[(YY_N_SIG_STATES - 1)*YY_N_COLS + 1])
#undef YY_D1
#undef YY_D2
#define YY_SET_NEXT_STATE(ss, c)					\
  do {									\
    (ss)= (ss)[c].next;							\
  } while (0)

#elif (YY_COMPRESS == YY_NO_COMPRESS && YY_TABLE_ENTRY == YY_DIFF_TABLE)
typedef YYDisp YYNext;
typedef YY_CONST YYNext *YYTState;
#define YY_SIG_STATE_NUM(s)						\
  ((YYState)(((s) - yyTab.next)/YY_N_COLS))
#define YY_NON_SIG_STATE_NUM(s)						\
  (YY_N_SIG_STATES + (((char*)(s)) - yyTab.u.dummy))
#define YY_NUM_STATE_TO_T_STATE(s)	(&yyTab.next[(s)*YY_N_COLS])
#define YY_SIG_LIMIT							\
  (&yyTab.next[(YY_N_SIG_STATES - 1)*YY_N_COLS + 1])
#undef YY_D1
#undef YY_D2
#define YY_SET_NEXT_STATE(ss, c)					\
  do {									\
    YY_CONST char *cs= (char *)(ss);					\
    cs+= (ss)[c];							\
    (ss)= (YYTState)cs;							\
  } while (0)

#elif (YY_COMPRESS == YY_NO_COMPRESS && YY_TABLE_ENTRY == YY_STATE_TABLE)
typedef YYXState YYNext;
typedef YYState YYTState;
#define YY_SIG_STATE_NUM(s)		(s)
#define YY_NON_SIG_STATE_NUM(s)		(s)
#define YY_NUM_STATE_TO_T_STATE(s)	(s)
#define YY_SIG_LIMIT			(YY_N_SIG_STATES)
#undef YY_D1
#undef YY_D2
#define YY_SET_NEXT_STATE(ss, c)					\
  do {									\
    (ss)= yyTab.next[ss][c];						\
  } while (0)

#elif (YY_COMPRESS == YY_COMB_COMPRESS && YY_TABLE_ENTRY == YY_ADDRESS_TABLE)
typedef struct YYNext { YY_CONST struct YYNext *next; YYCheck check; } YYNext;
typedef YY_CONST YYNext *YYTState;
#define YY_SIG_STATE_NUM(s)	((YYState)((s)[YY_STATE_INDEX].next))
#define YY_NON_SIG_STATE_NUM(s)						\
  (YY_N_SIG_STATES + (((char*)(s)) - yyTab.u.dummy))
#define YY_NUM_STATE_TO_T_STATE(numState) (yyTab.u.s.base[numState])
#define YY_SIG_LIMIT	(&yyTab.next[YY_MAX_BASE + 1])
#define YY_D1(s0Base, defBase, isTerminal) YY_N1(s0Base, defBase)
#define YY_D2(s0Base, def, isTerminal) YY_N2(s0Base, def)
#define YY_SET_NEXT_STATE(ss, c)					\
  do {									\
    YY_CONST YYNext *YY_CONST p= &(ss)[c];				\
    (ss)= (p->check == (c)) ? p->next : (ss)[YY_DEF_INDEX].next;	\
  } while (0)

#elif (YY_COMPRESS == YY_COMB_COMPRESS && YY_TABLE_ENTRY == YY_DIFF_TABLE)
typedef struct { YYDisp next; YYCheck check; } YYNext;
typedef YY_CONST YYNext *YYTState;
#define YY_SIG_STATE_NUM(s)	((YYState)((s)[YY_STATE_INDEX].next))
#define YY_NON_SIG_STATE_NUM(s)						\
  (YY_N_SIG_STATES + (((char*)(s)) - yyTab.u.dummy))
#define YY_NUM_STATE_TO_T_STATE(numState) (yyTab.u.s.base[numState])
#define YY_SIG_LIMIT	(&yyTab.next[YY_MAX_BASE + 1])
#define YY_D1(s0Base, defBase, isTerminal) YY_N1(s0Base, defBase)
#define YY_D2(s0Base, def, isTerminal) YY_N2(s0Base, def)
#define YY_SET_NEXT_STATE(ss, c)					\
  do {									\
    YY_CONST YYNext *YY_CONST p= &(ss)[c];				\
    YY_CONST char *sc= (char *)(ss);					\
    sc+= (p->check == (c)) ? p->next : (ss)[YY_DEF_INDEX].next;		\
    (ss)= (YYTState)sc;							\
  } while (0)

#elif (YY_COMPRESS == YY_COMB_COMPRESS && YY_TABLE_ENTRY == YY_STATE_TABLE)
typedef YYXState YYNext;
typedef YYState YYTState;
#define YY_SIG_STATE_NUM(s)		(s)
#define YY_NON_SIG_STATE_NUM(s)		(s)
#define YY_NUM_STATE_TO_T_STATE(s)	(s)
#define YY_SIG_LIMIT			(YY_N_SIG_STATES)
#define YY_D1(_s0Base, def, _isTerminal) (def)
#undef YY_D2
#define YY_SET_NEXT_STATE(ss, c)					\
  do {									\
    YY_CONST unsigned i= yyTab.u.s.base[ss] + (c);			\
    (ss)= (yyTab.check[i] == (c)) ? yyTab.next[i] : yyTab.def[ss];	\
  } while (0)

#elif (YY_COMPRESS == YY_ITERATIVE_COMPRESS && YY_TABLE_ENTRY == YY_ADDRESS_TABLE)
typedef struct YYNext { YY_CONST struct YYNext *next; YYCheck check; } YYNext;
typedef YY_CONST YYNext *YYTState;
#define YY_SIG_STATE_NUM(s)	((YYState)((s)[YY_STATE_INDEX].next))
#define YY_NON_SIG_STATE_NUM(s)						\
  (YY_N_SIG_STATES + (((char*)(s)) - yyTab.u.dummy))
#define YY_NUM_STATE_TO_T_STATE(numState)	(yyTab.u.s.base[numState])
#define YY_SIG_LIMIT	&yyTab.next[YY_MAX_BASE + 1]
#define YY_D1(s0Base, defBase, isTerminal)				\
  ((YYNext*)((((defBase - s0Base)*((int)sizeof(YYNext))) << 1)		\
   | (isTerminal)))
#define YY_D2(s0Base, def, isTerminal)					\
  ((YYNext*)								\
    ((((&yyTab.u.dummy[0] - (char*)&yyTab.next[0])			\
       + (def - YY_N_SIG_STATES) - (s0Base)*((int)sizeof(YYNext)))	\
      << 1)								\
     | (isTerminal)))
#define YY_SET_NEXT_STATE(ss, c)					\
  do {									\
    YY_CONST YYNext *YY_CONST p= &(ss)[c];				\
    if (p->check == (c)) {						\
      (ss)= p->next; break;						\
    }									\
    else {								\
      char *sc= (char*)(ss);						\
      int entry= (int)((ss)[YY_DEF_INDEX].next);			\
      sc+= (entry & ~1)/2; (ss)= (YYNext*)sc;				\
      if (entry & 1) break;						\
    }									\
  } while (1)

#elif (YY_COMPRESS == YY_ITERATIVE_COMPRESS && YY_TABLE_ENTRY == YY_DIFF_TABLE)
typedef struct { YYDisp next; YYCheck check; } YYNext;
typedef YY_CONST YYNext *YYTState;
#define YY_SIG_STATE_NUM(s)	((YYState)((s)[YY_STATE_INDEX].next))
#define YY_NON_SIG_STATE_NUM(s)						\
  (YY_N_SIG_STATES + (((char*)(s)) - yyTab.u.dummy))
#define YY_NUM_STATE_TO_T_STATE(numState) (yyTab.u.s.base[numState])
#define YY_SIG_LIMIT	(&yyTab.next[YY_MAX_BASE + 1])
#define YY_D1(s0Base, defBase, isTerminal)				\
    ((((defBase - s0Base) * ((int)sizeof(YYNext))) << 1) | (isTerminal))
#define YY_D2(s0Base, def, isTerminal)					\
   ((((&yyTab.u.dummy[0] - (char*)&yyTab.next[0])			\
      + (def - YY_N_SIG_STATES) - (s0Base)*((int)sizeof(YYNext)))	\
     << 1)								\
    | (isTerminal))
#define YY_SET_NEXT_STATE(ss, c)					\
  do {									\
    YY_CONST YYNext *YY_CONST p= &(ss)[c];				\
    YY_CONST char *sc= (char *)(ss);					\
    if (p->check == (c)) {						\
      sc+= p->next; (ss)= (YYTState)sc; break;				\
    }									\
    else {								\
      int entry= (ss)[YY_DEF_INDEX].next;				\
      sc+= (entry & ~1)/2; (ss)= (YYTState)sc;	 			\
      if (entry & 1) break;						\
    }									\
  } while (1)

#elif (YY_COMPRESS == YY_ITERATIVE_COMPRESS && YY_TABLE_ENTRY == YY_STATE_TABLE)
typedef YYXState YYNext;
typedef YYState YYTState;
#define YY_SIG_STATE_NUM(s)		(s)
#define YY_NON_SIG_STATE_NUM(s)		(s)
#define YY_NUM_STATE_TO_T_STATE(s)	(s)
#define YY_SIG_LIMIT			(YY_N_SIG_STATES)
#define YY_D1(_s0, def, isTerminal)	(((def)<<1) | (isTerminal))
#undef YY_D2
#define YY_SET_NEXT_STATE(ss, c)					\
  do {									\
    YY_CONST unsigned i= yyTab.u.s.base[ss] + (c);			\
    if (yyTab.check[i] == (c)) {					\
      (ss)= yyTab.next[i]; break;					\
    }									\
    else {								\
      unsigned entry= yyTab.def[ss];					\
      (ss)= entry >> 1;							\
      if (entry & 1) break;						\
    }									\
  } while (1)
#endif


#if (YY_START_OF_LINE == 1)
#if !YY_DO_CODE && !YY_BACKUP_OPTIMIZE
#define YY_SAVE_LAST()							\
  do {									\
    yyLastCh= yyBuf.lastCh; yyBuf.lastCh= yyCP[-1];			\
  } while (0)
#define YY_RESCAN_RESTORE()  do { yyBuf.lastCh= yyLastCh; } while (0)
#else
#define YY_SAVE_LAST()							\
  do {									\
    yyBuf.lastCh= yyCP[-1];						\
  } while (0)
#define YY_RESCAN_RESTORE()
#endif /* if !YY_DO_CODE && !YY_BACKUP_OPTIMIZE */
#else
#define YY_SAVE_LAST()
#define YY_RESCAN_RESTORE()
#endif /* if (YY_START_OF_LINE == 1) */

#if YY_ARRAY
#define YY_COPY_TEXT_TO_ARRAY()						\
  do {									\
    YYChar *src= YY_TEXT0, *dest= YY_TEXT;				\
    YYChar *end= &YY_TEXT0[YY_LENG] + 1;				\
    while (src < end)  *dest++= *src++;					\
  } while (0)
#else
#define YY_COPY_TEXT_TO_ARRAY()
#endif

#if YY_ARRAY
#define YY_RETERMINATE_ARRAY()	YY_TEXT[YY_LENG]= '\0'
#else
#define YY_RETERMINATE_ARRAY()
#endif

/* The following macro is called after matching a context pattern
 * PAT/CONTEXT where the length of PAT is known to be n.
 */
#define YY_SET_LENG(n) 							\
  do {									\
    assert(yyCP == yyBuf.cp);						\
    assert(yyCP == yyData.saveCP);					\
    *yyCP= yyData.saveCh;						\
    YY_LENG= (n); yyData.saveCP= yyBuf.cp= yyCP= YY_TEXT0 + (n);	\
    yyData.saveCh= *yyCP; *yyCP= '\0';					\
    YY_SAVE_LAST();							\
    YY_RETERMINATE_ARRAY();						\
  } while (0)

/* The following macro is called after matching a context pattern
 * PAT/CONTEXT where the length of CONTEXT is known to be n.
 */
#define YY_DEC_LENG(n) 							\
  do {									\
    assert(yyBuf.cp == yyCP);						\
    assert(yyCP == yyData.saveCP);					\
    *yyCP= yyData.saveCh;						\
    YY_LENG-= (n); yyData.saveCP= yyBuf.cp= yyCP-= (n);			\
    yyData.saveCh= *yyCP; *yyCP= '\0';					\
    YY_SAVE_LAST();							\
    YY_RETERMINATE_ARRAY();						\
  } while (0)

#if YY_HAS_INTRA_TOK_PAT

#if YY_N_INTRA_STATES == 0
#define YY_ALT_STATE(s)		YY_JAM_STATE
#else
#define YY_ALT_STATE(s0)						\
  ( ((s0) < YY_N_SIG_STATES + YY_N_INTRA_STATES)			\
    ? yyTab.u.s.intra[(s0) - YY_N_SIG_STATES]				\
    : YY_JAM_STATE							\
  )
#endif

	
#if YY_BACKUP_OPTIMIZE

#define YY_BACKUP(len, string)						\
  do {									\
    if (0 < len && len <= YY_LENG) {					\
      yyBuf.cp= yyReplace(YY_DATA_P, len, string, &yyIntraText);	\
      yyRSP-= len; yyAltState= *yyRSP;					\
    }									\
  } while (0)

#else /* !YY_BACKUP_OPTIMIZE */

#define YY_BACKUP(len, string)						\
  do {									\
    if (0 < len && len <= YY_LENG) {					\
      yyReplace(YY_DATA_P, len, string, &yyIntraText);			\
      yyBuf.cp= yyIntraText; goto yyRescan;				\
    }									\
  } while (0)

#endif /* else !YY_DO_CODE */
#endif /* if YY_HAS_INTRA_TOK_PAT */

/* 		          GLOBAL VARIABLES.				*/

/* 
 * Dummy buffer used for auto-initialization.  dummyBuf[0] will contain a
 * SENTINEL_CHAR to force initialization via yyProcessSentinel();
 * dummyBuf[1] will be overwritten by '\0' by the scanner to terminate
 * YY_TEXT before it realizes this is not a normal token.  At init, cp
 * == &dummyBuf[0] & sentinelP == &dummyBuf[0].  We'd really like to
 * put the dummyBuf within yyData, but can't because some cc's don't
 * allow initializing vars which have circular initialization dependencies.
 */
#define YY_INIT_BUF_SIZE 2
static YYChar yyDummyBuf[YY_INIT_BUF_SIZE];

/* Dummy stack used in 1st pass thru scanning loop at init. time. 	*/
static YYState yyDummyStk[1];	

FILE *YY_IN;			/* Input FILE pointer. */
FILE *YY_OUT;			/* Output FILE pointer. */
FILE *YY_ERR;			/* Error FILE pointer. */

#if YY_ARRAY
YYChar YY_TEXT[YYLMAX];		/* Array containing last matched lexeme. */
#endif
YYChar *YY_TEXT0=		/* Pointer to last matched lexeme in buffer.*/ 
  &yyDummyBuf[0];
int YY_LENG= 0;			/* # of chars in last token matched.	*/

#ifdef YY_ZL_DEBUG
int YY_ZLEX_DEBUG= 1;		/* Flag to turn debug messages on/off. 	*/
#endif

#if YY_LINENO_ACTN
int YY_LINENO= 0;		/* Initialization will increment it. */
#define YY_COL_NUM	(YY_CHAR_NUM - yyBuf.lastLinePos)
#define YY_SET_COL_NUM(c)						\
  do { yyBuf.lastLinePos= YY_CHAR_NUM - (c); } while (0)
#define YY_LINENOP_INIT	(&YY_LINENO)
#else
#define YY_LINENOP_INIT 0
#endif

#if __STDC__
static yy_size_t yyRead (YYChar *buf, yy_size_t nRead);
#else
static yy_size_t yyRead();
#endif

#if __STDC__
int YY_WRAP(void);
#else
int YY_WRAP();
#endif

/* This buffer is used during initialization and to cache the current 
 * dynamically-allocated buffer.
 */
static YYBuf yyBuf= {		
  NULL,				/* yyin */
  &yyDummyBuf[0],		/* buf */
  &yyDummyBuf[1],		/* end */
  YY_INIT_BUF_SIZE,		/* size */
  0,				/* nRead */
  0,				/* intraPos */
  &yyDummyBuf[0],		/* cp */
  0,				/* lastLinePos */
  0,				/* firstNLPos */
  0,				/* lineno */
  '\n',				/* lastCh */
  0,				/* hasNL */
  0				/* eofState */
};

/* Opaque pointer to current buffer.  Read-only for user.
 * It will ALWAYS point to a dynamically allocated  buffer, 
 * or be NULL.
 */
YYBufHandle YY_CURRENT_BUFFER= NULL;

static YYData yyData= {
  &YY_IN,				/* yyinP		*/
  &YY_OUT,				/* yyoutP		*/
  &YY_ERR,				/* yyerrP		*/
  &YY_TEXT0,				/* yytextP		*/
  &YY_LENG,				/* yylengP		*/
  YY_LINENOP_INIT,			/* yylinenoP		*/
  (YYBufP *)&YY_CURRENT_BUFFER,		/* yyCurrentBufP	*/	
  YY_TEXT_ARRAY_INIT,			/* textArray		*/
  &yyBuf,				/* bufP			*/
  yyRead,				/* read()		*/
  YY_WRAP,				/* yywrap()		*/
  &yyDummyBuf[0],			/* saveCP		*/
  &yyDummyBuf[0],			/* sentinelP		*/
  YY_SENTINEL_CHAR,			/* sentinelChar		*/
  '\0',					/* sentinelSave		*/
  YY_SENTINEL_CHAR,			/* saveCh		*/
  YY_INIT_SENTINEL,			/* flags		*/
  YY_OPT_INIT,				/* options		*/
  0,					/* doMore		*/
  NULL,					/* moreText		*/
  0,					/* moreLeng		*/
  NULL,					/* yyTextSave		*/
  0,					/* yyTextSaveSize	*/
  yyDummyStk,				/* stk			*/
  1,					/* stkSize		*/
  0,					/* startState		*/
  NULL					/* ssStkP		*/
};

YY_VOIDP YY_CONST YY_DATA_P= (YY_VOIDP) (&yyData);




/*			     DFA TABLES.				*/

/* The following macro is used for within yyTab. */
#if __STDC__
/* ANSI allows initializing first element of union. */
#define YY_UNION_STRUCT		union
#else
/* Do not assume that a union can be initialized. */
#define YY_UNION_STRUCT		struct
#endif


/*!*/

/* Tables defining actions. */
#if YY_HAS_ACTION_NUMBERS
/*!*/
#endif /* if YY_HAS_ACTION_NUMBERS */


/* 			SCANNER ROUTINES				*/


static yy_size_t 
#if __STDC__
yyRead (YYChar *buf, yy_size_t nRead) 
#else
yyRead (buf, nRead)
  YYChar *buf;
  yy_size_t nRead;
#endif
{
/* Read upto nRead chars. from YY_IN into buffer buf.  Return # of chars.
 * read or 0 on EOF.  Print error message and abort if read error.
 * This is a call-back routine from the library to allow it to use a
 * user-specified INPUT().
 */
  int result;
  YY_INPUT(buf, result, nRead);
  return result;
}

#if YY_HAS_VAR_CONTEXTS

#if YY_BACKUP_OPTIMIZE

#define YY_VAR_CONTEXT_RESCAN(patStart, contextStart)			\
  yyVarContextRescan(YY_TEXT0, YY_LENG, patStart, contextStart, yyRSP)

#else

#define YY_VAR_CONTEXT_RESCAN(patStart, contextStart)			\
  yyVarContextRescan(YY_TEXT0, YY_LENG, patStart, contextStart, 0)

#endif

#if 0
#define YY_VAR_CONTEXT_RESCAN(patStart, contextStart)			\
  do {
    if (yyVarContext == 0) { /* Entry after PAT/CONTEXT match. */
      yyVarContextText= YY_TEXT0; 
      yyVarContextSentinelP= &YY_TEXT0[YY_LENG]; 
      *yyVarContextSentinelP= YY_SENTINEL_CHAR;
      yyState= patStart; yyCP= YY_TEXT0; yyVarContext= 1; goto yyStateSwitch;
    }
    else if (yyVarContext == 1) { /* Entry after PAT rematch. */
      yyVarContextP= yyCP;
  } while (0)
#endif

/* Call-back routine for variable context rescan.  We need a call-back
 * routine to access next-state tables.  text and leng specifies 
 * pat/context.  Starting in state patStart scan text for longest 
 * match within text.  Then try to get to an accepting state starting
 * in state contextStart with rest of text.  If unsuccessful, shorten
 * pat match and try again for a context match.
 */

#if __STDC__
static YY_VOID 
yyVarContextRescan(YYChar *text, YYUInt leng, 
		   YYState patStart, YYState contextStart,
		   YYState *sp)
#else
static YY_VOID 
yyVarContextAdjust(text, leng, patStart, contextStart, sp)
  YYChar *text;
  YYUInt leng;
  YYState patStart;
  YYState contextStart;
  YYState *sp;
#endif
{  
  YYState *YY_CONST spBase=   
    (sp == NULL || (yyData.stk + yyData.stkSize) - sp < leng) 
    ? (YYState*)malloc(leng * sizeof(YYState))
    : sp;
  YYState *rSP= spBase;
  YYTState tstate= YY_NUM_STATE_TO_T_STATE(patStart);
  YYChar *yyCP= text;
  YY_CONST YYChar *YY_CONST end= text + leng;
  YYState s, state;
  if (!spBase) {
    YY_FATAL("Out of memory for context rescan.");
  }
  do { /* Scan for longest possible match for pattern. */
    unsigned yyChar= YY_CHAR_TO_UNSIGNED(*yyCP++); /* Get next character. */
    unsigned yyEqChar;
    YY_SET_EQ_CLASS(yyEqChar, yyChar);
    *rSP++ = YY_SIG_STATE_NUM(tstate);	/* Remember state in state-stack. */
    assert(yyCP - text == rSP - spBase);	/* rSP, yyCP in sync. */
    YY_SET_NEXT_STATE(tstate, yyEqChar);
  } while (tstate < YY_SIG_LIMIT && yyCP < end);
  state= YY_NON_SIG_STATE_NUM(tstate);
#if 0
  if (state != YY_JAM_STATE) { *rSP++= state; }
#endif
  if (state < YY_N_STATES) { *rSP++= state; }
  do { /* Backup pattern match until context matches. */
    YYTState ts= YY_NUM_STATE_TO_T_STATE(contextStart);
    assert(rSP > spBase);
    do { /* Pop states off stack, until an accepting state is found. */
      state= *--rSP;
    } while (state < YY_N_NON_FINAL_STATES);
    yyCP= text + (rSP - spBase);
    while (ts < YY_SIG_LIMIT && yyCP < end) { /* Scan for context. */
      unsigned yyChar= YY_CHAR_TO_UNSIGNED(*yyCP++); /* Get next character. */
      unsigned yyEqChar; 
      YY_SET_EQ_CLASS(yyEqChar, yyChar);	
      YY_SET_NEXT_STATE(ts, yyEqChar);
    }
    s= (ts < YY_SIG_LIMIT) ? YY_SIG_STATE_NUM(ts) : YY_NON_SIG_STATE_NUM(ts); 
  } while (yyCP < end || s < YY_N_NON_FINAL_STATES);
  leng= rSP - spBase;
  if (spBase != sp) free(spBase);
  YY_SET_LENG(leng);
}

#endif /* if YY_HAS_VAR_CONTEXTS */



/*			MACROS USED WITHIN yylex()			*/


/* Macro to check if we've landed up in a jam-state because of a
 * sentinel char.
 */
#if YY_COMPRESS == YY_NO_COMPRESS && YY_BACKUP_OPTIMIZE
#define YY_JAM_PROCESS()	do { yyState= *--yyRSP; } while (0)
#endif 

#if YY_COMPRESS != YY_NO_COMPRESS && YY_BACKUP_OPTIMIZE
#define YY_JAM_PROCESS()						\
  do {									\
    if (yyChar == YY_SENTINEL_CHAR) {					\
      yyChar++;	/* Set yyChar to any char != YY_SENTINEL_CHAR. */	\
      yyState= YY_SENTINEL_STATE;					\
    }									\
    else {								\
      yyState= *--yyRSP;						\
    }									\
  } while (0)
#endif 

#if YY_COMPRESS == YY_NO_COMPRESS && !YY_BACKUP_OPTIMIZE
#define YY_JAM_PROCESS()						\
  do {									\
    yyCP--;	/* Backup corrects for 1-char overscan. */		\
    /* If jammed, set yyState to state before overscan. */		\
    yyState-= YY_N_STATES;						\
  } while (0)
#endif 

#if YY_COMPRESS != YY_NO_COMPRESS && !YY_BACKUP_OPTIMIZE
#define YY_JAM_PROCESS()						\
  do {									\
    yyCP--;	/* Backup corrects for 1-char overscan. */		\
    if (yyChar == YY_SENTINEL_CHAR) {					\
      yyChar++;	/* Set yyChar to any char != YY_SENTINEL_CHAR. */	\
    } else {  /* We have a jam. */					\
      yyState-= YY_N_STATES;	 /* Set yyState to jam predecessor. */	\
    }									\
  } while (0)
#endif 


/* Macro to rescan the current lexeme, using a slower scan. */
#if !YY_BACKUP_OPTIMIZE && !YY_DO_CODE
#define YY_RESCAN()							\
  do {									\
    *yyData.saveCP= yyData.saveCh;	/* Replace NUL terminator. */	\
    yyLimit= yyCP;							\
    goto yyScan2;							\
  } while (0)
#else
/* We should never get here. */
#define YY_RESCAN() assert(("Bad rescan sentinel" , 0))
#endif /* if !YY_BACKUP_OPTIMIZE && !YY_DO_CODE */


#if YY_BACKUP_OPTIMIZE && !YY_DO_CODE
/* Macro to handle a sentinel action in the action switch. */
#define YY_DO_SENTINEL()						\
  do { /* Sentinel action: cp points just past sentinel char. */	\
    assert(yyCP == yyBuf.cp);	/* Cached yyCP in synch. */		\
    assert(yyCP == yyData.saveCP);/* Just terminated yytext. */		\
    *yyCP= yyData.saveCh;	/* Restore ch. clobbered by '\0'. */	\
    --yyCP;			/* Point to sentinel char. */		\
    yyState= *--yyRSP;		/* yyState to sentinel state pred. */	\
    goto yyDoSentinel;							\
  } while (0)
#else
/* We should never get here. */
#define YY_DO_SENTINEL() assert(0)	/* Bad sentinel. */
#endif /* #if YY_BACKUP_OPTIMIZE && !YY_DO_CODE */


/* Macro to handle a sentinel which resulted in a pseudo sentinel state.
 * Used in the default case in the action switch.
 */
#if !YY_BACKUP_OPTIMIZE && !YY_DO_CODE
#define YY_DO_PSEUDO_SENTINELS()					\
  do {									\
    *yyData.saveCP= yyData.saveCh;	/* Replace NUL terminator. */	\
    yyState-= YY_N_STATES; /* set to sentinel state predecessor. */	\
    goto yyDoSentinel;							\
  } while (0)
#else
/* We shouldn't get here. */
#define YY_DO_PSEUDO_SENTINELS() assert(("Bad pseudo-sentinel" , 0)) 
#endif /* if !YY_BACKUP_OPTIMIZE && !YY_DO_CODE */


/* A utility macro to continue scanning used in several situations. */
#if YY_DO_CODE
#define YY_CONTINUE(codeJamLabel)					\
  do {									\
    if (yyAltState == YY_JAM_STATE) { goto codeJamLabel; }		\
    else { yyState= yyAltState; goto yyStateSwitch; }			\
  } while (0)
#endif


#if !YY_DO_CODE && YY_BACKUP_OPTIMIZE
#define YY_CONTINUE(codeJamLabel)					\
  do {									\
    yyState= yyAltState;						\
    if (yyState < YY_N_SIG_STATES) goto yyContinueScan;			\
    else goto yyEndScan;						\
  } while (0)
#endif


#if !YY_DO_CODE && !YY_BACKUP_OPTIMIZE 
#define YY_CONTINUE(codeJamLabel)					\
  do {									\
    yyState= yyAltState;						\
    if (yyState < YY_N_SIG_STATES) goto yyContinueScan;			\
    else if (yyState != YY_JAM_STATE) goto yyTerminateText;		\
    else { /* Rescan upto (not including) yyLimit. */			\
      yyLimit= yyCP - 1; goto yyScan2;					\
    }									\
  } while (0)
#endif 


/* Macro to perform yylineno action in action switch. */
#if YY_LINENO_ACTN
#define YYLINENO_ACT()							\
  do {									\
    yyCP= yyBuf.cp;							\
    if (!yyBuf.hasNL) { /* no '\n' in current token so far. */		\
      yy_size_t nlPos=  yyBuf.nRead + (yyCP - yyBuf.buf);		\
      if (nlPos > yyBuf.lastLinePos) { /* Not previously counted. */	\
	yyBuf.hasNL= 1; yyBuf.firstNLPos= nlPos;			\
      }									\
    }									\
    assert(yyCP == yyData.saveCP);					\
    *yyCP= yyData.saveCh;						\
    yyAltState= YY_ALT_STATE(yyState);					\
    YY_CONTINUE(yyBackup);						\
 } while (0)
#endif /* #if YY_LINENO_ACTN */


/* Macro which is invoked before the action for any intra-token pattern. */
#define YY_INTRA_PRE(len)						\
  do {									\
   yyAltState= YY_ALT_STATE(yyState);					\
   yyIntraPos= yyBuf.nRead + (yyBuf.cp - yyBuf.buf);			\
   if (yyIntraPos <= yyBuf.intraPos)					\
     goto yyIntraEnd;							\
   yyIntraText= YY_TEXT0; yyIntraLeng= YY_LENG;				\
   YY_TEXT0= yyBuf.cp - (len); YY_LENG= (len);				\
   YY_COPY_TEXT_TO_ARRAY();						\
  } while (0)



/* Macro which is invoked after the action for any intra-token pattern. */
#define YY_INTRA_POST()	 goto yyIntraDone



/*			MAIN SCANNER ROUTINE.				*/


YY_DECL
{
  /* Declare auto vars having full function scope. */ 
  
  register unsigned yyChar;	/* Character read. */
#if YY_DO_CODE
  YYChar *yyLastAcceptCP;	/* CP when last accepted when recorded. */
  YYState yyLastAcceptState;	/* Last accepting state when recorded. */
#endif
#if YY_BACKUP_OPTIMIZE
  YYState *yyStk= yyData.stk;	/* Stack base. */
  register YYState *yyRSP;	/* Stack pointer. */
#endif 
  register YYState yyState;	/* Numeric scanner state. */
  register YYTState yyTState;	/* Transition scanner state. */
  register YYChar *yyCP;	/* Points to next scan char. */
  YYState yyAltState;		/* Used for continuing scanning. */
#if YY_HAS_ACTION_NUMBERS
  unsigned yyAct;		/* Action # to be executed. */
#endif
#if YY_HAS_INTRA_TOK_PAT
  YYChar *yyIntraText;		/* Save area for yytext. */
  int yyIntraLeng;		/* Save area for yyleng. */
  yy_size_t yyIntraPos;		/* Buffer position of point after intra pat. */
#endif	
#if !YY_BACKUP_OPTIMIZE && !YY_DO_CODE
  YYChar *yyLimit;		/* Limit for second slower rescan. */
#endif
#if YY_DO_CODE && !YY_BACKUP_OPTIMIZE 
  static YYChar *yyBackupSaveP= 0;/* Pointer to end of yytext to backup over. */
  YYChar yyBackupSave;		/* Char. originally at yyBackupSaveP. */
#endif
#if !YY_DO_CODE && !YY_BACKUP_OPTIMIZE && YY_START_OF_LINE
  YYChar yyLastCh;		/* Save char before yytext for rescanning. */
#endif
#if YY_DO_REJECT
  YYState yyRejectState;	/* Remember current state here for reject. */
#endif

/* Local declarations made by user. */
/*!*/
 
  while (1) { /* Do forever until user action returns or EOF. */

  yyRescan:   /* Come here to restart a scan. */	

    /* Declare and initialize variables used for each scan. */ 
    
#if YY_BACKUP_OPTIMIZE
    yyRSP= yyStk;			/* Initialize stack pointer. */ 
#endif
    yyCP= yyBuf.cp;			/* Cache char. ptr. */
    yyState= YY_START;			/* Set scan state to start state. */
    *yyData.saveCP= yyData.saveCh;	/* Replace NUL terminator. */
    YY_TEXT0= yyCP;			/* Setup yytext. */ 
 
    /* If --yylineno, then update yylineno from last scan. */ 
    
#if YY_LINENO_ACTN
    if (yyBuf.hasNL) {
      YY_CONST YYChar *p;
      for (p= yyBuf.buf + (yyBuf.firstNLPos - 1 - yyBuf.nRead); 
	   p < yyCP; p++) {
        if (*p == '\n') {
	  YY_LINENO++;
  	  yyBuf.lastLinePos= yyBuf.nRead + (p - yyBuf.buf) + 1;
        }
      }
      yyBuf.hasNL= 0;
    }
#endif /* if YY_LINENO_ACTN */
 
#if YY_DO_CODE
    /* Directly encoded DFA. */
/*!*/
#else /* Table-driven scanner. */
    /* Enter performance-critical inner-loop. */ 
    
  yyContinueScan:	/* Reenter after seeing a sentinel. */
    yyTState= YY_NUM_STATE_TO_T_STATE(yyState);
    do { /* This is the scanner inner-loop. */
      unsigned yyEqChar; 
      yyChar= YY_CHAR_TO_UNSIGNED(*yyCP++); /* Get next character. */
      YY_SET_EQ_CLASS(yyEqChar, yyChar);
#if YY_BACKUP_OPTIMIZE
      *yyRSP++ = YY_SIG_STATE_NUM(yyTState);/* Remember state in state-stack. */
      assert(yyCP - YY_TEXT0 == yyRSP - yyStk);/* sp, cp in sync. */
#endif
      YY_TRACE_TRANSIT(YY_SIG_STATE_NUM(yyTState), yyChar);
      YY_SET_NEXT_STATE(yyTState, yyEqChar);
    } while (yyTState < YY_SIG_LIMIT); 
    yyState= YY_NON_SIG_STATE_NUM(yyTState); 
    YY_TRACE_END(yyState); 
 
    /* Backup to correct for overscanning or for REJECT action. */ 
    
#if YY_BACKUP_OPTIMIZE
  yyEndScan:	/* Label for a sentinel which terminates scan loop. */ 
    if (yyState == YY_JAM_STATE) { 
      YY_JAM_PROCESS();
    }
  yyFindAction: /* Label for rejecting to shorter lexeme or backing up. */
    assert(yyState != YY_JAM_STATE);
    while (yyState < YY_N_NON_FINAL_STATES) {
      /* Pop states off stack, until an accepting state is found. */
      yyState= *--yyRSP; 
      YY_TRACE_REJECT(yyState);
    }
#endif /* #if YY_BACKUP_OPTIMIZE */

#if !YY_BACKUP_OPTIMIZE
  yyEndScan:	/* Label for a sentinel which terminates scan loop. */ 
    if (yyState >= YY_N_STATES) { /* Overscanned. */
      YY_JAM_PROCESS();
    }
#endif /* #if !YY_BACKUP_OPTIMIZE */
 
#endif /* if !YY_DO_CODE Table-driver scanner. */
    /* Compute yyleng; terminate yytext. */ 
    
#if YY_DO_CODE
yyGetRecorded:
  yyCP= yyLastAcceptCP; yyState= yyLastAcceptState;
#endif
  yyTerminateText:
#if YY_BACKUP_OPTIMIZE
    YY_LENG= yyRSP - yyStk; 
    yyData.saveCP= yyBuf.cp= yyCP= YY_TEXT0 + YY_LENG;
    /* yyBuf.cp, sp now in synch. */
#else
    YY_LENG= yyCP - YY_TEXT0;
    yyData.saveCP= yyBuf.cp= yyCP;
#endif	
    YY_SAVE_LAST();
    yyData.saveCh= *yyCP; *yyCP= '\0';
    YY_COPY_TEXT_TO_ARRAY();  /* Copy from buf to yytext[] if %array. */
 
    /* Perform user or fixed scanner action. */ 
    assert(YY_INV(&yyData));
    
#if YY_DO_REJECT
    yyRejectState= yyState; /* Remember original state for doing rejects. */
#endif  
  yyDoAction:		/* For eof and reject actions. */	
    YY_SET_ACT();
    YY_TRACE_ACT(yyAct);
    YY_USER_ACT;
    switch (yyState) { /* Action table. */
/* User actions specified in zlex source file. */
/*!*/
    } /* switch (yyState) */

    continue;	/* With outer scanning loop. */
    /* Segments of code entered and exited using gotos. */ 
    
  /* Code to process sentinel. */
  
#if YY_DO_CODE
      yyDoSentinel1: /* yyCP points past sentinel char. */
	yyCP--;	/* Point yyCP to sentinel character. */
#endif
      yyDoSentinel: { /* cp points to the sentinel char. */
	assert(*yyCP == YY_SENTINEL_CHAR);
	yyData.saveCP= yyBuf.cp= yyCP;
	yyData.saveCh= YY_SENTINEL_CHAR; *yyCP= '\0';
	YY_LENG= yyCP - YY_TEXT0;
#if YY_DO_CODE && !YY_BACKUP_OPTIMIZE
	if (yyCP == yyBackupSaveP) {
	  
	  *yyBackupSaveP= yyBackupSave; yyBackupSaveP= 0; 
  	  yyCP++; goto yyJamSwitch;

	}
	else 
#endif /* #if !YY_BACKUP_OPTIMIZE */
	if (yyCP != yyData.sentinelP) { /* A sentinel char in input. */
          
	{ yyAltState= 
	    (yyState < 
             sizeof(yyTab.u.s.sentinel)/sizeof(yyTab.u.s.sentinel[0])) 
	    ? yyTab.u.s.sentinel[yyState]
	    : YY_JAM_STATE;
	  yyCP++;	/* Sentinel-char read as normal char. */
#if YY_BACKUP_OPTIMIZE
	  yyRSP++;	/* Adjust stack to recontain sentinel state pred. */
#endif
	  YY_CONTINUE(yyJamSwitch);
	} /* { yyAltState= */
 
	}
	else if (yyBuf.eofState && yyCP >= yyBuf.end) {
	  /* End-of-file sentinel. */ 
          
            if (YY_TEXT0 != yyCP) { /* EOF but need to process current token. */
	      assert(yyState < YY_N_STATES);
	      yyCP++;	/* Make cp point past char. which caused jam. */
#if YY_DO_CODE
	      goto yyJamSwitch;
#else
#if !YY_BACKUP_OPTIMIZE
	      yyState+= YY_N_STATES;
#endif
	      goto yyEndScan;
#endif 
	    }
            else if (yyBuf.eofState == 1) { /* Process <<EOF>> act. */
	      YY_EOF_MSG();
              assert(yyData.saveCh == YY_SENTINEL_CHAR);
              YY_IN= 0; yyBuf.eofState= 2; 
	      yyState= yyData.startState; 
              goto yyDoAction;
            }
            else if (YY_IN) {	/* User has reset yyin in <<EOF>> act. */
              YY_RESTART(YY_IN); 
              continue;		/* Continue with outer scanning loop. */
            }
            else {
              return YY_EOF_OUT;  
            }
 
	}
	else { 
          /* Buffer end, stack overflow or yymore action. */ 
	  
	    assert(yyState < YY_N_SIG_STATES);
	    if ((yyData.flags & YY_INIT_SENTINEL)) { YY_USER_INIT; }
            yyProcessSentinel(&yyData, yyCP - YY_TEXT0);
            yyCP= yyBuf.cp;	/* Recache updated cp. */
#if YY_BACKUP_OPTIMIZE
            yyStk= yyData.stk;	/* Recache stack which may have changed. */
            yyRSP= yyStk + (yyCP - YY_TEXT0);
#endif
#if YY_DO_CODE
            goto yyStateSwitch;	/* Continue scan, sentinel char undone. */
#else
            goto yyContinueScan;/* Continue scan, after sentinel char undone. */
#endif /* #if YY_DO_CODE */
 
        }
      } /* yyDoSentinel */

  /* Code to reject to next lexeme. */ 
  
      yyBackup:
        yyCP= yyBuf.cp;			/* Recache after possible input(). */
        if (YY_TEXT0 + YY_LENG != yyCP) {
	  yyMakeAdjText(&yyData);	/* Ensure yytext ends at cp. */
	  yyCP= yyBuf.cp;
        }
	assert(yyData.saveCP == &YY_TEXT0[YY_LENG]);
        *yyData.saveCP= yyData.saveCh;/* Restore saved char. */
#if YY_DO_CODE
	yyBackupSaveP= &yyCP[-1]; 
	yyBackupSave= *yyBackupSaveP; *yyBackupSaveP= YY_SENTINEL_CHAR;
	yyBuf.cp= YY_TEXT0;
	goto yyRescan;	/* Rescan in outer scanning loop. */
#elif YY_BACKUP_OPTIMIZE	
        yyState= *--yyRSP; 		/* Pop state off stack. */
        goto yyFindAction;
#else
	yyLimit= yyCP - 1;
	goto yyScan2;
#endif /* #if YY_DO_CODE */
 
  /* Code to rescan text relatively slowly. */
  
#if !YY_DO_CODE && !YY_BACKUP_OPTIMIZE
  yyScan2: { /* Label to rescan chars upto but not including yyLimit. */	
    YYState acceptState;
    YYChar *acceptCP;
    yyCP= YY_TEXT0;
    YY_RESCAN_RESTORE(); /* Restore yyBuf.lastCh to get correct start state. */
    yyState= YY_START;
    yyTState= YY_NUM_STATE_TO_T_STATE(yyState);
    do { /* Repetition of inner-loop, but tracking accept states. */
      unsigned yyChar= YY_CHAR_TO_UNSIGNED(*yyCP++); /* Get next character. */
      unsigned yyEqChar; 
      YY_SET_EQ_CLASS(yyEqChar, yyChar);	
      YY_TRACE_TRANSIT(YY_SIG_STATE_NUM(yyTState), yyChar);
      YY_SET_NEXT_STATE(yyTState, yyEqChar);
      yyState= (yyTState < YY_SIG_LIMIT) 
	       ? YY_SIG_STATE_NUM(yyTState)
               : YY_NON_SIG_STATE_NUM(yyTState);
      assert(yyState < YY_N_SIG_STATES);
      if (YY_N_NON_FINAL_STATES <= yyState) {
	acceptState= yyState; acceptCP= yyCP;
      }
    } while (yyCP < yyLimit);
    yyState= acceptState; yyCP= acceptCP;
    goto yyTerminateText;
  } /* yyScan2: */
#endif


  /* Code to windup an intra-token action. */ 
  
#if YY_HAS_INTRA_TOK_PAT
yyIntraDone: 
  yyBuf.intraPos= yyIntraPos;
  YY_TEXT0= yyIntraText; YY_LENG= yyIntraLeng;
yyIntraEnd: {
  yyCP= yyBuf.cp;
  *yyData.saveCP= yyData.saveCh;
  YY_CONTINUE(yyBackup);
} /* yyIntraEnd */
#endif 
 
 
  } /* while (1) loop until return. */
}


/* 			SECTION 3 CODE					*/

/*!*/
