/*

File:	 potstinf.c
Purpose: Test program for optTab[] to info conversion. 

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#define PO_OFFLINE_FMT 1

#include "parseopt.h"

#include "potstop0.c"

int main() 
{
  optTab2TeXInfo(stdout, optTab, N_ELEMENTS(optTab));
  return 0;
}
