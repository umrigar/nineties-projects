/*

File:	 main.c
Purpose: Default main program which merely calls yylex().

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "libzlexp.h"

int yylex YY_PROTO((YY_VOID_ARGS));
int main YY_PROTO((int argc, char **argv));

int 
main(argc, argv)
  int argc;
  char **argv;
{
  return yylex();
}
