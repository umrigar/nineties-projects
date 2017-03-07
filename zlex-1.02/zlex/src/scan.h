/*

File:	 scan.h
Purpose: Zlex scanner interface.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _LSCAN
#define _LSCAN

#include "port.h"


extern char *yytext;

/* Module initialization/termination. */
VOID initScan PROTO((int argc, int nonOptArgN, ConstString argv[]));
VOID terminateScan PROTO((VOID_ARGS));

Int scan PROTO((VOID_ARGS));			/* Main scanning routine. */

/* Output a #line directive to outFile */
Count lineDir PROTO((FILE *outFile, Count lineN, ConstString fName));

/* Temporary file routines. */
Count outSrcAction PROTO((FILE *outFile, Index srcActN));
Count outDefs PROTO((FILE *outFile));
Count outLocalDecs PROTO((FILE *outFile));
Count outSec3 PROTO((FILE *outFile));
Count outScanParams PROTO((FILE *outFile));
Count nSrcActs PROTO((VOID_ARGS));

unsigned lineN PROTO((VOID_ARGS));
ConstString srcName PROTO((VOID_ARGS));

#endif
