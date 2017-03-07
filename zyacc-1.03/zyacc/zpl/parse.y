/*			     PROLOG PARSER.				*/
/*

Requires zyacc.  Because of inherited attributes and semantic predicates.

Handles dynamic operators.  However does not handle the same operator
declared both infix and postfix.

*/
%{
#include "plops.h"
#include "scan.h"

#include <stdio.h>

/* REDUCE_TEST(n) fails iff the lookahead is an infix or suffix operator
 * with precedence < n.
 */
#define REDUCE_TEST(n)							\
  (( yychar != NAME_TOK && yychar != ','  && 				\
     yychar != NUM_MINUS_TOK ) ||					\
   ( isInfixOrSuffixOp(							\
       (yychar == NAME_TOK)						\
       ? yylval.name.id 						\
       : ( (yychar == ',') ? commaID : minusID ),			\
       (n))								\
   )									\
  )

/* OP(id, type, max): id is a type-operator with precedence <= max.  The
 * precedence is stored in the global variable level.
 */
#define OP(id, type, max)						\
  (((level= opLevel(id, type)) > 0) && (level <= max))

#define PREFIX_OP(id)							\
   (opLevel((id), FX_OP) > 0 || opLevel((id), FY_OP) > 0)

/* Printing macros to print postfix representation of Prolog program. */
#define P(f, t)	printf(f, t)				/* printf() */
#define PS(s)	printf("%s", s)				/* Print string. */
#define PN(id)	printf("%s ", getNameString(id))	/* Print name. */
#define PF(f,n)	printf("%s/%d ", getNameString(f), n)	/* Print functor. */

static Index level; 	/* Variable used within OP-macro in %tests. */
static Index minusID;	/* Name for '-' symbol. */
static Index commaID;	/* Name for ',' symbol. */

%}

%token	<name>(Index2 $id) 	NAME_TOK VAR_TOK;
%token 	<natNum>(long $num) 	NAT_NUM_TOK STR_CHAR_TOK;
%token	<realNum>(double $num) 	REAL_NUM_TOK;
%token				WS_LPAREN_TOK;
%token 				FULL_STOP_TOK
%token 	%nowarn			LEX_ERR_TOK
%token				NUM_MINUS_TOK	/* '-' if followed by digit. */

/*

Term(%in int $max, %out int $n): Parses a maximal prefix of the input which 
is a term of level $n where $n <= $m.  Hence we must quit gobbling the
input if any of the following conditions is true:

a.  We see a non-operator at a point where a suffix or infix operator is
    expected.

b.  We see a suffix or infix operator which cannot be parsed into the 
    current term.

Note that ',' is an operator.

*/

%%
PrologText
  : PrologText ReadTerm 
  | /* empty */
  ;
ReadTerm
  : Term(1200, $n) FULL_STOP_TOK 	{ PS("\n"); }
  | error FULL_STOP_TOK	   		{ PS("\n"); }	
  ;
Term(%in int $max, int $n)
  : Term0 				{ $n= 0; }
  | PrefixOpAtom($max, $n)
  | PrefixTerm($max, $n)
  | InfixTerm($max, $n)
  | SuffixTerm($max, $n)
  ;
Term0
  : NAME_TOK($id) %test(!PREFIX_OP($id)){ PN($id); }		/* atom. */
  | NAT_NUM_TOK($n)			{ P("%ld ", $n); }	/* integer. */
  | REAL_NUM_TOK($v)			{ P("%g ", $v); }	/* float. */
  | NUM_MINUS_TOK NAT_NUM_TOK($n)	{ P("-%ld ", $n); }	/* integer. */
  | NUM_MINUS_TOK REAL_NUM_TOK($v)	{ P("-%g ", $v); }	/* float. */
  | VAR_TOK($id)			{ PN($id); }		/* Variable. */
  | '{' Term(1200, $n) '}'  		{ PS("{} "); }		/* {}(Term). */
  | NAME_TOK($id) '(' Arguments($n) ')' { PF($id, $n); }	/* Structure. */
  | LParen Term(1200, $n) ')'
  | List
  | String
  ;
Arguments(int $nArgs)
  : Term999($m)				{ $nArgs= 1; }
  | Term999($m) ',' Arguments($nArgs1)	{ $nArgs= $nArgs1 + 1; }	
  ;
List
  : '[' ListExpr ']'
  ;
ListExpr
  : Term999($m)				{ PS("[] ./2 "); }
  | Term999($m) ',' ListExpr		{ PS("./2 "); }
  | Term999($m1) '|' Term999($m2)	{ PS("./2 "); }
  ;
Term999(int $m)		/* Introduced to avoid attribute-conflicts. */
  : Term(999, $m)
  ;
String
  : StrChar String			{ PS("./2 "); }
  | StrChar				{ PS("[] ./2 "); }
  ;
StrChar
  : STR_CHAR_TOK($n)			{ P("%ld ", $n); }
  ;
PrefixOpAtom(%in int $max, int $n)
  : FXOp($max, $id, $n) %test(REDUCE_TEST($n))	
					{ PN($id); }
  | FYOp($max, $id, $n) %test(REDUCE_TEST($n+1))	
					{ PN($id); }
  ;
PrefixTerm(%in int $max, int $n)
  : FXOp($max, $id, $n) Term($n - 1, $m) %test(REDUCE_TEST($n))
                                        { PF($id, 1); }
  | FYOp($max, $id, $n) Term($n, $m) %test(REDUCE_TEST($n + 1))
                                        { PF($id, 1); }
  ;
InfixTerm(%in int $max, int $n)
  : Term($max, $m1) XFXOp($max, $id, $n) Term($n - 1, $m2) 
    %test($m1 < $n && REDUCE_TEST($n))
                                        { PF($id, 2); }
  | Term($max, $m1) XFYOp($max, $id, $n) Term($n, $m2)
    %test($m1 < $n && REDUCE_TEST($n + 1))
                                        { PF($id, 2); }
  | Term($max, $m1) YFXOp($max, $id, $n) Term($n - 1, $m2)
    %test($m1 <= $n && REDUCE_TEST($n))
                                        { PF($id, 2); }
  | Term($max, $m1) %test($max >= 1000)
    ',' Term(1000, $m2) %test($m1 < 1000 && REDUCE_TEST(1001))
                                        { $n= 1000; PS(",/2 "); }
  ;
SuffixTerm(%in int $max, int $n)
  : Term($max, $m) XFOp($max, $id, $n) 
    %test($m < $n && REDUCE_TEST($n))
     					{ PF($id, 1); }
  | Term($max, $m) YFOp($max, $id, $n) 
    %test($m <= $n && REDUCE_TEST($n))
     					{ PF($id, 1); }
  ;
FXOp(%in int $max, Index $id, int $n)
  : NAME_TOK($id) %test(OP($id, FX_OP, $max))
					{ $n= level; }
  ;
FYOp(%in int $max, Index $id, int $n)
  : NAME_TOK($id) %test(OP($id, FY_OP, $max))
					{ $n= level; }
  ;
XFXOp(%in int $max, Index $id, int $n)
  : NAME_TOK($id) %test(OP($id, XFX_OP, $max))
					{ $n= level; }
  | NUM_MINUS_TOK %test(OP(minusID, XFX_OP, $max))
                                        { $id= minusID; $n= level; }
  ;
XFYOp(%in int $max, Index $id, int $n)
  : NAME_TOK($id) %test(OP($id, XFY_OP, $max))
					{ $n= level; }
  | NUM_MINUS_TOK %test(OP(minusID, XFY_OP, $max))
                                        { $id= minusID; $n= level; }
  ;
YFXOp(%in int $max, Index $id, int $n)
  : NAME_TOK($id) %test(OP($id, YFX_OP, $max))
					{ $n= level; }
  | NUM_MINUS_TOK %test(OP(minusID, YFX_OP, $max))
                                        { $id= minusID; $n= level; }
  ;
XFOp(%in int $max, Index $id, int $n)
  : NAME_TOK($id) %test(OP($id, XF_OP, $max))
					{ $n= level; }
  ;
YFOp(%in int $max, Index $id, int $n)
  : NAME_TOK($id) %test(OP($id, YF_OP, $max))
					{ $n= level; }
  ;
LParen
  : WS_LPAREN_TOK
  | '('
  ;
%%

int main(int argc, CONST char *argv[]) {
  initScan();
  initOps();
  minusID= getName("-", 1);	/* Set name for "-" symbol. */
  commaID= getName(",", 1);	/* Set name for "," symbol. */
#if YYDEBUG
  if (argc > 1) { sscanf(argv[1], "%d", &YYDEBUG_VAR); }
#endif
  /* Add some suffix operators for testing. */
  addOp(getName("^^", 2), XF_OP, 400);
  addOp(getName("??", 2), YF_OP, 300);
  return yyparse();
}
