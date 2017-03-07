#include "ccport.h"

typedef struct {
  const unsigned hi;
  const char *const fName;
} FileNameInfo;

typedef struct {
  const unsigned *const counters;
  const FileNameInfo *const fNames;
  const unsigned *const lineNums;
  const unsigned nCounters;
  unsigned isInit;
} Prof;

#define OUT_FILE_NAME	"zprof.out"

void 
_bbProfOut(profP)
  const Prof *const profP;
{
  Count n= profP->nCounters;
  if (n > 0) {
    FILE *outFile= fopen(OUT_FILE_NAME, "a");
    CONST FileNameInfo * fP= profP->fNames;
    CONST unsigned *CONST linesP= profP->lineNums;
    CONST unsigned *CONST countersP= profP->counters;
    Index i= 0;
    if (!outFile) {
      fprintf(stderr, 
	      "could not open profile output file %s\n", OUT_FILE_NAME);
      return;
    }
    do {
      ConstString fName= fP->fName;
      CONST Index hi= fP->hi;
      for (; i <  hi; i++) {
	fprintf(outFile, "%s:%d: %d\n", fName, linesP[i], countersP[i]);
      }
      if (hi == n) break;
      fP++;
    } while (TRUE);
    fclose(outFile);
  } /* if (n > 0) */
}
