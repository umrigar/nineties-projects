/*

File:	 utlnglen.c
Purpose: Return the # of chars in a printf() representation of an int.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "zutil.h"

/* Since printf() can't be depended on to return # of chars output on all
 * systems, the following is useful to find out the # of chars in a "%d",
 * "%x" or "%o" representation of an int.  base must be in {8, 10, 16}.
 */
Size 
longPrintLen(n, base) 
  Long n;
  Int base;
{
  Size len= 1;	/* Will have at least 1 digit. */
  if (n < 0) { len++; n= -n; }
  while (n >= base) { n/= base; len++; }
  return len;
}

    

