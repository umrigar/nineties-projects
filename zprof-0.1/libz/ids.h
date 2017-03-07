/*

File:	 ids.h
Purpose: Non-reentrant id routines.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _IDS_H
#define _IDS_H

#include "ccport.h"

/* This is a non-reentrant but convenient interface to the string-space
 * and text-id table routines.  Assumes that it is given NUL-terminated text.
 * The lengths given and returned from this module do not include the 
 * terminating NUL. 
 */

/* Initialize: if ignoreCase is TRUE, then case is ignored in subsequent
 * character comparison.
 */
VOID initID PROTO((BooleanX ignoreCase));

/* Terminate module. */
VOID terminateID PROTO((VOID_ARGS));

/* Return index of string of length len.  Identical strings will always
 * return the same index.
 */
Index getID PROTO((ConstString string, Size len));

/* Return index of string of length len.  Identical strings will always
 * return the same index.  Return NIL if string is not in the table.
 */
Index getIDFail PROTO((ConstString string, Size len));



/* Return the string for ID id (which should have been previously returned
 * by getID()).
 */
ConstString getIDString PROTO((Index id));

/* Return the length for ID id (which should have been previously returned
 * by getID()).
 */
Size getIDLen PROTO((Index id));

/* Intern text without keeping track of it: this can result in duplicate
 * text being intern'd.
 */
ConstString internText PROTO((CONST Char *text, Size len));


#endif /* ifndef _IDS_H */
