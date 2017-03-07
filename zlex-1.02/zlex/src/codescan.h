/*

File:	 codescan.h
Purpose: Output DFA as code.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _CODESCAN_H
#define _CODESCAN_H

#include "port.h"

VOIDP initCode PROTO((VOID_ARGS));
Count outMaps PROTO((FILE *outFile));
Count outDFACode PROTO((FILE *outFile));

#endif

