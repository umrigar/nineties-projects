/*

File:	 lalr.h
Purpose: Parser generation interface.

Last Update Time-stamp: "97/07/10 18:07:51 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _LALR_H

#include "port.h"

#include <stdio.h>

VOID setExpect PROTO((unsigned nExpected));
VOID doLALR PROTO((VOID_ARGS));
VOID doHTML PROTO((ConstString inName, ConstString outName));
VOID doVerbose PROTO((ConstString inName, ConstString outName));
Count outGramTabs PROTO((FILE *outFile));
Count outLALRParams PROTO((FILE *outFile));
VOID doVCG PROTO((FILE *outFile));
Index nextNonTermItem PROTO((Index state, Index *cursorP));
Count lalrNStates PROTO((VOID_ARGS));
Count maxNonTermItems PROTO((VOID_ARGS));

#endif
