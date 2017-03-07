/*

File:	 pohelp.c
Purpose: Options help routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "optp.h"

Index 
_ZoptHelp(optContextP, indexX, optTab, nEntries)
  CONST OptContext *CONST optContextP;
  CONST unsigned indexX;
  CONST OptInfo *CONST optTab;
  unsigned nEntries;
{
  CONST Index lo= indexX;
  Index hi;
  ConstString tabP;
  for (hi= lo; (optTab[hi].flags & ARG_TYPE_MASK) == SYNONYM_OPT_FLAG; hi++) ;
  tabP= strchr(optTab[hi].doc, '\t');
  if (tabP) {
    fprintf(optContextP->helpFile, "%.*s", 
	    tabP - optTab[hi].doc, optTab[hi].doc);
  }
  else {
    fprintf(optContextP->helpFile, "%s", optTab[hi].doc);
  }
  return hi;
}

VOID 
optHelp(optContextP0, indexX, optTab, nEntries)
  CONST OptContext *CONST optContextP0;
  CONST unsigned indexX;
  CONST OptInfo *CONST optTab;
  unsigned nEntries;
{
  OptContext optContext;
  OptContext *CONST optContextP= &optContext;
  _ZinitOptContext(optContextP0, optContextP);
  _ZoptHelp(optContextP, indexX, optTab, nEntries);
  VOID_RET();
}

VOID
allOptsHelp(optContextP0, intro, optTab, nEntries)
  CONST OptContext *CONST optContextP0;
  CONST char *intro;
  CONST OptInfo *CONST optTab;
  unsigned nEntries;
{
  OptContext optContext;
  OptContext *CONST optContextP= &optContext;
  unsigned i;
  _ZinitOptContext(optContextP0, optContextP);
  fprintf(optContextP->helpFile, "%s\n", intro);
  for (i= 0; i < nEntries; i++) {
    i= _ZoptHelp(optContextP, i, optTab, nEntries);
  }
  VOID_RET();
}
