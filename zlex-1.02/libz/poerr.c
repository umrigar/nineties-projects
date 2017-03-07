/*

File:	 poerr.c
Purpose: Options error routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "optp.h"

/* Print an appropriate error message corresponding to errRetValue.
 * If optHelp is non-NULL, then print help for the specified option
 * which is in error (if it can be identified), followed by the 
 * contents of optHelp followed by a newline.
 */
VOID 
parseOptErr(optContextP0, argc, argv, errRetValue, optHelp, optTab, nEntries)
  CONST OptContext *optContextP0;
  Int argc;
  ConstString argv[];
  Int errRetValue;
  ConstString optHelp;
  CONST OptInfo *optTab;
  unsigned nEntries;
{
  OptContext optContext;
  OptContext *CONST optContextP= &optContext;
  CONST OptErr errCode= ERR_RET_ERRN(errRetValue);
  CONST Index argN= ERR_RET_ARGN(errRetValue);
  CONST Boolean nextArg= ERR_RET_NEXTARG(errRetValue);
  FILE *errFile;
  _ZinitOptContext(optContextP0, optContextP);
  errFile= optContextP->errFile;
  assert(errRetValue < 0);
  switch (errCode) {
    ConstString errType;
    case AMBIGUOUS_OPT_ERR:
      fprintf(errFile, "ambiguous option name in `%s'.\n",
	      argv[argN]);
      break;
    case BAD_OPT_ERR:
      fprintf(errFile, "unknown option name in `%s'.\n", 
	      argv[argN]);
      break;
    case EXPECTED_VAL_OPT_ERR:
      fprintf(errFile, "option value for option `%s' not found.\n", 
	      argv[argN]);
      break;
    case UNEXPECTED_VAL_OPT_ERR:
      fprintf(errFile, "unexpected option value in option `%s'.\n", 
	      argv[argN]);
      break;
    case AMBIGUOUS_VAL_OPT_ERR:
      errType= "ambiguous";
      goto checkOptErr;
    case RANGE_VAL_OPT_ERR:
      errType= "range error in";
      goto checkOptErr;
    case CHECK_OPT_ERR: 
      errType= "invalid";
    checkOptErr:
      if (nextArg) {
	fprintf(errFile, 
		"%s option value `%s' for option `%s'.\n",
		errType, argv[argN + 1], argv[argN]);
      }
      else {
	fprintf(errFile, "%s option value in `%s'.\n", 
		errType, argv[argN]);
      }
      break;
    default:
      assert(0);
      break;
  } /* switch (errCode) */
  if (optHelp) {
    if (errCode != AMBIGUOUS_OPT_ERR && errCode != BAD_OPT_ERR) {
      ConstString arg= argv[argN];
      CONST Int optN= (IS_SHORT_OPT(arg))
	              ? _ZfindShortOpt(arg[1], optTab, nEntries)
	              : _ZfindLongOpt(&arg[2], optTab, nEntries);
      assert(optN >= 0);
      _ZoptHelp(optContextP, optN, optTab, nEntries);
    }
    fprintf(optContextP->helpFile, "%s\n", optHelp);
  }
  VOID_RET();
}


