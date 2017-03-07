/*

File:	 options.h
Purpose: Provide access to struct for option values.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _OPTIONS_H
#define _OPTIONS_H

#include "port.h"

enum {
  NO_COMPRESS, COMB_COMPRESS, ITERATIVE_COMPRESS
};

enum {
  ADDRESS_TABLE, DIFF_TABLE, STATE_TABLE
};

typedef struct {		/* USER MODULES. */
  ConstString outFileName;	/* main */
  ConstString prefix;		/* dfa */
  ConstString traceFileName;	/* main */
  Count charSetSize;		/* eqclass, dfa, parse, patterns */
  unsigned binCodeParam;	/* codescan */
  unsigned linCodeParam;	/* codescan */
  unsigned sentinelChar;	/* dfa, patterns */
  int colWastePercent;		/* outdfa */
  int compress;			/* dfa */
  int tableEntry;		/* outdfa */
  Index1 defaultAct;		/* main */
  Boolean align;		/* main */
  Boolean backupOptimize;	/* codescan, dfa, outdfa */
  Boolean doArray;		/* main */
  Boolean doCode;		/* codescan */
  Boolean doDebug; 		/* dfa */
  Boolean doReject;		/* dfa, main */
  Boolean doStdio;		/* main */
  Boolean doYYLineNo;		/* patterns */
  Boolean dummy;		/* For deprecated options. */
  Boolean equivClasses;		/* eqclass */
  Boolean ignoreCase;		/* parse */
  Boolean isVerbose;		/* main */
  Boolean lexCompat;		/* main, scan. */
  Boolean lineDir;		/* scan */
  Boolean toStdOut;		/* main */
  Boolean transitionCompress;	/* codescan */
  Boolean wsInPatterns;		/* scan */
} Options;

extern CONST Options *CONST optionsP;

Int processOptions PROTO((Int argc, ConstString argv[]));
VOID setOptions PROTO((ConstString optStr));
VOID forcedOptions PROTO((VOID_ARGS));
Int helpFn PROTO((Int id, VOIDP checkP, VOIDP valP, ConstString argP));

#endif /* ifndef _OPTIONS_H */
