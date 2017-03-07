/*

File:	 ssstk.c
Purpose: Support for start-state stacks.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"

YY_VOID
yyPushState(d, newState)
  YYDataHandle d;
  YYState newState;
{
  enum { STK_SIZE_INC= 8 };
  YYData *YY_CONST dP= (YYData *)d;
  if (!dP->ssStkP) {
    dP->ssStkP= (YYSSStack *)malloc(sizeof(YYSSStack));
    if (!dP->ssStkP) {
      yyZlexFatal(*dP->yyerrP, "Out of memory for start-state stack.");
    }
    dP->ssStkP->base= (YYState *)malloc(STK_SIZE_INC * sizeof(YYState));
    if (!dP->ssStkP->base) {
      yyZlexFatal(*dP->yyerrP, "Out of memory for start-state stack.");
    }
    dP->ssStkP->size= STK_SIZE_INC; dP->ssStkP->index= 0;
  }
  if (dP->ssStkP->index >= dP->ssStkP->size) {
    dP->ssStkP->base= (YYState *)
      realloc(dP->ssStkP->base, 
	      (dP->ssStkP->size+= STK_SIZE_INC) * sizeof(YYState));
    if (!dP->ssStkP->base) {
      yyZlexFatal(*dP->yyerrP, "Out of memory for start-state stack.");
    }
  }
  dP->ssStkP->base[dP->ssStkP->index++]= dP->startState;
  dP->startState= newState;
}

YY_VOID
yyPopState(d)
  YYDataHandle d;
{
  YYData *YY_CONST dP= (YYData *)d;
  if (!dP->ssStkP || dP->ssStkP->index == 0) {
    yyZlexFatal(*dP->yyerrP, "Attempt to pop empty start-state stack.");
  }
  dP->startState= dP->ssStkP->base[--dP->ssStkP->index];
}

YYState
yyTopState(d)
  YYDataHandle d;
{
  YYData *YY_CONST dP= (YYData *)d;
  if (!dP->ssStkP || dP->ssStkP->index == 0) {
    yyZlexFatal(*dP->yyerrP, "Attempt to read empty start-state stack.");
  }
  return dP->ssStkP->base[dP->ssStkP->index - 1];
}

