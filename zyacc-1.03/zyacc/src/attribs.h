/*

File:	 attribs.h
Purpose: Attribute processing interface.

Last Update Time-stamp: "97/07/28 17:21:54 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _ATTRIBS
#define	_ATTRIBS

#include "port.h"

#include <stdio.h>

typedef enum {
  IN_ATOK,		/* %in: occurs only in declarations. */
  OUT_ATOK,		/* %out: occurs only in declarations. */
  TEXT_ATOK,		/* TEXT_TOK. */
  SHORT_TEXT_ATOK,	/* SHORT_TEXT_TOK. */
  NUM_VAR_ATOK,		/* $n. */
  ID_VAR_ATOK,		/* $id in attribute expression. */
  ZZ_ATOK		/* Used for extending enum --- MUST BE LAST. */
} ATokType;

VOID initAttribs PROTO((VOID_ARGS));
Boolean hasTermIDVars PROTO((VOID_ARGS));
Count nInIDVars PROTO((VOID_ARGS));
Index dclTermAttribs PROTO((Index termID, Index litID, Index litVal, Index val, 
			    Index assocPrec, Index typeID, Count nDec,
			    BooleanX nowarn)); 
Index dclNonTermAttribs PROTO((Index nonTermID, Index type)); 
VOID beginRuleAttribs PROTO((VOID_ARGS)); 
VOID endRuleAttribs PROTO((Count nRHS));
Index addRHSAttribs PROTO((Index id, Index val));
VOID setAttribParseError PROTO((VOID_ARGS));
VOID addAttribSeq PROTO((Index lineN));
VOID addAttribTok PROTO((Index type, CONST VOIDP infoP));
VOID addActAttribs PROTO((Index actN, Index lineNum));
VOID addTestAttribs PROTO((VOID_ARGS));
VOID computeAttribs PROTO((VOID_ARGS));

VOID printInAttribExps PROTO((FILE *outFile, Index state));
Count outTest PROTO((FILE *outFile, Index testExp, Index nonTerm));
Count outTestCases PROTO((FILE *outFile));
Count outSemanticTypes PROTO((FILE *outFile));
Count outAttribsTermTypes PROTO((FILE *outFile));
Count outInAttribExps PROTO((FILE *outFile));
Count outAttribParams PROTO((FILE *outFile));
VOID outAttribVar PROTO((FILE *outFile, ConstString attribName));
VOID outIDVar PROTO((FILE *outFile, Index idVar, Int offset));
Count outAttribActPrefix PROTO((FILE *outFile, Index ruleN));
Count outAttribActSuffix PROTO((FILE *outFile, Index ruleN));

#endif /* ifndef _ATTRIBS */
