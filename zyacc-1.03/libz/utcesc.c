/*

File:	 utcesc.c
Purpose: Inplace processing of C-style escape sequences.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "zutil.h"

/* Define values for escape characters which are in ANSI, but not K&R. */
#ifndef BEL
#define BEL		7
#endif

#ifndef VT
#define VT		11
#endif

/* Remove any C-escape sequences inplace in s having srcLen
 * characters.  Though s can contain arbitrary characters it is assumed
 * that s[srcLen] == '\0'.  Return the number of characters in the
 * converted string (not counting the terminating '\0').
 */
Count 
translateCEscapes(s, srcLen)
  Char *s;
  Count srcLen;
{
  CONST Char *CONST endP= s + srcLen;
  Char *srcP;
  Char *destP;

  for (srcP= s, destP= s; srcP < endP;) { /* Body increments srcP & destP. */
    if (*srcP != '\\') { /* Not a escape sequence. */
      *destP++= *srcP++;
    }
    else {
      srcP++;
      if (*srcP == 'x' && isxdigit(srcP[1])) {
	int v= 0;
	for (srcP= srcP + 1; isxdigit(*srcP); srcP++) {
	  v= 16*v 
	     + (isdigit(*srcP) ? *srcP - '0' : (toupper(*srcP) - ('A' + 10)));
	}
	*destP++= v;
      }
      else if ('0' <= *srcP && *srcP <= '7') {
	int v= 0;
	int n;
	for (n= 0; n < 3 && '0' <= *srcP && *srcP <= '7'; n++, srcP++) {
	  v= 8*v + (*srcP - '0');
	}
	*destP++= v;
      }
      else {
	ConstString escChars= "abfnrtv";
	ConstString p= strchr(escChars, *srcP);
#if __STDC__
	ConstString translateChars= "\a\b\f\n\r\t\v";
#else
	static char translateChars[]= "x\b\f\n\r\tx";
	translateChars[0]= BEL; translateChars[6]= VT;
#endif
	*destP++= (p) ? translateChars[p - escChars] : *srcP;
	srcP++;
      }
    } 
  } /* for (srcP= s, destP= s; srcP < endP;) */
  *destP= '\0';
  return destP - s;
}

#ifdef TEST_CESC

int main() {
  enum { LENG= 80 };
  char line[LENG];
  while ((  fputs("Enter line: ", stdout) 
          , fflush(stdout) 
          , fgets(line, LENG, stdin))) {
    int n= translateCEscapes(line, strlen(line));
    fwrite(line, 1, n, stdout);
  }
  exit(0);
}

#endif
