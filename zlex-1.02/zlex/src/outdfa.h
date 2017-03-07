/*

File:	 outdfa.h
Purpose: Compress and output DFA tables.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/
#ifndef _COMPDFA_H
#define _COMPDFA_H

#include "port.h"

VOID compressDFA PROTO((VOID_ARGS));

Count outDFATabs PROTO((FILE *outFile));

/* Output action number table. */
Count outActionTabs PROTO((FILE *outFile));

Count outCompDFAParams PROTO((FILE *outFile));

VOID printCompDFAStats PROTO((FILE *outFile));

#endif /* ifndef _COMPDFA_H */
