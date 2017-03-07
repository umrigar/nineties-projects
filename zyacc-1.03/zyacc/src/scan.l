/*

File:	 scan.l
Purpose: Lexical analyzer.

Last Update Time-stamp: "97/07/28 17:22:07 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/* -*-c-*- */
/*

zyacc Scanner.  Requires zlex.

If we encounter ":" or "|" in section 1, we assume we have a missing "%%";
we signal an error, start lexing assuming we are in section 2 and return
a MARK_TOK (for "%%").

If we see a ";" within parentheses we again assume a error.  We return
a ')'.

Requires call-aheads to semantic routines: 

translateNumVar(int varNum, Index varTypeID, NumVar *numVarP)
VOID translateNumVar(int varNum, Index varTypeID, NumVar *numVarP)

which should fill in numVarP after checking for errors.  These routines are 
necessary to allow semantic routines to do their stuff without bothering
at all about outputting $-variables occurring in actions into tmpFiles.

*/


/* 			LEX MACROS.					*/
cChar		([^\n\']|\\.)			/* Char. const. char. */
sChar		([^\"\n]|\\.)			/* String char. */
id		([._a-zA-Z][._a-zA-Z0-9]*)	/* YACC identifier. */
c_id		([_a-zA-Z][_a-zA-Z0-9]*)	/* C identifier. */
type		("<"{c_id}">")			/* YACC type designator. */
dir		(\%[a-zA-Z]+)			/* YACC directive. */
ws		[ \t\v\f]			/* Whitespace except newline. */
digit		[0-9]
oct		[0-7]
hex		[0-9a-fA-F]
num		({digit}+)	       		/* Number. */
option		[-=_[:alnum:]]+			/* Option word. */


/*			   LEX START STATES.				*/

/* Each state may have watch chars which cause special actions to be taken.
 * For instance, a '"' may cause entry to the STRING state, a '\n' may bump
 * the linecounter, a delimiter like a paren or brace may bump a 
 * delimiter counter, a '/' (if followed by a '*') may start a comment.
 * In the following, the phrase "copied if temp. file" means the constituent
 * is copied into the temporary file if its FILE pointer is non-NULL.
 */

%s OPTIONS	/* In a %option line. */
%x ACT		/* Action within balanced braces.  Copied to temp. file.
		 * Watch set: { '"' '\'' '{' '}' '$' '/' '\n' }.  */
%x COMMENT	/* Comment.  Copied if temp file.  Watch set: { '/' '\n' }. */
%x DEF		/* Definitions within decorated braces. Copied to temp. file.
		 * Watch set: { '"' '\'' '%' '/' '\n' }. */
%x LINE		/* After a %line directive. */
%x PAREN	/* Declarations/expressions within balanced paren.  Not copied.
		 * Watch set: { '"' '\'' '(' ')' '$' '/' '\n' }. */
%x S3		/* Section 3.  Copied to temp. file.  Watch set: { '\n' }. */
%x STRING	/* C-String.  Copied if temp. file.
		 * Watch set: { '"' '\\' '\n' }. 
		 */
%x XUNION	/* Union definition. Copied to temp. file.
		 * Watch set: { '"' '\'' '{' '}' '/' '\n' }.  */
%x VAR		/* Within a yacc-style $-variable. Translated; copied if
		 * temp. file.  At most 2 actions when in this state. 
		 */


/*				C DECLARATIONS.				*/

%{

#include "attribs.h"
#include "gram.h"
#include "options.h"
#include "scan.h"
#include "zparse.h"

#include "area.h"
#include "error.h"
#include "ids.h"
#include "zutil.h"

#undef NDEBUG

#include <assert.h>

/* Other system includes via port.h. */

static struct {		/* Globals - in struct to avoid namespace pollution. */

  /* lineSrcName may contain escape sequences; srcName will not. */
  ConstString srcName; /* Name of zyacc source file for error messages. */
  ConstString lineSrcName;/* Name of zyacc source file for #line directives. */

  ConstString *argv;	/* Command-line arguments. */
  Count2 argc;		/* Number of command line arguments. */
  Count2 argn;		/* Number of processed command line arguments. */
  FILE *actsFile;	/* FILE ptr for section 2 actions temporary file. */
  FILE *defsFile[2];	/* FILE ptr for %{ %} definitions. */
  FILE *tFile;		/* FILE ptr to current temporary file. */
  FILE *unionFile;	/* FILE ptr for %union temporary file. */
  Index2 actN;		/* Number of current action. */
  Count2 actNLines;	/* # of lines in current action. */
  Count2 codeLineN;	/* Line # at start of last action or expression. */
  Count2 delimDepth;	/* Used to keep track of nested delimiters. */
  Index2 lastSS;	/* Last state when in comment or var. */
  Count2 lineNum;	/* Current line number in source. */
  Index2 lineState;	/* 0: Seen #line; 1: Seen number; 2: Seen file. */
  Count2 parseLineN0;	/* Line # of last token delivered to parser. */
  Count2 parseLineN1;	/* Line # of second last token delivered to parser. */
  Boolean hasUnion;	/* True if %union declaration used. */
  Boolean hasLoc;	/* True if we've seen a @n location-variable. */
  Boolean hasRule;	/* True if we've seen a ':' in section 2. */
  Boolean hasStart;	/* True if we've seen a %start directive. */
  Boolean inArg;	/* TRUE if we are within a non-terminal argument. 
                         * Strips leading WS & comments from PAREN args.
			 * Necessary for XID to be correctly identified
			 * if %in or %out not immediately after '('.
			 */
  Boolean isTyped;	/* TRUE if has <type> tag. */
  Count1 secN;		/* Current section #. */
} globals;

#define G globals

/* For each user action keep track of how many lines it occupies in
 * the temporary file and its corresponding reduction # (set by
 * higher-level code).  
 */

typedef struct {
  Count2 nLines;	/* # of lines in actFile occupied by this act. */
  Index2 reductN;	/* Reduction number associated with this act. */
  Index2 attribAssign; 	/* Implicit attribute assignment if non-NIL. */
} ActInfo;

static AREA (acts, ActInfo, 6, UINT2_MAX);
#define ACTS(i) A_ACCESS(acts, ActInfo, i)


Index 
newAct()
{
  Index a= A_NEXT(acts);
  ACTS(a).nLines= 0; ACTS(a).reductN= ACTS(a).attribAssign= NIL;
  return a;
}

VOID 
setActReduction(actN, reductN)
  Index actN;
  Index reductN;
{
  assert(actN < A_NENTRIES(acts));
  ACTS(actN).reductN= reductN;
  VOID_RET();
}

VOID 
addActAttribAssign(ruleN, lhsIDVar, lhsOffset, rhsID)
  Index ruleN;
  Index lhsIDVar;
  Int lhsOffset;
  Index rhsID;
{
  Index actN= A_NENTRIES(acts) - 1;
  assert(ACTS(actN).reductN == ruleN);
  outIDVar(G.actsFile, lhsIDVar, lhsOffset);
  fputs("= ", G.actsFile);
  outAttribVar(G.actsFile, getIDString(rhsID));
  fputs(";\n", G.actsFile); ACTS(actN).nLines++;
  VOID_RET();
}

Count
nActions()
{
  return A_NENTRIES(acts);
}

#define NUM_VAR_MACRO	"YY_NUM_VAR"
#define LOC_VAR_MACRO	"YY_LOC_VAR"

/* Macros which try to keep lex code-sections short. */

#define GET_ID()	getID(yytext, yyleng)

#define OUTS(s)								\
  do {									\
    assert(G.tFile != NULL); fputs(s, G.tFile);				\
  } while (0)

#define OUTS_IF(s)							\
  do {									\
    if (G.tFile != NULL) OUTS(s);					\
  } while (0)

#define OUT_TEXT()	OUTS(yytext)

#define OUT_TEXT_IF()	OUTS_IF(yytext)

#define LINE_DIR()	lineDir(G.tFile, G.lineNum)

#define LIT_RET(c)							\
  do {									\
    if (1) { /* Avoid statement not reached warnings. */                \
      TOKVAL.lit.val= (c); TOKVAL.lit.id= GET_ID(); return TOK(LIT); 	\
    }                                                                   \
  } while (0)

#define TEXT_RET()							\
  do {									\
    if (1) { /* Avoid statement not reached warnings. */                \
      if (yyleng < sizeof(yylval.shortText)) {				\
        strncpy(TOKVAL.shortText, yytext, sizeof(yylval.shortText));	\
        return TOK(SHORT_TEXT);						\
      }									\
      else {								\
        TOKVAL.text= internText(yytext, yyleng);	 		\
        return TOK(TEXT);						\
      }									\
    }                                                                   \
  } while (0)	

#define CHAR_RET(c)							\
  do {									\
    if (1) { /* Avoid statement not reached warnings. */                \
      assert(sizeof(yylval.shortText) >= 2); 				\
      TOKVAL.shortText[0]= c; TOKVAL.shortText[1]= '\0'; 		\
      return TOK(SHORT_TEXT);						\
    }                                                                   \
  } while (0)	

#define LAST_SS_TEST(SS, SSAct, ElseAct)				\
  do {									\
    if (G.lastSS == SS) { SSAct; } else { ElseAct; }			\
  } while (0)

#define UNION_EOF_ERROR()						\
  errorAtLine("%d: Unexpected end-of-file in union definition.", 	\
	G.codeLineN); 

enum { /* Pseudo-tokens which don't get returned by scanner.  Must be < 0. */
  LINE_PSEUDO_TOK= -1
};

#ifdef TEST_SCAN

#include "scandefs.h"
static YYSType yylval;

static Char ctok[2]= { '\0', '\0' };	/* Return char-toks as 1 char. str. */
#define CTOK(x) ((int)(ctok[0]= x , &ctok[0]))
#define TOK(x)		((int) STRINGIFY(x))
#define TOK_EQ(unknownTok, litTok)					\
  ((unknownTok) != 0 							\
   && strcmp((ConstString) (unknownTok), STRINGIFY(litTok)) == 0)
#define CTOK_EQ(unknownTok, litTok)					\
  ((unknownTok) != 0 && unknownTok[0] == litTok)

#else	/* !def TEST_SCAN. */

#ifdef TEST_REC
#include "recdec.h"
#include "scandefs.h"
#define yylval yyLVAL	/* yyLVAL used by scanner. yylval ignored. */
static YYSType yyLVAL;
#else
#include "gram.h" 	/* For translateNumVar() & translateLHSVar(). */
#include "parse.h"	/* For YYSTYPE. */
typedef YYSTYPE YYSType;
#endif /* ifdef TEST_REC */

#define TOK(x)		TOK_CAT(x,_TOK)
#define CTOK(x)		x
#define END_TOK		0		/* Returned at start of section 3. */
#define TOK_EQ(unknownTok, litTok)	(unknownTok == TOK(litTok))
#define CTOK_EQ(unknownTok, litTok)	(unknownTok == litTok)

#endif /* ifdef TEST_SCAN */

/* Static prototypes. */
static VOID eofError PROTO((Count lineN, ConstString delimiter));
static Index getDir PROTO((char *dir, Size len));
static VOID setSrcFile PROTO((VOID_ARGS));
static VOID defBegin PROTO((VOID_ARGS));
static Count outTmpFile PROTO((FILE *outFile, FILE *tmpFile));
static VOID initPredefinedIDs PROTO((VOID_ARGS));
static VOID addOptionsText PROTO((VOID_ARGS));
static ConstString getOptionsText PROTO((VOID_ARGS));


/*			TOKEN BUFFER.					*/

/* Take care of buffering tokens to resolve problematic cases. */

/* yylex() directly writes token value into val component of current
 * TokEntry to avoid unnecessary copying.  yylex() returns the token
 * type to scan() which then writes the type into the TokEntry and
 * increments ptrs.  We may need an ID, '(' followed by IN or OUT to
 * change the ID to XID.  
 */

enum { MAX_BUFFER= 4 };	/* Must be 1 > # of tokens to be stored. */
typedef struct { 
  YYSType val;
  Int tok;
  Index2 lineN;		/* Source line # at end of token. */
} TokEntry;

static struct {
  TokEntry *readP;	/* Next location to be read from. */
  TokEntry *writeP;	/* Next location to be written into.  
			 * ALWAYS points to an empty slot.
			 */
  TokEntry buf[MAX_BUFFER];
} tokBuf;

#define INIT_TOK_BUF()							\
  do {									\
    tokBuf.readP= tokBuf.writeP= tokBuf.buf;				\
  } while (0)

#define BUFP_INC(p)	/* Return circular increment of buf. ptr. p. */ \
  (((p) + 1 == &tokBuf.buf[MAX_BUFFER]) ? tokBuf.buf : (p) + 1)
#define IS_EMPTY_TOKBUF()	(tokBuf.readP == tokBuf.writeP)
#define IS_FULL_TOKBUF()	(BUFP_INC(tokBuf.writeP) == tokBuf.readP)  
#define PUT_TOKBUF()	/* Add next token to token buffer. */		\
  do {									\
    assert(!IS_FULL_TOKBUF());						\
    tokBuf.writeP->tok= yylex(); /* val written directly by yylex(). */	\
    tokBuf.writeP->lineN= G.lineNum;					\
    tokBuf.writeP= BUFP_INC(tokBuf.writeP);				\
  } while (0)
#define GET_TOKBUF(tokEntryP)	/* Point tokEntryP to next TokEntry */	\
  do {				/* read from tokBuf.		    */	\
    if (IS_EMPTY_TOKBUF()) PUT_TOKBUF();				\
    tokEntryP= tokBuf.readP; tokBuf.readP= BUFP_INC(tokBuf.readP);	\
  } while (0)
#define CURRENT_TOKBUF_ENTRYP(tokEntryP) 				\
  do {									\
    if (IS_EMPTY_TOKBUF()) PUT_TOKBUF();				\
    tokEntryP= tokBuf.readP;						\
  } while (0)
#define NEXT_TOKBUF_ENTRYP(tokEntryP) /* Update tokEntryP to next. */	\
  do {									\
    tokEntryP= BUFP_INC(tokEntryP);					\
    if (tokEntryP == tokBuf.writeP) PUT_TOKBUF();			\
  } while (0)
#define TOKVAL	(tokBuf.writeP->val)

%}

/*			    LEX PATTERNS.				*/
/* LIMITATIONS:  Haven't bothered with escaped newlines within char-consts
 * or comment-delimiters.
 */

/* Many actions are identical.  However we have avoided using lex's |
 * feature because the start-states and patterns are complex enough
 * that it is preferable to keep all patterns for a particular start
 * state together.  If desired, the same effect as lex's | feature 
 * can be gotten by replacing all but one of an identical set of action
 * by a goto to the action --- any decent compiler should be able to
 * optimize away the goto's from the switch generated within yylex().
 */

%%
%{ 	/* Variables local to generated scanner function. */
struct { /* Obviously these vars are undefined between calls to scanner fn. */
  Count2 commentLineN;	/* Line # at start of last comment. */
  Index2 varType;	/* The id of type in the usage $<type>nn. */
} locals;
#define L locals
%}

<OPTIONS>{option} 	{ addOptionsText(); }
<OPTIONS>\n		{ G.lineNum++; BEGIN INITIAL; 
                          TOKVAL.text= getOptionsText(); 
			  return TOK(OPTION_VAL); 
                        }
\n			{ G.lineNum++; }
{ws}+			{ }
{id}			{ TOKVAL.id= GET_ID(); return TOK(ID); }
{num}			{ TOKVAL.num= atoi(yytext); return TOK(NUM); }
{type}			{ yytext[yyleng - 1]= 0; /* Clobber '>'. */
			  TOKVAL.type= getID(&yytext[1], yyleng - 2);
			  yytext[yyleng - 1]= '>'; /* Restore '>' for errors. */
			  G.isTyped= TRUE;
			  return TOK(TYPE_ID);
			}
";"			{ return CTOK(';'); }
","			{ return CTOK(','); }
":"			{ if (G.secN == 1) {
                            error("unexpected `:'. Possibly missing `%%%%'");
                            G.secN= 2; return TOK(MARK);
			  }
                          else {
			    G.hasRule= TRUE; return CTOK(':'); 
			  }
			}
"|"			{ if (G.secN == 1) {
                            error("unexpected `|'.  Possibly missing `%%%%'");
                            G.secN= 2; return TOK(MARK);
			  }
			  else 
			    return CTOK('|'); 
			}
\'\\a\'			{ LIT_RET('\007'); /* Some cc's choke on '\a'. */ }
\'\\b\'			{ LIT_RET('\b'); }
\'\\f\'			{ LIT_RET('\f'); }
\'\\n\'			{ LIT_RET('\n'); }
\'\\r\'			{ LIT_RET('\r'); }
\'\\t\'			{ LIT_RET('\t'); }
\'\\v\'			{ LIT_RET('\013'); /* Some cc's choke on '\v'. */}
\'\\{oct}{1,3}\'	{ int ch; sscanf(&yytext[2], "%o", &ch); LIT_RET(ch); }
\'\\x{hex}+\'		{ int ch; sscanf(&yytext[3], "%x", &ch); LIT_RET(ch); }
\'{cChar}\'		{ LIT_RET(yytext[1]); }
\'{cChar}{2,}\'		{ LIT_RET(NIL); }
\'[^\n\']+		{ error("unterminated character constant %s", 
				yytext); 
			}
"%%"			{ G.secN++; 
			  if (G.secN == 3) { 
			    BEGIN S3; return TOK(END); /* Quit parsing. */ 
			  }
			  return TOK(MARK); 
			}
{dir}			{ int dir= getDir(&yytext[1], yyleng - 1); 
                          if (dir >= 0) return dir;
                        }
"%{"			{ defBegin(); }
"{"			{ G.delimDepth= 1; G.tFile= G.actsFile; 
			  G.actNLines= LINE_DIR(); 
			  TOKVAL.act.actN= G.actN= newAct();
			  TOKVAL.act.lineNum= G.codeLineN= G.lineNum;
			  OUT_TEXT(); BEGIN(ACT); return CTOK('{'); 
			}
"("			{ G.delimDepth= 1; G.inArg= FALSE;
			  TOKVAL.lineN= G.codeLineN= G.lineNum;
			  BEGIN(PAREN); return CTOK('('); 
			}
"/*"			{ L.commentLineN= G.lineNum;
			  G.lastSS= YY_START;  BEGIN COMMENT; 
			}
.			{ return CTOK(yytext[0]); }

/* Section 3.  Merely copy source to temporary file. */
<S3>\n			{ G.lineNum++; OUT_TEXT(); }
<S3>.+			{ OUT_TEXT(); }
<S3><<EOF>>		{ return 0; }

/* Within { ... } actions. */
<ACT>\n			{ G.lineNum++; G.actNLines++; OUT_TEXT();  }
<ACT>"{"		{ G.delimDepth++; OUT_TEXT(); }
<ACT>"}"		{ OUT_TEXT();
			  if (--G.delimDepth == 0) {
			    ACTS(G.actN).nLines= G.actNLines + 1; 
                            OUTS("\n"); G.tFile= NULL; 
			    BEGIN INITIAL; return CTOK('}');
			  }
			}
<ACT>\"			{ G.lastSS= YY_START; OUT_TEXT(); BEGIN STRING; }
<ACT>"/*"		{ L.commentLineN= G.lineNum; OUT_TEXT();
			  G.lastSS= YY_START;  BEGIN COMMENT; 
			}
<ACT>"/"/[^*]		{ OUT_TEXT(); }
<ACT>\'{cChar}*\'	{ OUT_TEXT(); }
<ACT>\'{cChar}*$	{ warn("Unterminated C character constant."); 
			  OUT_TEXT();
                        }
<ACT>"$"		{ G.lastSS= YY_START; 
			  BEGIN VAR; L.varType= NIL; 
			}
<ACT>"@"[-]?{num}       { fprintf(G.tFile, "%s(%d)", LOC_VAR_MACRO,
				  atoi(&yytext[1]));
                          G.hasLoc= TRUE;
                        }
<ACT>[^\n{}\"/\'$@]+	{ OUT_TEXT(); }
<ACT>. 			{ OUT_TEXT(); }
<ACT><<EOF>>		{ eofError(G.codeLineN, "}"); return 0; }

/* Seen comment-start '/' followed by '*'. */
<COMMENT>\n		{ G.lineNum++; OUT_TEXT_IF(); }
<COMMENT>"*/"		{ OUT_TEXT_IF(); BEGIN G.lastSS; }
<COMMENT>"*"+/[^/]	{ OUT_TEXT_IF(); }
<COMMENT>"/*"		{ warn("/* within comment."); OUT_TEXT_IF(); }
<COMMENT>"/"+/[^*]	{ OUT_TEXT_IF(); }
<COMMENT>[^\n*/]+	{ OUT_TEXT_IF(); }
<COMMENT><<EOF>>	{ eofError(L.commentLineN, "*/"); return 0; }

/* Within %{ ... %} definitions. */
<DEF>\n			{ G.lineNum++; OUT_TEXT(); }
<DEF>"%}"		{ OUTS("\n"); BEGIN INITIAL; G.tFile= NULL; }
<DEF>"%"/[^}]		{ OUT_TEXT(); }
<DEF>"/*"		{ L.commentLineN= G.lineNum; OUT_TEXT();
			  G.lastSS= YY_START;  BEGIN COMMENT; 
			}
<DEF>"/"/[^*]		{ OUT_TEXT(); }
<DEF>\"			{ G.lastSS= YY_START; OUT_TEXT(); BEGIN STRING; }
<DEF>\'{cChar}*\'	{ OUT_TEXT(); }
<DEF>\'{cChar}*$	{ warn("Unterminated C character constant."); 
			  OUT_TEXT();
                        }
<DEF>[^\n%/\"\']+	{ OUT_TEXT(); }
<DEF><<EOF>>		{ eofError(G.codeLineN, "%}"); return 0; }

/* Seen %line directive. */
<LINE>\n		{ G.lineNum++; BEGIN G.lastSS; }
<LINE>{ws}+		
<LINE>{num}		{ if (G.lineState) {
                            error("unexpected number %s in %%line directive",
                                  yytext);
                          }
                          else {
                            G.lineNum= atoi(yytext) - 1;
                          }
                        }
<LINE>\"{sChar}+\"      { if (G.lineState < 2) {
                            setSrcFile(); G.lineState= 2;
                          }
                          else {
                            error("duplicate file name %s in %%line directive",
			          yytext);
                          }
                        }
<LINE>\"{sChar}*$       { error(
                            "unterminated file name in %%line directive"); 
                        }

/* Seen '('.  Use G.inArg to identify start of argument. If we see a ';',
 * we definitely have an error --- we report it and return to INITIAL.
 */
<PAREN>\n		{ G.lineNum++; if (G.inArg) CHAR_RET('\n'); }
<PAREN>{ws}+		{ if (G.inArg) CHAR_RET(' '); }
<PAREN>{dir}		{ int dir= getDir(&yytext[1], yyleng - 1); 
                          if (dir >= 0) return dir;
                        }
<PAREN>"("		{ G.inArg= TRUE; G.delimDepth++; CHAR_RET('('); }
<PAREN>")"		{ if (--G.delimDepth == 0) { 
                            BEGIN INITIAL; return CTOK(')');
			  }
			  else {
			    G.inArg= TRUE; CHAR_RET(')');
			  }
			}
<PAREN>","		{ if (G.delimDepth > 1) {
			    G.inArg= TRUE; CHAR_RET(',');
			  }
			  else {
			    TOKVAL.lineN= G.lineNum; 
			    G.inArg= FALSE; return CTOK(','); 
			  }
			}
<PAREN>"/*"		{ L.commentLineN= G.lineNum;
			  G.lastSS= YY_START; BEGIN COMMENT; 
                          if (G.inArg) CHAR_RET(' ');
                        }
<PAREN>"/"/[^*]		{ G.inArg= TRUE; CHAR_RET('/'); }
<PAREN>\"		{ G.inArg= TRUE; 
                          G.lastSS= PAREN; BEGIN STRING; CHAR_RET('"'); 
                        }
<PAREN>\'{cChar}*\'	{ G.inArg= TRUE; TEXT_RET();
                        }
<PAREN>\'{cChar}*$	{ warn("Unterminated C character constant."); 
                          G.inArg= TRUE; TEXT_RET();
                        }
<PAREN>"$"		{ G.inArg= TRUE; G.lastSS= YY_START; 
			  BEGIN VAR; L.varType= NIL; 
			}
<PAREN>";"		{ error(
                            "`;' within parentheses; Possibly missing a `)'");
                          BEGIN INITIAL; return CTOK(')'); /* Lie. */
                        }
<PAREN>[^\n \t\v(),/\"\'$;]+	
                        { G.inArg= TRUE; TEXT_RET(); }
<PAREN><<EOF>>		{ eofError(G.codeLineN, ")"); return 0; }

/* Seen '"'. */
<STRING>{sChar}*\"	{ BEGIN G.lastSS; 
                          LAST_SS_TEST(PAREN, TEXT_RET(), OUT_TEXT_IF());
                        }
<STRING>{sChar}*\\\n	{ G.lineNum++; 	/* Escaped newline. */
                          LAST_SS_TEST(PAREN, TEXT_RET(), OUT_TEXT_IF()); 
                        }
<STRING>{sChar}*$	{ warn("Unterminated C string."); BEGIN G.lastSS; 
                          LAST_SS_TEST(PAREN, TEXT_RET(), OUT_TEXT_IF()); 
                        }

/* Within %union { ... }. */
<XUNION>\n		{ G.lineNum++; OUT_TEXT(); }
<XUNION>"/*" 		{ L.commentLineN= G.lineNum; OUT_TEXT();
			  G.lastSS= YY_START;  BEGIN COMMENT; 
			}
<XUNION>"/"/[^*]	{ OUT_TEXT(); }
<XUNION>"{"		{ if (++G.delimDepth > 1) OUT_TEXT(); }
<XUNION>"}"		{ if (--G.delimDepth) {
			    OUT_TEXT();
			  }
                          else {
			    OUTS("\n"); BEGIN INITIAL; G.tFile= NULL;
			  }
			}
<XUNION>\"		{ G.lastSS= YY_START; OUT_TEXT(); BEGIN STRING; }
<XUNION>\'{cChar}*\'	{ OUT_TEXT(); }
<XUNION>\'{cChar}*$	{ warn("Unterminated C character constant."); 
			  OUT_TEXT();
                        }
<XUNION>[^\n/{}\"\']+	{ OUT_TEXT(); }
<XUNION><<EOF>>		{ UNION_EOF_ERROR(); return 0; }
/* Seen '$'. */
<VAR>\n			{ error("invalid $-variable syntax");
			  G.lineNum++; BEGIN G.lastSS; 
			}
<VAR>{type}		{ if (L.varType == NIL) {
                            yytext[yyleng - 1]= 0;
			    L.varType= getID(&yytext[1], yyleng - 2);
			  }
			  else {
			    error("multiply typed $ variable");
			  }
			}
<VAR>"-"?{num}		{ BEGIN G.lastSS; 
                          translateNumVar(atoi(yytext), L.varType, 
					  &TOKVAL.numVar); 
			  LAST_SS_TEST(PAREN,
			    return TOK(NUM_VAR),
			    assert(G.lastSS == ACT && G.tFile != NULL); 
                            fprintf(G.tFile, "%s(%d, %s)", NUM_VAR_MACRO,
				    TOKVAL.numVar.offset, 
				    (TOKVAL.numVar.type == NIL)
				    ? "YYUnknown"
				    : getIDString(TOKVAL.numVar.type)));
			}
<VAR>"$"		{ BEGIN G.lastSS; 
                          translateLHSVar(L.varType, &TOKVAL.numVar); 
			  LAST_SS_TEST(PAREN,
			    return TOK(NUM_VAR),
			    assert(G.lastSS == ACT && G.tFile != NULL); 
                            fprintf(G.tFile, "%s(%d, %s)", NUM_VAR_MACRO,
				    TOKVAL.numVar.offset, 
				    (TOKVAL.numVar.type == NIL)
				    ? "YYUnknown"
				    : getIDString(TOKVAL.numVar.type)));
			}
<VAR>{c_id}		{ if (L.varType != NIL) {
                            error("named $-variable cannot be typed");
			  }
			  if (G.tFile) {
			    assert(G.lastSS == ACT); 
			    outAttribVar(G.tFile, yytext);
			  }
			  BEGIN G.lastSS;
                          TOKVAL.id= GET_ID(); return TOK(ID_VAR);
			}
<VAR>.			{ error("invalid $-variable syntax"); 
                          BEGIN G.lastSS; 
                        }

%%

/*			TOP-LEVEL SCANNER.				*/

/* Solve problem of determining whether a ID_TOK should become a
 * XID_TOK.  If the current token is a ID_TOK and the last token was a
 * MARK_TOK or a ';' then change the current token to a XID_TOK.  Read
 * ahead and make the same change if the next token is a COLON_TOK, or
 * if it is a '(' followed by either an IN_TOK or an OUT_TOK.  Need to
 * buffer semantic value too.  LIMITATION: Normally the syntax says
 * that a %out declaration is optional.  However because of the above
 * approach in identifying a XID token, there is a clumsy exception to
 * the optional %out rule: The %out declaration may not be omitted for
 * the 1st argument of a non-terminal if the preceeding rule was not
 * terminated by a ';'.
 */
int 
scan()
{
  TokEntry *tokP;
  static Boolean ruleEnd= FALSE;	/* Becomes TRUE after 1st "%%". */
  CURRENT_TOKBUF_ENTRYP(tokP);
  if (G.secN == 2 && TOK_EQ(tokP->tok, ID)) {
    if (ruleEnd) {
      tokP->tok= TOK(XID);
    }
    else {
      CONST TokEntry *nextTokP= tokP;
      NEXT_TOKBUF_ENTRYP(nextTokP);
      if (CTOK_EQ(nextTokP->tok, ':')) {
	tokP->tok= TOK(XID);
      }
      else if (CTOK_EQ(nextTokP->tok, '(')) {
	NEXT_TOKBUF_ENTRYP(nextTokP);
	if (TOK_EQ(nextTokP->tok, IN) || TOK_EQ(nextTokP->tok, OUT)) {
	  tokP->tok= TOK(XID);
	}
      }
    }
  }
  GET_TOKBUF(tokP);
  yylval= tokP->val;
  G.parseLineN1= G.parseLineN0; G.parseLineN0= tokP->lineN;
  ruleEnd= ( CTOK_EQ(tokP->tok, ';') || TOK_EQ(tokP->tok, MARK) );
  return tokP->tok;
}

/*			IDENTIFIER ROUTINES				*/

/* Routines here convert between C-type string lengths (which do not
 *  the terminating '\0' in the length computation) used
 * external to this module, and the length used by textID which does
 * not regard '\0' as special. 
 */

typedef struct { 	/* Struct to  YACC directives. */
  ConstString dir;
  Int tok;
} DirInfo; 

/* These directives are only recognized in the INITIAL and PAREN 
 * start states. 
 */
static DirInfo directives[]= {	/* Predefined ZYACC directives. */
  { "expect", TOK(EXPECT) },
  { "in", TOK(IN) },
  { "left", TOK(LEFT) },
  { "line", LINE_PSEUDO_TOK },
  { "look", LOOK_TOK }, 
  { "nonassoc", TOK(NONASSOC) },
  { "nowarn", TOK(NOWARN) }, 
  { "option", TOK(OPTION) },
  { "out", TOK(OUT) },
  { "prec", TOK(PREC) },
  { "pure_parser", TOK(PURE) },
  { "right", TOK(RIGHT) },
  { "start", TOK(START) },
  { "token", TOK(TOKEN) },
  { "test", TOK(TEST) },
  { "type", TOK(TYPE) },
  { "union", TOK(UNION) }
};

#define N_DIR	(sizeof(directives)/sizeof(DirInfo)) /* # of directives. */

static VOID 
initPredefinedIDs()
{
  Index i;
  initID(FALSE);
  for (i= 0; i < N_DIR; i++) {
    ConstString dir= directives[i].dir;
    Index dirID= getID(dir, strlen(dir));
    assert(i == dirID);
    dirID= 0; /* To avoid warning when NDEBUG. */
  }
  VOID_RET();
}

static Index 
getDir(dir, len)
  char *dir;
  Size len;
{
  Index i;
  for (i= 0; i < len; i++) dir[i]= tolower(dir[i]);
  { Index dirNum= getID(dir, len);
    Index tok= (dirNum != NIL && dirNum < N_DIR) 
               ? directives[dirNum].tok 
	       : TOK(LEX_ERR);
    switch (tok) { /* Special handling for certain tokens. */
      case TOK(IN):
      case TOK(OUT):
	TOKVAL.lineN= G.lineNum; 
	break;
      case TOK(START):
	if (G.hasStart) {
	  error("multiple %%start directive");
	}
	else {
	  G.hasStart= TRUE;
	}
	break;
      case TOK(UNION):
	BEGIN XUNION; G.hasUnion= TRUE; 
	G.tFile= G.unionFile; 
	G.codeLineN= G.lineNum; LINE_DIR();
	break;
      case LINE_PSEUDO_TOK:
	G.lineState= 0; G.lastSS= YY_START; BEGIN LINE;
	break;
      case TOK(OPTION):
	BEGIN OPTIONS;
	break;
    }
    return tok;
  }
}


/*			    SOURCE CONTROL.				*/

static VOID 
eofError(lineN, delimiter)
  Count lineN;
  ConstString delimiter;
{
  error("unexpected EOF in construct starting at %d.  Expecting %s",
	lineN, delimiter);
  VOID_RET();
}

unsigned 
errorLineN()
{
  return (hasLookahead()) ? G.parseLineN1 : G.parseLineN0;
}

ConstString 
errorSrc()
{
  return G.srcName; 
}

/* Entered when a %line directive sets a source file name. */
static VOID 
setSrcFile()
{
  Count n= yyleng - 2;
  yytext[yyleng - 1]= '\0';
  /* Used for #line directives. */
  G.lineSrcName= getIDString(getID(&yytext[1], n));

  n= translateCEscapes(&yytext[1], yyleng - 2); 
  G.srcName= (n = yyleng - 2)
             ? G.lineSrcName
             : getIDString(getID(&yytext[1], n));
}



/*			    DEFINITIONS.				*/

static VOID 
defBegin()
{
  if (G.secN == 2) {
    if (G.hasRule) {
      error("can't have a definitions section after the first rule");
    }
    if (!G.defsFile[1] && !(G.defsFile[1]= tmpfile())) {
      fatal("can't open temporary file");
    }
  }
  BEGIN DEF; 
  G.tFile= G.defsFile[G.secN - 1]; 
  LINE_DIR(); 
}


/*			   OPTIONS TEXT ROUTINE				*/

/* Contains text of option specified in program. */
static AREA(optionsText, Char, 4, UINT2_MAX);
#define OPTIONS_TEXT(i)	A_ACCESS(optionsText, Char, i)

/* Add the options word in yytext to current optionsText followed by a 
 * space.
 */
static VOID
addOptionsText()
{
  ConstString p;
  Index i;
  for (p= yytext; *p != '\0'; p++) {
    i= A_NEXT(optionsText);
    OPTIONS_TEXT(i)= *p;
  }
  i= A_NEXT(optionsText); OPTIONS_TEXT(i)= ' ';
}

/* Return accumulated options text and reset area index.  Note that the
 * returned text should be completely processed before the next call
 * to addOptionsText().
 */
static ConstString
getOptionsText()
{
  Index i= A_NEXT(optionsText);
  OPTIONS_TEXT(i)= '\0';
  A_CUT(optionsText, 0);
  return &OPTIONS_TEXT(0);
}

#undef OPTIONS_TEXT


/*			INITIALIZATION.					*/

int 
yywrap()
{
  if (G.argn >= G.argc) {
    return 1;
  }
  else {
    G.srcName= G.lineSrcName= G.argv[G.argn++];
    G.lineNum= 1;
    if (strcmp(G.srcName, "-") == 0) {
      clearerr(stdin); yyin= stdin; G.srcName= G.lineSrcName= "<stdin>";
    }
    else {
      yyin= fopen(G.srcName, "r");
      if (!yyin) fatal("could not open source file `%s'", G.srcName);
    }
    return 0;
  }
}

VOID 
initScan(argc, nonOptArgN, argv)
  int argc;
  int nonOptArgN;
  ConstString argv[];
{
  initError(NULL, NIL, NULL, errorSrc, errorLineN, NULL);
  G.srcName= G.lineSrcName= "<stdin>"; G.secN= G.lineNum= 1;
  G.argc= argc; G.argn= nonOptArgN; G.argv= argv;
  yywrap(); /* Setup first file if any; otherwise default to stdin. */
  G.actsFile= tmpfile();
  G.defsFile[0]= tmpfile();
  G.unionFile= tmpfile();
  if (!G.actsFile || ! G.defsFile[0] || !G.unionFile)
    error("could not open temporary file");
  initPredefinedIDs();
  INIT_TOK_BUF();
  VOID_RET();
}



/*			OUTPUT ROUTINES.				*/

Count 
lineDir(outFile, lineN)
  FILE *outFile;
  Index lineN;
{
  Count nLines= 0;
  if (G.tFile && optionsP->doLine) {
    fprintf(outFile, "#line %d \"%s\"\n", lineN, G.lineSrcName);
    nLines= 1;
  }
  return nLines;
}

/* Copy tmpFile to outFile after rewinding it.  Return count of 
 * # newlines output.  
 */
static Count 
outTmpFile(outFile, tmpFile)
  FILE *outFile;
  FILE *tmpFile;
{
  int c;
  Count nLines= 0;
  rewind(tmpFile);
  for (c= getc(tmpFile); c != EOF; c= getc(tmpFile)) {
    if (c == '\n') nLines++;
    putc(c, outFile);
  }
  return nLines;
}

Count 
outActCases(outFile)
  FILE *outFile;
{
  Index i;
  CONST Count nActs= A_NENTRIES(acts);
  Index lineN= 0;
  rewind(G.actsFile);
  for (i= 0; i < nActs; i++) {
    CONST Count actNLines= ACTS(i).nLines;
    Count nLines= 0;
    fprintf(outFile, "\tcase %d:\n", ACTS(i).reductN); lineN++;
    lineN+= outAttribActPrefix(outFile, ACTS(i).reductN);
    while (nLines < actNLines) {
      int c= getc(G.actsFile);
      assert(c != EOF);
      putc(c, outFile);
      if (c == '\n') nLines++;
    }
    lineN+= nLines;
    lineN+= outAttribActSuffix(outFile, ACTS(i).reductN);
    fputs("\tbreak;\n", outFile); lineN++;
  }
  return lineN;
}

/* Print out action actN to outFile.  There should not be any I/O on
 * actsFile between calls to this routine.  For efficiency, successive
 * calls should use successive actN s. 
 */
VOID
printAct(outFile, actN)
  FILE *outFile;
  Index actN;
{
  static Index readyActN= NIL;	/* Action ready to print. */
  if (readyActN > actN || readyActN == NIL) {
    rewind(G.actsFile); readyActN= 0;    
  }
  for (; readyActN < actN; readyActN++) {
    CONST Count actNLines= ACTS(readyActN).nLines;
    Count nLines= 0;
    while (nLines < actNLines) {
      int c= getc(G.actsFile);
      assert(c != EOF);
      if (c == '\n') nLines++;
    }
  }
  { CONST Count actNLines= ACTS(actN).nLines;
    Count nLines= 0;
    while (nLines < actNLines) {
      int c= getc(G.actsFile);
      assert(c != EOF);
      putc(c, outFile);
      if (c == '\n') nLines++;
    }
  }
  readyActN++;
  VOID_RET();
}

Count 
outDefs(outFile)
  FILE *outFile;
{
  return outTmpFile(outFile, G.defsFile[0]);
}

Count
outLocals(outFile)
  FILE *outFile;
{
  return (G.defsFile[1]) ? outTmpFile(outFile, G.defsFile[1]) : 0;
}

Count 
outYYSTYPE(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  if (G.hasUnion || hasTermIDVars()) {
    fputs("typedef union {\n", outFile); nLines++;
    nLines+= outTmpFile(outFile, G.unionFile);
    nLines+= outAttribsTermTypes(outFile);
    fputs("} YYSTYPE;\n\n", outFile); nLines+= 2;
  }
  else if (!G.isTyped) {
    fputs("#ifndef YYSTYPE\n#define YYSTYPE int\n#endif\n", outFile); 
    nLines+= 3;
  }
  else {
    /* User should have defined YYSTYPE directly or via include file. */
  }
  return nLines;
}

Count 
outSec3(outFile)
  FILE *outFile;
{
  CONST Index sec3StartLineN= G.lineNum;
  if (G.secN == 3) {
    G.tFile= outFile;
    LINE_DIR(); yylex(); 
    OUTS("\n"); G.lineNum++;
    G.tFile= NULL;
  }
  return G.lineNum - sec3StartLineN;
}

Count 
outScanParams(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  fprintf(outFile, "#define YY_HAS_LOC %d\n", G.hasLoc); nLines++;
  fprintf(outFile, "#define YY_IS_TYPED %d\n", G.isTyped); nLines++;
  fprintf(outFile, "#define YY_SRC_NAME \"%s\"\n", G.srcName); nLines++;
  return nLines;
}

#ifdef TEST_SCAN

int main(int argc, char *argv[]);

int 
main(argc, argv)
  int argc;
  char *argv[];
{
  int t;
  initScan(argc > 1 ? argv[1] : NULL, TRUE);
  t= scan();
  while (t != 0 && !TOK_EQ(t, END)) {
    printf("yytext lookahead= %s; Token= %s", yytext, (ConstString) t);
    if (TOK_EQ(t, ID) || TOK_EQ(t, XID))
      printf("; ID number= %d; ID string= %s", 
	     yylval.id, getIDString(yylval.id));
    else if (TOK_EQ(t, LIT))
      printf("; Literal id= %d; Literal value= %d", 
	     yylval.lit.id, yylval.lit.val);
    else if (TOK_EQ(t, NUM))
      printf("; Number= %d", yylval.num);
    else if (TOK_EQ(t, TYPE))
      printf("; Type id= %d; Type string= %s", 
	     yylval.type, getIDString(yylval.type));
    else if (TOK_EQ(t, VAR_NUM))
      printf("; Var number= %d; Var type= %d", 
	     yylval.numVar.offset, yylval.numVar.type);
    else if (TOK_EQ(t, LHS_VAR))
      printf("; Var type= %d", yylval.type);
    else if (TOK_EQ(t, ID_VAR))
      printf("; Var id= %d; Var name= %s", 
	     yylval.id, getIDString(yylval.id));
    else if (TOK_EQ(t, TEXT))
      printf("; Text= %s", yylval.text);
    else if (TOK_EQ(t, SHORT_TEXT))
      printf("; Text= %s", yylval.shortText);
    printf(".\n");
    t= scan();
  }
  printf("\nDEFINITIONS:\n");
  outDefs(stdout); 
  printf("\nUNION:\n");
  outUnion(stdout); 
  printf("\nACTIONS:\n");
  outActCases(stdout); 
  printf("\nSECTION 3:\n");
  outSec3(stdout);
  return 0;
}

#endif
