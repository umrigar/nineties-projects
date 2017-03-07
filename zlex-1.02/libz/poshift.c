/*

File:	 poshift.c
Purpose: Parse options, shifting non-option arguments.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "optp.h"

static Index shiftNonOptArgs PROTO((ConstString argv[], Index lo, Index mid,
				    Index hi));

/* Given segments [lo, mid) and [mid, hi) in array argv, interchange
 * the two segments: i.e. the entries originally at [mid, hi) are
 * moved to [lo, lo + hi - mid) and the entries originally in [lo,
 * mid] are moved to [lo + hi - mid, hi).  Return the start index of
 * the moved 1st segment.  Not particularly efficient (quadratic), but
 * should be called rarely.
 */
static Index
shiftNonOptArgs(argv, lo, mid, hi)
  ConstString argv[];
  Index lo;
  Index mid; 
  Index hi;
{
  CONST Count nSeg1= mid - lo;	/* # of entries in first segment. */
  Index i;
  for (i= 0; i < nSeg1; i++) { /* Rotate [lo, hi) left by 1 position. */
    ConstString arg= argv[lo];
    Index j;
    for (j= lo + 1; j < hi; j++) {
      argv[j - 1]= argv[j];
    }
    argv[hi - 1]= arg;
  }
  return hi - nSeg1;
}

/* Parse options in argv (with argc entries) starting from argv[1],
 * according to table optTab having nEntries.  argv[] is reorganized
 * so that all non-option arguments are shifted to its hi end.  If
 * there are no errors, then the return value is > 0 and is the number
 * of the first non-option argument (argc if none).  If an error
 * occurs, then a negative encoded value is returned where the
 * retValue is 
 * 
 *   -((argN<<(OPT_ERR_BIT + 1)) | (nextArg << OPT_ERR_BIT) | errCode) 
 *
 * where errCode is the OptErr value, argN is the argument # in argv[]
 * for the options in which the error occurred and nextArg is 1 iff the
 * error is in the option value specified in argv[argN + 1].
 */
int 
parseOpt(optContextP0, argc, argv, optTab, nEntries)
  CONST OptContext *CONST optContextP0;
  int argc;
  ConstString argv[];
  CONST OptInfo *CONST optTab;
  unsigned nEntries;
{
  OptContext optContext;
  OptContext *CONST optContextP= &optContext;
  Index nonOptArgNLo= argc;
  Index nonOptArgNHi= argc;
  Index argN;
  _ZinitOptContext(optContextP0, optContextP);
  for (argN= 1; argN < argc; argN++) {
    Int parseResult= _ZparseOptTillNonOpt(optContextP, argc, argv, argN, 
					  optTab, nEntries);
    if (parseResult < 0) {
      return parseResult;
    }
    else { /* We have a non-option argument. */
      argN= parseResult;
      if (nonOptArgNLo == nonOptArgNHi) { /* 1st non-option arg. */
	nonOptArgNLo= argN; nonOptArgNHi= argN + 1;
      }
      else {
	nonOptArgNLo= shiftNonOptArgs(argv, nonOptArgNLo, nonOptArgNHi, argN);
	nonOptArgNHi= argN + 1;
      }
    }
  }
  return nonOptArgNLo;
}
