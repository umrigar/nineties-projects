/* Automatically generated from parse.y by zyacc version 1.02. */
#ifndef _YY_DEFS_H
#define _YY_DEFS_H

typedef union {
#line 52 "parse.y"
 
  Index2 id;		/* ID index for macros and start-states. */
  UInt ch;		/* For single-chars in patterns. */
  Index2 actN;		/* Action number. */
  Pattern pat;
  Index2 lineN;		/* Line number in source file. */
  struct {
    ISet ss;		/* Set of start-states used in pattern. */
    Index2 lineN;	/* Line # at which start-states begin. */
  } ssLineN;
  ISet ss;
  VOIDP rawClass;
  Index class;
  UInt num;		/* Numbers for repetition pattern. */
  struct {
    Index2 lo, hi;	/* Inclusive boundaries of character range. */
  } charRange;
  struct {
    Index2 lo, hi;	/* Inclusive boundaries. */
  } repeatRange;	/* Range used for repetition pattern. */
  ConstString text;	/* Options text. */

} YYSTYPE;

extern YYSTYPE yylval;
#define ACT_TOK 257
#define CHAR_TOK 258
#define COLON_BEGIN_TOK 259
#define COLON_END_TOK 260
#define EOF_PAT_TOK 261
#define ID_TOK 262
#define LEX_DIR_TOK 263
#define MACRO_TOK 264
#define NEXT_ACT_TOK 265
#define NUM_TOK 266
#define OPTION_VAL_TOK 267
#define OPTION_TOK 268
#define SEC_TOK 269
#define SS_ID_TOK 270
#define STARTX_TOK 271
#define START_TOK 272
#define ARRAY_OPTION_TOK 273
#define POINTER_OPTION_TOK 274
#endif /* ifndef _YY_DEFS_H */
