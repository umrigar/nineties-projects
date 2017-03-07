/*

File:	 sig.c
Purpose: Signature to insert into library for identification purposes.

Last Update Time-stamp: <97/06/24 09:20:11 zdu>

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/* Signature to allow identification of library.  SIGNATURE is a CPP
 * macro which can be defined on the command-line.
 */

#ifndef SIGNATURE
#define SIGNATURE "Zerksis"
#endif

char *bbSig= SIGNATURE;
