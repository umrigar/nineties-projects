/*

File:	 textid.h
Purpose: Associating small-ints (IDS) with strings.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _TEXTID_H
#define _TEXTID_H

#include "ccport.h"

/* In text strings '\0' is not special and is included in any length
 * computations.  Hence to store C-type strings along with the terminating
 * '\0', the provided length should strlen() + 1.
 */

typedef Index2 ID;
typedef Index IDX;	/* For parameter passing. */
typedef VOIDP TextID;

/* Initialize and return a TextID.  If ignoreCase is TRUE, then case will
 * be ignored when comparing characters in subsequent getTextID()s. 
 */
TextID initTextID PROTO((BooleanX ignoreCase));

/* Return the ID corresponding to text (having a total of len chars).  
 * Return NIL if not found in textID.
 */
ID getTextID PROTO((TextID textID, CONST char *text, Size len));

/* Add the text (having a total of len chars) to textID and return the
 * corresponding ID.
 */
ID addTextID PROTO((TextID textID, CONST char *text, Size len));

/* Return the pointer to the stored text for id (which must have been 
 * previously returned by either getTextID() or addTextID()).
 */
CONST Char *idText PROTO((TextID textID, IDX id));

/* Return the total # of characters stored for id (which must have been 
 * previously returned by either getTextID() or addTextID()).
 */
Size idLen PROTO((TextID textID, IDX id));

/* Return the text and the length in a single call. */
CONST Char *idTextLen PROTO((TextID textID, IDX id, Size *lenP));

/* Forget all IDs added to textID after and including id (which
 * must have been previously returned by either getTextID() or
 * addTextID()). 
 */
VOID popTextID PROTO((TextID textID, IDX id));

/* Delete the TextID structure denoted by textID and free all memory
 * occupied by it. 
 */
VOID delTextID PROTO((TextID textID));

#endif /* ifndef _TEXTID_H */
