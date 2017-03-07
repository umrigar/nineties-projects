#ifndef _ERRORZ_H
#define _ERRORZ_H

#include "ccport.h"

enum {	/* Flags giving error type and what information should be printed. */
  ER_ERROR= 0x01,	/* Normal error. */
  ER_FATAL= 0x02,	/* Terminates program after printing message. */	
  ER_WARN= 0x04,	/* Prefix message with warning. */
  ER_SYS= 0x08,		/* Follow with system error message. */
  ER_PRG= 0x10,		/* Prefix message with program name. */
  ER_FILE= 0x20,	/* Prefix message with file name. */
  ER_LINE= 0x40,	/* Prefix message with line number. */
  ER_NL= 0x80		/* Output NL after message. */
};

/* Function which returns file or program name. */
typedef ConstString (*StringErFnP) PROTO((VOID_ARGS));

/* Function which returns a line number. */
typedef unsigned (*LineErFnP) PROTO((VOID_ARGS));

/* Function to format some prefix of args on outFile according to extended
 * format specifier fmt (which looks like "%...C", where ... are any flags,
 * and C is the extended format specifier).
 */
typedef va_list (*FmtFnP) PROTO((FILE *outFile, ConstString fmt, va_list args));

typedef struct {
  char fmtChar;		/* Char. specifying special format. */
  FmtFnP fmtFn;		/* Function for printing out special format. */
} ErrorFmt;

typedef struct {
  FILE *errFile;	/* Where errors should be printed out. */
  StringErFnP prgFn;	/* Function which returns program name. */
  StringErFnP fileFn;	/* Function which returns line name. */
  LineErFnP lineFn;	/* Function which returns line number. */
  Count nErrors;	/* # of calls with ER_ERROR flag set. */
  Count maxErrors;	/* Max. # of errors before bailing out. */
  ErrorFmt *fmt;	/* Address of NUL-terminated array for printing
			 * special formats.   
			 */
} ErrorStruct;


/* Print out message specified by fmt on errFile with any modifications
 * suggested by errFlags.  fmt understands all printf-style
 * conversions including flags, modifiers and size specifications).
 * It is extensible in understanding %X for some character X (which
 * cannot be a character occurring in a printf() flag, modifier or
 * size specification) which occurs in errP->fmt, by calling the
 * corresponding fmtFn.
 */
VOID errorz PROTO((ErrorStruct *errP, unsigned errFlags, ConstString fmt, ...));

VOID verrorz PROTO((ErrorStruct *errP, unsigned errFlags, 
		    ConstString fmt, va_list ap));


/* The following routine is called by libz routines on detecting an error.
 * A default definition is provided, but the user can override the default
 * definition by providing one which prints out the message specified by
 * fmt (along with additional information) and then does an error exit(). 
 */
#if CC_STDARG
VOID libzError PROTO((unsigned errFlags, ConstString fmt, ...));
#else
VOID libzError PROTO(());
#endif

#endif
