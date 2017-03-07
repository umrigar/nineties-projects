/*

File:	 main.h
Purpose: Interface to main program for parser-generator.

Last Update Time-stamp: "97/07/10 18:08:44 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _MAIN_H
#define _MAIN_H

#include "port.h"

extern ConstString dirs;
VOID skelHelp PROTO((FILE *helpFile));
Int versionFn PROTO((Int id, VOIDP checkP, VOIDP valP, ConstString argP));
Int buildFn PROTO((Int id, VOIDP checkP, VOIDP valP, ConstString argP));


#endif
