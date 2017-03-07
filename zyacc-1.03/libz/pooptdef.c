/*

File:	 pooptdef.c
Purpose: Setup option defaults.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "optp.h"

#include "memalloc.h"

OptErr
parseOptDefaults(optContextP, optTab, nEntries)
  CONST OptContext *CONST optContextP;
  CONST OptInfo *CONST optTab;
  unsigned nEntries;
{
  Index i;
  for (i= 0; i < nEntries; i++) {
    if (optTab[i].doc) {
      ConstString tabP= strchr(optTab[i].doc, '\t');
      if (tabP) {
	ConstString argP= tabP[1] == '\0' ? NULL : tabP + 1;
	CONST OptErr err= 
	  (optTab[i].optFn)(optTab[i].id, optTab[i].checkP, optTab[i].valP, 
			    argP);
	if (err != NO_OPT_ERR) return err;
      }
    }
  }
  return NO_OPT_ERR;
}
