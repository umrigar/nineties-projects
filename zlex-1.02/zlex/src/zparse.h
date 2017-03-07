/*

File:	 zparse.h
Purpose: Parser interface.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _PARSE
#define _PARSE

#include "port.h"

int yyparse PROTO((VOID_ARGS));
VOID initParse PROTO((VOID_ARGS));
Boolean hasStartLinePat PROTO((VOID_ARGS)); /* TRUE if `^' pattern used. */
/* Output parser parameters to outFile as #defines. */
Count outParseParams PROTO((FILE *outFile));

#endif
