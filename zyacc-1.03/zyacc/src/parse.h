/* Automatically generated from parse.y by zyacc version 1.03. */
#ifndef _YY_DEFS_H
#define _YY_DEFS_H

typedef union {
#line 52 "parse.y"
 
  Index id;			/* Index of identifier. */
  struct {			/* Literal in single quotes. */
    Index2 id;			/* Index literal text (including quotes). */
    Index2 val;			/* The integer value of the literal. */
  } lit;		
  NumVar numVar;		/* A numeric $-variable of form $<type>NN. */
  ConstString text;		/* Pointer to saved yytext. */
  char shortText 		/* Avoid interning short strings if returned */
    [sizeof(ConstString)];	/* text fits here. */
  Int num;			/* A number. */
  Int lineN;			/* Line # in source file. */
  Index type;			/* Identifier for <type>. */
  ActSem act;
  Count count;			/* Count of number in lists. */
  Index2 assocPrec;		/* Associativity and precedence. */

} YYSTYPE;

extern YYSTYPE yylval;
#define EXPECT_TOK 257
#define ID_TOK 258
#define ID_VAR_TOK 259
#define IN_TOK 260
#define LEFT_TOK 261
#define LEX_ERR_TOK 262
#define LHS_VAR_TOK 263
#define LIT_TOK 264
#define MARK_TOK 265
#define LOOK_TOK 266
#define NONASSOC_TOK 267
#define NOWARN_TOK 268
#define NUM_TOK 269
#define NUM_VAR_TOK 270
#define OPTION_TOK 271
#define OPTION_VAL_TOK 272
#define OUT_TOK 273
#define PREC_TOK 274
#define PURE_TOK 275
#define RIGHT_TOK 276
#define SHORT_TEXT_TOK 277
#define START_TOK 278
#define TEST_TOK 279
#define TEXT_TOK 280
#define TOKEN_TOK 281
#define TYPE_TOK 282
#define TYPE_ID_TOK 283
#define UNION_TOK 284
#define XID_TOK 285
#endif /* ifndef _YY_DEFS_H */
