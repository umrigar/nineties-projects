/*

File:	 poparse.c
Purpose: Parse options.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "optp.h"

static Size longOptLen PROTO((ConstString optName));

static Int processOpt PROTO((CONST OptContext *CONST optContextP, 
			     Int argc, ConstString argv[], 
			     CONST OptInfo *CONST optTab, UInt nEntries, 
			     Index argN, Int optN, ConstString shortOptP,
			     Boolean *nextArgP));

/* Return the length of long option name optName (optName must point to
 * the actual name past the OPTION_CHARs. 
 */
static Size 
longOptLen(optName)
  ConstString optName;
{
  ConstString p;
  assert(IS_LONG_OPT_CHAR(optName[0]));
  for (p= optName + 1; IS_LONG_OPT_CHAR(*p); p++) ;
  return p - optName;
}

/* Returns the index of short option optChar in options table optTab.
 * Returns -ve error code on error.
 */
Int
_ZfindShortOpt(optChar, optTab, nEntries)
  Int optChar;
  CONST OptInfo *optTab;
  UInt nEntries;
{
  Index i;
  for (i= 0; i < nEntries; i++) {
    if (optTab[i].shortOpt != '\0' && optTab[i].shortOpt == optChar) return i;
  }
  return -BAD_OPT_ERR;
}

/* Returns the index of long option optName in options table optTab.
 * Returns -ve error code on error.
 */
Int
_ZfindLongOpt(optName, optTab, nEntries)
  ConstString optName;
  CONST OptInfo *CONST optTab;
  UInt nEntries;
{
  Int lastMatch= -BAD_OPT_ERR;
  Count nMatch= 0;
  CONST Size optNameLen= longOptLen(optName);
  Index i;
  for (i= 0; i < nEntries; i++) {
    if (optTab[i].longOpt && 
	strncmp(optTab[i].longOpt, optName, optNameLen) == 0) {
      nMatch++;
      if (nMatch > 1) {
	return -AMBIGUOUS_OPT_ERR;
      }
      else {
	lastMatch= i;
      }
    }
  }
  return lastMatch;
}

static Int 
processOpt(optContextP, argc, argv, optTab, nEntries, argN, optN, shortOptP,
	   nextArgP)
  CONST OptContext *CONST optContextP;
  Int argc;
  ConstString argv[];
  CONST OptInfo *CONST optTab;
  UInt nEntries;
  Index argN;
  Int optN;
  ConstString shortOptP;
  Boolean *nextArgP;
{
  Int retVal= NO_OPT_ERR;
  *nextArgP= FALSE;
  if (optN < 0) retVal= -optN;
  else {
    CONST Boolean isLongOpt= (shortOptP == NULL);
    ConstString arg= argv[argN];
    ConstString nameEnd= 
      (isLongOpt) ? arg + 2 + longOptLen(arg + 2) : shortOptP + 1;
    CONST Boolean valInArg= 
      (isLongOpt) ? *nameEnd == ARG_CHAR : *nameEnd != '\0';
    ConstString val= NULL;
    CONST OptInfo *oP;
    while ((optTab[optN].flags & ARG_TYPE_MASK) == SYNONYM_OPT_FLAG) optN++;
    oP= &optTab[optN];
    switch (oP->flags & ARG_TYPE_MASK) {
      case NO_OPT_FLAG:
	if (isLongOpt && valInArg) retVal= UNEXPECTED_VAL_OPT_ERR;
	break;
      case OPTIONAL_OPT_FLAG:
	if (valInArg) val= nameEnd + isLongOpt;
	break;
      case REQUIRED_OPT_FLAG:
	if (valInArg) {
	  val= nameEnd + isLongOpt;
	}
	else if (argN < argc - 1) {
	  val= argv[argN + 1]; *nextArgP= TRUE;
	}
	else {
	  retVal= EXPECTED_VAL_OPT_ERR;
	}
	break;
      default:
	assert(0);
	break;
    } /* switch */
    if (retVal == NO_OPT_ERR && 
	(!optContextP->preFn || !optContextP->preFn(optTab[optN].id, val))) {
      ConstString xVal= 
	(val && optContextP->internFn && (oP->flags & INTERN_OPT_FLAG) != 0)
	? optContextP->internFn(val, strlen(val))
	: val;
      retVal= oP->optFn(oP->id, oP->checkP, oP->valP, xVal);
      if (retVal == NO_OPT_ERR && optContextP->postFn) {
	optContextP->postFn(oP->id, val);
      }
    }
    if (retVal == NO_OPT_ERR && valInArg && !isLongOpt &&
	(oP->flags & ARG_TYPE_MASK) == NO_OPT_FLAG) {
      /* Recursive call to allow multiple short options without option
       * values in a single word.
       */
      Int optN1= _ZfindShortOpt(*++shortOptP, optTab, nEntries);
      retVal= processOpt(optContextP, argc, argv, optTab, nEntries, argN,
			 optN1, shortOptP, nextArgP);
    }
  } /* else optN >= 0 */
  return retVal;
}

/* Set all values in *optContextP to defaults. */
VOID 
setOptContextDefaults(optContextP)
  OptContext *CONST optContextP;
{
  optContextP->helpFile= stdout;
  optContextP->errFile= stderr;
  optContextP->internFn= NULL;
  optContextP->preFn= NULL;
  optContextP->postFn= NULL;
  VOID_RET();
}

VOID
_ZinitOptContext(optContextP0, optContextPZ)
  CONST OptContext *CONST optContextP0;
  OptContext *CONST optContextPZ;
{
  if (!optContextP0) {
    setOptContextDefaults(optContextPZ);
  }
  else {
    *optContextPZ= *optContextP0;
    if (!optContextPZ->helpFile) optContextPZ->helpFile= stdout;
    if (!optContextPZ->errFile) optContextPZ->errFile= stderr;
  }
  VOID_RET();
}

/* Parse options in argv (with argc entries) starting from argv[argIndex],
 * according to table optTab having nEntries.  Options are parsed until
 * any one of the following conditions becomes true:  the end of argv[]
 * is encountered; an error is detected; a non-option argument is detected.
 * If there are no errors, then the return value is > 0 and is the number
 * of the first non-option argument (argc if none).  If an error
 * occurs, then a negative encoded value is returned: the ERR_RET_*
 * macros should be used to decode the return value.
 */
Int 
_ZparseOptTillNonOpt(optContextP, argc, argv, argIndex, optTab, nEntries)
  CONST OptContext *CONST optContextP;
  Int argc;
  ConstString argv[];
  Int argIndex;
  CONST OptInfo *CONST optTab;
  unsigned nEntries;
{
  Index argN;
  assert(argIndex < argc);
  for (argN= argIndex; argN < argc; argN++) {
    ConstString arg= argv[argN];
    Int argErr= NO_OPT_ERR;
    Boolean nextArg= FALSE;
    if (IS_SHORT_OPT(arg)) {
      argErr= processOpt(optContextP, argc, argv, optTab, nEntries, argN,
			 _ZfindShortOpt(arg[1], optTab, nEntries), &arg[1],
			 &nextArg);
    }
    else if (IS_LONG_OPT(arg)) {
      argErr= processOpt(optContextP, argc, argv, optTab, nEntries, argN,
			 _ZfindLongOpt(&arg[2], optTab, nEntries), NULL,
			 &nextArg);
    }
    else if (IS_TERM_OPT(arg)) {
      return argN + 1;
    }
    else { /* Non-option argument. */
      return argN;
    }
    if (argErr != NO_OPT_ERR) return CONS_ERR_RET(argN, nextArg, argErr);
    if (nextArg) argN++;
  }
  return argc;
}

/* Parse options in argv (with argc entries) starting from argv[argIndex],
 * according to table optTab having nEntries.  Options are parsed until
 * any one of the following conditions becomes true:  the end of argv[]
 * is encountered; an error is detected; a non-option argument is detected.
 * If there are no errors, then the return value is > 0 and is the number
 * of the first non-option argument (argc if none).  If an error
 * occurs, then a negative encoded value is returned: the ERR_RET_*
 * macros should be used to decode the return value.
 */
Int 
parseOptTillNonOpt(optContextP0, argc, argv, argIndex, optTab, nEntries)
  CONST OptContext *CONST optContextP0;
  Int argc;
  ConstString argv[];
  Int argIndex;
  CONST OptInfo *CONST optTab;
  unsigned nEntries;
{
  OptContext optContext;
  OptContext *CONST optContextP= &optContext;
  _ZinitOptContext(optContextP0, optContextP);
  return _ZparseOptTillNonOpt(optContextP, argc, argv, argIndex, 
			      optTab, nEntries);
}


