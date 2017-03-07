/*

File:	 buildrec.c
Purpose: Record compilation date, time and command.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "buildrec.h"

#ifndef BUILD_FLAGS
#define BUILD_FLAGS ""
#endif

/* In case we have a really ancient cc */
#ifndef __DATE__
#define __DATE__ 	"unknown date"
#endif

#ifndef __TIME__
#define __TIME__ 	"unknown time"
#endif

#ifndef BUILD_CMD
#define BUILD_CMD      	"unknown command"
#endif

BuildRec buildRec= { __DATE__, __TIME__ , BUILD_CMD };

