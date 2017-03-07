/*

File:	 libzyacp.h
Purpose: Private interface for runtime zyacc library.

Last Update Time-stamp: "97/07/28 17:06:06 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _LIBZYACP_H
#define _LIBZYACP_H


#include <config.h>

#define YY_ZYACC_DEBUG 1
#include "libzyacc.h"

/* Name of environmental variables used for specifying remote debugging. */
#define APPLET_ENV_VAR		"ZYDEBUG_APPLET"
#define CODEBASE_ENV_VAR	"ZYDEBUG_CODEBASE"
#define HTMLBASE_ENV_VAR	"ZYDEBUG_HTMLBASE"
#define SRCBASE_ENV_VAR		"ZYDEBUG_SRCBASE"
#define PORT_ENV_VAR		"ZYDEBUG_PORT"

#define FLAG(bitNum)	(1 << (bitNum))

/* Break/Display flags.  Used for each symbol & for temp break points
 * and global break points.  Form flags by shifting 1 left by enum const below.
 */
enum {
  NON_TERM_BREAK_F,	/* break on reduce with specified nonterm as LHS */
  NON_TERM_DISPLAY_F,	/* display on reduce with specified nonterm as LHS */
  RULE_BREAK_F,		/* break on reduce by specified rule */
  RULE_DISPLAY_F,	/* display on reduce by specified rule */
  TERM_BREAK_F,		/* break on specified terminal shift */
  TERM_DISPLAY_F	/* display on specified terminal shift */
};

/* Global break/display flags + misc flags */
enum {
  DEBUG_INIT_F= YY_DEBUG_IGNORE_F + 1, 	/* done initialization */
  REMOTE_F,				/* being used remotely */
  REDUCE_BREAK_F,			/* break before any reduce */
  SHIFT_BREAK_F,			/* break before any shift */
  REDUCE_DISPLAY_F,			/* display before any reduce */
  SHIFT_DISPLAY_F,			/* display before any shift */
  VERBOSE_DISPLAY_F			/* display verbosely */
  /* no more flags can fit in a 8-bit char. */
};

/* The foll. may cause problems with some system headers.  If so, include
 * the headers before this file.
 */
#undef FALSE
#undef TRUE
enum {
  FALSE= 0, TRUE= 1
};

YYBoolean yyRemoteDebug YY_PROTO((YYDebugData *YY_CONST debugDataP));

#endif /* ifndef _LIBZYACP_H */
