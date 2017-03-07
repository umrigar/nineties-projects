/*

File:	 libzlex.h
Purpose: External interface to zlex library.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _LIBZLEX_H
#define _LIBZLEX_H

#include <stdio.h>

/* These are definitions exported by the zlex library. */


#if __STDC__

#ifndef YY_CONST
#define YY_CONST		const
#endif

#ifndef YY_VOID
#define YY_VOID			void
#endif

#ifndef YY_VOIDP
#define YY_VOIDP		void *
#endif

#ifndef YY_VOID_ARGS
#define YY_VOID_ARGS		void
#endif

#ifndef YY_PROTO
#define YY_PROTO(p)		p
#endif

#else	/* !defined(__STDC__) || __STDC__ == 0. */

#ifndef YY_CONST
#define YY_CONST
#endif

#ifndef YY_VOID
#define YY_VOID
#endif

#ifndef YY_VOIDP
#define YY_VOIDP		char *
#endif

#ifndef YY_VOID_ARGS
#define YY_VOID_ARGS
#endif

#ifndef YY_PROTO
#define YY_PROTO(p)		()
#endif

#endif /* ifdef __STDC__ */

#if YY_16_BIT
typedef unsigned short YYChar;
#else
typedef char YYChar;
#endif
typedef unsigned YYCharX;	/* For parameter passing. */

typedef int YYState;	 	/* Type for state not stored in table. */

typedef unsigned char YYUChar;
typedef YYUChar YYBool;		/* 0 == FALSE; 1 == TRUE. */
typedef unsigned YYBoolX;	/* For parameter passing. */

typedef unsigned long yy_size_t;

#ifndef BUFSIZ
#define BUFSIZ	1024
#endif

#define YY_BUF_SIZE	(8 * BUFSIZ)

#define YY_NULL 0

typedef YY_VOIDP YYDataHandle;	/* Opaque handle to scanner state. */
typedef YY_VOIDP YYBufHandle;	/* Opaque handle to scanner buffer. */

/* 			LIBRARY FUNCTIONS				*/

#if YY_16_BIT
#define yyCreateBuffer		yyCreateBuffer16
#define yyDeleteBuffer		yyDeleteBuffer16
#define yyFlushBuffer		yyFlushBuffer16
#define yySwitchToBuffer	yySwitchToBuffer16
#define yyMemBuffer		yyMemBuffer16
#define yyBytesBuffer		yyBytesBuffer16
#define yyStringBuffer		yyStringBuffer16
#define yyGet			yyGet16
#define yyLess			yyLess16
#define yyMore			yyMore16
#define yyUnput			yyUnput16
#define yyRestart		yyRestart16
#define yyPopState		yyPopState16
#define yyPushState		yyPushState16
#define yyTopState		yyTopState16
#endif /* if YY_16_BIT */

/* Buffer routines. */
YYBufHandle yyCreateBuffer YY_PROTO ((YYDataHandle d, FILE *f, yy_size_t s));
YY_VOID yyDeleteBuffer YY_PROTO ((YYDataHandle d, YYBufHandle b));
YY_VOID yyFlushBuffer YY_PROTO ((YYDataHandle d, YYBufHandle bP));
YY_VOID yySwitchToBuffer YY_PROTO ((YYDataHandle d, YYBufHandle b));

/* Memory Buffer routines. */
YYBufHandle yyMemBuffer YY_PROTO ((YYDataHandle d, YYChar *buf, yy_size_t len));
YYBufHandle yyBytesBuffer YY_PROTO ((YYDataHandle d, 
				     YY_CONST YYChar *bytes, int len));
YYBufHandle yyStringBuffer YY_PROTO ((YYDataHandle d, YY_CONST YYChar *str));

/* Action routines. */
int yyGet YY_PROTO ((YYDataHandle d));
YY_VOID yyLess YY_PROTO ((YYDataHandle d, int n));
YY_VOID yyMore YY_PROTO ((YYDataHandle d));
YY_VOID yyUnput YY_PROTO ((YYDataHandle d, YYCharX ch));

/* Restart routine. */
YY_VOID yyRestart YY_PROTO ((YYDataHandle d, FILE *fp));

/* Start-state stack routines. */
YY_VOID yyPopState YY_PROTO ((YYDataHandle d));
YY_VOID yyPushState YY_PROTO ((YYDataHandle d, YYState newState));
YYState yyTopState YY_PROTO ((YYDataHandle d));

#endif /* ifndef _LIBZLEX_H */
