/*

File:	 scan.h
Purpose: Simple ANSI-C parser.

Last Update Time-stamp: <97/06/24 09:24:25 zdu>

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _SCAN_H
#define _SCAN_H

#include "ccport.h"

typedef struct {
  CONST char *text;	/* text of token + preceeding white space */
  Size len1;		/* length of whitespace etc before token */
  Size len;		/* total length of text */
  ConstString fName;	/* name of source file */
  Index lineNum;	/* line # in file fName. */
  Index id;		/* index of intern'd identifier. */
} TokSem;


VOID initScan PROTO((VOID_ARGS));
VOID terminateScan PROTO((VOID_ARGS));

VOID newFile PROTO((ConstString fName));

Index scan PROTO((VOID_ARGS));

Index lineNum PROTO((VOID_ARGS));
ConstString fileName PROTO((VOID_ARGS));
void yyerror PROTO((ConstString s));

#endif
