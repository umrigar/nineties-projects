/*

File:	 parse.y
Purpose: Parser for zlex source files.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/* 

We do not use yytext as the semantic value of any token which ensures
that the parser lookahead does not clobber yytext.

*/

%{

#include "chrclass.h"
#include "options.h"
#include "patterns.h"
#include "scan.h"
#include "zparse.h"

#include "area.h"
#include "error.h"
#include "iset.h"

#include <ctype.h>

static struct {
  Boolean isExclusive;		/* TRUE if current ss-decl is exclusive. */
} globals;
#define G globals

typedef enum {
  EOF_RE,			/* RE for <<EOF>> pattern. */
  INTRA_TOK_RE,			/* RE for intra-token pattern. */
  START_RE,			/* RE with `^' pattern. */
  SIMPLE_RE			/* Not one of the above. */
} REType;
  
VOID yyerror PROTO((Char *msg));

#define yylex scan

%}

%union {
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
}

%token	<lineN>		'<'
%token	<lineN>		'+'
%token	<actN>		ACT_TOK	
%token 	<ch>		CHAR_TOK
%token			COLON_BEGIN_TOK
%token			COLON_END_TOK
%token			EOF_PAT_TOK
%token	<id>		ID_TOK
%token			LEX_DIR_TOK
%token	<id>		MACRO_TOK
%token 	<actN>		NEXT_ACT_TOK
%token	<num>		NUM_TOK
%token	<text>		OPTION_VAL_TOK
%token			OPTION_TOK
%token			SEC_TOK
%token	<id>		SS_ID_TOK
%token			STARTX_TOK
%token			START_TOK
%token			ARRAY_OPTION_TOK
%token			POINTER_OPTION_TOK

%type	<pat>		baseRegExp
%type	<pat>		catRegExp
%type	<charRange>	classElement
%type	<rawClass>	classElements
%type	<id>		ctypeClass
%type	<pat>		postRegExp
%type	<repeatRange>	numRange
%type	<pat>		regExp
%type	<pat>		optRightContext
%type	<ssLineN>	optSSList
%type	<pat>		rightContext
%type	<ss>		ssUseList

%%

/*
INFORMAL TOKEN DESCRIPTIONS USING LEX SYNTAX:

ACT_TOK	
  : A sequence of actions including C-brace actions, decorated brace actions,
    indented actions, or a newline after a section 2 pattern
  ;
CHAR_TOK
  : A character which is not a lex meta-character within its current context
  ;
COLON_BEGIN_TOK
  : "[:"
  ;
COLON_END_TOK
  : ":]"
  ;
EOF_PAT_TOK
  : "<<EOF>>"
  ;
ID_TOK
  : An identifier
  ;
LEX_DIR_TOK
  : A lex directive at the start of a line in section 1
  ;
MACRO_TOK
  : A macro call within braces
  ;
NEXT_ACT_TOK
  : '|' action (which is not part of a pattern)
  ;
NUM_TOK
  : A number which occurs within braces as a repetition count
  ;
OPTION_VAL_TOK
  : An option word after a %option directive.
  ;
OPTION_TOK
  : ^"%option"
  ;
SEC_TOK
  : ^"%%"
  ;
SS_ID_TOK
  : An identifier which is used as a start-state name
  ;
STARTX_TOK
  : ^"%"[xX] signalling the start of an exclusive start state declaration
  ;
START_TOK
  : ^"%"[sS] signalling the start of an inclusive start state declaration
  ;
ARRAY_OPTION_TOK
  : ^"%array"
  ;
POINTER_OPTION_TOK
  : ^"%pointer"
  ;
 */

lexProgram
  : section1 SEC_TOK { beginSec2Patterns(); } section2 { endSec2Patterns(); }
  ;
section1
  : options restSection1
  | options
  ;
options	
  : /* EMPTY */
  | options optionLine
  ;
optionLine
  : OPTION_TOK OPTION_VAL_TOK 		{ setOptions($2); }
  | OPTION_TOK
  | ARRAY_OPTION_TOK  			{ setOptions("--array"); }
  | POINTER_OPTION_TOK  		{ setOptions("--array=0"); }
  ;
restSection1
  : section1Line 
  | restSection1 section1Line
  ;
section1Line
  : startDec 
  | def
  | lexDir
  | error
  ;
lexDir
  : LEX_DIR_TOK
  | LEX_DIR_TOK OPTION_VAL_TOK
  ;
startDec
  : START_TOK { G.isExclusive= FALSE; } ssDefList
  | STARTX_TOK { G.isExclusive= TRUE; } ssDefList
  ;
ssDefList
  : /* EMPTY */ 
  | ssDefList SS_ID_TOK { defStartState($2, G.isExclusive, lineN()); }
  ;
def		
  : ID_TOK { /*$<lineN>$= lineN();*/ } regExp { defMacro($1, $3, lineN()); }
  | ID_TOK { defMacro($1, NIL, lineN()); }
  ;
section2
  : ACT_TOK sec2Patterns
  ;
sec2Patterns
  : /* EMPTY */
  | sec2Patterns actPatterns
  ;
actPatterns
  : lastActPattern
  | intermediateActPattern actPatterns
  | '+' regExp ACT_TOK { addIntraTokPat($1, $2, $3); }
  | error
  ;
intermediateActPattern
  : optSSList regExp optRightContext NEXT_ACT_TOK
    { addRegularPat(FALSE, RELEASE_ISET($1.ss), $1.lineN, $2, $3, $4); }
  | optSSList '^' regExp optRightContext NEXT_ACT_TOK 
    { addRegularPat(TRUE, RELEASE_ISET($1.ss), $1.lineN, $3, $4, $5); }
  | optSSList rightContext NEXT_ACT_TOK
    { addRegularPat(FALSE, RELEASE_ISET($1.ss), $1.lineN, NIL, $2, $3); }
  | optSSList '^' rightContext NEXT_ACT_TOK 
    { addRegularPat(TRUE, RELEASE_ISET($1.ss), $1.lineN, NIL, $3, $4); }
  | optSSList EOF_PAT_TOK NEXT_ACT_TOK 
    { addEOFPat(RELEASE_ISET($1.ss), $1.lineN, $3); }
  ;
lastActPattern
  : optSSList regExp optRightContext ACT_TOK
    { addRegularPat(FALSE, RELEASE_ISET($1.ss), $1.lineN, $2, $3, $4); }
  | optSSList '^' regExp optRightContext ACT_TOK
    { addRegularPat(TRUE, RELEASE_ISET($1.ss), $1.lineN, $3, $4, $5); }
  | optSSList rightContext ACT_TOK
    { addRegularPat(FALSE, RELEASE_ISET($1.ss), $1.lineN, NIL, $2, $3); }
  | optSSList '^' rightContext ACT_TOK 
    { addRegularPat(TRUE, RELEASE_ISET($1.ss), $1.lineN, NIL, $3, $4); }
  | optSSList EOF_PAT_TOK ACT_TOK
    { addEOFPat(RELEASE_ISET($1.ss), $1.lineN, $3); }
  ;
optRightContext
  : /* EMPTY */ 	{ $$= NIL; }
  | rightContext
  ;
rightContext
  : '$'			{ $$= classPat(rawClassToClass(
				addRawClass(newRawClass(), '\n', '\n'))); 
                        }
  | '/' regExp		{ $$= $2; }
  ;
optSSList
  : /* EMPTY */		{ $$.ss= BAD_ISET; $$.lineN= lineN(); }
  | '<' ssUseList '>'	{ $$.ss= $2; $$.lineN= $1; }
  ;
ssUseList
  : SS_ID_TOK			{  $$= unaryISet(getStartState($1)); }
  | ssUseList ',' SS_ID_TOK	{  $$= addISet($1, getStartState($3)); }	
  ;
regExp
  : regExp '|' catRegExp	{ $$= altPat($1, $3); }
  | catRegExp
  ;
catRegExp
  : catRegExp postRegExp	{ $$= catPat($1, $2); }
  | postRegExp
  ;
postRegExp
  : postRegExp '*'		{ $$= repeatPat($1, 0, NIL); }
  | postRegExp '?'		{ $$= repeatPat($1, 0, 1); }
  | postRegExp '+'		{ $$= repeatPat($1, 1, NIL); }
  | postRegExp numRange		{ $$= repeatPat($1, $2.lo, $2.hi); }
  | baseRegExp
  ;
baseRegExp
  : '(' regExp ')'		{ $$= $2; }
  | '.' 			{ $$= classPat(dotClass()); }
  | CHAR_TOK			{ $$= classPat(rawClassToClass(
                                        addRawClass(newRawClass(), $1, $1))); 
                                }
  | MACRO_TOK			{ $$= macroPat($1); }
  | '[' classElements ']'	{ $$= classPat(rawClassToClass($2)); }
  | '[' '^' classElements ']' 	{ $$= classPat(negClass(rawClassToClass($3))); }
  ;
classElements
  : classElement		{ $$= addRawClass(newRawClass(), 
						  $1.lo, $1.hi); 
                                }
  | ctypeClass 			{ $$= ctypeAddToRawClass(newRawClass(), $1); }
  | classElements classElement	{ $$= addRawClass($1, $2.lo, $2.hi); }
  | classElements ctypeClass    { $$= ctypeAddToRawClass($1, $2); }
  ;
classElement
  : CHAR_TOK			{ $$.lo= $$.hi= $1; }
  | CHAR_TOK '-' CHAR_TOK	{ $$.lo= $1; $$.hi= $3; }
  ;
ctypeClass
  : COLON_BEGIN_TOK ID_TOK COLON_END_TOK
                                { $$= $2; }
  ;
numRange	
  : '{' NUM_TOK '}'		{ $$.lo= $$.hi= $2; }
  | '{' NUM_TOK ',' '}'		{ $$.lo= $2; $$.hi= NIL; }
  | '{' NUM_TOK ',' NUM_TOK '}'	{ $$.lo= $2; $$.hi= $4; }
  ;

%%

/* Parser initialization. */
VOID 
initParse()
{
  VOID_RET();
}

/* Syntax error routine. */
VOID 
yyerror(s)
  Char *s;
{
  ConstString p;
  for (p= yytext; isspace(*p); p++) ;
  if (*p == '\0') { /* yytext is empty or contains only whitespace. */
    error("parse error");
  }
  else {
    error("parse error before `%s'.", yytext);
  }
  VOID_RET();
}

