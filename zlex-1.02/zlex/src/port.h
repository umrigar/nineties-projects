/*

File:	 port.h
Purpose: Porting interface.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/*

This file should be included in every file.  It should be included in the
header file corresponding to a source file, rather than directly by the
source file.  The one exception is the file corresponding to the main
program which doesn't have a header file.
 
*/

#include "ccport.h"	/* From libz. */
#include "osport.h"	/* From libz. */

