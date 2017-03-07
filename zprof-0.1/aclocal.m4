dnl Copied from autoconf distribution.  Change action to define CC_CONST
dnl to 0 or 1.
AC_DEFUN(ZZ_CC_CONST,
[dnl This message is consistent in form with the other checking messages,
dnl and with the result message.
AC_MSG_CHECKING([for working 'const'])
AC_CACHE_VAL(zz_cv_cc_const,
[AC_TRY_COMPILE(,
changequote(<<, >>)dnl
<<
/* Ultrix mips cc rejects this.  */
typedef int charset[2]; const charset x;
/* SunOS 4.1.1 cc rejects this.  */
char const *const *ccp;
char **p;
/* NEC SVR4.0.2 mips cc rejects this.  */
struct point {int x, y;};
static struct point const zero;
/* AIX XL C 1.02.0.0 rejects this.
   It does not let you subtract one const X* pointer from another in an arm
   of an if-expression whose if-part is not a constant expression */
const char *g = "string";
ccp = &g + (g ? g-g : 0);
/* HPUX 7.0 cc rejects these. */
++ccp;
p = (char**) ccp;
ccp = (char const *const *) p;
{ /* SCO 3.2v4 cc rejects this.  */
  char *t;
  char const *s = 0 ? (char *) 0 : (char const *) 0;

  *t++ = 0;
}
{ /* Someone thinks the Sun supposedly-ANSI compiler will reject this.  */
  int x[] = {25, 17};
  const int *foo = &x[0];
  ++foo;
}
{ /* Sun SC1.0 ANSI compiler rejects this -- but not the above. */
  typedef const int *iptr;
  iptr p = 0;
  ++p;
}
{ /* AIX XL C 1.02.0.0 rejects this saying
     "k.c", line 2.27: 1506-025 (S) Operand must be a modifiable lvalue. */
  struct s { int j; const int *ap[3]; };
  struct s *b; b->j = 5;
}
{ /* ULTRIX-32 V3.1 (Rev 9) vcc rejects this */
  const int foo = 10;
}
>>,
changequote([, ])dnl
zz_cv_cc_const=yes, zz_cv_cc_const=no)])dnl
AC_MSG_RESULT($zz_cv_cc_const)
if test $zz_cv_cc_const = yes; then
  AC_DEFINE(CC_CONST, 1)
else
  AC_DEFINE(CC_CONST, 0)
fi
])

AC_DEFUN(ZZ_CC_LIMITS,
[dnl This message is consistent in form with the other checking messages,
dnl and with the result message.
AC_MSG_CHECKING([for <limits.h> file])
AC_CACHE_VAL(zz_cv_cc_limits,
[AC_TRY_COMPILE(,
changequote(<<, >>)dnl
<<
#include <limits.h>
/* Ensure that it defines the things we need. */

#ifndef UCHAR_MAX
  Cause a syntax error.
#endif

#ifndef USHRT_MAX
  Cause a syntax error.
#endif

#ifndef UINT_MAX
  Cause a syntax error.
#endif

#ifndef ULONG_MAX
  Cause a syntax error.
#endif

#ifndef CHAR_MAX
  Cause a syntax error.
#endif

#ifndef CHAR_MIN
  Cause a syntax error.
#endif

#ifndef SCHAR_MAX
  Cause a syntax error.
#endif

#ifndef SCHAR_MIN
  Cause a syntax error.
#endif

#ifndef SHRT_MIN
  Cause a syntax error.
#endif

#ifndef SHRT_MAX
  Cause a syntax error.
#endif

#ifndef INT_MIN
  Cause a syntax error.
#endif

#ifndef INT_MAX
  Cause a syntax error.
#endif

#ifndef LONG_MIN
  Cause a syntax error.
#endif

#ifndef LONG_MAX
  Cause a syntax error.
#endif

#ifndef CHAR_BIT
  Cause a syntax error.
#endif

>>,
changequote([, ])dnl
zz_cv_cc_limits=yes, zz_cv_cc_limits=no)])dnl
AC_MSG_RESULT($zz_cv_cc_limits)
if test $zz_cv_cc_limits = yes; then
  AC_DEFINE(CC_LIMITS, 1)
else
  AC_DEFINE(CC_LIMITS, 0)
fi
])

dnl AC_C_LONG_DOUBLE checks if long double works at runtime without a
dnl cross-compilation backup.  We settle here for a compile-time check.
AC_DEFUN(ZZ_CC_LONG_DOUBLE,
[dnl This message is consistent in form with the other checking messages,
dnl and with the result message.
AC_MSG_CHECKING([if compiler accepts 'long double'])
AC_CACHE_VAL(zz_cv_cc_long_double,
[AC_TRY_COMPILE(,
changequote(<<, >>)dnl
<<
long double a= 12.22;
unsigned n= sizeof(long double);
>>,
changequote([, ])dnl
zz_cv_cc_long_double=yes, zz_cv_cc_long_double=no)])dnl
AC_MSG_RESULT($zz_cv_cc_long_double)
if test $zz_cv_cc_long_double = yes; then
  AC_DEFINE(CC_LONG_DOUBLE, 1)
else
  AC_DEFINE(CC_LONG_DOUBLE, 0)
fi
])

AC_DEFUN(ZZ_CC_PROTO,
[dnl This message is consistent in form with the other checking messages,
dnl and with the result message.
AC_MSG_CHECKING([for function prototypes])
AC_CACHE_VAL(zz_cv_cc_proto,
[AC_TRY_COMPILE(,
changequote(<<, >>)dnl
<<
/* Some declarations. */
unsigned f(int a);
char *p(unsigned a, char **);

/* Some typedefs. */
typedef unsigned F(int a);
typedef char *P(unsigned a, char **);

/* See if it works within a struct. Blows MIPS/ULTRIX cc. */
struct {
  int (*f)(int a, float b);
} x;
>>,
changequote([, ])dnl
zz_cv_cc_proto=yes, zz_cv_cc_proto=no)])dnl
AC_MSG_RESULT($zz_cv_cc_proto)
if test $zz_cv_cc_proto = yes; then
  AC_DEFINE(CC_PROTO, 1)
else
  AC_DEFINE(CC_PROTO, 0)
fi
])

AC_DEFUN(ZZ_CC_SIGNED,
[dnl This message is consistent in form with the other checking messages,
dnl and with the result message.
AC_MSG_CHECKING([for working 'signed' keyword])
AC_CACHE_VAL(zz_cv_cc_signed,
[AC_TRY_COMPILE(,
changequote(<<, >>)dnl
<<
signed char c;
signed int a= -1;
signed long b= -25;
>>,
changequote([, ])dnl
zz_cv_cc_signed=yes, zz_cv_cc_signed=no)])dnl
AC_MSG_RESULT($zz_cv_cc_signed)
if test $zz_cv_cc_signed = yes; then
  AC_DEFINE(CC_SIGNED, 1)
else
  AC_DEFINE(CC_SIGNED, 0)
fi
])

AC_DEFUN(ZZ_CC_SIZE_T,
[dnl This message is consistent in form with the other checking messages,
dnl and with the result message.
AC_MSG_CHECKING([for working 'size_t' type in <stdio.h>])
AC_CACHE_VAL(zz_cv_cc_size_t,
[AC_TRY_COMPILE(,
changequote(<<, >>)dnl
<<
#include <stdio.h>
size_t c;
size_t *p;
>>,
changequote([, ])dnl
zz_cv_cc_size_t=yes, zz_cv_cc_size_t=no)])dnl
AC_MSG_RESULT($zz_cv_cc_size_t)
if test $zz_cv_cc_size_t = yes; then
  AC_DEFINE(CC_SIZE_T, 1)
else
  AC_DEFINE(CC_SIZE_T, 0)
fi
])

AC_DEFUN(ZZ_CC_STDARG,
[dnl This message is consistent in form with the other checking messages,
dnl and with the result message.
AC_MSG_CHECKING([for working <stdarg.h> file and ... in prototypes])
AC_CACHE_VAL(zz_cv_cc_stdarg,
[AC_TRY_COMPILE(,
changequote(<<, >>)dnl
<<
#include <stdarg.h>
int *f(int a, ...);
>>,
changequote([, ])dnl
zz_cv_cc_stdarg=yes, zz_cv_cc_stdarg=no)])dnl
AC_MSG_RESULT($zz_cv_cc_stdarg)
if test $zz_cv_cc_stdarg = yes; then
  AC_DEFINE(CC_STDARG, 1)
else
  AC_DEFINE(CC_STDARG, 0)
fi
])

AC_DEFUN(ZZ_CC_STRCAT,
[dnl This message is consistent in form with the other checking messages,
dnl and with the result message.
AC_MSG_CHECKING([for catenation of adjacent string literals])
AC_CACHE_VAL(zz_cv_cc_strcat,
[AC_TRY_COMPILE(,
changequote(<<, >>)dnl
<<
char *str= 
  "abc\n" 
  "def\n";
>>,
changequote([, ])dnl
zz_cv_cc_strcat=yes, zz_cv_cc_strcat=no)])dnl
AC_MSG_RESULT($zz_cv_cc_strcat)
if test $zz_cv_cc_strcat = yes; then
  AC_DEFINE(CC_STRCAT, 1)
else
  AC_DEFINE(CC_STRCAT, 0)
fi
])

AC_DEFUN(ZZ_CC_STRINGIFY,
[dnl This message is consistent in form with the other checking messages,
dnl and with the result message.
AC_MSG_CHECKING([for working cpp stringify operator])
AC_CACHE_VAL(zz_cv_cc_stringify,
[AC_TRY_COMPILE(,
changequote(<<, >>)dnl
<<
#define F(a)	#a
char *greet= F(Hello world);
>>,
changequote([, ])dnl
zz_cv_cc_stringify=yes, zz_cv_cc_stringify=no)])dnl
AC_MSG_RESULT($zz_cv_cc_stringify)
if test $zz_cv_cc_stringify = yes; then
  AC_DEFINE(CC_STRINGIFY, 1)
else
  AC_DEFINE(CC_STRINGIFY, 0)
fi
])

AC_DEFUN(ZZ_CC_TOK_CAT,
[dnl This message is consistent in form with the other checking messages,
dnl and with the result message.
AC_MSG_CHECKING([for working cpp token catenation operator])
AC_CACHE_VAL(zz_cv_cc_tok_cat,
[AC_TRY_COMPILE(,
changequote(<<, >>)dnl
<<
#define F(a, b)	a ## b
/* double d= F(1e, -4); This is too hard for some cc's which almost work */
   double d= F(1, 4);
>>,
changequote([, ])dnl
zz_cv_cc_tok_cat=yes, zz_cv_cc_tok_cat=no)])dnl
AC_MSG_RESULT($zz_cv_cc_tok_cat)
if test $zz_cv_cc_tok_cat = yes; then
  AC_DEFINE(CC_TOK_CAT, 1)
else
  AC_DEFINE(CC_TOK_CAT, 0)
fi
])

AC_DEFUN(ZZ_CC_UINT,
[dnl This message is consistent in form with the other checking messages,
dnl and with the result message.
AC_MSG_CHECKING([for unsigned ('u' or 'U') integer suffix])
AC_CACHE_VAL(zz_cv_cc_uint,
[AC_TRY_COMPILE(,
changequote(<<, >>)dnl
<<
unsigned i= 0xFFFFu;
unsigned j= 0xFFFFU;
#if (12U > 1u)
unsigned k;
#endif
>>,
changequote([, ])dnl
zz_cv_cc_uint=yes, zz_cv_cc_uint=no)])dnl
AC_MSG_RESULT($zz_cv_cc_uint)
if test $zz_cv_cc_uint = yes; then
  AC_DEFINE(CC_UINT, 1)
else
  AC_DEFINE(CC_UINT, 0)
fi
])

AC_DEFUN(ZZ_CC_VOID,
[dnl This message is consistent in form with the other checking messages,
dnl and with the result message.
AC_MSG_CHECKING([for working 'void'])
AC_CACHE_VAL(zz_cv_cc_void,
[AC_TRY_COMPILE(,
changequote(<<, >>)dnl
<<
typedef void *VoidP;
>>,
changequote([, ])dnl
zz_cv_cc_void=yes, zz_cv_cc_void=no)])dnl
AC_MSG_RESULT($zz_cv_cc_void)
if test $zz_cv_cc_void = yes; then
  AC_DEFINE(CC_VOID, 1)
else
  AC_DEFINE(CC_VOID, 0)
fi
])

AC_DEFUN(ZZ_CC_ANSI_TO_KNR,
[dnl Define output var ANSI_TO_KNR to "ansi2knr.0" if cc catenates 
dnl adjacent string literals and handles prototypes; else to 
dnl "ansi2knr.1".
AC_MSG_CHECKING([whether 'ansi2knr' should be run])
AC_REQUIRE([ZZ_CC_PROTO])dnl
AC_REQUIRE([ZZ_CC_STRCAT])dnl
AC_CACHE_VAL(zz_cv_cc_ANSI_TO_KNR,
[zz_cv_cc_ANSI_TO_KNR="ansi2knr.1"
test $zz_cv_cc_proto = "yes" && test $zz_cv_cc_strcat = "yes" && \
zz_cv_cc_ANSI_TO_KNR="ansi2knr.0"
])dnl
if test $zz_cv_cc_ANSI_TO_KNR = "ansi2knr.1"; then
  AC_MSG_RESULT("yes")
else
  AC_MSG_RESULT("no")
fi
ANSI_TO_KNR=$zz_cv_cc_ANSI_TO_KNR
AC_SUBST(ANSI_TO_KNR)dnl
])

AC_DEFUN(ZZ_LIB_REALLOC_NULL,
[dnl Define CPP macro REALLOC_NULL if realloc() and free() handle NULL.
AC_MSG_CHECKING([if realloc() and free() can handle a NULL pointer])
AC_CACHE_VAL(zz_cv_lib_realloc_null,
[AC_TRY_RUN(
changequote(<<, >>)dnl
<<
int main() {
  free(0); realloc(0, 0); exit(0);
}
>>,
changequote([, ])dnl,
zz_cv_lib_realloc_null=yes, zz_cv_lib_realloc_null=no, dnl
zz_cv_lib_realloc_null=no)])dnl
AC_MSG_RESULT($zz_cv_lib_realloc_null)
if test $zz_cv_lib_realloc_null = yes; then
  AC_DEFINE(REALLOC_NULL, 1)
else
  AC_DEFINE(REALLOC_NULL, 0)
fi
])

