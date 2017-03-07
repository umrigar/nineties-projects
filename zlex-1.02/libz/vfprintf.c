/*

File:	 vfprintf.c
Purpose: Replacement for standard C library vfprintf() routine.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "ccport.h"

#ifndef HAVE_VPRINTF

#define SPRINTF(buf, fmt, type, ap, n)					\
  do { 									\
    int r= sprintf(buf, fmt, va_arg(ap, type));				\
    if (r < 0) return r;						\
    n+= strlen(buf);							\
  } while (0)

#define FPRINTF(file, fmt, type, ap, n)					\
  do { 									\
    int r= fprintf(file, fmt, va_arg(ap, type));			\
    if (r < 0) return r;						\
    n+= r;								\
  } while (0)

#ifdef CC_STDARG
#else
#endif


/* We assume that shorts and chars widened to ints before call. */
#define SHORT_PROMOTE int
#define CHAR_PROMOTE int

int 
vfprintf(FILE *stream, CONST char *format, va_list ap) 
/* Assumes args meets format specifications.  Works only insofar as the
 * underlying printf() works. 
 */
{
  CONST char *p= format;
  int n= 0;	/* Number of characters output. */
  for (p= format; *p; p++) {
    if (*p != '%') { /* Regular character. */
      fputc(*p, stream); n++;
    }
    else { /* Possible conversion specification. */
      p++;
      if (*p == '%') { /* Just a '%'. */
	fputc('%', stream); n++;
      }
      else { /* Definite conversion specification. */
	char fmt[80];	/* Allow hopefully enough space for format string. */
	Index i= 0;
	enum { NORMAL, SHORT, LONG } len;
	char c;
	fmt[i++]= '%';
	for (c= *p; !strchr("cdeEfgGinopsuxX", c); c= *++p) {
	  if (c == '*') {
	    SPRINTF(&fmt[i], "%d", int, ap, i);
	  }
	  else {
	    fmt[i++]= c;
	    len= (c == 'l' || c == 'L') ? LONG : (c == 'h' ? SHORT : NORMAL);
	  }
	} /* for (c= *p; !strchr("cdeEfgGinopsuxX", c); c= *++p) */
	fmt[i++]= c; fmt[i++]= '\0';
	switch (c) {
	  case 'c': 
	    FPRINTF(stream, fmt, CHAR_PROMOTE, ap, n); break;
	  case 'd': case 'i':
	    switch (len) {
	      case NORMAL: 
		FPRINTF(stream, fmt, int, ap, n); break;
	      case SHORT:
		FPRINTF(stream, fmt, SHORT_PROMOTE, ap, n); break;
	      case LONG:
		FPRINTF(stream, fmt, long, ap, n); break;
	    }
	    break;
	  case 'u': case 'o': case 'x': case 'X':
	    switch (len) {
	      case NORMAL: 
		FPRINTF(stream, fmt, unsigned int, ap, n); break;
	      case SHORT:
		FPRINTF(stream, fmt, unsigned SHORT_PROMOTE, ap, n); break;
	      case LONG:
		FPRINTF(stream, fmt, unsigned long, ap, n); break;
	    }
	    break;
	  case 'f': case 'e': case 'E': case 'g': case 'G': 
	    switch (len) {
#ifdef CC_LONG_DOUBLE
	      case LONG:
		FPRINTF(stream, fmt, long double, ap, n); break;
#endif
	      default:
		FPRINTF(stream, fmt, double, ap, n); break;
	      
	    }
	    break;
	  case 's':
	    FPRINTF(stream, fmt, char *, ap, n); break;
	  case 'p':
	    FPRINTF(stream, fmt, VOIDP, ap, n); break;
	  case 'n':
	    switch (len) {
	      case NORMAL: 
		FPRINTF(stream, fmt, int *, ap, n); break;
	      case SHORT:
		FPRINTF(stream, fmt, short *, ap, n); break;
	      case LONG:
		FPRINTF(stream, fmt, long *, ap, n); break;
	    }
	    break;
	} /* switch (c) */
      } /* else definite conversion specification. */
    } /* else possible conversion specification. */
  } /* while (*p) */
  va_end(ap);
  return n;
}

#endif /* !HAVE_VPRINTF */

#ifdef TEST_VFPRINTF
/* Only testing ANSI-version. */


int 
printArgs(CONST char *fmt, ...)
{
  va_list ap;
  int n;
  va_start(ap, fmt);
  n= vfprintf(stdout, fmt, ap);
  va_end(ap);
  return n;
}

int main()
{
  printf("%%*s= %%4d:\n");
  printArgs("%*s= %4d\n", 10, "String", 27);
  printf("%%8.3g %%*.*g:\n");
  printArgs("%8.3g %*.*E\n", 32.134, 12, 2, 28.134e12);
  printf("|%%7lx| |%%*ho| |%%4c|:\n");
  printArgs("|%7lx| |%*ho| |%4c|\n", (unsigned long)1023, 
	    8, (unsigned short)256, 'a');
  exit(0);
}

#endif /* ifdef TEST_VFPRINTF */
