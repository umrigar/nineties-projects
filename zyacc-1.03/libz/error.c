#include "error.h"

static ErrorStruct errStruct;

VOID initError(errFile, maxErrors, prgFn, fileFn, lineFn, fmt)
  FILE *errFile;
  Count maxErrors;
  StringErFnP prgFn; 	/* prgFn() should return the name of the program. */
  StringErFnP fileFn;   /* fileFn() should return the current file name. */
  LineErFnP lineFn;	/* lineFn() should return a unsigned line #. */
  ErrorFmt *fmt;	/* Address of an array for special format specs. */
{
  errStruct.errFile= (errFile) ? errFile : stderr;
  errStruct.maxErrors= (maxErrors == NIL) ? 25 : maxErrors;;
  errStruct.prgFn= prgFn;
  errStruct.fileFn= fileFn;
  errStruct.lineFn= lineFn;
  errStruct.nErrors= 0;
  errStruct.fmt= fmt;
  VOID_RET();
}


#if CC_STDARG

/* Print error message, prefixed by file:lineNum:, suffixed by '\n', 
 * bump error count. 
 */
VOID error(ConstString fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  verrorz(&errStruct, ER_ERROR|ER_NL|ER_FILE|ER_LINE, fmt, ap);
  va_end(ap);
  VOID_RET();
}

/* Print error message, prefixed by file:, suffixed by '\n', 
 * bump error count. 
 */
VOID errorAtLine(ConstString fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  verrorz(&errStruct, ER_ERROR|ER_NL|ER_FILE, fmt, ap);
  va_end(ap);
  VOID_RET();
}

/* Print error message, prefixed by file:lineNum:, suffixed by '\n', 
 * bump error count, then exit(1).
 */
VOID fatal(ConstString fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  verrorz(&errStruct, ER_ERROR|ER_NL|ER_FILE|ER_LINE|ER_FATAL, fmt, ap);
  va_end(ap);
  VOID_RET();
}


/* Print error message, prefixed by file:lineNum: warning:, suffixed by '\n'. */
VOID warn(ConstString fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  verrorz(&errStruct, ER_NL|ER_FILE|ER_LINE|ER_WARN, fmt, ap);
  va_end(ap);
  VOID_RET();
}


/* Print a message without any extra action (no '\n'). */
VOID message(ConstString fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  verrorz(&errStruct, 0, fmt, ap);
  va_end(ap);
  VOID_RET();
}

#else /* !CC_STDARG */

/* Print error message, prefixed by file:lineNum:, suffixed by '\n', 
 * bump error count. 
 */
VOID error(va_alist)
  va_dcl
{
  ConstString fmt;
  va_list ap;
  va_start(ap);
  fmt= va_arg(ap, ConstString);
  verrorz(&errStruct, ER_ERROR|ER_NL|ER_FILE|ER_LINE, fmt, ap);
  va_end(ap);
  VOID_RET();
}

/* Print error message, prefixed by file:, suffixed by '\n', 
 * bump error count. 
 */
VOID errorAtLine(va_alist)
  va_dcl
{
  ConstString fmt;
  va_list ap;
  va_start(ap);
  fmt= va_arg(ap, ConstString);
  verrorz(&errStruct, ER_ERROR|ER_NL|ER_FILE, fmt, ap);
  va_end(ap);
  VOID_RET();
}

/* Print error message, prefixed by file:lineNum:, suffixed by '\n', 
 * bump error count, then exit(1).
 */
VOID fatal(va_alist)
  va_dcl
{
  ConstString fmt;
  va_list ap;
  va_start(ap);
  fmt= va_arg(ap, ConstString);
  verrorz(&errStruct, ER_ERROR|ER_NL|ER_FILE|ER_LINE|ER_FATAL, fmt, ap);
  va_end(ap);
  VOID_RET();
}


/* Print error message, prefixed by file:lineNum: warning:, suffixed by '\n'. */
VOID warn(va_alist)
  va_dcl
{
  ConstString fmt;
  va_list ap;
  va_start(ap);
  fmt= va_arg(ap, ConstString);
  verrorz(&errStruct, ER_ERROR|ER_NL|ER_FILE|ER_LINE|ER_WARN, fmt, ap);
  va_end(ap);
  VOID_RET();
}


/* Print a message without any extra action (no '\n'). */
VOID message(va_alist)
  va_dcl
{
  ConstString fmt;
  va_list ap;
  va_start(ap);
  fmt= va_arg(ap, ConstString);
  verrorz(&errStruct, 0, fmt, ap);
  va_end(ap);
  VOID_RET();
}

#if 0
/* Print a message without any extra action (no '\n'). */
VOID warn(va_alist)
  va_dcl
{
  ConstString fmt;
  va_list ap;
  va_start(ap);
  fmt= va_arg(ap, ConstString);
  verrorz(&errStruct, 0, fmt, ap);
  va_end(ap);
  VOID_RET();
}
#endif

#endif /* else !CC_STDARG */

/* Return # of errors so far. */
unsigned nErrors()
{	
  return errStruct.nErrors;
}

