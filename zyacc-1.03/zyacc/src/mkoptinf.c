/*

File:	 options.c
Purpose: Options processing.

Last Update Time-stamp: "97/07/10 18:10:08 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#define PO_OFFLINE_FMT 1

#include "parseopt.h"

#include "opttab0.c"

/* Produce file containing options information to be included in TexInfo
 * documentation.
 */
int main() 
{
  optTab2TeXInfo(stdout, optTab, N_ELEMENTS(optTab));
  return 0;
}
