/*

File:	 fatal.c
Purpose: Fatal error routine.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
YY_VOID
yyZlexFatal(FILE *err, YY_CONST char *s)
{
  fprintf(err, "%s", s);
  exit(1);
}


