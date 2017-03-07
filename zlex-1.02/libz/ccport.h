/*

File:	 ccport.h
Purpose: Porting definitions to work-around compiler and library differences.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/
#ifndef _CCPORT
#define _CCPORT

#ifdef HAVE_CONFIG_H
/* As we use <config.h> and use `gcc -MM' to extract dependency
 * information, config.h won't be included as a prerequisite for
 * object files in the automatically extracted dependencies.  Hence
 * Makefiles will need to add an explicit dependency.  We can't use
 * include "config.h", as we don't know which directory config.h will
 * be created in until the actual build.  Also autoconf manual 
 * recommends using <config.h>.
 */
#include <config.h>
#endif

/*
Define basic types in an effort to obtain portability.  When porting to
a machine with different size words, this could be one of the files
which may need to be modified (via ifdefs if necessary).

Most macros defined here are defined conditionally, hence they can be
overriden via the command line or a previously included file.

We defined three varieties of Ints: Int1, Int2, Int4 which occupy AT
LEAST 1, 2, and 4 8-bit bytes respectively.  Similarly for unsigned
ints (UInt1, UInt2 and UInt4).  We also define MAX and MIN values for
these varieties: these values are independent of the selected base
types.  We define similar variants for common uses of such ints (Index
for array indices, Count for counts, etc). We also use variants
without any number suffix like UInt and Index, which map to the most
convenient source type and do not make any guarantees on size except
the guarantee made by the ANSI standard on the unqualified source
type.  Typically for numbers <= 2, we use the variants without numbers
as the types of variables (possibly automatic) which are not stored
within data structures, while we use the types with numbers for types
stored within data structures.

We use <limits.h> to find the smallest type which occupies the
required number of bytes.  If we can't use <limits.h> we assume that
the minimum ANSI ranges are met by the builtin integer types.

We try to make minimal assumptions about the cpp used.  We don't
indent cpp directives.  #error directives are indented so that
hopefully pre-ANSI compilers don't choke.

*/


/****************************** CC Values ******************************/ 

/*

#define CC macros specifying compiler and run-time library
variants.  These can be regarded as the inputs to this file.
If __STDC__, then all these macros default to 1; else 0.

*/

/* CC_ANSI == 1 iff compiler claims to be ANSI by defining __STDC__ == 1. */
#ifndef CC_ANSI
#if    __STDC__ == 1   /* Dont use ifdef as some ccs #define __STDC_ 0. */
#define CC_ANSI 1
#else
#define CC_ANSI 0
#endif
#endif

/* CC_CONST == 1 iff cc understands const. */
#ifndef CC_CONST
#define CC_CONST CC_ANSI
#endif

/* CC_LIMITS == 1 iff there is a <limits.h> file. */
#ifndef CC_LIMITS
#define CC_LIMITS CC_ANSI
#endif

/* CC_PROTO == 1 iff cc understands function prototypes. */
#ifndef CC_PROTO
#define CC_PROTO CC_ANSI
#endif

/* CC_SIGNED == 1 iff cc understands signed keyword. */
#ifndef CC_SIGNED
#define CC_SIGNED CC_ANSI
#endif

/* CC_SIZE_T == 1 iff cc library defines size_t. */
#ifndef CC_SIZE_T
#define CC_SIZE_T CC_ANSI
#endif

/* CC_STDARG == 1 iff cc understands ... & there is a <stdarg.h> file. */
#ifndef CC_STDARG
#define CC_STDARG CC_ANSI
#endif

/* CC_STRINGIFY == 1 iff cpp understands # MacroParam means "MacroParam". */
#ifndef CC_STRINGIFY
#define CC_STRINGIFY CC_ANSI
#endif

/* CC_TOK_CAT == 1 iff cpp understands ##. */
#ifndef CC_TOK_CAT
#define CC_TOK_CAT CC_ANSI
#endif

/* CC_U_INT == 1 iff cc & cpp understand constants like 1u. */
#ifndef CC_TOK_CAT
#define CC_TOK_CAT CC_ANSI
#endif

/* CC_VOID == 1 iff cc understands VOID. */
#ifndef CC_VOID
#define CC_VOID CC_ANSI
#endif


/******************************* CC_CONST ******************************/

#if CC_CONST

#ifndef CONST
#define CONST const
#endif

#else	/* CC_CONST == 0 */

#ifndef CONST
#define CONST
#endif

/* In case const has been used directly in a system header file. */
#ifndef const
#define const
#endif

#endif	/* if CC_CONST */


/****************************** CC_PROTO *******************************/

#ifndef PROTO
#if CC_PROTO
#define PROTO(args)	args
#else
#define PROTO(args)	()
#endif
#endif /* ifndef PROTO */



/****************************** CC_SIGNED ******************************/

#if CC_SIGNED

#ifndef SIGNED
#define SIGNED signed
#endif

#else	/* CC_SIGNED == 0. */

#ifndef SIGNED
#define SIGNED
#endif

/* In case signed has been used directly in a system header file. */
#ifndef signed
#define signed
#endif

#endif 	/* if CC_SIGNED */


/******************************* CC_VOID *******************************/

#if CC_VOID 

/* Function return type. */
#ifndef VOID
#define VOID  void
#endif

/* Generic pointer. */
#ifndef VOIDP
#define VOIDP void * 
#endif

/* Empty arguments. */
#ifndef VOID_ARGS
#define VOID_ARGS void
#endif

/* Return from a function returning VOID. */
#ifndef VOID_RET
#define VOID_RET() return
#endif

#else	/* CC_VOID == 0. */

/* Function return type. */
#ifndef VOID
#define VOID int
#endif

/* Generic pointer. */
#ifndef VOIDP
#define VOIDP char *
#endif

/* Empty arguments. */
#ifndef VOID_ARGS
#define VOID_ARGS
#endif

/* Return from a function returning VOID. */
#ifndef VOID_RET
#define VOID_RET() return 0
#endif

/* In case void has been used directly in a system header file, there
 * doesn't appear to be any way of handling the two uses as in f(void)
 * and f(void *).
 */
#ifndef void
#define void char /* Assume used as void * */	
#endif

#endif	/* #if CC_VOID */



/****************************** CC_SIZE_T ******************************/

#ifndef SIZE_T
#if CC_SIZE_T || HAVE_STDDEF_H
#define SIZE_T size_t
#else
#define SIZE_T unsigned	/* Maybe unsigned long? */
#endif
#endif


/***************************** CC_STRINGIFY ****************************/

/* Note that the intent below is that we do not expand the argument of
 * STRINGIFY() (like ANSI cc where the argument of the # stringify
 * operator is not expanded).  For non-ANSI cc's, this intent may or may 
 * not be achieved.
 */

#ifndef STRINGIFY

#if CC_STRINGIFY
#define STRINGIFY(s)		#s
#else
#define STRINGIFY(s)		"s"
#endif

#endif


/***************************** CC_TOK_CAT ******************************/

/* On some super brain-damaged cc's, when the non-ANSI definition of
 * TOK_CAT is used, there shouldn't be any spaces between the actual
 * arguments to the macro: i.e., TOK_CAT(one,token) will work but
 * TOK_CAT(one, tok) will not work.
 *
 * If the non-ANSI form of TOK_CAT doesn't work, then the following
 * could be tried:
 * #define _IDENTITY(x)		x
 * #define TOK_CAT(t1, t2)	_IDENTITY(t1)t2
 *
 * Note that the intent below is that we do expand the arguments of
 * TOK_CAT() (unlike ANSI cc where arguments around the ## catenation
 * operator are not expanded).  For non-ANSI cc's, this intent may or
 * may not be achieved.
 */

#ifndef TOK_CAT

#if CC_TOK_CAT
#define TOK_CAT(t1, t2)		_TOK_CAT(t1, t2)
#define _TOK_CAT(t1, t2)	t1 ## t2
#else
#define TOK_CAT(t1, t2)		t1/* */t2
#endif

#endif


/******************************* CC_U_INT ******************************/

/* Avoid using the following macro in #if tests, as some cpps get upset
 * at function-like macros used within cpp expressions.
 */

#ifndef MAKE_U_INT
#if CC_U_INT
#define MAKE_U_INT(i)	TOK_CAT(i,U)
#else
#define MAKE_U_INT(i)	i
#endif
#endif


/*			STANDARD LIBRARY HEADERS.			*/

/* We include all library headers here.  This is because this file
 * defines symbols normally defined by <limits.h>.  Even if the system
 * doesn't appear to have a satisfactory <limits.h> some of the
 * symbols in <limits.h> may get defined by an indirect route thru
 * some other header.  By including all the headers here, and making
 * the definitions of the <limits.h> constants conditional, we try to
 * avoid problems.
 */

/* One would hope that all systems have some forms of these headers. */
#include <assert.h>
#include <ctype.h>
#include <stdio.h>

#if CC_STDARG
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>	/* for NULL, size_t. */
#endif

#if STDC_HEADERS
#include <stdlib.h>
#include <string.h>
#else			/* we don't appear to have ANSI headers. */
#if !HAVE_STRCHR
#define strchr index
#define strrchr rindex
#endif
char *strchr(), *strrchr();
#if !HAVE_MEMCPY
#define memcpy(d, s, n) 	bcopy((s), (d), (n))
#define memmove(d, s, n)	bcopy((s), (d), (n))
/* Redefinition of memcmp() only guaranteed to work for equality checks. */
#define memcmp(d, s, n)		bcmp((s), (d), (n))
#endif	/* if !HAVE_MEMCPY */

#endif	/* else we don't appear to have ANSI headers. */



/****************************** CC_LIMITS ******************************/

/* In the following include we check CC_ANSI in addition to CC_LIMITS.
 * That is because when we don't have ANSI, some <limits.h> define
 * unsigned constants using a (unsigned) cast.  This causes problems
 * when the constant is used in a cpp #if expression.
 */
#if CC_LIMITS && CC_ANSI
#include <limits.h>
#else	/* <limits.h> not included. */

/* Since some cpps get upset at function style macros in #ifs, we define
 * these constants here, rather than using MAKE_U_INT().
 */
#if CC_U_INT
#define _UCHAR_MAX	0xFFU
#define _USHRT_MAX	0xFFFFU
#define _ULONG_MAX	0xFFFFFFFFU
#else					/* Take our chances here. */
#define _UCHAR_MAX	0xFF
#define _USHRT_MAX	0xFFFF
#define _ULONG_MAX	0xFFFFFFFF
#endif

/* Assume what should be worst-case limits */

/* Plain char may be either signed or unsigned.  Assume worst. */
#ifndef CHAR_MIN
#define CHAR_MIN	0
#endif
#ifndef CHAR_MAX
#define CHAR_MAX 	127
#endif

#if CC_SIGNED
#ifndef SCHAR_MIN
#define SCHAR_MIN	-128
#endif
#else
/* Since non-ansi cc's have only plain chars, don't assume neg. values. */
#ifndef SCHAR_MIN
#define SCHAR_MIN	0
#endif
#endif /* if CC_SIGNED */
#ifndef SCHAR_MAX
#define SCHAR_MAX	127
#endif

#ifndef UCHAR_MAX
#define UCHAR_MAX	_UCHAR_MAX
#endif

#ifndef SHRT_MIN
#define SHRT_MIN	-32768
#endif
#ifndef SHRT_MAX
#define SHRT_MAX	32767
#endif

#ifndef USHRT_MAX
#define USHRT_MAX	_USHRT_MAX
#endif

#ifndef INT_MIN
#define INT_MIN		SHRT_MIN
#endif
#ifndef INT_MAX
#define INT_MAX		SHRT_MAX
#endif

#ifndef UINT_MAX
#define UINT_MAX	USHRT_MAX
#endif

#ifndef LONG_MIN
#define LONG_MIN	(-LONG_MAX - 1) /* Some cc's choke on direct val. */
#endif
#ifndef LONG_MAX
#define LONG_MAX	2147483647L
#endif

#ifndef ULONG_MAX
#define ULONG_MAX	_ULONG_MAX
#endif

#ifndef CHAR_BIT
#define CHAR_BIT	8
#endif

#endif /* else <limits.h> not included. */


/*************************** FLAVORS OF INTS ***************************/

/* We type variants 1, 2, 4 where type-variant n has at least the
 * range of values representable in n 8-bit bytes.
 *
 * We don't use function-like macros within #if #elif tests as some
 * ccs get confused.
 */

/******************************** Int2 *********************************/

#define INT2_MIN (-0x8000L)
#define INT2_MAX 0x7FFFL
#if (SCHAR_MIN <= INT2_MIN && SCHAR_MAX >= INT2_MAX)
  typedef SIGNED char Int2;
#elif (SHRT_MIN <= INT2_MIN && SHRT_MAX >= INT2_MAX)
  typedef short Int2;
#else
  #error "Type short does not meet ANSI range."
#endif

/******************************** Int1 *********************************/

#define INT1_MIN (-0x80)
#define INT1_MAX 127

#if CC_SIGNED	/* We can produce signed chars. */

#if (SCHAR_MIN <= INT1_MIN && SCHAR_MAX >= INT1_MAX)
  typedef SIGNED char Int1;
#else
  #error "Type signed char does not meet ANSI range."
#endif

#else	/* CC_SIGNED == 0. */

  typedef Int2 Int1;

#endif	/* if CC_SIGNED */

/******************************** Int4 *********************************/

#define INT4_MIN (-0x80000000L)
#define INT4_MAX 0x7FFFFFFFL
#if (SCHAR_MIN <= INT4_MIN && SCHAR_MAX >= INT4_MAX)
  typedef SIGNED char Int4;
#elif (SHRT_MIN <= INT4_MIN && SHRT_MAX >= INT4_MAX)
  typedef short Int4;
#elif (INT_MIN <= INT4_MIN && INT_MAX >= INT4_MAX)
  typedef int Int4;
#elif (LONG_MIN <= INT4_MIN && LONG_MAX >= INT4_MAX)
  typedef long Int4;
#else
  #error "Type long does not meet ANSI range."
#endif


/*************************** FLAVORS OF UINTS **************************/

/******************************** UInt1 ********************************/

#define UINT1_MAX 0xFF
#if (UCHAR_MAX >= UINT1_MAX)
  typedef unsigned char UInt1;
#else
  #error "Type unsigned char does not meet ANSI range."
#endif

/******************************** UInt2 ********************************/

#define UINT2_MAX 0x0000FFFFL
#if (UCHAR_MAX >= UINT2_MAX)
  typedef unsigned char UInt2;
#elif (USHRT_MAX >= UINT2_MAX)
  typedef unsigned short UInt2;
#else
  #error "Type unsigned short does not meet ANSI range."
#endif

/******************************** UInt4 ********************************/

/* The problem here is that some cpps regard any large number like 
 * 0xFFFFFFFFL as -1!  We cope with this by comparing max sizes of
 * *signed* types which shouldn't have the problem.
 */
#define UINT4_MAX ((unsigned)0xFFFFFFFFL)
#define _XMAX 0x7FFFFFFFL
#if (CHAR_MAX >= _XMAX)
  typedef unsigned char UInt4;
#elif (SHRT_MAX >= _XMAX)
  typedef unsigned short UInt4;
#elif (INT_MAX >= _XMAX)
  typedef unsigned int UInt4;
#elif (LONG_MAX >= _XMAX)
  typedef unsigned long UInt4;
#else
  #error "Type unsigned long does not meet ANSI range."
#endif
#undef _XMAX


/************************* CONVENIENCE TYPEDEFS ************************/

/* The following types are for convenience and to allow all types to
 * start with a upper-case letter. 
 */

typedef float		Float;
typedef double		Double;
/* We could typedef LDouble here, but we don't need it and some ccs
 * don't support it.
 */

typedef int		Int;
typedef short		Short;
typedef long		Long;
typedef unsigned int 	UInt;
typedef unsigned short 	UShort;
typedef unsigned long 	ULong;


typedef unsigned char	UChar;
#ifdef CC_SIGNED
typedef SIGNED char 	SChar;
#endif
typedef char		Char;

typedef UInt1		Boolean;
typedef unsigned 	BooleanX;	/* For parameter declarations. */

/* The foll. may cause problems with some system headers.  If so, include
 * the headers before this file.
 */
#undef FALSE
#undef TRUE
enum { FALSE= 0, TRUE= 1 };

typedef UInt1		Truth3;
enum { FALSE_3= FALSE, TRUE_3= TRUE, UNKNOWN_3 };

typedef Char 		*String;	/* NUL-terminated Char-string. */

typedef CONST Char 	*ConstString; 	/* ptr. to CONST Strings. */

typedef UInt1		Byte;		/* At least 0...255.	*/


/************************** ABSTRACT TYPEDEFS **************************/

/* Abstract out most common uses of integers.  Typically, the type without
 * any size qualification (like plain Index) is used to declare local vars,
 * whereas the size qualified type (like Index2) is used for more permanent
 * data where long-term storage is involved.
 */

/* Count Types --- used as types of counters. */
typedef UInt  Count;
typedef UInt1 Count1;
typedef UInt2 Count2;
typedef UInt4 Count4;


/* Index Types --- used to index arrays. */
typedef UInt  Index;
typedef UInt1 Index1;
typedef UInt2 Index2;
typedef UInt4 Index4;

/* Size Types --- used to Size arrays and strings. Cast to size_t when passing
 * to system functions.
 */
typedef UInt  Size;
typedef UInt1 Size1;
typedef UInt2 Size2;
typedef UInt4 Size4;

enum {	/* Used to indicate invalid index, etc.	*/
  NIL= UINT2_MAX 
};


/*			TYPE CHECKING OF GENERIC TYPES.			*/

/*

Assume the following scenario:

typedef int FHandle;		#File handle.
typedef int WHandle;		#Window handle.
void function(FHandle, WHandle);	

void g() {
  FHandle f;
  WHandle w;
  function(w, f);		#Erroneous call.
}

The above erroneous call cannot be signalled by the compiler because as
far as it is concerned an int is an int is an int.

A solution is to use structure wrappers around the generic types.  So for
example if the above typedef were changed to

typedef struct { int val; } FHandle;		#File handle.
typedef struct { int val; } WHandle;		#Window handle.

the compiler would now signal the erroneous call.

We define macros TYPE(t) to declare a struct wrapper around some type
t; the macro V(x) to access the value of an object x having a wrapped
type; TYPE_INIT(v) as a initializer with value v.  Since passing
structures can be expensive with some compilers, we make the wrapper
conditional on the preprocessor symbol CHECK_GENERIC_TYPES.  Using
V(x) continually to access the value of x is rather tedious, but can't
be helped.

We don't need to necessarily run the program with CHECK_GENERIC_TYPES;
we need to merely compile it.  Hence it isn't necessary to have
separate libraries compiled with -DCHECK_GENERIC_TYPES.  It also may be
acceptable to get some errors with -DCHECK_GENERIC_TYPES if those
are expected (see below).

The wrapped types can come in the way of using function pointers.
Function pointers to functions involving wrapped types make sense only
if the underlying generic types are the same.  The function pointers
would have to be declared using the generic type as illustrated in the
following scenario:

typedef TYPE(VOIDP) Module1Context;
typedef TYPE(VOIDP) Module2Context;

void printModule1(Module1Context);
void printModule2(Module2Context);

void (*p)(VOIDP);

p= printModule1; 
p= printModule2;

The above will be ok if !-DCHECK_GENERIC_TYPES but the assignment
statements to p will result in compile-time errors if
-DCHECK_GENERIC_TYPES.  

This sort of situation can arise when storing function pointers in a
table where the function pointers are type compatible based on their
generic types but incompatible based on their wrapped types.

One solution is to define prototypes (no definitions are needed if
compilation, not execution is the goal) for generic versions of the
functions.  So if we define:

#ifdef CHECK_GENERIC_TYPES
#define PM1 printModule1X
#define PM2 printModule2X
void printModule1X(VOIDP);
void printModule2X(VOIDP);
#else
#define PM1 printModule1
#define PM2 printModule2
#endif

then

p= PM1;
p= PM2;

will be legal whether or not CHECK_GENERIC_TYPES is defined.

Instead of all this messing around, it may be preferable to allow
errors when -DCHECK_GENERIC_TYPES (with the expected errors commented
in the source).

****************
This seems a decent idea, but I haven't used it as the V()s clutter up
the code too much.  What would be really nice is to use a cc which
had the right kind of (non-standard) typedef compatibility: 

We can think of typedefs establishing a DAG with nodes equal to the
typedef names and types and a `typedef A B;' definition establishing
an edge from B to A.  2 typedef's are compatible iff there is a path
from one to the other along the chain of typedef definitions.  Hence
given the typedefs

typedef int Int;
typedef Int WHandle;
typedef Int FHandle;


WHandle == Int == int and FHandle == Int == int, but WHandle != FHandle.
****************

*/

#ifdef CHECK_GENERIC_TYPES
#define TYPE(T)	struct { T val; }
#define V(x)	((x).val)
#define TYPE_INIT(v)	{ (v) }
#else
#define TYPE(T)	T
#define V(x)	(x)
#define TYPE_INIT(v)	(v)
#endif


/*			    CONVENIENCE MACROS.				*/

/* Return # of elements in a statically allocated array. */
#define N_ELEMENTS(array)	(sizeof(array)/sizeof(array[0]))


#endif /* ifndef _CCPORT */

