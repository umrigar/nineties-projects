/*

File:	 zparse.h
Purpose: Parser interface.

Last Update Time-stamp: "97/07/10 18:16:13 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _PARSE_H
#define _PARSE_H

#include "port.h"

#include <stdio.h>

int yyparse PROTO((VOID_ARGS));
Boolean hasLookahead PROTO((VOID_ARGS));
Count outParseParams PROTO((FILE *outFile));

#ifdef TEST_GRAM

VOID printAssocPrec PROTO((Index assocPrec));

#endif

#endif /* ifndef _PARSE_H. */
