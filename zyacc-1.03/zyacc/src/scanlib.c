/*** DO NOT EDIT: libzlex sources produced using /usr/local/share/zlex/mklibsrc fullscan.o scanlib.c */
/*  ===File: libzlexp.h===  */
/******* DO NOT EDIT *********/
/* automatically produced by catenating libzlex.h libzlexs.h and libzlex0.h */

/*

File:	 libzlex.h
Purpose: External interface to zlex library.

Last Update Time-stamp: "97/07/12 15:54:49 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

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

/* use for declaring a return value */
#ifndef YY_VOID
#define YY_VOID			void
#endif

/* use for returning a void value */
#ifndef YY_VOID_RET
#define YY_VOID_RET()		return
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

/* use for declaring a return value */
#ifndef YY_VOID
#define YY_VOID			int
#endif

/* use for returning a void value */
#ifndef YY_VOID_RET
#define YY_VOID_RET()		return 0
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
/*

File:	 libzlex0.h
Purpose: Private interface to zlex library.

Last Update Time-stamp: "97/07/12 15:51:33 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _LIBZLEX0_H
#define _LIBZLEX0_H

/* Private declarations for zlex library. 
 * Assumes external and skeleton parser declarations previously included.
 */

#include <config.h>

#include <assert.h>
#ifdef HAVE_STDDEF_H
#include <stddef.h>	/* for NULL, size_t. */
#endif

#if STDC_HEADERS
#include <stdlib.h>
#include <string.h>
#endif

enum {
  STK_SIZE_INC= 256 
};

/* The following are local to the library and not for external consumption. */

#if YY_16_BIT
#define yyMakeBuf		yyMakeBuf16
#define yyMoveBytes		yyMoveBytes16
#define yyMoveText		yyMoveText16
#define yySaveText		yySaveText16
#define yyShiftBuffer		yyShiftBuffer16
#define yyUpdateSentinel	yyUpdateSentinel16
#endif /* if YY_16_BIT */

YYBufHandle yyMakeBuf YY_PROTO ((YYData *dP, FILE *yyin, 
				 YYChar *mem, yy_size_t size));
YYChar *yyMoveBytes YY_PROTO ((YYChar *YY_CONST dest, 
			       YY_CONST YYChar *YY_CONST src, 
			       yy_size_t len));
YYChar *yyMoveText YY_PROTO ((YYData *dP, YYChar *dest));
YY_VOID yySaveText YY_PROTO ((YYData *dP));
YYChar *yyShiftBuffer YY_PROTO ((YYData *dP, YY_CONST yy_size_t nFree));
YY_VOID yyUpdateSentinel YY_PROTO ((YYData *dP, YYBoolX inTok));


#endif /* ifndef _LIBZLEX0_H */
/*  ===File: makeadj.c===  */
/*

File:	 makeadj.c
Purpose: Routine to make yytext adjacent to cp.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/



/* Make yytext adjacent to just before cp. */
YYChar *
yyMakeAdjText(dP)
  YYData *dP;
{
  YYBufP bP= dP->bufP;
  int yyLeng= *dP->yylengP;
  if (bP->cp - yyLeng < bP->buf) {
    bP->cp= yyShiftBuffer(dP, yyLeng);
  }
  return yyMoveText(dP, bP->cp - yyLeng);
}

/*  ===File: movetext.c===  */
/*

File:	 movetext.c
Purpose: Move yytext routine.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

 
/* Move yytext to dest.  Assumes enough space at dest.  Update yytext 
 * & saveCh.
 * Will work even if dest area overlaps current yytext. 
 */
YYChar *
yyMoveText(dP, dest)
  YYData *dP;
  YYChar *dest;
{
  YYChar *yyText= *dP->yytextP;
  int yyLeng= *dP->yylengP;
  *dP->saveCP= dP->saveCh;
  *dP->yytextP= yyText= yyMoveBytes(dest, yyText, yyLeng);
  dP->saveCP= &yyText[yyLeng]; dP->saveCh= yyText[yyLeng]; yyText[yyLeng]= '\0';
  return yyText;
}
/*  ===File: shiftbuf.c===  */
/*

File:	 shiftbuf.c
Purpose: Shift buffer routine.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/* Shift the current buffer in *dP so as to ensure that there are 
 * nFree unused locations before cp.  Preserves contents of locations
 * in buffer before cp.
 */
YYChar *
yyShiftBuffer(dP, nFree)
  YYData *dP;
  YY_CONST yy_size_t nFree;
{
  YYBufP bP= dP->bufP;
  YYChar *cp= bP->cp;
  YYChar nUsed= bP->end - cp;
  yy_size_t neededSize= nFree + nUsed + 2; /* 2 extra for sentinel. */
  assert(cp - bP->buf < nFree); 	/* Which is why we are shifting. */
  *dP->sentinelP= dP->sentinelSave;
  if (neededSize > bP->size) { /* Grow buffer. */
    if (!(bP->buf= (YYChar *)
	  realloc(bP->buf, (bP->size= neededSize)*sizeof(YYChar)))) {
      yyZlexFatal(*dP->yyerrP, "Could not reallocate buffer.");
    }
  }
  cp= yyMoveBytes(bP->buf + nFree, cp, nUsed);
  bP->end= cp + nUsed;
  bP->cp= cp;
  yyUpdateSentinel(dP, 0);
  return bP->cp;
}

/*  ===File: move.c===  */
/*

File:	 move.c
Purpose: Move bytes routine.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

 
/* Move len bytes from src to dest.  Will work even for overlaps. */
YYChar *
yyMoveBytes(dest, src, len)
  YYChar *YY_CONST dest;
  YY_CONST YYChar *YY_CONST src;
  yy_size_t len;
{
  YY_CONST YYChar *p;
  YYChar *q;
  if (src < dest) { /* Move from hi addresses down to lo addresses. */
    for (p= src + len, q= dest + len; p > src; ) *--q= *--p;
  }
  else if (src > dest) { /* Move from lo addresses up to hi addresses. */
    YY_CONST YYChar *YY_CONST srcEnd= src + len;
    for (p= src, q= dest; p < srcEnd; ) *q++ = *p++;
  }
  return dest;
}

/*  ===File: sentinel.c===  */
/*

File:	 sentinel.c
Purpose: Sentinel processing routines.

Last Update Time-stamp: "97/07/30 08:43:54 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

 
static YY_VOID yyRefreshBuffer YY_PROTO ((YYData *dP));
static YY_VOID yyCheckStack YY_PROTO((YYData *dP, unsigned nStk));
static YY_VOID yyMoreAction YY_PROTO((YYData *dP));
static YY_VOID yyScanInit YY_PROTO((YYData *dP));

typedef enum {
  YY_BUF_SENTINEL= 2 * YY_INIT_SENTINEL,	/* Main buf. overflow.	*/
  YY_MORE_SENTINEL= 2 * YY_BUF_SENTINEL,	/* YYMORE action. 	*/
  YY_STK_SENTINEL= 2 * YY_MORE_SENTINEL		/* Stack overflow.	*/
} SentinelType;

/* 
 * Update sentinel. cp points to next character to be read. 
 * Setting the sentinel for buffer and stack overflow is straightforward:
 * merely set it for the min. of the # of chars which can be processed
 * before overflow.  Setting the sentinel for yymore() is somewhat
 * more complex.  If dP->doMore > 0 on entry, then yymore processing is
 * needed. If dP->doMore == 1, then the yyMore sentinel is set to fire at
 * the next attempt to read a character, and dP->doMore is set to > 1.
 * If dP->doMore > 1 (we must have entered here from input()), then the 
 * yyMore sentinel is set to fire at the second attempt to read a character
 * (thus allowing a character to be read by input()).
 * Routines which call this routine should first restore the char at the
 * sentinel location *BEFORE DOING ANYTHING ELSE* using the assignment:
 * 	*dP->sentinelP= dP->sentinelSave;
 */
YY_VOID 
yyUpdateSentinel(dP, nStk)
  YYData *dP;
  unsigned nStk;
{
  YY_CONST YYBufP bufP= dP->bufP;
  YYChar *YY_CONST cp= bufP->cp;

  /* Compute increment as difference between current cp and location 
   * at sentinel placement. 
   */
  YY_CONST unsigned cpInc= 
    (dP->flags & YY_INIT_SENTINEL) ? 0 /* !! */ : (bufP->end - cp);
  YY_CONST unsigned stkInc= 
    (dP->options & YY_BACKUP_OPTIMIZE_FLAG) ? (dP->stkSize - nStk) : cpInc; 
  YY_CONST unsigned moreInc= 
    dP->doMore ? (dP->doMore == 1 ? 1 - dP->doMore++ : 1) : cpInc + 1;
  unsigned inc= (cpInc < stkInc) ? cpInc : stkInc; /* min(cpInc, stkInc). */
  inc= (moreInc < inc) ? moreInc : inc; /* min(cpInc, stkInc, moreInc). */

  /* Point sentinelP to location where sentinel will be placed. */
  dP->sentinelP= cp + inc; 

  /* Save character at sentinel location. */
  if (dP->sentinelP == dP->saveCP) { 
    dP->sentinelSave= dP->saveCh; 
    dP->saveCh= dP->sentinelChar;
  }
  else {
    dP->sentinelSave= *dP->sentinelP;  
  }

  /* Place sentinel. */
  *dP->sentinelP= dP->sentinelChar;

  /* Record cause for sentinel. */
  dP->flags=
    (dP->flags & YY_INIT_SENTINEL) /* In case, we're called before init. */
    | (inc == moreInc ? YY_MORE_SENTINEL : 0)
    | (inc == cpInc ? YY_BUF_SENTINEL : 0)
    | (((dP->options & YY_BACKUP_OPTIMIZE_FLAG) && inc == stkInc) 
       ? YY_STK_SENTINEL 
       : 0);

  assert(YY_INV(dP));
  YY_VOID_RET();
}

/* Refresh the buffer in dP from the file pointed to by its yyin.  Ensure that
 * contents of yytext is not changed, by moving it if necessary.  Update
 * yytext pointer to point to moved contents.
 */
static YY_VOID
yyRefreshBuffer(dP)
  YYData *dP;
{
  YY_CONST YYChar *yyText= *dP->yytextP;
  YY_CONST int yyLeng= *dP->yylengP;
  YY_CONST YYBufP bufP= dP->bufP;
  YY_CONST YYBool textInBuf= (bufP->buf <= yyText && yyText <= bufP->end);
  YY_CONST yy_size_t textInBufLen= (textInBuf) ? yyLeng : 0;
  YY_CONST YYChar *YY_CONST textEnd= &yyText[textInBufLen];
  YY_CONST YYChar *src= yyText;
  YYChar *dest= bufP->buf;

  assert(bufP->eofState == 0);
  bufP->nRead+= (bufP->end - bufP->buf) - textInBufLen;

  /* Copy yytext to start of buffer. */
  while (src < textEnd) *dest++= *src++;

  /* Ensure space for retained yytext[], YY_BUF_SIZE, plus one
   * extra char. used for sentinel or terminating yytext[], plus
   * one extra char used for terminating "sentinel token".
   */
  if (textInBufLen + YY_BUF_SIZE +  1 + 1 > bufP->size) { /* Grow buffer. */
    yy_size_t newSize= textInBufLen + YY_BUF_SIZE + 1 + 1;
    if (!(bufP->buf= (YYChar *) realloc(bufP->buf, newSize))) {
      yyZlexFatal(*dP->yyerrP, "Out of memory for buffer.");
    }
    bufP->size= newSize;
  } /* if (nFree < YY_BUF_SIZE + 2) grow buffer. */

  { /* Refresh buffer. */
    YY_CONST unsigned long nFree= bufP->size - textInBufLen;
    YY_CONST unsigned long nRead= (nFree/YY_BUF_SIZE)*YY_BUF_SIZE;
    long result;
    assert(nRead >= YY_BUF_SIZE);
    bufP->cp= bufP->buf + textInBufLen;
    do {
      result= dP->read(bufP->cp, nRead);
    } while (result == YY_NULL && !dP->yywrap());
    bufP->eofState= (result == YY_NULL);
    bufP->end= bufP->cp + result; *bufP->end= dP->sentinelChar;
  }

  if (textInBuf) {  /* Update yytext. */
    *dP->yytextP= bufP->buf;
    dP->saveCP= *dP->yytextP + *dP->yylengP;
  }
  YY_VOID_RET();
}


static YY_VOID
yyCheckStack(dP, nStk)
  YYData *dP;
  unsigned nStk;
{
  /* Usually we won't really need to grow the stack even if a STK_SENTINEL
   * fires, as the STK_SENTINEL location is not updated at the start of
   * every token.
   */
  if (nStk >= dP->stkSize) {
    if (!(dP->stk= (YYState *)realloc(dP->stk, dP->stkSize+= STK_SIZE_INC))) {
      yyZlexFatal(*dP->yyerrP, "Out of memory for growing stack.");
    }
  }
  YY_VOID_RET();
}

/* Did yymore() after previous token.  Currently at start of next token. 
 * in inner scanner loop. 
 * NOTE: The stack is automagically adjusted to contain dummy moreLeng 
 * entries, when yyProcessSentinel() returns to yylex().
 */
static YY_VOID
yyMoreAction(dP)
  YYData *dP;
{
  YYChar *moreText= dP->moreText;
  YY_CONST unsigned long moreLeng= dP->moreLeng;
  YYBufP bP= dP->bufP;
  assert(*dP->yylengP == 0); 
  dP->doMore= 0;
  if (moreText + moreLeng != bP->cp) { /* Text not contiguous to cp. */
    if (bP->cp - bP->buf < moreLeng) { /* Not enough space in buffer. */
      yyShiftBuffer(dP, moreLeng);
    }
    moreText= yyMoveBytes(bP->cp - moreLeng, moreText, moreLeng);
  }
  *dP->yytextP= moreText;
  *dP->yylengP= moreLeng;	/* To maintain invariant. */
  YY_VOID_RET();
}

/* Initialize the scanner.  Specifically, initialize NULL file
 * variables, and create a stack (if we are not a code scanner).  If
 * YY_CURRENT_BUFFER is NULL then create and refresh a buffer.
 */
static YY_VOID 
yyScanInit(dP)
  YYData *dP;
{
  assert((dP->flags & YY_INIT_SENTINEL) != 0);
  if (*dP->yyinP == NULL) *dP->yyinP= stdin;
  if (*dP->yyoutP == NULL) *dP->yyoutP= stdout;
  if (*dP->yyerrP == NULL) *dP->yyerrP= stderr;
  if ((dP->options & YY_BACKUP_OPTIMIZE_FLAG)) {
    if (!(dP->stk= (YYState *)
	  malloc((dP->stkSize= STK_SIZE_INC) * sizeof(YYState)))) {
      yyZlexFatal(*dP->yyerrP, "Out of memory for initial stack.");
    }
  }
  dP->flags^= YY_INIT_SENTINEL;
  if (*dP->yyCurrentBufP == NULL) { /* Setup a buffer. */
    YYBuf *bufP= (YYBuf *)yyCreateBuffer(dP, *dP->yyinP, YY_BUF_SIZE);
    yySwitchToBuffer(dP, bufP);
  }
  assert(dP->bufP->buf == dP->bufP->cp);
  *dP->yytextP= dP->bufP->buf;
}

YY_VOID
yyProcessSentinel(dP, nStk)
  YYData *dP;
  unsigned nStk;
{
  assert(dP->bufP->cp == dP->sentinelP);
  assert(*dP->bufP->cp == dP->sentinelChar);

  assert(YY_INV(dP));

  /* If we don't use a stack, then the stack sentinel should never fire. */
  /* Some cc's get upset if assert's args are not on a single line! */
  assert((dP->options & YY_BACKUP_OPTIMIZE_FLAG) ? 1 : (dP->flags & YY_STK_SENTINEL) == 0);

  /* Restore char at previous sentinel. */
  *dP->sentinelP= dP->sentinelSave;

  if ((dP->flags & YY_INIT_SENTINEL)) {
    yyScanInit(dP);
  }
  if ((dP->flags & YY_MORE_SENTINEL)) {
    yyMoreAction(dP);
  }
  if (dP->flags & YY_STK_SENTINEL) {
    yyCheckStack(dP, nStk);
  }
  if (dP->bufP->eofState == 0 && (dP->flags & YY_BUF_SENTINEL)) {
    if (dP->bufP->yyin) { /* File buffer. */
      yyRefreshBuffer(dP);
    }
    else { /* In-memory buffer. */
      dP->bufP->eofState= 1;
    }
  }
  yyUpdateSentinel(dP, nStk);
  YY_VOID_RET();
}

/*  ===File: fatal.c===  */
/*

File:	 fatal.c
Purpose: Fatal error routine.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

 
YY_VOID
yyZlexFatal(err, s)
  FILE *err;
  YY_CONST char *s;
{
  fprintf(err, "%s", s);
  exit(1);
}


/*  ===File: creatbuf.c===  */
/*

File:	 creatbuf.c
Purpose: Create a buffer for a file.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

 
/* Create a buffer with space for at least s chars (have at least
 * YY_TEXT_XPAND extra chars. to allow yytext to be retained. 
 */
YYBufHandle 
yyCreateBuffer(d, f, s)
  YYDataHandle d;
  FILE *f;
  unsigned long s; 
{
  enum {YY_TEXT_XPAND= 120};
  YYData *YY_CONST dP= (YYData *)d;
  YY_CONST yy_size_t size= s + YY_TEXT_XPAND;
  YYChar *buf= (YYChar *) malloc((size) * sizeof(*buf));
  if (buf == 0) 
    yyZlexFatal(*dP->yyerrP, "Out of memory for buffer.");
  return yyMakeBuf(dP, f, buf, size);
}


/*  ===File: swtchbuf.c===  */
/*

File:	 swtchbuf.c
Purpose: Switch current buffer to specified one.

Last Update Time-stamp: "97/07/12 15:53:09 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


YY_VOID
yySwitchToBuffer(d, b)
  YYDataHandle d;
  YYBufHandle b;
{
  YYData *YY_CONST dP= (YYData *)d;
  if (b == NULL) {
    yyZlexFatal(*dP->yyerrP, "Cannot switch to a NULL buffer.");
  }
  else {
    YYBuf *YY_CONST bP0= dP->bufP;		/* Old buffer. */
    YYBuf *YY_CONST  bP1= (YYBufP)b;		/* New buffer. */
    *dP->sentinelP= dP->sentinelSave;
    if (dP->options & YY_LINENO_FLAG) {
      bP0->lineno= *dP->yylinenoP;
      *dP->yylinenoP= bP1->lineno;
    }
    if (*dP->yyCurrentBufP) **dP->yyCurrentBufP= *bP0;
    *bP0= *bP1; *dP->yyCurrentBufP= bP1;
    *dP->yyinP= bP1->yyin;
    yyUpdateSentinel(dP, 0);	
  }
  YY_VOID_RET();
}




/*  ===File: makebuf.c===  */
/*

File:	 makebuf.c
Purpose: Allocate and initialize a buffer for a file or in-memory buffer.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

 
/* Create and initialize a buffer.  If yyin == 0, then it is an in-memory
 * buffer.  Otherwise it is a file buffer.  The memory area for the buffer
 * is a total of size bytes pointed to by mem.
 */
YYBufHandle
yyMakeBuf(dP, yyin, mem, size)
  YYData *dP;
  FILE *yyin;
  YYChar *mem;
  yy_size_t size;
{
  YYBufP bP= (YYBufP) malloc(sizeof(*bP));
  if (bP == 0) {
    yyZlexFatal(*dP->yyerrP, "Out of memory for buffer.");
  }
  bP->yyin= yyin;
  bP->buf= mem;
  bP->size= size;
  bP->nRead= 0;
  bP->intraPos= bP->lastLinePos= bP->firstNLPos= 0;
  bP->lineno= 1;
  bP->hasNL= 0;
  bP->lastCh= '\n';		/* Pretend we're at start of line. */
  bP->eofState= 0; 
  bP->cp= bP->buf; 		/* Point to buffer start. */
  bP->end= (yyin) ? bP->buf : &mem[size - 2];
  return (YYBufHandle)bP;
}
/*  ===File: restart.c===  */
/*

File:	 restart.c
Purpose: Support for yyrestart().

Last Update Time-stamp: "97/07/12 15:52:31 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

 
YY_VOID 
yyRestart(d, fp)
  YYDataHandle d;
  FILE *fp;
{
  YYData *YY_CONST dP= (YYData *)d;
  YYBuf *bP= dP->bufP;
  *dP->sentinelP= dP->sentinelSave;
  bP->yyin= fp;
  bP->eofState= 0; 
  bP->nRead+= bP->end - bP->cp; bP->end= bP->cp;
  yyUpdateSentinel(dP, 0);
  YY_VOID_RET();
}

