/*

File:	 outvec.c
Purpose: Output vector routine used to output tables.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/*

File:	outvec.c


COPYRIGHT_M4

*/

#include "outvec.h"

/* Output elements of a vector, with indexes in the range [0,
 * nEntries), separated by ', 's with no comma after the last element.
 * Each element is output by calling fn with the index of the element.
 * Indents each line by indent and outputs no more than nPerLine
 * elements per line.  Returns without newline being output after
 * last element.
 */
Count 
outVec(outFile, indent, nPerLine, nEntries, fn)
  FILE *outFile;
  Index indent;
  Count nPerLine;
  Count nEntries;
  VOID (*fn)(FILE *outFile, Index index);
{
  Count nLines= 0;
  Index i;
  fprintf(outFile, "%*s", indent, "");
  for (i= 0; i < nEntries; i++) {
    (*fn)(outFile, i);
    if (i != nEntries - 1) {
      fputs(", ", outFile);
      if (i % nPerLine == nPerLine - 1) {
	fprintf(outFile, "\n%*s", indent, ""); nLines++;
      }
    }
  } /*   for (i= 0; i < nEntries; i++) */
  return nLines;
}

/* Output elements of a Index2 vector with indexes in the range [0,
 * nEntries), separated by ', 's with no comma after the last element.
 * Each element is output using "%5d" format.  Indents each line by
 * indent and outputs no more than nPerLine elements per line. Returns
 * without newline being output after last element.
 */
Count 
outIndex2Vec(outFile, indent, nPerLine, nEntries, vec)
  FILE *outFile;
  Index indent;
  Count nPerLine;
  Count nEntries;
  Index2 vec[];
{
  Count nLines= 0;
  Index i;
  fprintf(outFile, "%*s", indent, "");
  for (i= 0; i < nEntries; i++) {
    fprintf(outFile, "%5d", vec[i]);
    if (i != nEntries - 1) {
      fputs(", ", outFile);
      if (i % nPerLine == nPerLine - 1) {
	fprintf(outFile, "\n%*s", indent, ""); nLines++;
      }
    }
  } /*   for (i= 0; i < nEntries; i++) */
  return nLines;
}



