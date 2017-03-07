/*

File:	 parse.y
Purpose: Grammar for parser input.

Last Update Time-stamp: "97/07/20 17:09:06 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/*

Parser for grammar with semantic typing.


PARSE ERRORS

yyparse returns with a nonzero value on various errors like EOF within
a structured construct.  Those errors represent such a screwed-up
source anyway, that further analysis is probably not warranted.

Attempts to make the MARK_TOK between the defs and rules optional (a
possibly common error) results in conflicts: with the current grammar
2 RR-conflicts which would do the right thing and a shift-reduce
conflict which does the wrong thing.  Hence I am relying on the
scanner to deliver a MARK_TOK if it sees a ':' or '|' (which cannot
occur within a legal section 1).

A parse error within a rule causes the entire rule (including all
further alternatives) to be discarded.  Attempts to remedy this by
adding error rules run into conflicts caused by the optional semicolon
rule terminator.

*/

%{

#include "attribs.h"
#include "gram.h"
#include "lalr.h"
#include "options.h"
#include "scan.h"
#include "zparse.h"

#include "error.h"

%}

%union {
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
};

%token	<lineN>		'('
%token			')' 
%token	<lineN>		',' 
%token			':' 
%token			';' 
%token	<act>		'{' 
%token			'|' 
%token			'}' 
%token			EXPECT_TOK	/* "%expect". */
%token	<id>		ID_TOK 		/* A YACC identifier. */
%token	<id>		ID_VAR_TOK	/* $Identifier. */
%token			IN_TOK 		/* "%in". */
%token			LEFT_TOK 	/* "%left". */
%token	%nowarn		LEX_ERR_TOK	/* A lexical error. */
%token	<numVar>	LHS_VAR_TOK 	/* $$ or $<Type>$ */
%token	<lit>		LIT_TOK		/* A literal in single quotes. */
%token			MARK_TOK	/* "%%". */
%token			LOOK_TOK	/* "%look" */
%token			NONASSOC_TOK 	/* "%nonassoc" */
%token			NOWARN_TOK 	/* "%nowarn" */
%token	<num>		NUM_TOK 	/* A decimal number. */
%token	<numVar>	NUM_VAR_TOK	/* $Number or $<Type>Number. */
%token			OPTION_TOK	/* "%option" */
%token	<text>		OPTION_VAL_TOK	/* "%option" */
%token			OUT_TOK 	/* "%out". */
%token			PREC_TOK 	/* "%prec". */
%token			PURE_TOK	/* "%pure". */
%token			RIGHT_TOK 	/* "%right". */
%token	<shortText>	SHORT_TEXT_TOK	/* A string which fits in YYSTYPE. */
%token			START_TOK 	/* "%start". */
%token			TEST_TOK 	/* "%test". */
%token	<text>		TEXT_TOK 	/* A string which has been interned. */
%token			TOKEN_TOK 	/* "%token". */
%token			TYPE_TOK	/* "%type". */
%token  <type>		TYPE_ID_TOK	/* <type>. */
%token			UNION_TOK	/* "%union". */
%token	<id>		XID_TOK 	/* Rule LHS identifier. */

%type	<act>		act
%type	<count>		altElements
%type	<lit>		idOrLit
%type	<num>		optTokVal
%type	<num>		lookDepth
%type 	<count>		attrDecs attrDecList

%{

int yyerror PROTO((char *s));

#define yylex scan

static struct {
  Index2 assocPrec;	/* The assoc/prec. of the current declaration. */
  Index2 precLevel;	/* Precedence level counter. */
  Index2 prec;		/* The precedence associated with the declaration. */
  Index2 type;		/* The type associated with current declaration. */
  Count2 decCount;	/* Count of # of declarations. */
  Index2 lineN;		/* Line # for attrib. exprs. */
  Boolean isPure;	/* TRUE if %pure parser specified. */
  Boolean nowarn;	/* TRUE if %nowarn for current token declaration. */
} globs;

#define G globs

%}

%%

spec		
  : options defs MARK_TOK rules
  ; 
options
  : /* EMPTY */
  | options OPTION_TOK optionVals optSemi
  ;
optionVals
  : OPTION_VAL_TOK			{ setOptions($1); }
  | optionVals OPTION_VAL_TOK		{ setOptions($2); }
  | optionVals ',' OPTION_VAL_TOK	{ setOptions($3); }
  | error
  ;
defs
  : /* empty */
  | defs def
  ;
def
  : START_TOK startIDList optSemi
  | PURE_TOK optSemi			{ G.isPure= TRUE; }
  | EXPECT_TOK NUM_TOK optSemi 		{ setExpect($2); }
  | TYPE_TOK type nonTermList 
  | termDecDir termType termList optSemi
  | UNION_TOK optSemi 			{ dclTyped(); }
  | error optSemi
  ;
startIDList
  : ID_TOK
      { addStartNonTerm($1); }
  | startIDList ID_TOK
      { addStartNonTerm($2); }
  |  startIDList ',' ID_TOK
      { addStartNonTerm($3); }
  ;
termDecDir	
  : TOKEN_TOK optNowarn
      { G.assocPrec= NIL;  }
  | LEFT_TOK optNowarn	
      { G.assocPrec= MAKE_ASSOC_PREC(LEFT_ASSOC, G.precLevel++); }
  | RIGHT_TOK optNowarn	
      { G.assocPrec= MAKE_ASSOC_PREC(RIGHT_ASSOC, G.precLevel++); }
  | NONASSOC_TOK optNowarn	
      { G.assocPrec= MAKE_ASSOC_PREC(NON_ASSOC, G.precLevel++); }
  ;
optNowarn
  : NOWARN_TOK	
      { G.nowarn= TRUE; }
  | /* empty */
      { G.nowarn= FALSE; }
  ;
termType
  : type attrDecs 
      { G.decCount= $2; }
  | /* empty */
      { G.type= NIL; G.decCount= 0; }
nonTermList	
  : nonTermDec		
  | nonTermList nonTermDec 
  | nonTermList ',' nonTermDec
  ;
nonTermDec	
  : ID_TOK
      { dclNonTermAttribs($1, G.type); }
  ;
termList	
  : termDec
  | termList termDec
  | termList ',' termDec
  ;
termDec		
  : ID_TOK optTokVal
      { dclTermAttribs($1, NIL, NIL, $2, G.assocPrec, 
		       G.type, G.decCount, G.nowarn); 
      }
  | LIT_TOK optTokVal
      { dclTermAttribs(NIL, $1.id, $1.val, $2, G.assocPrec, 
		       G.type, G.decCount, G.nowarn); 
      }
  | ID_TOK '=' LIT_TOK optTokVal
      { dclTermAttribs($1, $3.id, $3.val, $4, G.assocPrec, 
		       G.type, G.decCount, G.nowarn); 
      }
  | LIT_TOK '=' ID_TOK optTokVal
      { dclTermAttribs($3, $1.id, $1.val, $4, G.assocPrec, 
		       G.type, G.decCount, G.nowarn); 
      }
  ;	
attrDecs	
  : '(' { G.lineN= $1; } attrDecList ')'
      { $$= $3; }
  | /* empty */
      { $$= 0; }
  ;
attrDecList	
  : attrDec			{ addAttribSeq(G.lineN); $$= 1; }
  | attrDecList ',' attrDec 	{ addAttribSeq($2); $$= $1 + 1; }
  ;
attrDec
  : attrType attrPrefix idVar attrSuffix
  | error 			{ setAttribParseError(); }
  ;
attrPrefix	
  : attrPrefix attrTextTok
  | attrTextTok
  ;
attrSuffix
  : attrSuffix attrTextTok
  | /* empty */
  ; 
rules
  : rules rule
  | rule
  ;
rule
  : ruleLHS alts optSemi
  | error optSemi
  ;
ruleLHS
  : XID_TOK 
    attrDecs ':' 
      { dclNonTermAttribs($1, NIL); }
  ;
alts
  : altBody
  | alts '|' altBody
  ;
altBody
  :   { beginRuleAttribs(); }
    altElements
      { endRuleAttribs($2); }
  ;
altElements
  : altElements rhsElement
      { $$= $1 + 1; }
  | /* empty */ 
      { $$= 0; }
  ;
rhsElement
  : ID_TOK attrVals
      { addRHSAttribs($1, NIL); }
  | LIT_TOK attrVals
      { addRHSAttribs($1.id, $1.val); }
  | PREC_TOK idOrLit
      { setRulePrec($2.id); }
  | LOOK_TOK '(' lookDepth ')'
      { if ($3 != 0 && $3 != 1) warn("%%look argument should be 0 or 1");
        setRuleLook($3 != 0); 
      }
  | act
      { addActAttribs($1.actN, $1.lineNum); }
  | TEST_TOK '(' testExp ')'
      { addTestAttribs(); }
  ;
lookDepth
  : TEXT_TOK
      { int d; sscanf($1, "%d", &d); $$= d; }
  | SHORT_TEXT_TOK
      { int d; sscanf($1, "%d", &d); $$= d; }
  ;
testExp
  : attrExp			{ addAttribSeq(G.lineN); }
  ;
attrVals
  : '(' { G.lineN= $1; } attrExpList ')'
  | /* empty */
  ;
attrExpList
  : attrExp			{ addAttribSeq(G.lineN); }
  | attrExpList ',' attrExp 	{ addAttribSeq($2); }
  ;	
attrExp
  : attrExpTok
  | attrExp attrExpTok
  | error 			{ setAttribParseError(); }
  ;
attrExpTok
  : idVar
  | numVar
  | attrTextTok
  ;
act
  : '{' actVars '}'
  ;
actVars
  : /* empty */
  | actVars idVar
  | actVars NUM_VAR_TOK
  | actVars LHS_VAR_TOK
  ;
optTokVal
  : NUM_TOK
  | /* empty */
      { $$= NIL; }
  ;
type
  : TYPE_ID_TOK
      { G.type= $1; }
  ;
optSemi
  : ';'
  | /* empty */
  ;
attrType
  : IN_TOK		{ addAttribTok(IN_ATOK, NULL);  }
  | OUT_TOK		{ addAttribTok(OUT_ATOK, NULL);  }
  | /* empty */		{ addAttribTok(OUT_ATOK, NULL); }
  ;	
attrTextTok
  : TEXT_TOK		{ addAttribTok(TEXT_ATOK, (VOIDP)$1); }
  | SHORT_TEXT_TOK	{ addAttribTok(SHORT_TEXT_ATOK, (VOIDP)$1); }
  ;
numVar
  : NUM_VAR_TOK		{ addAttribTok(NUM_VAR_ATOK, (VOIDP)&$1); }
  | LHS_VAR_TOK		{ addAttribTok(NUM_VAR_ATOK, (VOIDP)&$1); }
  ;
idVar
  : ID_VAR_TOK 		{ addAttribTok(ID_VAR_ATOK, (VOIDP)&$1); }
idOrLit	
  : ID_TOK		{ $$.id= $1; $$.val= NIL; }
  | LIT_TOK
  ;

%%

int 
yyerror(s)
  char *s;
{
  error("%s at `%s'", s, yytext);  
  return 0;
}

Boolean 
hasLookahead()
{
  return yychar != YYEMPTY;
}

Count 
outParseParams(outFile)
  FILE *outFile;
{
  Count lineCount= 0;
  fprintf(outFile, "#define YY_IS_PURE %d\n", G.isPure); lineCount++;
  return lineCount;
}

#ifdef TEST_GRAM

VOID 
printAssocPrec(assocPrec1)
  Index assocPrec1;
{
  Index2 assocPrec= (Index2) assocPrec1;
  if (assocPrec != NIL) {
    printf(" ");
    switch (ASSOC(assocPrec)) {
      case NON_ASSOC:
        printf("NON_ASSOC");
	break;
      case LEFT_ASSOC:
        printf("LEFT");
	break;
      case RIGHT_ASSOC:
        printf("RIGHT");
	break;
      default:
	INTERNAL_ERROR();
    }
    printf(" %d", PREC(assocPrec));
  }
  VOID_RET();
}


#endif
