/*

File:	 sig.c
Purpose: Signature to insert into library for identification purposes.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/* Signature to allow identification of library.  SIGNATURE is a CPP
 * macro which can be defined on the command-line.
 */

#ifndef SIGNATURE
#define SIGNATURE "Zerksis"
#endif

char *yySig= SIGNATURE;
