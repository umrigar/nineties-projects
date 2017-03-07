/*

File:	 outvec.c
Purpose: Output vector of ints or strings.

Last Update Time-stamp: "97/07/30 08:40:49 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "outvec.h"

/* System includes via port.h. */

/* This macro is used to centralize the logic for outputting
 * separators between vector elements.  Output no more than nPerRow
 * elements per line.  Each non-last element is followed either by 
 * ", \n" catenated with indent, or by ", " depending on whether or not
 * it is the last element on a line.  The last element is followed by
 * a '\n' if there are more than nPerRow elements; otherwise by ' '. 
 */
#define ELEMENT_SEPARATOR(outFile, index, size, nPerRow, nlCount, indent)\
  do {									\
    if ((index) == (size) - 1) { /* Last element. */			\
      fputs((index) < (nPerRow) ? " " : ((nlCount)++ , "\n"), outFile);	\
    }									\
    else {								\
      fputs(", ", outFile);						\
      if ((index) % (nPerRow) == (nPerRow) - 1) { 			\
	fputs("\n", outFile); (nlCount)++; fputs(indent, outFile);	\
      }									\
    }									\
  } while (0)

#define ACCESS_PAREN(f, i) 	(f(i))
#define ACCESS_BRACKET(b, i)	(b[i])

/* The following macro should be used to define the body of a function
 * f having the following prototype: 
 *   Count f(FILE *outFile, ConstString hdr, Count nEntries, access) 
 * where access is either a ptr to an integer type (in which case the
 * macro argument delimiter below should be BRACKET) or a ptr. to a
 * function (*)(Index) with appropriate return value (in which case
 * the macro argument delimiter below should be PAREN). 
 *
 * *** NOTE: In actual args, to macro, don't have space before delimiter
 * as some brain-damages non-ANSI cc's have trouble with token pasting.
 */
#define OUT_VEC_BODY(fmt, nPerLine, delimiter)				\
  do {									\
    Count nlCount= 0;							\
    if (nEntries != 0) {						\
      ConstString indent= "  "; /* Indent at start of each line. */	\
      Index i;								\
      fprintf(outFile, "static %s[%d]= {\n%s", 				\
	      hdr, nEntries, indent);					\
      nlCount++;							\
      for (i= 0; i < (nEntries); i++) {					\
  	fprintf(outFile, fmt, TOK_CAT(ACCESS_,delimiter)(access, i));	\
  	ELEMENT_SEPARATOR(outFile, i, nEntries, 			\
	                  nPerLine, nlCount, indent);			\
      }									\
      fputs("};\n", outFile); nlCount++;				\
    } /* else (nEntries != 0) */					\
    return nlCount;							\
  } while (0)

Count 
outIValVec(outFile, hdr, nEntries, access)
  FILE *outFile;
  ConstString hdr;
  Count nEntries;
  CONST Index2 access[];
{
  OUT_VEC_BODY("%5d", 10,BRACKET);	/* No space before BRACKET. */
  return 0;
}

Count 
outIFnVec(outFile, hdr, nEntries, access)
  FILE *outFile;
  ConstString hdr;
  Count nEntries;
  Index (*access) PROTO((Index));
{
  OUT_VEC_BODY("%5d", 10,PAREN);	/* No space before PAREN. */
  return 0;
}

Count 
outSFnVec(outFile, hdr, nEntries, access)
  FILE *outFile;
  ConstString hdr;
  Count nEntries;
  ConstString (*access) PROTO((Index));
{
  OUT_VEC_BODY("\"%s\"", 4,PAREN);	/* No space before PAREN. */
  return 0;
}


