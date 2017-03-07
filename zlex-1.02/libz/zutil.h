/*

File:	 zutil.h
Purpose: Utility routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _UTIL_H
#define _UTIL_H

#include "ccport.h"

/* Return ceiling(lg2(n)). Returns 0 if n == 0. */
unsigned ceilingLg2 PROTO((unsigned n));

/* Return floor(lg2(n)). Returns 0 if n == 0. */
unsigned floorLg2 PROTO((unsigned n));

/* Change the suffix of a string from one of several to a specified
 * value.  Specifically find a suffix of str which matches a suffix in
 * the NULL-terminated array suffixVec (if several match different
 * suffixes the suffix which occurs earliest in suffixVec is chosen).
 * If none of the strings in suffixVec match a suffix of str, then the
 * empty suffix is chosen.  Remove the chosen suffix from str and add
 * newSuffix, returning the new name in dynamically allocated memory
 * which can subsequently be FREE'd.  This routine can be used to
 * change file extensions.
 */
ConstString chStrSuffix PROTO((ConstString str, ConstString suffixVec[],
			       ConstString newSuffix));

/* Open file fName for writing: if it exists and is not a /dev file,
 * then it opens it with truncation only if prgName occurs within the
 * first 5 lines (each line containing upto 120 chars) of it.  Uses
 * the "error" library module which should have been previously
 * initialized.
 */
FILE *safeOpen PROTO((ConstString fName, ConstString prgName));

/* 

Open file fileName in a given mode. The file is searched for using the
search directories specified by dirs which is a list of directories
separated by ':'s (';' for MSDOG).  If an entry in dirs starts with a
'$' and the second character is not '$', then it is interpreted to be
an environment variable whose value specifies a list of directories to
be searched recursively.  If a directory name starts with a '$'
character, then that '$' (only the starting '$') must be quoted by
repeating it.  If the file can be opened in the requested mode, then
its FILE descriptor is returned; otherwise NULL is returned.  If the
file is found and succDir is non-NULL, then *succDirP is returned as a
pointer within dirList to the directory in which it was found.  If the
file is found and succDirLenP is non-NULL, then *succDirLenP is
returned as the length of the success directory name (the length does
not include any terminating DIRECTORY_SEPARATOR).

If dirList is empty, then the current directory is searched.  An empty
component in dirList also specifies a search of the current directory:
an empty component is specified when dirList has either two
adjacent PATH_SEPARATORs or starts or ends with a PATH_SEPARATOR.

If the system getenv() routine is non-reentrant, then the current
implementation does not support a environment variable dirList
component which contains another environment variable.

*/

FILE *findFileDir PROTO((ConstString fileName, ConstString mode,
			 ConstString dirList, 
			 ConstString *succDirP, Int *succDirLenP));


/* Wrapper around findFileDir without any success directory arguments. */
FILE *findFile PROTO((ConstString fileName, ConstString mode,
		      ConstString dirList));

/* Read the rest of file into a dynamically allocated array (which can
 * subsequently be FREEd).
 */
Char *readFile PROTO((FILE *file));

/* Remove any C-escape sequences inplace in s having srcLen
 * characters.  Though s can contain arbitrary characters it is assumed
 * that s[srcLen] == '\0'.  Return the number of characters in the
 * converted string (not counting the terminating '\0').
 */
Count translateCEscapes PROTO((Char *s, Count srcLen));

/* Return a dynamically allocated copy of string s with all chars. changed 
 * to lower-case.  Returned value can subsequently be FREE'd by the caller.
 */
ConstString toLowerString PROTO((ConstString s));

/* Return a dynamically allocated copy of string s with all chars. changed 
 * to upper-case.  Returned value can subsequently be FREE'd by the caller.
 */
ConstString toUpperString PROTO((ConstString s));

/* Return a dynamically allocated copy of the first len chars of
 * string s with all chars. changed to lower-case, terminated by
 * '\0'.  Returned value can subsequently be FREE'd by the caller.
 */
ConstString toLowerStringN PROTO((ConstString s, Size len));

/* Return a dynamically allocated copy of the first len chars of
 * string s with all chars. changed to upper-case, terminated by '\0'.
 * Returned value can subsequently be FREE'd by the caller.
 */
ConstString toUpperStringN PROTO((ConstString s, Size len));

/* Return a pointer to the first occurrence of the first n characters of
 * string ct in cs.  NULL if none.
 */
ConstString strNStr PROTO((ConstString cs, ConstString ct, Size n));

/* Since printf() can't be depended on to return # of chars output on all
 * systems, the following is useful to find out the # of chars in a "%d",
 * "%x" or "%o" representation of an int.  base must be in {8, 10, 16}.
 */
Size longPrintLen PROTO((Long n, Int base));

#endif /* ifndef _UTIL_H */
