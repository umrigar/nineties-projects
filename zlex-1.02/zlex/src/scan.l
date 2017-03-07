/*

File:	 scan.l
Purpose: Zlex scanner.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/* -*-c-*- */
/*

Allows whitespace within patterns when optionsP->wsInPatterns.  The
pattern is terminated by seeing the start of an action or by a newline
(not within parentheses).

When whitespace is allowed in patterns, the following restrictions
facilitate recognition of a C-brace-action:

--  {lex_macro} uses are regarded as indivisible tokens and cannot 
    contain whitespace.

--  There cannot be any whitespace between the '{' and the first 
    digit specifying the start of a repetition range.

When whitespace is allowed, the most severe restriction (which may break
existing lex programs) is that if a action follows a pattern on the same
line and is not enclosed in braces, it will be considered as part of the
pattern! 

Actions are of three types:

--  Indented Actions:  A sequence of indented lines.

--  Decorated Actions: A sequence of lines enclosed within ^"%{" and
    ^"%}".

--  C Brace Actions: A sequence of lines within (balanced) braces.

Brace actions are C-interpreted --- i.e. braces are balanced except
when within a C-comment, C-string or C-character-constant.  The other
two actions are uninterpreted --- once within those actions, there is
no recognition of any C-construct.

A temporary file stores the "actions".  Each action is identified
using an index referred to as a ActIndex.  The global definitions
section from section 1 has ActIndex 0.  The local definitions section
(before the first pattern) from section 2 has ActIndex 1.  Each
subsequent user action with number (starting at 0) actNum has ActIndex
actNum + 2.

In section 2, a newline is considered to be a action by the local
yylex().  For the purpose of assigning action indexes, a sequence of
actions which have no intervening patterns are considered to be a
single action (tho' the local yylex() delivers individual ACT_TOKs for
each action in the sequence).  Each such action sequence is terminated
by a section 2 pattern or by a "%%" marker or EOF.

A top-level scan() wrapper around the local yylex() routine, is used
for token massaging using upto 2-token lookahead.

*/

cChar		([^'\\\n]|\\.)		/* C char-constant character. */
id		([a-zA-Z_][a-zA-Z0-9_]*)/* Identifier. */
num		([0-9]+)		/* Integer. */
octNum		([0-7]{1,3})		/* Octal number. */
hexNum		([0-9a-fA-F]+)		/* Hexadecimal number. */
macro		(\{{id}\})		/* Use of a lex macro. */
sChar		([^"\\\n]|\\.)		/* C string character. */
ws		[ \t\f\v]		/* Whitespace within line. */
xClass		("-"|"]"|"^")		/* Special char-class start chars. */
option		[-=_[:alnum:]]+		/* Option word. */

%s DEFS		/* Within the definitions section. */
%s OPTIONS	/* In a %option line. */
%s RANGE	/* Within a {num, num} range. */
%s SS_DEF	/* Defining start-states. */
%s SS_USE	/* Using start-states. */

%x BRACE	/* Within balanced braces C-code section. */
%x CLASS	/* Within a lex character class. */
%x COLON	/* With [: :] */
%x COMMENT	/* Within slash-star comment. */
%x CSTR		/* With a C-string. */
%x DECORATED	/* Within ^"%{" and ^"%}" code-section. */
%x INDENT	/* Within section of indented code. */
%x LINE		/* Within %line directive line. */
%x SEC3		/* In section 3: merely copy source to G.outFile. */
%x SS_START	/* A '^' followed a '>' terminating a start-state list. */
%x STR		/* Within a lex pattern string. */
%x TABLE	/* In lex table declaration. */
%x X_CLASS	/* At the very beginning of a character-class. */

%{

#include "files.h"
#include "scan.h"

#include "area.h"
#include "error.h"
#include "ids.h"
#include "memalloc.h"
#include "strspace.h"
#include "textid.h"
#include "zutil.h"

#include <ctype.h>

typedef struct {
  Area optionsText;	/* Accumulate text of options here. */
  FILE *outFile;	/* FILE pointer for output file in section 3. */
  FILE *tmpFile;	/* FILE pointer for temporary file. */

  /* lineSrcName may contain escape sequences; srcName will not. */
  ConstString lineSrcName; /* Name of source file for #line directives. */
  ConstString srcName;	/* Name of source file for error messages. */

  ConstString *argv;	/* Command-line arguments. */
  Count2 argc;		/* Number of command line arguments. */
  Count2 argn;		/* Number of processed command line arguments. */
  Index lineN;		/* Current line # in source file. */
  Index delimLineN;	/* Line # where paren/brace nesting started. */	
  Index2 lineState;	/* 0: Seen #line; 1: Seen NN; 2: Seen file name. */
  Count1 depth;		/* Depth of paren/brace nesting. */
  Boolean doCopy;	/* TRUE if comments should be copied. */
  Index1 secN;		/* Section #: 1, 2 or 3. */
  Index1 retSS;		/* Start-state to return to after comment or %line. */
} Globals;

static Globals *globalsP;
#define G (*globalsP)
  
#define END_TOK YY_NULL	/* Return at start of section 3. */

#define YY_DECL	static int yylex()

#define LAST_STATE()	(G.secN == 2 ? INITIAL : DEFS)

#define COPY()	do { fputs(yytext, G.tmpFile); } while (0)
#define COPY_NL()  do { fputs(yytext, G.tmpFile); } while (0)
#define XCOPY() if (G.doCopy) COPY()
#define XCOPY_NL() if (G.doCopy) COPY_NL()

static Int cEscChar PROTO((int c));
static Int lexDir PROTO((ConstString dir));
static VOID endAct PROTO((VOID_ARGS));
static Index nextAct PROTO((VOID_ARGS));
static Boolean action PROTO((ConstString text));
static VOID eofError PROTO((Count lineN, ConstString delimiter));
static VOID setSrcFile PROTO((VOID_ARGS));
static VOID addOptionsText PROTO((VOID_ARGS));
static ConstString getOptionsText PROTO((VOID_ARGS));
static VOID initTmpFile PROTO((VOID_ARGS));
static VOID terminateTmpFile PROTO((VOID_ARGS));
#if DO_TRACE
static VOID dumpActs PROTO((FILE *dumpFile));
#endif

#if TEST_SCAN
static Char ctok[2]= { '\0', '\0' };	/* Return char-toks as 1 char. str. */
#define CTOK(x) ((int)(ctok[0]= x , &ctok[0]))
#define TOK(x)		((int) #x)
#define TOK_EQ(unknownTok, litTok)					\
  ((unknownTok) != 0 							\
   && strcmp((ConstString) (unknownTok), #litTok) == 0)
#define CTOK_EQ(unknownTok, litTok)					\
  ( (unknownTok) != 0 && ((ConstString)(unknownTok))[0] == litTok	\
   && ((ConstString)(unknownTok))[1] == '\0' )
typedef struct {     	/* Just those we need to compile stand-alone. */
  Boolean lexCompat;
  Boolean lineDir;
  Boolean wsInPatterns;
} Options;
static Options options;
static Options *optionsP= &options;
typedef union {
  Int ch;
  ConstString opt;
  Index lineN;
  Index id;
  Index actN;
  Int num;
} YYSTYPE;
static YYSTYPE yylval;

#else /* !TEST_SCAN */

#include "options.h"
#include "patterns.h"

#include "parse.h"

#define TOK(x)		x
#define CTOK(x)		x
#define TOK_EQ(unknownTok, litTok)	(unknownTok == litTok)
#define CTOK_EQ(unknownTok, litTok)	(unknownTok == litTok)

#endif

%}

%%
/*                       INCLUSIVE START STATES. 			*/
<DEFS>^{id}	{ yylval.id= getID(yytext, yyleng); return TOK(ID_TOK); }
<DEFS>^"%"{id}	{ Int t= lexDir(&yytext[1]); if (t >= 0) return t; }

<OPTIONS>{option} { addOptionsText(); }
<OPTIONS>\n	{ G.lineN++; BEGIN DEFS; 
                  yylval.text= getOptionsText(); return TOK(OPTION_VAL_TOK); 
                }
<RANGE>{num}	{ yylval.num= atoi(yytext); return TOK(NUM_TOK); }
<RANGE>","	{ return CTOK(','); }
<RANGE>"}"	{ BEGIN LAST_STATE(); return CTOK('}'); }

<SS_DEF>{id}	{ yylval.id= getID(yytext, yyleng); return TOK(SS_ID_TOK); }
<SS_DEF>\n	{ G.lineN++; BEGIN DEFS; }

<SS_USE>{id}	{ yylval.id= getID(yytext, yyleng); return TOK(SS_ID_TOK); }
<SS_USE>","	{ return CTOK(','); }
<SS_USE>">"	{ BEGIN INITIAL; return CTOK('>'); }
<SS_USE>">"/"^"	{ BEGIN SS_START; return CTOK('>'); }

/*		PATTERNS FOR ALL INCLUSIVE START STATES.		*/

\n		{ G.lineN++;
		  if (!optionsP->wsInPatterns && G.depth > 0) {
		    error("Missing `)'."); G.depth= 0; BEGIN LAST_STATE();
		  }
		  if ((G.depth == 0 || !optionsP->wsInPatterns) 
		      && G.secN == 2) { 
                    if (action("")) return TOK(ACT_TOK);
                  }
		}	

^{ws}*\n	{ G.lineN++;  /* Ignore blank lines. */ }
^{ws}+		{ if (!optionsP->wsInPatterns || G.depth == 0) {
		    BEGIN INDENT; if (action(yytext)) return TOK(ACT_TOK);
		  } 
		}
{ws}*"/*"	{ if (optionsP->wsInPatterns || YY_START == OPTIONS) {
                    G.retSS= YY_START; BEGIN COMMENT; 
		  }
		  else {
		    G.retSS= INDENT; G.depth= 0; G.doCopy= 1; BEGIN COMMENT; 
		    if (action(yytext)) return TOK(ACT_TOK);
		  }
		}
{ws}+		{ if (G.secN == 2 && !optionsP->wsInPatterns) {
		    if (G.depth > 0) { error("Missing `)'."); G.depth= 0; }
		    BEGIN INDENT; if (action(yytext)) return TOK(ACT_TOK);
		  }
		}
{ws}+/[|{\n]	/* Ignore ws right before action. */

"{"		{ if (G.depth) error("Unbalanced parentheses.");
		  BEGIN BRACE; G.delimLineN= G.lineN;  
		  G.depth= 1; if (action(yytext)) return TOK(ACT_TOK);
		}

^"%{".*		{ G.delimLineN= G.lineN; BEGIN DECORATED; 
		  G.depth= 0; if (action(&yytext[2])) return TOK(ACT_TOK);
		}

^"%%"		{ G.secN++; endAct(); BEGIN (G.secN == 2) ? INITIAL : SEC3; 
		  return (G.secN == 3) ? TOK(END_TOK) : TOK(SEC_TOK); 
                }
^"%line" 	{ if (optionsP->lexCompat) { REJECT; }
                  else { G.retSS= YY_START; BEGIN LINE; }
                }
"<<EOF>>"	{ return TOK(EOF_PAT_TOK); }
^"<"		{ if (G.secN == 2) BEGIN SS_USE; 
                  yylval.lineN= G.lineN; return CTOK('<'); 
                }
^"^"		{ return CTOK('^'); }
"("		{ G.depth++; BEGIN INITIAL; return TOK('('); }
")"		{ if (G.depth == 0) { 
		    warn("Unbalanced `)'"); 
		  }
		  else {
		    G.depth--;
		  }
		  if (G.depth == 0) BEGIN LAST_STATE();
		  return CTOK(')');
		}
"["		{ BEGIN CLASS; return CTOK('['); }
"["/{xClass}	{ BEGIN X_CLASS; return CTOK('['); }
"("		{ G.depth= 1; return CTOK('('); }
{macro}         { yytext[yyleng - 1]= '\0'; 
                  yylval.id= getID(&yytext[1], yyleng - 2);
		  return TOK(MACRO_TOK);
                }
\{/[0-9]        { BEGIN RANGE; return CTOK('{'); }  
\\{octNum}	{ sscanf(&yytext[1], "%o", &yylval.ch); return TOK(CHAR_TOK); } 
\\x{hexNum}	{ sscanf(&yytext[2], "%x", &yylval.ch); return TOK(CHAR_TOK); } 
\"		{ BEGIN STR; }
\\.	        { yylval.ch= cEscChar((UChar)yytext[1]); return TOK(CHAR_TOK); }
\\/\n		{ error("Invalid escaped newline."); }
"*"		{ return CTOK('*'); }
"+"             { yylval.lineN= G.lineN; return CTOK('+'); }
"?"             { return CTOK('?'); }
"|"             { return CTOK('|'); }
"."             { return CTOK('.'); }
"$"		{ return CTOK('$'); }
"/"		{ return CTOK('/'); }
.               { yylval.ch= (UChar)yytext[0]; return TOK(CHAR_TOK); }
<<EOF>>		{ if (G.depth > 0) eofError(G.delimLineN, ")"); }

/*			EXCLUSIVE START STATES.				*/

<BRACE>\n		{ G.lineN++; COPY_NL(); }
<BRACE>"}"		{ COPY(); if (--G.depth == 0) BEGIN INDENT; }
<BRACE>"{"		{ G.depth++; COPY(); }
<BRACE>\"		{ BEGIN CSTR; COPY(); }
<BRACE>\'{cChar}*\'	{ COPY(); }
<BRACE>\'{cChar}*	{ warn("Unterminated C character-constant."); }
<BRACE>[^{}/\n"']+	|
<BRACE>"/"		{ COPY(); }
<BRACE>"/*"		{ G.retSS= BRACE; COPY(); 
	                  G.doCopy= TRUE; BEGIN COMMENT; 
			}
<BRACE><<EOF>>		{ eofError(G.delimLineN, "}"); }

<COLON>":]"		{ BEGIN CLASS; return TOK(COLON_END_TOK); }
<COLON>{id}		{ yylval.id= getID(yytext, yyleng); 
                          return TOK(ID_TOK); 
			}
<COLON>.|\n		{ error("Invalid character within [: ... :]."); 
                          BEGIN LAST_STATE();
                        }
<COLON><<EOF>>		{ error("EOF within [: ... :]."); }

<COMMENT>\n		{ G.lineN++; XCOPY_NL(); }
<COMMENT>\*+/[^/]	|
<COMMENT>\*		|
<COMMENT>[^\n*]+	{ XCOPY(); }
<COMMENT>"*/"		{ XCOPY(); BEGIN G.retSS; }
<COMMENT><<EOF>>	{ eofError(G.delimLineN, "*/"); }

<CSTR>\n		{ G.lineN++; COPY_NL(); }
<CSTR>{sChar}*\"	{ COPY(); BEGIN BRACE; }
<CSTR>{sChar}*\\$	{ COPY(); }
<CSTR>{sChar}*$		{ warn("Unterminated C-string."); 
			  COPY(); BEGIN BRACE; 
			}

<DECORATED>\n		{ G.lineN++; COPY_NL(); }
<DECORATED>^"%}"	{ BEGIN INDENT; }
<DECORATED>^[^\n%].*	|
<DECORATED>^\%$		|
<DECORATED>^\%[^\n}].*	{ COPY(); }
<DECORATED><<EOF>>	{ eofError(G.delimLineN, "%}"); }

<INDENT>\n		{ G.lineN++; COPY_NL(); 
			  G.depth= 0; BEGIN LAST_STATE(); 
			}
<INDENT>\n/[ \t\n]	{ G.lineN++; COPY_NL(); }
<INDENT>.+		{ COPY(); }

/* Seen %line directive. */
<LINE>\n		{ G.lineN++; BEGIN G.retSS; }
<LINE>{ws}+		
<LINE>{num}		{ if (G.lineState) {
                            error("Unexpected number %s in %%line directive.",
                                  yytext);
                          }
                          else {
                            G.lineN= atoi(yytext) - 1;
                          }
                        }
<LINE>\"{sChar}+\"      { if (G.lineState < 2) {
                            setSrcFile(); G.lineState= 2;
                          }
                          else {
                            error("Duplicate file name %s in %%line directive.",
			          yytext);
                          }
                        }
<LINE>\"{sChar}*$       { error(
                            "Unterminated file name in %%line directive."); 
                        }

<SEC3>.*\n		{ fputs(yytext, G.outFile); G.lineN++; }
<SEC3>.+		{ fputs(yytext, G.outFile); }

<CLASS>\n		{ error("Unterminated character class."); 
			  G.lineN++; BEGIN LAST_STATE(); 
			}
<STR>\n			{ error("Unterminated pattern string."); 
			  G.lineN++; BEGIN LAST_STATE(); 
			}
<CLASS,STR>\\{octNum}	{ sscanf(&yytext[1], "%o", &yylval.ch);
			  return TOK(CHAR_TOK);
			} 
<CLASS,STR>\\x{hexNum}	{ sscanf(&yytext[2], "%x", &yylval.ch);
			  return TOK(CHAR_TOK);
			} 
<CLASS,STR>\\.		{ yylval.ch= cEscChar((UChar)yytext[1]); 
			  return TOK(CHAR_TOK); 
			}
<CLASS>"]"		{ BEGIN LAST_STATE(); return CTOK(']'); }
<CLASS>"[:"		{ BEGIN COLON; return TOK(COLON_BEGIN_TOK); }
<STR>\"			{ BEGIN LAST_STATE(); }
<CLASS>"-"              { return CTOK('-'); }
<CLASS>"-"/"]"		|
<CLASS,STR>.		{ yylval.ch= (UChar)yytext[0]; return TOK(CHAR_TOK); }

<TABLE>\n		{ G.lineN++; }
<TABLE>^"%"[tT].+	{ BEGIN DEFS; }			
<TABLE>.+

/*	   EXCLUSIVE START-STATES WITH GUARANTEED LOOKAHEAD.		*/

<SS_START>"^"		{ BEGIN INITIAL; return CTOK('^'); }
<SS_START>.|\n		{ INTERNAL_ERROR(); }

<X_CLASS>"^"/"]"|"-"	{ return CTOK('^'); }
<X_CLASS>"^"		{ BEGIN CLASS; return CTOK('^'); }
<X_CLASS>"]"		{ BEGIN CLASS; yylval.ch= ']'; return TOK(CHAR_TOK); }
<X_CLASS>"-"		{ BEGIN CLASS; yylval.ch= '-'; return TOK(CHAR_TOK); }
<X_CLASS>.|\n		{ INTERNAL_ERROR(); }

%%

/*			TOP-LEVEL SCANNER ROUTINE.			*/

/* Buffer upto 2 tokens and filter tokens from yylex() as follows: 
 * The sequence '|' ACT_TOK converted to NEXT_ACT_TOK (no endAct()).
 * '$' converted to CHAR_TOK if not followed by ACT_TOK or by '|' ACT_TOK.
 * A sequence of consecutive ACT_TOKs replaced by a single one (the first).
 * Calls endAct() after a sequence of ACT_TOKs completed.
 * We buffer only a single semantic value of the last token read from
 * yylex() only when we convert a '$' to a CHAR_TOK.  When we do so,
 * we set a flags and restore yylval from the semantic value buffer
 * at the next entry to this routine.
 * Possibilities of tokens stored in buffer are:
 *    | X  or X where X may have a semantic value.
 */
int 
scan()
{
#define IS_EMPTY()  (bufIndex0 == bufIndex1)
#define IS_FULL()   ((bufIndex1 + 1) % N1 == bufIndex0)
#define READ_TOK(tok)							\
  do {									\
    assert(bufIndex0 <= N);						\
    assert(bufIndex1 <= N);						\
    if (IS_EMPTY()) { /* Buffer is empty. */		\
      tok= yylex();							\
    }									\
    else { /* Get from buffer. Increment read-pointer circularly. */	\
      tok= tokBuf[bufIndex0]; bufIndex0= (bufIndex0 + 1) % N1;		\
    }									\
  } while (0)	
#define SAVE_TOK(tok)							\
  do {									\
    assert(bufIndex0 <= N);						\
    assert(bufIndex1 <= N);						\
    assert(!IS_FULL());							\
    tokBuf[bufIndex1]= tok; bufIndex1= (bufIndex1 + 1) % N1;		\
  } while (0)

  enum {
    N= 2,			/* Max. # of tokens buffered. */
    N1= N + 1			/* 1 extra for circular buffer. */
  };
  static Boolean lastIsAct	/* True if last token delivered was ACT. */
    = FALSE;
  static Int tokBuf[N1];	/* Circular token buffer: 1 extra entry. */
  static Index bufIndex0= 0;	/* Points to next read index for tokBuf[]. */
  static int bufIndex1= 0;	/* Points to next write index for tokBuf[]. */
  static YYSTYPE yylvalBuf;	/* Buffer for single semantic value. */
  static Boolean isLValBuf=	/* True if yylvalBuf contains a value. */
    FALSE;
  int tok;			/* Token to be delivered. */
  if (isLValBuf) { yylval= yylvalBuf; isLValBuf= FALSE; }
  do {
    READ_TOK(tok);
    if (lastIsAct && tok != TOK(ACT_TOK)) { 
      endAct(); lastIsAct= FALSE; 
    }
    if (CTOK_EQ(tok, '|')) {
      int nextTok;
      READ_TOK(nextTok);
      if (TOK_EQ(nextTok, ACT_TOK) && G.secN == 2) {
        tok= TOK(NEXT_ACT_TOK); 
      }
      else {
	SAVE_TOK(nextTok);
      }
    }
    else if (CTOK_EQ(tok, '$')) {
      int nextTok0= yylex();
      assert(IS_EMPTY());
      if (CTOK_EQ(nextTok0, '|')) {		/* '$' '|' */
	int nextTok1= yylex();
	if (TOK_EQ(nextTok1, ACT_TOK)) {	/* '$' '|' ACT_TOK */
	  SAVE_TOK(TOK(NEXT_ACT_TOK));		/* '$' unchanged. */
	}
	else {					/* '$' '|' ^ACT_TOK */
	  assert(isLValBuf == FALSE);
	  yylvalBuf= yylval; isLValBuf= TRUE;
	  SAVE_TOK(nextTok0); SAVE_TOK(nextTok1);
	  tok= TOK(CHAR_TOK); yylval.ch= '$';	/* '$' transformed. */
	}
      }
      else if (TOK_EQ(nextTok0, ACT_TOK)) {	/* '$' ACT_TOK */
	SAVE_TOK(nextTok0);			/* '$' unchanged. */
      }
      else {					/* '$' ^{'|', ACT_TOK} */
	assert(isLValBuf == FALSE);
	yylvalBuf= yylval; isLValBuf= TRUE;
	SAVE_TOK(nextTok0);	
	tok= TOK(CHAR_TOK); yylval.ch= '$';	/* '$' transformed. */
      }
    }
  } while (lastIsAct && TOK_EQ(tok, ACT_TOK));
  if (tok == TOK(ACT_TOK)) lastIsAct= TRUE;
  return tok;
#undef IS_EMPTY
#undef IS_FULL
#undef READ_TOK
#undef SAVE_TOK
}


/*			     LEX DIRECTIVES.				*/

enum { /* Pseudo-token values must be < 0. */
  LINE_PSEUDO_TOK= -1,
  TABLE_PSEUDO_TOK= -2
};

/* Recognize the lex-directive dir and return a corresponding token.
 * Set the next start-state if appropriate.
 */
static Int 
lexDir(dir)
  ConstString dir;
{
  
  if (dir[1] == '\0') {
    switch (toupper(dir[0])) {
      case 'S':
	BEGIN SS_DEF;
	return TOK(START_TOK); 
      case 'X':
	BEGIN SS_DEF;
	return TOK(STARTX_TOK); 
      case 'T':
	BEGIN TABLE;
	return TABLE_PSEUDO_TOK;
      case 'E': case 'K': case 'N': case 'O':
	BEGIN OPTIONS;
        return TOK(LEX_DIR_TOK);
    }
  }
  if (strncmp(dir, "option", 6) == 0) {	/* Allow "option*". */
    BEGIN OPTIONS; return TOK(OPTION_TOK);
  }
  else if (strcmp(dir, "array") == 0) {
    BEGIN OPTIONS; return TOK(ARRAY_OPTION_TOK); 
  }
  else if (strcmp(dir, "pointer") == 0 || strcmp(dir, "ptr") == 0) {
    BEGIN OPTIONS; return TOK(POINTER_OPTION_TOK); 
  }
  else if ((strcmp(dir, "line") == 0)) {
    G.retSS= YY_START; BEGIN LINE; return LINE_PSEUDO_TOK; 
  }
  else {
    error("Invalid zlex directive %s.", dir);
    BEGIN OPTIONS;
    return TOK(LEX_DIR_TOK);
  }
}


/*			TRANSLATE C-ESCAPES.				*/

/* Define values for escape characters which are in ANSI, but not K&R. */
#ifndef BEL
#define BEL		7
#endif

#ifndef VT
#define VT		11
#endif

/* Return the translation of the c-escape-char c. */
static int 
cEscChar(c)
  int c;
{
  switch (c) {
    case 'a':
#if __STDC__
      return '\a';
#else
      return BEL;		/* Ok for ASCII; bad elsewhere. */
#endif
    case 'b':
      return '\b';
    case 'f':
      return '\f';
    case 'n':
      return '\n';
    case 'r':
      return '\r';
    case 't':
      return '\t';
    case 'v':
#if __STDC__
      return '\v';
#else
      return VT;
#endif
    default:
      return c;
  }
}

/*			  ERROR ROUTINES				*/


unsigned 
lineN() 
{
  return G.lineN;
}


static VOID 
eofError(lineN, delimiter)
  Count lineN;
  ConstString delimiter;
{
  errorAtLine("%d: Unexpected EOF.  Expected `%s'.", lineN, delimiter);
}

ConstString
srcName()
{
  return G.srcName; 
}

/* Entered when a %line directive sets a source file name. */
static VOID 
setSrcFile()
{
  Count n= yyleng - 2;
  G.lineSrcName= internText(&yytext[1], n);  /* Used for #line directives. */
  n= translateCEscapes(&yytext[1], yyleng - 2); 
  G.srcName= (n = yyleng - 2)
             ? G.lineSrcName
             : internText(&yytext[1], n);
}


/*			   OPTIONS TEXT ROUTINE				*/

#define OPTIONS_TEXT(i)	A_ACCESS(G.optionsText, Char, i)

/* Add the options word in yytext to current optionsText followed by a 
 * space. 
 */
static VOID
addOptionsText()
{
  ConstString p;
  Index i;
  for (p= yytext; *p != '\0'; p++) {
    i= A_NEXT(G.optionsText);
    OPTIONS_TEXT(i)= *p;
  }
  i= A_NEXT(G.optionsText); OPTIONS_TEXT(i)= ' ';
}

/* Return accumulated options text and reset area index.  Note that the
 * returned text should be completely processed before the next call
 * to addOptionsText().
 */
static ConstString
getOptionsText()
{
  Index i= A_NEXT(G.optionsText);
  OPTIONS_TEXT(i)= '\0';
  A_CUT(G.optionsText, 0);
  return &OPTIONS_TEXT(0);
}

#undef OPTIONS_TEXT


/*		  TEMPORARY FILE & ACTION ROUTINES			*/

/*

Temporary file has following sections:

1 section for declarations in lex file.
1 section for declarations in yylex() function.
n sections for user actions, where n is the number of non-`|' user actions
  in section 2.

for a total of (n + 2) sections.
Some of the sections may be empty.

*/

static VOID 
initTmpFile() 
{
  G.tmpFile= tmpfile();
  if (!G.tmpFile) fatal("Could not open temporary file.");
  VOID_RET();
}

static VOID
terminateTmpFile()
{
  if (fclose(G.tmpFile) == EOF) {
    fatal("Could not close temporary file.");
  }
  VOID_RET();
}

Count 
lineDir(outFile, lineN, fName)
  FILE *outFile;
  Count lineN;
  ConstString fName;
{
  if (optionsP->lineDir) {
    fprintf(outFile, "#line %d \"%s\"\n", lineN, fName);
    return 1;
  }
  else 
    return 0;
}

typedef struct {
  long endOffset;	/* ftell() returned offset of action end. */
} Act;
static AREA (acts, Act, 8, UINT2_MAX);
#define ACTS(a) A_ACCESS(acts, Act, a)

Count 
nSrcActs()
{
  return A_NENTRIES(acts);
}

static VOID 
endAct()
{
  Index a= A_NEXT(acts);
  ACTS(a).endOffset= ftell(G.tmpFile);
}

static Index 
nextAct()
{
  return A_NENTRIES(acts);
}

static Boolean 
action(text)
  ConstString text;
{
  lineDir(G.tmpFile, G.lineN, G.lineSrcName);
  fputs(text, G.tmpFile);
  yylval.actN= nextAct();
  return (G.secN == 2);
}

/* Output source action n to outFile: 0: section 1 definitions; 1: yylex
 * local declarations; 2 ... : scanner actions.  Assumes that outDefs()
 * has been previously called (rewinds G.tmpFile).
 */
Count
outSrcAction(outFile, srcActN)
  FILE *outFile;
  Index srcActN;
{
  long startOffset= (srcActN > 0) ? ACTS(srcActN - 1).endOffset : 0;
  long endOffset= ACTS(srcActN).endOffset;
  Count nLines= 0;
  long i;
  assert(srcActN < A_NENTRIES(acts));
  fseek(G.tmpFile, startOffset, 0); /* Some systems don't know SEEK_SET */
  for (i= startOffset; i < endOffset; i++) {
    CONST int c= getc(G.tmpFile);
    assert(c != EOF);
    if (c == '\n') nLines++;
    fputc(c, outFile);
  }
  return nLines;
}

/* Output section 1 definitions. */
Count
outDefs(outFile)
  FILE *outFile;
{
  rewind(G.tmpFile);
  return outSrcAction(outFile, 0);
}

/* Output local definitions within yylex().  Assumes that last tmpFile
 * output call was to outDefs().
 */
Count
outLocalDecs(outFile)
  FILE *outFile;
{
  return outSrcAction(outFile, 1);
}

Count
outSec3(outFile)
  FILE *outFile;
{
  CONST Count lineNum= G.lineN;
  Count nLines= 0;
  G.outFile= outFile;
  nLines+= lineDir(G.outFile, G.lineN, G.lineSrcName);
  yylex();
  fclose(yyin); yyin= NULL;
  nLines+= G.lineN - lineNum;
  fputs("\n", outFile); nLines++;
  G.outFile= NULL;
  return nLines;
}

Count 
outScanParams(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  fprintf(outFile, "#define YY_SRC_NAME \"%s\"\n",  G.srcName);
  nLines++;
  return nLines;
}



/*			INITIALIZATION.					*/

int 
yywrap()
{
  if (G.argn >= G.argc) {
    return 1;
  }
  else {
    G.srcName= G.lineSrcName= G.argv[G.argn++];
    G.lineN= 1;
    if (strcmp(G.srcName, "-") == 0) {
      clearerr(stdin); yyin= stdin; G.srcName= G.lineSrcName= "<stdin>";
    }
    else {
      yyin= fopen(G.srcName, "r");
      if (!yyin) fatal("Could not open source file %s.", G.srcName);
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
  globalsP= MALLOC_VEC(1, Globals);
  INIT_AREA(G.optionsText, Char, 6, UINT2_MAX);
  G.srcName= G.lineSrcName= "<stdin>"; G.secN= G.lineN= 1;
  G.argc= argc; G.argn= nonOptArgN; G.argv= argv;
  G.delimLineN= G.lineState= G.depth= 0;
  G.depth= 0; G.doCopy= FALSE; 
  G.retSS= 0;
  yywrap(); /* Setup first file if any; otherwise default to stdin. */
  initTmpFile();
  BEGIN DEFS;
  VOID_RET();
}

VOID
terminateScan()
{
#if DO_TRACE
  if (files.dumpFile) dumpActs(files.dumpFile);
#endif
  terminateTmpFile();
  delArea(&G.optionsText);
  delArea(&acts);
  FREE(globalsP);
  VOID_RET();
}

#if DO_TRACE
static VOID 
dumpActs(dumpFile)
  FILE *dumpFile;
{
  CONST Index nActs= A_NENTRIES(acts) - 1;
  Index a;
  fputs("SOURCE ACTIONS:\n", dumpFile);
  for (a= 2; a < nActs; a++) {
    fprintf(dumpFile, "%d:\n", a);
    outSrcAction(dumpFile, a);
  }
}
#endif


#ifdef TEST_SCAN

Int 
main(argc, argv)
  Int argc;
  ConstString argv[];
{
  Int tok;
  initID();
  initError(NULL, errorSrc, lineN, NULL);
  initScan(argc, 1, argv);
  options.wsInPatterns= (argc > 1);
  G.outFile= stdout;
  while ((tok= scan())) {
    printf("yytext= `%s'; Token= %s; ", yytext, (ConstString) tok);
    if (strcmp((ConstString) tok, "ID_TOK") == 0) {
      printf("ID= %d; IDText= %s;", 
	     yylval.id, getIDString(yylval.id));
    }
    else if (strcmp((ConstString) tok, "CHAR_TOK") == 0) {
      printf("Char= %c;", yylval.ch);
    }
    else if (strcmp((ConstString) tok, "NUM_TOK") == 0) {
      printf("Number= %d;", yylval.num);
    }
    else if (strcmp((ConstString) tok, "ACT_TOK") == 0) {
      printf("Act #= %d;", yylval.actN);
    }
    printf("\n");
  }
  return 0;
}

#endif
