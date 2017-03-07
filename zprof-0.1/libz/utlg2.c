/*

File:	 utlg2.c
Purpose: log2 utility routines.

Last Update Time-stamp: <97/07/28 12:00:46 zdu>

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "zutil.h"

/* Return ceiling(lg2(n)). Returns 0 if n == 0. */
unsigned 
ceilingLg2(n)
  unsigned n;
{
  unsigned floor= floorLg2(n);
  return floor + ((((unsigned)1) << floor) < n);
}

/* Return floor(lg2(n)). Returns 0 if n == 0. */
unsigned 
floorLg2(n)
  unsigned n;
{
  unsigned floor= 0;
  while (n > 1) { n >>= ((unsigned)1); floor++; }
  return floor;
}
