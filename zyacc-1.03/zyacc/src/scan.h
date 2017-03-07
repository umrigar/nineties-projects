/*

File:	 scan.h
Purpose: Scanner interface.

Last Update Time-stamp: "97/07/18 13:14:23 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _SCAN_H
#define _SCAN_H

#include "port.h"

#include <stdio.h>

extern char *yytext;

/* Scanning routines. */

typedef struct {	/* action semantics */
  Index2 actN;		/* action # */
  Index2 lineNum;	/* line # at which action occurs */
} ActSem;	

/* Initialization. */ 
VOID initScan PROTO((Int argc, Int argn, ConstString argv[]));

/* Main scanning routine. */
Int scan PROTO((VOID_ARGS));

/* Action routines. */
Index newAct PROTO((VOID_ARGS));
Count nActions PROTO((VOID_ARGS));
VOID printAct PROTO((FILE *outFile, Index actN));
VOID setActReduction PROTO((Index actN, Index reductN));
VOID addActAttribAssign PROTO((Index ruleN, Index lhsIDVar, 
			Int lhsOffset, Index rhsID));
Count nActions PROTO((VOID_ARGS));

/* Output routines. */
Count outActCases PROTO((FILE *outFile));
VOID printAct PROTO((FILE *outFile, Index actN));
Count outDefs PROTO((FILE *outFile));
Count outLocals PROTO((FILE *outFile));
Count outYYSTYPE PROTO((FILE *outFile));
Count outSec3 PROTO((FILE *outFile));
Count outScanParams PROTO((FILE *outFile));
Count lineDir PROTO((FILE *outFile, Index lineN));

unsigned errorLineN PROTO((VOID_ARGS));
ConstString errorSrc PROTO((VOID_ARGS));

int yywrap PROTO((VOID_ARGS));

#endif
