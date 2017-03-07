/*

File:	 error.h
Purpose: Non-reentrant error routines.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _ERROR_H
#define _ERROR_H

#include "ccport.h"
#include "errorz.h"

/* Convenient interface to reentrant error routines. */

/* Initialize error processing.  Any of the following parameters can be NULL. 
 * or NIL (for maxErrors) to receive default values.
 */
VOID initError PROTO((
  FILE *errFile,	/* FILE for error message output; default stderr. */
  Count maxErrors,	/* Max. # of errors before bailing out; default 25. */
  StringErFnP prgFn, 	/* prgFn() returns program name; default none. */
  StringErFnP fileFn,   /* fileFn() returns source file name; default none.  */
  LineErFnP lineFn,	/* lineFn() returns source line #; default none. */
  ErrorFmt *fmt		/* Array ptr for special format specs: default none. */
));

/* Return # of errors so far. */
unsigned nErrors PROTO((VOID_ARGS));

#if CC_STDARG
/* Print error message, prefixed by file:lineNum:, suffixed by '\n', 
 * bump error count. 
 */
VOID error PROTO((ConstString fmt, ...));

/* Print error message, prefixed by file:, suffixed by '\n', 
 * bump error count. 
 */
VOID errorAtLine PROTO((ConstString fmt, ...));

/* Print error message, prefixed by file:lineNum:, suffixed by '\n', 
 * bump error count, then exit(1).
 */
VOID fatal PROTO((ConstString fmt, ...));

/* Print error message, prefixed by file:lineNum: warning:, suffixed by '\n'. */
VOID warn PROTO((ConstString fmt, ...));

/* Print a message without any extra action (no '\n'). */
VOID message PROTO((ConstString fmt, ...));

#else /* !CC_STDARG */

/* Print error message, prefixed by file:lineNum:, suffixed by '\n', 
 * bump error count. 
 */
VOID error PROTO(());

/* Print error message, prefixed by file:, suffixed by '\n', 
 * bump error count. 
 */
VOID errorAtLine PROTO(());

/* Print error message, prefixed by file:lineNum:, suffixed by '\n', 
 * bump error count, then exit(1).
 */
VOID fatal PROTO(());

/* Print error message, prefixed by file:lineNum: warning:, suffixed by '\n'. */
VOID warn PROTO(());

/* Print a message without any extra action (no '\n'). */
VOID message PROTO(());

#endif

/*			CONVENIENCE MACROS.				*/

#define MEM_ERROR()							\
  fatal("Out of memory at %s:%d.", __FILE__, __LINE__)

#define INTERNAL_ERROR()						\
  fatal("Internal error at %s:%d.", __FILE__, __LINE__)

#define FILE_ERROR(fileName)						\
  fatal("Could not open file %s.", fileName)


#endif /* ifndef _ERROR_H */


