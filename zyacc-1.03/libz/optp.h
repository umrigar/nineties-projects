/*

File:	 optp.h
Purpose: Private interface for option parsing routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _OPTP_H
#define _OPTP_H

#include "parseopt.h"


enum { 
  OPT_CHAR= '-', 	/* Starts a option. */
  ARG_CHAR= '=', 	/* Separates a long option from its argument. */
  ARG_TYPE_MASK= 3	/* Used for masking out arg type from flags. */
};

#define OPT_CHAR_STRING "-"

/* A character which may occur within a long option. */
#define IS_LONG_OPT_CHAR(c) (isalnum(c) || c == '_' || c == '-') 

/* Recognize a short option. */
#define IS_SHORT_OPT(arg)						\
  ((arg)[0] == OPT_CHAR && (arg)[1] != '\0' && (arg)[1] != OPT_CHAR)

/* Recognize a long option. */
#define IS_LONG_OPT(arg)						\
  ((arg)[0] == OPT_CHAR && (arg)[1] == OPT_CHAR && IS_LONG_OPT_CHAR((arg)[2]))

/* The option '--' terminates all option processing. */
#define IS_TERM_OPT(arg)						\
  ((arg)[0] == OPT_CHAR && (arg)[1] == OPT_CHAR && (arg)[2] == '\0')

#define GET_OPT(argc, argv, argN) \
  ( ((argN) >= (argc)) ? (char *) 0 : ((argv)[argN]) )

#define CONS_ERR_RET(argN, nextArg, errN)				\
  (-(Int)((argN) << (OPT_ERR_BIT + 1) | ((nextArg) << OPT_ERR_BIT) | (errN)))


Int _ZfindShortOpt PROTO((Int optChar, CONST OptInfo *CONST optTab, 
			  UInt nEntries));
Int _ZfindLongOpt PROTO((ConstString optionName, CONST OptInfo *CONST optTab, 
			 UInt nEntries));
VOID _ZinitOptContext PROTO((CONST OptContext *CONST optContextP0,
			     OptContext *CONST optContextPZ));

Index _ZoptHelp PROTO((CONST OptContext *CONST optContextP,
		       CONST unsigned indexX, 
		       CONST OptInfo *CONST optTab, unsigned nEntries));

OptErr _ZcheckOptLong PROTO((long val, CONST long *checkP));

Int _ZparseOptTillNonOpt 
  PROTO((CONST OptContext *CONST optContextP,
	 Int argc, ConstString argv[], Int argIndex,
	 CONST OptInfo *CONST optTab, unsigned nEntries));

/* Macro to define some simple option functions. */
#define OPT_FN(type, nullRet) 						\
int 									\
TOK_CAT(TOK_CAT(type,OptFn),nullRet) (id, checkP, valP, argP)		\
  int id;								\
  VOIDP checkP;								\
  VOIDP valP;		     						\
  ConstString argP;							\
{									\
  long v;								\
  if (argP) {								\
    OptErr err;								\
    char *endP;								\
    v= strtol(argP, &endP, 0);						\
    if (*endP != '\0') return CHECK_OPT_ERR;				\
    err= _ZcheckOptLong(v, checkP); 					\
    if (err) return err;						\
  }									\
  else	{								\
    v= nullRet;								\
  }									\
  *((type *) valP)= (type) v;						\
  return NO_OPT_ERR;							\
}

#endif /* ifndef _OPTP_H */
