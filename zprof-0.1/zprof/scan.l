/*

File:	 scan.l
Purpose: Simple C scanner.

Last Update Time-stamp: <97/07/30 07:27:23 zdu>

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/* 

Scanner for subset of C.  Assumes input may or may not have been run thru
preprocessor.  Handles residual # directives for line # and file name.
We assume the input is correct --- hence minimal error checking.  Does
not really care about the semantics of most tokens.

*/

%option	--yylineno

id		[[:alpha:]_][[:alnum:]_]*
schar		"\\"(.|\n)|[^\\\"\n]		
cchar		"\\"(.|\n)|[^\\'\n]
dec		[[:digit:]]+
hex		0[xX][[:xdigit:]]+
isuff		[lLuU]
int		({dec}|{hex}){isuff}?
fract		"."{dec}
exp		[eE][-+]?{dec}
fsuff		[fFlL]
float		({dec}?{fract}{exp}?|{dec}{fract}?{exp}|{dec}"."{exp}?){fsuff}?
fName		\"[^\"\n]*\"

%x	CPP
%x	COMMENT


%{

#include "options.h"
#include "out.h"
#include "scan.h"

#include "error.h"
#include "ids.h"
#include "memalloc.h"

#include <stdio.h>
#include <string.h>

/* Lexical value. */

typedef enum {		/* State within # directive. */
  START_LINE= 0,	/* seen # or #[ \t]*line directive. */	
  NUM_LINE,		/* seen 1st line number within line. */
  CPP_LINE		/* not # or #line directive; merely copy */
} LineState;

typedef struct {
  Char *buf;		/* writable area */
  Char *dest;		/* addr of next writable char in buf[] */
  Char *end;		/* addr 1 beyond last writable location in buf[] */
} Buf;

static struct {
  Buf buf[2];		/* buffer for "whitespace" + yytext */
  Index bufNum;		/* index of buf[] currently being written */
  Index commentLineN;	/* line # where comment starts */
  LineState lineState;	/* state within # directive */
  ConstString tmpName;	/* temporary name created by tmpnam() */
  Index2 lineID;	/* id for "line" */
  Boolean inCond;	/* true if '?' seen and ';' not seen. */
} globs;
#define G globs

static Index getTok PROTO((Index id));
static VOID growBuf PROTO((Buf *buf, ConstString text, Size len));

#define OUT(text, len)							\
  do {									\
    Buf *CONST bufP= &G.buf[G.bufNum];					\
    if (bufP->dest + (len) < bufP->end) {				\
      memcpy(bufP->dest, (text), (len));		  		\
      bufP->dest+= (len);						\
    }									\
    else {								\
      growBuf(bufP, (text), (len));					\
    }									\
  } while (0)

#define TOK_SEM	yylval.tok.t

/* For running stand-alone unit-test */
#if TEST_SCAN

static struct {
  struct {
    TokSem t;
  } tok;
} yylval;

enum { 
  AUTO_TOK= 257,
  BREAK_TOK,
  CASE_TOK,
  CHAR_TOK,
  CONST_TOK,
  CONTINUE_TOK,
  DEFAULT_TOK,
  DO_TOK,
  DOUBLE_TOK,
  ELSE_TOK,
  ENUM_TOK,
  EXTERN_TOK,
  FLOAT_TOK,
  FOR_TOK,
  GOTO_TOK,
  IF_TOK,
  INT_TOK,
  LONG_TOK,
  REGISTER_TOK,
  RETURN_TOK,
  SHORT_TOK,
  SIGNED_TOK,
  SIZEOF_TOK,
  STATIC_TOK,
  STRUCT_TOK,
  SWITCH_TOK,
  TYPEDEF_TOK,
  UNION_TOK,
  UNSIGNED_TOK,
  VOID_TOK,
  VOLATILE_TOK,
  WHILE_TOK,

  ADD_ASSGN_TOK,
  AND_ASSGN_TOK,
  BOOL_AND_TOK,
  BOOL_OR_TOK,
  DEC_TOK,  
  DEREF_TOK,
  DIV_ASSGN_TOK,
  DOT_DOT_TOK,
  EQ_TOK,   
  GE_TOK,    
  INC_TOK,   
  LE_TOK,  
  LSH_ASSGN_TOK,
  LSH_TOK,  
  MOD_ASSGN_TOK,
  MULT_ASSGN_TOK,
  NE_TOK,   
  OR_ASSGN_TOK,
  RSH_ASSGN_TOK,
  RSH_TOK,  
  SUB_ASSGN_TOK,
  XOR_ASSGN_TOK,

  ID_TOK,
  NUM_TOK,
  STRING_TOK
};
#else
#include "parse.h"

#endif


%}

%%
"+="			{ return ADD_ASSGN_TOK; }
"-="			{ return SUB_ASSGN_TOK; }
"*="			{ return MULT_ASSGN_TOK; }
"/="			{ return DIV_ASSGN_TOK; }
"%="			{ return MOD_ASSGN_TOK; }
"<<="			{ return LSH_ASSGN_TOK; }
">>="			{ return RSH_ASSGN_TOK; }
"&="			{ return AND_ASSGN_TOK; }
"^="			{ return XOR_ASSGN_TOK; }
"|="			{ return OR_ASSGN_TOK; }
"->"			{ return DEREF_TOK; }
"++"			{ return INC_TOK; }
"--"			{ return DEC_TOK; }
"<<"			{ return LSH_TOK; }
">>"			{ return RSH_TOK; }
"<="			{ return LE_TOK; }
">="			{ return GE_TOK; }
"=="			{ return EQ_TOK; }
"!="			{ return NE_TOK; }
"&&"			{ return BOOL_AND_TOK; }
"||"			{ return BOOL_OR_TOK; }
"..."                   { return DOT_DOT_TOK; }
{id}			{ TOK_SEM.id= getID(yytext, yyleng);
			  return getTok(TOK_SEM.id);
			}
{float}|{int} 		{ return NUM_TOK; }
L?\'{cchar}*\'		{ return NUM_TOK; }
L?\'{cchar}*$		{ if (!optionsP->isSilent) {
                            error("Unterminated character constant."); 
                          }
                          return NUM_TOK;
                        }
L?\"{schar}*\"		{ return STRING_TOK; }
L?\"{schar}*$		{ if (!optionsP->isSilent) {
                            error("Unterminated string."); 
                          }
                          return STRING_TOK; 
                        }
([ \t\v\f\n]|\\\n)+     { OUT(yytext, yyleng); }
"/*"			{ OUT(yytext, yyleng); G.commentLineN= yylineno; 
                          BEGIN COMMENT; 
                        }
^[ \t]*#		{ OUT(yytext, yyleng); BEGIN CPP; 
                          G.lineState= START_LINE; 
                        }
.			{ return yytext[0]; }

/* Within a slash-star comment. */
<COMMENT>"*/"		{ OUT(yytext, yyleng); BEGIN INITIAL; }
<COMMENT><<EOF>>	{ errorAtLine("%d: EOF within comment", 
				      G.commentLineN); 
			}
<COMMENT>\n+		{ OUT(yytext, yyleng); }
<COMMENT>[^*\n]+	|
<COMMENT>"*"+/[^/]      { OUT(yytext, yyleng); }

/* Within a # line.  G.lineState keeps track of what we've seen so far. */
<CPP>{id} 		{ if (G.lineState != START_LINE || 
			      getID(yytext, yyleng) != G.lineID) {
                            G.lineState= CPP_LINE;
                          }
                          OUT(yytext, yyleng);
                        }
<CPP>{dec}		{ if (G.lineState == START_LINE) {
			    sscanf(yytext, "%d", &yylineno); 
			    G.lineState|= NUM_LINE;
			  }
                          else {
			    G.lineState= CPP_LINE;
			  }
                          OUT(yytext, yyleng);
			}
<CPP>{fName}		{ if (G.lineState == NUM_LINE) {
                            yytext[yyleng - 1]= '\0'; 
			    TOK_SEM.fName= 
			      getIDString(getID(&yytext[1], yyleng-2));
			    G.lineState= CPP_LINE; yytext[yyleng - 1]= '"';
			  }
                          else {
			    G.lineState= CPP_LINE;
			  }
                          OUT(yytext, yyleng);
			}
<CPP>[ \t]+		|
<CPP>\\\n		|
<CPP>.                 	{ OUT(yytext, yyleng); }
<CPP>\n			{ yylineno--; BEGIN INITIAL; OUT(yytext, yyleng); }

%%

/* Setup scanner to start scanning a new file.  Assumes it is not currently
 * scanning a file. 
 */
VOID
newFile(fName)
  ConstString fName;
{
  CONST Count fNameLen= strlen(fName);
  ConstString srcName= fName;
  enum { MAX_CMD= 256 };
  Char cmd[MAX_CMD];	/* hopefully large enough for any path used */
  if (optionsP->isPreprocess) { 
    ConstString cpp= getenv("CPP");
    ConstString cc= getenv("CC");
    ConstString tmpName= tmpnam(NULL);
    G.tmpName= tmpName;
    if (!tmpName) { 
      fatal("too many source files: cannot create name for temporary file"); 
    }
    if (system(NULL) == 0) { fatal("no shell to run preprocessor"); }
    if (cc == NULL) cc= "cc";
    if (cpp) {
      if (strlen(cpp) + 1 + fNameLen + 2 + strlen(tmpName) + 1 > MAX_CMD) {
	fatal("cpp command too long");
      }
      else {
	sprintf(cmd, "%s %s >%s", cpp, fName, tmpName);
      }
    }
    else {
      if (strlen(cc) + 4 + fNameLen + 2 + strlen(tmpName) + 1 > MAX_CMD) {
	fatal("cpp command too long");
      }
      else {
	sprintf(cmd, "%s -E %s >%s", cc, fName, tmpName);
      }
    }
    if (system(cmd) != 0) { /* assume unix-style return value */
      fatal("cpp command failed for %s", fName);
    }
    srcName= tmpName;
  }
  if ((yyin= fopen(srcName, "r")) == NULL) {
    fatal("could not open file %s", srcName);
  }
  yyrestart(yyin); TOK_SEM.fName= fName; yylineno= 1;
}

Index
lineNum()
{
  return yylineno;
}


ConstString
fileName()
{
  return TOK_SEM.fName;
}

static ConstString reserved[]= { /* Table of reserved words */
  "auto",
  "break",
  "case",
  "char",
  "const",
  "continue",
  "default",
  "do",
  "double",
  "else",
  "enum",
  "extern",
  "float",
  "for",
  "goto",
  "if",
  "int",
  "long",
  "register",
  "return",
  "short",
  "signed",
  "sizeof",
  "static",
  "struct",
  "switch",
  "typedef",
  "union",
  "unsigned",
  "void",
  "volatile",
  "while"
};

VOID 
initScan() 
{
  int i;
  initError(NULL, 10000, NULL, fileName, lineNum, NULL);
  initID(FALSE);
  for (i= 0; i < N_ELEMENTS(reserved); i++) {
    Index j= getID(reserved[i], strlen(reserved[i]));
    assert(i == j);
  }
  G.lineID= getID("line", 4);
}

VOID 
terminateScan() 
{
  terminateID();
}

static VOID 
growBuf(bufP, text, len)
  Buf *bufP;
  ConstString text;
  Size len;
{
  enum { LG2_GROW_INC= 12 };
  CONST Size nGrow= 
    ((len + (1 << LG2_GROW_INC) - 1) >> LG2_GROW_INC) << LG2_GROW_INC;
  CONST Index currentIndex= bufP->dest - bufP->buf;
  CONST Size newSize= bufP->end - bufP->buf + nGrow;
  bufP->buf= REALLOC(bufP->buf, newSize);
  bufP->dest= bufP->buf + currentIndex;
  bufP->end= bufP->buf + newSize;
  memcpy(bufP->dest, text, len); bufP->dest+= len;
  VOID_RET();
}

/* Assumes that token #s are contiguous and that the reserved
 * words were entered into the string-table in the order of their
 * token #s.  Ensured by keeping both lexemes and token names
 * sorted alphabetically.  Could be maintained more reliably using
 * macros.
 */
static Index
getTok(id)
  Index id;
{
  return id < N_ELEMENTS(reserved) ? AUTO_TOK + id : ID_TOK;
}

/* Wrapper around yylex() which does cleanup on EOF; ensures that
 * every token delivered is first OUTput; delivers only a single
 * STRING_TOK for a sequence of adjacent string tokens; implements
 * a kludge for processing conditional expressions.
 */
Index
scan()
{
  Buf *CONST bufP= &G.buf[G.bufNum];  
  int tok= yylex(); 
  if (tok == 0) { /* EOF */
    if (G.tmpName != NULL) {
      remove(G.tmpName); G.tmpName= NULL;
    }
  }
  else {
    OUT(yytext, yyleng);
  }
  TOK_SEM.text= bufP->buf; TOK_SEM.len= bufP->dest - bufP->buf;
  TOK_SEM.len1= TOK_SEM.len - yyleng;
  TOK_SEM.lineNum= yylineno;
  G.bufNum= 1 - G.bufNum; G.buf[G.bufNum].dest= G.buf[G.bufNum].buf;
  return tok;
}

void 
yyerror(s)
  ConstString s;
{
  if (!optionsP->isSilent) error("parse error at `%s'", yytext);
}

#if TEST_SCAN

/* unit test: could be nicer about printing out token names instead of 
 * numbers.
 */
int 
main(int argc, char *argv[])
{
  int tok;
  initScan();

  if (argc > 1) { 
    newFile(argv[1]); 
  }

  do {
    tok= scan();
    fprintf(stderr, ">> %s:%d: tok= %d text= %.*s id= %d\n", 
	    TOK_SEM.fName,  yylineno, tok, (Int)TOK_SEM.len, TOK_SEM.text, 
	    tok == ID_TOK ? TOK_SEM.id : -1);
  } while (tok != 0);
  return 0;
}

#endif
