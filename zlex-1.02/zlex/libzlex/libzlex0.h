/*

File:	 libzlex0.h
Purpose: Private interface to zlex library.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _LIBZLEX0_H
#define _LIBZLEX0_H

/* Private declarations for zlex library. 
 * Assumes external and skeleton parser declarations previously included.
 */

#include <config.h>

#include <assert.h>
#ifdef HAVE_STDDEF_H
#include <stddef.h>	/* for NULL, size_t. */
#endif

#if STDC_HEADERS
#include <stdlib.h>
#include <string.h>
#endif

enum {
  STK_SIZE_INC= 256 
};

/* The following are local to the library and not for external consumption. */

#if YY_16_BIT
#define yyMakeBuf		yyMakeBuf16
#define yyMoveBytes		yyMoveBytes16
#define yyMoveText		yyMoveText16
#define yySaveText		yySaveText16
#define yyShiftBuffer		yyShiftBuffer16
#define yyUpdateSentinel	yyUpdateSentinel16
#endif /* if YY_16_BIT */

YYBufHandle yyMakeBuf YY_PROTO ((YYData *dP, FILE *yyin, 
				 YYChar *mem, yy_size_t size));
YYChar *yyMoveBytes YY_PROTO ((YYChar *YY_CONST dest, 
			       YY_CONST YYChar *YY_CONST src, 
			       yy_size_t len));
YYChar *yyMoveText YY_PROTO ((YYData *dP, YYChar *dest));
YY_VOID yySaveText YY_PROTO ((YYData *dP));
YYChar *yyShiftBuffer YY_PROTO ((YYData *dP, YY_CONST yy_size_t nFree));
YY_VOID yyUpdateSentinel YY_PROTO ((YYData *dP, YYBoolX inTok));

#ifndef VOID_RET
#if __STDC__
#define VOID_RET() return
#else
#define VOID_RET() return 0
#endif
#endif

#endif /* ifndef _LIBZLEX0_H */
