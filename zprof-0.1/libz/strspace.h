/*

File:	 strspace.h
Purpose: Interning strings in a string-space.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _STRSPACE
#define _STRSPACE

#include "ccport.h"

/*

These routines are used to save strings in a string-space.  Routines
are provided to add strings, and extend the last string added.  There
is no restriction on the chars contained in strings.  The length of
all strings (including those constructed by catenation) should be
strictly less than UINT_MAX --- this is not checked, since the limit
is unlikely to be exceeded.  The user can specify whether a '\0' should
automatically be appended to each string as it is added.

In the sequel, textLen refers to the # of chars in text including any 
NULs.

*/

/* Make StrSpace a opaque type. */
typedef VOIDP StrSpace;

/* Initialize and return an empty string-space. If appendNUL is true, then
 * NULs ('\0's) are automatically appended to any strings subsequently
 * added.
 */
StrSpace initStrSpace PROTO((BooleanX appendNUL));

/* Add text to string-space strSpace and return its string-space pointer. 
 * If text is NULL, then merely reserve space without giving it
 * any contents.
 */
Char *addStrSpace PROTO((StrSpace strSpace, CONST Char *text, 
			 unsigned textLen));

/* Destructively append text to the last string added to the
 * string-space.  Return a pointer to the appended string.  Previous
 * string-space pointers to the last string are no longer valid. 
 */
Char *catStrSpace PROTO((StrSpace strSpace, CONST Char *text, unsigned textLen));

/* Destroy space in strSpace allocated since (including) text. */
VOID popStrSpace PROTO((StrSpace strSpace, char *text));

/* Delete string-space strSpace, releasing all memory occupied by it. */
VOID delStrSpace PROTO((StrSpace strSpace));

#endif /* ifndef _STRSPACE */
