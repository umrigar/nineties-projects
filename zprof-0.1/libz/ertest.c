/*

File:	 ertest.c
Purpose: Test program for error routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "errorz.h"

static ConstString prgName PROTO((VOID_ARGS));
static ConstString fileName PROTO((VOID_ARGS));
static unsigned lineNum PROTO((VOID_ARGS));
static va_list fmtDPrint PROTO((FILE *outFile, ConstString fmt, va_list args));
int main PROTO((int argc, char **argv));

ErrorFmt specialFormats[]= {
  { 'D', fmtDPrint },
  { '\0', NULL }
};

ErrorStruct errStruct= { 
  stderr, prgName, fileName, lineNum, 0, 1000, specialFormats
};

static ConstString
prgName()
{
  return "Error test program";
}

static ConstString
fileName()
{
  return "Error test file";
}

static unsigned 
lineNum()
{
  return 1091;
}

static va_list
fmtDPrint(fp, fmt, ap)
  FILE *fp;
  ConstString fmt;
  va_list ap;
{
  int i= va_arg(ap, int);
  fprintf(fp, "FMT(%d)", i);
  return ap;
}

Int 
main(argc, argv)
  int argc;
  char **argv;
{
  ConstString fName= "$@#.@@";
  errorz(&errStruct, ER_NL, 
	 "This should be unadorned except for a `\\n' terminator.");
  errorz(&errStruct, ER_NL|ER_FILE|ER_LINE, 
	 "This should contain `file:line: ' prefix.");
  errorz(&errStruct, ER_NL|ER_FILE|ER_LINE|ER_PRG|ER_WARN, 
	 "This should contain `program:file:line: ' prefix with warning.");
  errorz(&errStruct, ER_NL|ER_ERROR,
	 "%-*.*s:%*d: Prefixed with `__FILE__:__LINE__: ' %20.4s",
	 20, 6, __FILE__, 10, __LINE__, "some string");
  fopen(fName, "r"); 
  errorz(&errStruct, ER_NL|ER_SYS|ER_ERROR,
	 "Attempted to open non-existent file `%s'.", fName);
  errorz(&errStruct, 0, "Special format %D.\n", 983);

  fprintf(stderr, "# of errors= %d\n", errStruct.nErrors);
  errorz(&errStruct, ER_NL|ER_FATAL, "We die now ... g'bye...");
  return 0;
}
  
