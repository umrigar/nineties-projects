/* Automatically generated from parse.y by zyacc version 1.03. */
#ifndef _YY_DEFS_H
#define _YY_DEFS_H

typedef union {
  struct {
    Index2 id;
  } name;
  struct {
    long num;
  } natNum;
  struct {
    double num;
  } realNum;
} YYSTYPE;

extern YYSTYPE yylval;
#define NAME_TOK 257
#define VAR_TOK 258
#define NAT_NUM_TOK 259
#define STR_CHAR_TOK 260
#define REAL_NUM_TOK 261
#define WS_LPAREN_TOK 262
#define FULL_STOP_TOK 263
#define LEX_ERR_TOK 264
#define NUM_MINUS_TOK 265
#endif /* ifndef _YY_DEFS_H */
