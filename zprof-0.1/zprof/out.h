/*

File:	 out.h
Purpose: Output routines

Last Update Time-stamp: <97/06/24 09:25:08 zdu>

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _OUT_H
#define _OUT_H

#include "ccport.h"

#include "scan.h"

typedef enum {
  NO_COUNTER,		/* no counter needed */
  COMPOUND_COUNTER,	/* counter at start of compound */
  COND_COUNTER,		/* counter for ?: conditional expressions */
  FN_COUNTER,		/* counter for function entry */
  STMT_COUNTER		/* counter for statements */
} CounterType;

VOID initOut PROTO((VOID_ARGS));
VOID terminateOut PROTO((VOID_ARGS));

VOID beginOutFile PROTO((ConstString fName));
VOID endOutFile PROTO((VOID_ARGS));

VOID putOut PROTO((CONST TokSem *CONST tokSemP));

VOID beginCounter PROTO((CounterType counterType));
VOID endCounter PROTO((CounterType counterType));
  


#endif /* ifndef _OUT_H */
