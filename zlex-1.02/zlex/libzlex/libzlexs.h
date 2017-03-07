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
