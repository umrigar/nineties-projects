/*

File:	 options.h
Purpose: Provide access to struct for option values.

Last Update Time-stamp: "97/07/10 18:11:06 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _OPTIONS_H
#define _OPTIONS_H

#include "ccport.h"

typedef struct {
  ConstString extPrefix;/* Prefix for external symbols. */
  ConstString filePrefix; /* Prefix for constructing file names. */
  ConstString outName;	/* Output file name for generated parser. */
  ConstString termSuffix; /* Suffix for terminal names. */
  Boolean doDefs;	/* Nonzero to generate YACC definitions. */
  Boolean doGram;	/* Nonzero if reference grammar file required. */
  Boolean doLine;	/* Nonzero if parser should contain #line dirs. */
  Boolean cmdLineDebug;	/* Nonzero if debugging request on cmd. line. */
  Boolean doHTML;	/* Nonzero if html file requested. */
  Boolean doRRLonger;	/* Nonzero if longer rule preferred in RR conflict. */
  Boolean doVerbose;	/* Nonzero if verbose desc. file requested. */
  Boolean doYacc;	/* Nonzero for YACC compatibility. */
} Options;

extern CONST Options *CONST optionsP;

Int processOptions PROTO((Int argc, ConstString argv[]));
VOID setOptions PROTO((ConstString optStr));
VOID forcedOptions PROTO((VOID_ARGS));
Int helpFn PROTO((Int id, VOIDP checkP, VOIDP valP, ConstString argP));

#endif /* ifndef _OPTIONS_H */
