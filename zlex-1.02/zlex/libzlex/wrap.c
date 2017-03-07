/*

File:	 wrap.c
Purpose: Default yywrap() routine which merely returns 1.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"
 
int yywrap YY_PROTO((YY_VOID_ARGS));

int 
yywrap() 
{
  return 1;
}

