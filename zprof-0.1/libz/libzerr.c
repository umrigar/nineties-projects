#include "errorz.h"

/* The following routine is called by libz routines on detecting an error.
 * A default definition is provided, but the user can override the default
 * definition by providing one which prints out the message specified by
 * fmt (along with additional information) and then does an error exit(). 
 */

#if CC_STDARG

VOID libzError(unsigned errFlags, ConstString fmt, ...)
{
  va_list ap;
  ErrorStruct errStruct;
  errStruct.errFile= stderr;
  errStruct.prgFn= errStruct.fileFn= NULL; errStruct.lineFn= NULL;
  errStruct.nErrors= 0; errStruct.maxErrors= 1000; errStruct.fmt= NULL;
  va_start(ap, fmt);
  verrorz(&errStruct, errFlags, fmt, ap);
  va_end(ap);
  VOID_RET();
}

#else /* !CC_STDARG */

VOID libzError(va_alist)
  va_dcl
{
  va_list ap;
  unsigned errFlags;
  ConstString fmt;
  ErrorStruct errStruct;
  errStruct.prgFn= errStruct.fileFn= NULL; errStruct.lineFn= NULL;
  errStruct.nErrors= 0; errStruct.maxErrors= 1000; errStruct.fmt= NULL;
  va_start(ap);
  errFlags= va_arg(ap, unsigned);
  fmt= va_arg(ap, ConstString);
  verrorz(&errStruct, errFlags, fmt, ap);
  va_end(ap);
  VOID_RET();
}

#endif /* else !CC_STDARG */

