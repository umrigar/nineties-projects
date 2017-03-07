/*

File:	 bstest.c
Purpose: Test program for bit-set routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


/*

Simple test program which allows the user to type in set expressions and
have them evaluated.

Set expressions involve following operators (in order of increasing 
precedence):

=		Assignment
==, <=,		Comparison (-1, 0, or 1 result), Subset
+, - 		Union or adding element to set, Relative set difference or
                removing element from set.
* 		Intersection
| set expr | 	Cardinality of set expr.
()		May be used to group set expressions.

Sets can be represented as usual as a comma-separated list of small
non-negative integers enclosed in braces, or as a range lo..hi.

In addition, we have 10 variables $0, ..., $9 which can store any value
(Boolean, Int or Set) and can be used within expressions.  $ by itself is
equivalent to $0.

We use the following  grammar:

Input		: Exp EOI
		;
Exp 		: LValue '=' Exp
		| EqExp
		;
LValue		: RelExp		// But only vars. allowed.
		;
RelExp		: UnionExp '==' UnionExp
		| UnionExp '<=' UnionExp
		| UnionExp
		;
UnionExp	: UnionExp '+' IntExp
		| UnionExp '-' IntExp
		| IntExp
		;
IntExp		: IntExp '*' PrimExp
		| PrimExp
		;
PrimExp		: VAR
		| '{' NList '}'
		| '{' '}'
		| '(' Exp ')'
		| '|' Exp '|'
		| Num '..' Num
		;
NList		: NUM ',' NList
		| NUM
		;
*/

#include "bset.h"

#include <setjmp.h>

typedef enum {
  LBRACE, RBRACE, UNION, DIFF, INTERSECTION, NUM, VAR, COMMA, CARD, EQ, 
  ASSIGN, EOI, ERR, LPAREN, RPAREN, SUBSET, RANGE
} TokType;

typedef struct {
  TokType type;
  UInt2 val;
} Tok;

typedef enum {
  SET_VAL, NUM_VAL, VAR_NUM, BOOLEAN_VAL
} ExpType;

typedef struct {
  ExpType type;
  Int2 val;
} Expr;

static Tok tok;

static jmp_buf env;

static Char *linePtr;

Expr vars[10];

static Boolean getLine PROTO((VOID_ARGS));
static Tok nextTok PROTO((VOID_ARGS));
static VOID err PROTO((ConstString s));
static Expr input PROTO((VOID_ARGS));
static Expr exp PROTO((VOID_ARGS));
static Expr relExp PROTO((VOID_ARGS));
static Expr unionExp PROTO((VOID_ARGS));
static Expr intExp PROTO((VOID_ARGS));
static Expr primExp PROTO((VOID_ARGS));
static Expr nList PROTO((VOID_ARGS));
static Expr rVal PROTO((Expr e));
static VOID printExp PROTO((Expr e));


/* Returns FALSE on EOF. */
static Boolean 
getLine()
{
  enum {LINE_SIZE= 120};
  static Char line[LINE_SIZE];
  linePtr= line;
  return (fgets(line, LINE_SIZE, stdin) != NULL);
}
    

static Tok 
nextTok()
{
  Tok t;
  while (*linePtr == ' ' || *linePtr == '\t') linePtr++;
  switch (*linePtr) {
    case '{': 
      t.type= LBRACE; linePtr++; break;
    case '}': 
      t.type= RBRACE; linePtr++; break;
    case '(': 
      t.type= LPAREN; linePtr++; break;
    case ')': 
      t.type= RPAREN; linePtr++; break;
    case '+': 
      t.type= UNION; linePtr++; break;
    case '-': 
      t.type= DIFF; linePtr++; break;
    case '*': 
      t.type= INTERSECTION; linePtr++; break;
    case ',': 
      t.type= COMMA; linePtr++; break;
    case '|': 
      t.type= CARD; linePtr++; break;
    case '\n': 
      t.type= EOI; linePtr++; break;
    case '=': 
      linePtr++; 
      if (*linePtr == '=') {
	t.type= EQ; linePtr++;
      }
      else
	t.type= ASSIGN;
      break;
    case '<':
      linePtr++; 
      if (*linePtr == '=') {
	t.type= SUBSET; linePtr++;
      }
      else
	t.type= ERR;
      break;
    case '.':
      linePtr++; 
      if (*linePtr == '.') {
	t.type= RANGE; linePtr++;
      }
      else
	t.type= ERR;
      break;
    case '$':
      t.type= VAR;
      t.val= (isdigit(*++linePtr)) ? *linePtr++ - '0' : 0;
      break;
    case '0': case '1': case '2': case '3': case '4': case '5': 
    case '6': case '7': case '8': case '9':
      t.type= NUM; t.val= 0;
      while (isdigit(*linePtr)) t.val= t.val * 10 + *linePtr++ - '0';
      break;
    default:
      t.type= ERR; break;
  }
  return t;
}

static VOID 
err(s)
  ConstString s;
{
  fprintf(stderr, "Error: %s. Flushing input.\n", s);
  longjmp(env, 1);
}

static Expr 
rVal(e)
  Expr e;
{
  return (e.type == VAR_NUM) ? vars[e.val] : e;
}

static Expr 
input()
{
  Expr e;
  e= exp();
  if (tok.type != EOI) err("Newline expected");
  return e;  
}

static Expr 
exp()
{
  Expr e1;
  e1= relExp();
  if (tok.type == ASSIGN) {
    Expr e2;
    if (e1.type != VAR_NUM) err("$ var expected before '='");
    tok= nextTok(); e2= exp();
    vars[e1.val]= rVal(e2);
  }
  return e1;
}

static Expr 
relExp()
{
  Expr e1;
  e1= unionExp();
  if (tok.type == EQ || tok.type == SUBSET) {
    CONST TokType t= tok.type;
    Expr e2;
    Expr z;
    tok= nextTok(); e2= unionExp();
    e1= rVal(e1); e2= rVal(e2);
    if (e1.type != SET_VAL || e2.type != SET_VAL) err("'==' only for sets");
    if (t == EQ) {
      z.type= NUM_VAL, z.val= compareBSet(e1.val, e2.val);
    }
    else {
      z.type= BOOLEAN_VAL; z.val= isSubsetBSet(e1.val, e2.val);
    }
    return z;
  }
  else
    return e1;
}

static Expr 
unionExp()
{
  Expr e;
  e= intExp();
  while (1) {
    if (tok.type == UNION) {
      Expr e2;
      tok= nextTok(); e2= intExp();
      e= rVal(e); e2= rVal(e2);
      if (e.type != SET_VAL) err("Left operator of + must be a set.");
      if (e2.type == SET_VAL)
	e.val= unionBSet(e.val, e2.val);
      else if (e2.type == NUM_VAL)
	e.val= addBSet(e.val, e2.val);
      else
	err("Right operator of + must be set or non-negative integer.");
    }
    else if (tok.type == DIFF) {
      Expr e2;
      tok= nextTok(); e2= intExp();
      e= rVal(e); e2= rVal(e2);
      if (e.type != SET_VAL) err("Left operator of - must be a set.");
      if (e2.type == SET_VAL)
	e.val= diffBSet(e.val, e2.val);
      else if (e2.type == NUM_VAL)
	e.val= rmBSet(e.val, e2.val);
      else
	err("Right operator of - must be set or non-negative integer.");
    }
    else break;
  }
  return e;
}

static Expr 
intExp()
{
  Expr e;
  e= primExp();
  while (tok.type == INTERSECTION) {
    Expr e2;
    tok= nextTok(); e2= primExp();
    e= rVal(e); e2= rVal(e2);
    if (e.type != SET_VAL || e2.type != SET_VAL) err("'*' only for sets.");
    e.val= intersectBSet(e.val, e2.val);
  }
  return e;  
}

static Expr 
primExp()
{
  Expr z;
  if (tok.type == VAR) {
    z.type= VAR_NUM; z.val= tok.val; tok= nextTok(); 
  }
  else if (tok.type == LPAREN) {
    tok= nextTok(); z= exp();
    if (tok.type != RPAREN) err("')' expected.");
    tok= nextTok();
  }
  else if (tok.type == LBRACE) {
    tok= nextTok(); z.type= SET_VAL;
    if (tok.type == RBRACE) {
      tok= nextTok(); z.val= newBSet();
    }
    else {
      z= nList(); 
      if (tok.type == RBRACE) 
	tok= nextTok();
      else 
	err("'}' expected");
    }
  }
  else if (tok.type == CARD) {
    Expr e;
    tok= nextTok(); e= rVal(exp());
    if (e.type != SET_VAL) err("Set expression expected");
    z.type= NUM_VAL; z.val= sizeBSet(e.val);
    if (tok.type != CARD) err("'|' expected");
    tok= nextTok();
  }
  else if (tok.type == NUM) {
    BSetElement lo= tok.val;
    tok= nextTok();
    if (tok.type != RANGE) { 
      z.type= NUM_VAL; z.val= lo; 
    }
    else {
      tok= nextTok();
      if (tok.type != NUM) 
	err("Number expected.");
      else {
	BSetElement hi= tok.val;
	tok= nextTok();
	z.type= SET_VAL; z.val= rangeBSet(newBSet(), lo, hi);
      }
    }
  }
  else 
    err("Invalid input");
  return z;
}

static Expr 
nList()
{
  Expr z;
  BSet s= newBSet();
  if (tok.type != NUM) 
    err("Number expected.");
  else {
    s= addBSet(s, tok.val);
    tok= nextTok();
    while (tok.type == COMMA) {
      tok= nextTok();
      if (tok.type != NUM) 
        err("Number expected.");
      else {
        addBSet(s, tok.val);
	tok= nextTok();
      }
    }
  }
  z.type= SET_VAL; z.val= s;
  return z;
}

static VOID 
printExp(e)
  Expr e;
{
  e= rVal(e);
  switch (e.type) {
    case SET_VAL:
      fprintBSet(stdout, e.val); break;
    case NUM_VAL:
      printf("%d", e.val); break;
    case BOOLEAN_VAL:
      printf("%s", (e.val) ? "TRUE" : "FALSE"); break;
    default:
      fprintf(stderr, "%s:%d: Internal error\n", __FILE__, __LINE__); 
      break;
  }
  printf("\n");
}

int main PROTO((VOID_ARGS));

int main() {
  initBSet();
  while (1) { /* Terminate with CNTRL-C. */
    printf("> "); fflush(stdout);
    if (!getLine()) break;
    tok= nextTok();
    if (setjmp(env) == 0) {
      printExp(input());
    }
  }
  return 0;
}
