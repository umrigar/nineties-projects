/*

File:	 utopen.h
Purpose: Safe-open of a file for writing.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "error.h"
#include "zutil.h"

enum {
  LINE_LEN= 120,
  NLINES= 5
};

/* Open file fName for writing: if it exists and is not a /dev file,
 * then it opens it with truncation only if prgName occurs within the
 * first 5 lines (each line containing upto 120 chars) of it.  Uses
 * the "error" library module which should have been previously
 * initialized.
 */
FILE *
safeOpen(fName, prgName)
  ConstString fName;
  ConstString prgName;
{
  FILE *f;
  if (strncmp("/dev", fName, 4) != 0) {
    f= fopen(fName, "r");
    if (f) {
      char line[LINE_LEN];
      ConstString p;
      Index i;
      for (i= 0; i < NLINES && (p= fgets(line, LINE_LEN, f)); i++) {
	if (strstr(p, prgName)) break;
      }
      fclose(f);
      if (p == NULL || i == NLINES) {
	fatal("File `%s' does not appear to have been generated by %s.\n",
	      fName, prgName);
      }
    }
  }
  f= fopen(fName, "w");
  if (!f) FILE_ERROR(fName);
  return f;
}

