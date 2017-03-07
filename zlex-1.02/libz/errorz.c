#include "autobuf.h"
#include "errorz.h"

#include <errno.h>

#if CC_STDARG

VOID 
errorz(ErrorStruct *errP, unsigned errFlags, ConstString fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  verrorz(errP, errFlags, fmt, ap);
  va_end(ap);
  VOID_RET();
}

#else /* !CC_STDARG */

VOID 
errorz(va_alist)
 va_dcl
{
  va_list ap;
  ErrorStruct *errP;
  unsigned errFlags;
  ConstString fmt;
  va_start(ap);
  errP= va_arg(ap, ErrorStruct *);
  errFlags= va_arg(ap, unsigned);
  fmt= va_arg(ap, ConstString);
  verrorz(errP, errFlags, fmt, ap);
  va_end(ap);
  VOID_RET();
}

#endif /* else !CC_STDARG */


/* The characters which can appear between the '%' and the conversion-char
 * in a printf() format specification.  Assumes errFile in scope.
 */
#define PRINTF_FLAG_CHARS	"-+# 0123456789*.lLh"

#define DO_FPRINTF(nStars, type) 					\
  do {									\
    switch (nStars) {							\
      case 0:								\
	fprintf(errFile, AUTO_BUF(xfmt), va_arg(ap, type));		\
	break;								\
      case 1:								\
	fprintf(errFile, AUTO_BUF(xfmt), 				\
		va_arg(ap, int), va_arg(ap, type));			\
	break;								\
      default:								\
	fprintf(errFile, AUTO_BUF(xfmt), 				\
		va_arg(ap, int), va_arg(ap, int), va_arg(ap, type));	\
	break;								\
    } /* switch (nStars) */						\
  } while (0)

VOID 
verrorz(errP, errFlags, fmt, ap)
  ErrorStruct *errP; 
  unsigned errFlags; 
  ConstString fmt; 
  va_list ap;
{
  ConstString p;
  Boolean prefixSpecs= FALSE;
  FILE *errFile= errP->errFile;
  DCL_AUTO_BUF(char, 64) xfmt;
  INIT_AUTO_BUF(xfmt);

  /* Take care of flags with prefix effects. */
  errP->nErrors+= ((errFlags & ER_ERROR) != 0);
  if ((errFlags & ER_PRG) != 0 && errP->prgFn) {
    ConstString prg= errP->prgFn();
    if (prg) {
      fprintf(errFile, "%s:", prg);
      prefixSpecs= TRUE;
    }
  }
  if ((errFlags & ER_FILE) != 0 && errP->fileFn) {
    ConstString file= errP->fileFn();
    if (file) {
      fprintf(errFile, "%s:", file);
      prefixSpecs= TRUE;
    }
  }
  if ((errFlags & ER_LINE) != 0 && errP->lineFn) {
    UInt lineN= errP->lineFn();
    if (lineN) {
      fprintf(errFile, "%d:", lineN);
      prefixSpecs= TRUE;
    }
  }
  if (prefixSpecs) fprintf(errFile, " ");

  if (errFlags & ER_WARN) {
    fprintf(errFile, "warning: ");
  }

  /* Process fmt, calling fprintf() or user routines as appropriate. */
  for (p= fmt; *p; p++) {
    if (*p != '%') {
      fputc(*p, errFile);
    }
    else { /* format specifier. */
      ConstString fmtStart= p;
      Count nStars= 0;
      Size fmtLen;
      while (strchr(PRINTF_FLAG_CHARS, *++p)) nStars+= (*p == '*');
      if (nStars > 2) {
	libzError(ER_FATAL, "Too many `*' specifiers in format `%s'\n", fmt);
      }
      fmtLen= p - fmtStart + 1;
      CHECK_AUTO_BUF(xfmt, fmtLen + 1);
      strncpy(AUTO_BUF(xfmt), fmtStart, fmtLen);
      AUTO_BUF(xfmt)[fmtLen]= '\0';
      switch (*p) {
	case '%':
	  fputc('%', errFile);
	  break;
	case 'c':  case 'd':  case 'i':  
        case 'u':  case 'o':  case 'x':  case 'X':
	  /* We assume shorts are widened to ints.  Hence no check for h. */
	  if (p[-1] == 'l') DO_FPRINTF(nStars, long);
	  else DO_FPRINTF(nStars, int);
	  break;
	case 'f':  case 'e':  case 'E':  case 'g':  case 'G':
#if HAS_LONG_DOUBLE
	  if (p[-1] == 'L') DO_FPRINTF(nStars, long double);
	  else 
#endif
	    DO_FPRINTF(nStars, double);
	  break;
	case 's':  case 'p':  case 'n':
	  DO_FPRINTF(nStars, VOIDP);
	  break;
	default: {
	  CONST char c= *p;
	  ErrorFmt *fP= errP->fmt;
	  if (fP) {
	    while (fP->fmtChar != '\0' && fP->fmtChar != c) fP++;
	  }
	  if (fP && fP->fmtChar == c) {
	    ap= fP->fmtFn(errFile, AUTO_BUF(xfmt), ap);
	  }
	  else {
	    libzError(ER_FATAL,
		      "Bad format `%%%c' in error format `%s'\n", c, fmt);
	  }
	} /* default */
      } /* switch (*++p) */
    } /* else format specifier. */
  } /* for (p= fmt; *p; p++) */

  /* Process flags with suffix effects. */
  if (errFlags & ER_NL) {
    fprintf(errFile, "\n");
  }
  if (errFlags & ER_SYS) {
    /* We'd really like strerror(), but that may not be portable. */
    fprintf(errFile, "system error\n"); 
  }
  if (errFlags & ER_FATAL) {
    fprintf(errFile, "fatal termination.\n");
    exit(1);
  }
  if (errP->nErrors >= errP->maxErrors) {
    fprintf(errFile, "too many errors --- bailing out\n"); exit(1);
  }
  FREE_AUTO_BUF(xfmt);
  VOID_RET();
}

