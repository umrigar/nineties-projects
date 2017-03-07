/*

File:	 sentinel.c
Purpose: Sentinel processing routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
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
  VOID_RET();
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
  VOID_RET();
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
  VOID_RET();
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
  VOID_RET();
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
  assert((dP->options & YY_BACKUP_OPTIMIZE_FLAG) \
         ? 1 \
         : (dP->flags & YY_STK_SENTINEL) == 0);

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
  VOID_RET();
}

