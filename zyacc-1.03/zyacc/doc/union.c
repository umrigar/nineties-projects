typedef union {
  struct { double v; } val;      /* NUM_TOK */
  struct { const char *id; } id; /* ID_TOK */
} YYSTYPE;
