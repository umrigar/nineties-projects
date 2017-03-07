/*

File:	 debug.c
Purpose: Debug handler for zyacc.

Last Update Time-stamp: "97/07/30 07:44:40 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/*

The commands are documented in the cmds[] table below.
*/

#include "libzyacp.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define VERBOSE_PROMPT		"zydebug> "
#define NON_VERBOSE_PROMPT	"% "
#define ERR_PREFIX		"!"
#define OUT_PREFIX		"  "


/* Command argument types */
typedef enum {
  NO_ARG= 0,					/* no argument */

  NON_TERM_ARG= 0x1,				/* name of a nonterminal */
  TERM_ARG= NON_TERM_ARG*2,			/* name of a terminal */
  NUM_ARG= TERM_ARG*2,				/* number */

  ALL_NON_TERMS_ARG= NUM_ARG*2,			/* %n */
  ALL_TERMS_ARG= ALL_NON_TERMS_ARG*2,		/* %t */
  HELP_ARG= ALL_TERMS_ARG*2, 			/* single letter cmd-name */
  ANY_ACT_ARG= ALL_TERMS_ARG|ALL_NON_TERMS_ARG	/* * */		

} ArgType;

/* Command arguments */
typedef struct {
  ArgType argType;	/* type of argument */
  YYUInt num;		/* number associated with argument */
  YYUChar flags;	/* flags associated with argument */
} Arg;


typedef struct {
  YY_CONST char cmd;		/* single character command */
  YY_CONST YYUShrt argTypes;	/* flags of permitted argument types */
  YY_CONST char *helpMsg;	/* help message. */
} Cmd;

static Cmd cmds[]= {
  { 'b', 
    ALL_NON_TERMS_ARG|ALL_TERMS_ARG|NON_TERM_ARG|TERM_ARG|NUM_ARG,
    "\
b [breakSpec]\n\
  Set or list breakpoint(s) as specified by breakSpec.  \n\
\n\
  At a breakpoint, the parser stops and the user can type in commands\n\
  (it may or may not display its current state, depending on whether or\n\
  not a displaypoint is set too).  If breakSpec is omitted then list all\n\
  breakpoints.  breakSpec can have one of the following forms with the\n\
  specified meaning:\n\
\n\
    TERM:	Terminal with name TERM is about to be shifted.\n\
    NON_TERM: 	Any rule with LHS nonterminal named NON_TERM is \n\
                about to be reduced.\n\
    RULE_NUM:	Rule number RULE_NUM is about to be reduced.\n\
    `%n':	Reduce action on any nonterminal.\n\
    `%t':	Shift action on any terminal.\n\
    `*':	Both shift and reduce actions.\n\
" },

  { 'B', 
    ALL_NON_TERMS_ARG|ALL_TERMS_ARG|NON_TERM_ARG|TERM_ARG|NUM_ARG,
    "\
B [breakSpec]\n\
  Clear breakpoint(s) as specified by breakSpec.  \n\
\n\
  At a breakpoint, the parser stops and the user can type in commands\n\
  (it may or may not display its current state, depending on whether or\n\
  not a displaypoint is set too).  If breakSpec is omitted then clear all\n\
  breakpoints.  breakSpec can have one of the following forms with the\n\
  specified meaning:\n\
\n\
    TERM:	Terminal with name TERM is about to be shifted.\n\
    NON_TERM: 	Any rule with LHS nonterminal named NON_TERM is \n\
                about to be reduced.\n\
    RULE_NUM:	Rule number RULE_NUM is about to be reduced.\n\
    `%n':	Reduce action on any nonterminal.\n\
    `%t':	Shift action on any terminal.\n\
    `*':	Both shift and reduce actions.\n\
" },

  { 'c',
    ALL_NON_TERMS_ARG|ALL_TERMS_ARG|NON_TERM_ARG|TERM_ARG|NUM_ARG,
    "\
c [temporaryBreakSpec]\n\
  Continue execution until a breakpoint is entered.\n\
\n\
  If temporaryBreakSpec is specified, then it specifies a temporary break\n\
  point which is automatically cleared whenever the next breakpoint is\n\
  entered. temporaryBreakSpec can have one of the following forms with the\n\
  specified meaning:\n\
\n\
    TERM:	Terminal with name TERM is about to be shifted.\n\
    NON_TERM: 	Any rule with LHS nonterminal named NON_TERM is \n\
                about to be reduced.\n\
    RULE_NUM:	Rule number RULE_NUM is about to be reduced.\n\
    `%n':	Reduce action on any nonterminal.\n\
    `%t':	Shift action on any terminal.\n\
    `*':	Both shift and reduce actions.\n\
" },

  { 'd', 
    ALL_NON_TERMS_ARG|ALL_TERMS_ARG|NON_TERM_ARG|TERM_ARG|NUM_ARG,
    "\
d [displaySpec]\n\
  Set or list displaypoint(s) as specified by displaySpec.  \n\
\n\
  At a displaypoint, the parser displays its current state (it may or\n\
  may not stop to let the user interact with it, depending on whether or\n\
  not a breakpoint is set too).  If displaySpec is omitted then list \n\
  all displaypoints.  displaySpec can have one of the following forms:\n\
\n\
    TERM:	Terminal with name TERM is about to be shifted.\n\
    NON_TERM: 	Any rule with LHS nonterminal named NON_TERM is \n\
                about to be reduced.\n\
    RULE_NUM:	Rule number RULE_NUM is about to be reduced.\n\
    `%n':	Reduce action on any nonterminal.\n\
    `%t':	Shift action on any terminal.\n\
    `*':	Both shift and reduce actions.\n\
" }, 

  { 'D',
    ALL_NON_TERMS_ARG|ALL_TERMS_ARG|NON_TERM_ARG|TERM_ARG|NUM_ARG,
    "\
D [displaySpec]\n\
  Clear displaypoint(s) as specified by displaySpec.  \n\
\n\
  At a displaypoint, the parser displays its current state (it may or\n\
  may not stop to let the user interact with it, depending on whether or\n\
  not a breakpoint is set too).  If displaySpec is omitted then clear \n\
  all displaypoints.  displaySpec can have one of the following forms:\n\
\n\
    TERM:	Terminal with name TERM is about to be shifted.\n\
    NON_TERM: 	Any rule with LHS nonterminal named NON_TERM is \n\
                about to be reduced.\n\
    RULE_NUM:	Rule number RULE_NUM is about to be reduced.\n\
    `%n':	Reduce action on any nonterminal.\n\
    `%t':	Shift action on any terminal.\n\
    `*':	Both shift and reduce actions.\n\
" }, 

  { 'f', 0,
    "\
  Display contents of parser source file.\n\
" },

  { 'h', 
    HELP_ARG,
    "\
h [cmd]\n\
  Print help on single-letter command cmd.  If cmd is omitted, give help\n\
  on all commands.\n\
" },

  { 'l', 
    ALL_NON_TERMS_ARG|ALL_TERMS_ARG|NUM_ARG,
    "\
l [listSpec]\n\
  List terminals, non-terminals or rules as specified by listSpec.  \n\
\n\
  If listSpec is omitted, then all rules are listed.  Otherwise listSpec\n\
  can be one of the following:\n\
\n\
   `%n':	List all non-terminal symbols.\n\
   `%t': 	List all terminal symbols.\n\
   RULE_NUM:	List rule with number RULE_NUM.\n\
" },

  { 'm', NUM_ARG, "\
m [depth]\n\
  Set max. depth printed on stack.  \n\
\n\
  If depth is 0 or omitted, then entire stack is printed.\n\
" },

  { 'M', 0, "\
M\n\
  Display max. length of token semantics.\n\
" },

  { 'n', 0, "\
n\n\
  Execute parser till next shift action.\n\
\n\
Equivalent to `c %t'.\n\
" },

  { 'p', 0, "\
p\n\
  Print current parser state.\n\
" },

  { 'q', 0, "\
q\n\
  Quit debugger and run parser without debugging.\n\
" },

  { 's', 0, "\
s \n\
<blank line>\n\
  Single-step parser to next shift or reduce action.\n\
\n\
  Equivalent to `c *'.\n\
" },

  { 'v', 0,
    "\
v\n\
  Turn on verbose output.\n\
" },

  { 'V', 0,
    "\
V\n\
  Turn off verbose output.\n\
" },

};
  
static YY_CONST char *getArg 
  YY_PROTO((YY_CONST YYDebugData *YY_CONST debugDataP,
	    YY_CONST char *YY_CONST lineP,
	    Arg *YY_CONST argP,
	    YYUInt cmdIndex));
static YY_VOID setResetFlag
  YY_PROTO((YYDebugData *YY_CONST debugDataP, 
	    YY_CONST Arg *YY_CONST argP,
	    YYUInt doSet));
static YY_VOID helpCmd 
  YY_PROTO((YY_CONST YYDebugData *YY_CONST debugDataP,
	    YY_CONST Arg *YY_CONST argP));
static YY_VOID debugInit YY_PROTO((YYDebugData *YY_CONST debugDataP));
static YY_CONST char *getSymName 
  YY_PROTO((YYDebugData *YY_CONST debugDataP, YYUInt sym));
static YYUInt ruleLength 
  YY_PROTO((YY_CONST YYDebugData *YY_CONST debugDataP,
	    YYXRuleN ruleN));
static YY_VOID outSrc 
  YY_PROTO((YY_CONST YYDebugData *YY_CONST debugDataP));
static YY_VOID outRule 
  YY_PROTO((YYDebugData *YY_CONST debugDataP, YYUInt ruleN, 
	    YYBoolean isMultiLine));
static YY_VOID display
  YY_PROTO((YYDebugData *YY_CONST debugDataP,
	    YYUInt actN,
	    YYXTerm tok,
	    int yyChar,
	    YY_VOIDP yylvalP,
	    YYXState gotoState,
	    YY_VOIDP sp));
static YY_VOID allBreaks 
  YY_PROTO((YYDebugData *YY_CONST debugDataP,
	    YY_CONST YYBoolean doList,
	    YY_CONST YYBoolean isDisplay));
static YY_VOID listCmd
  YY_PROTO((YYDebugData *YY_CONST debugDataP,
	    YY_CONST Arg *YY_CONST argP));
static YY_VOID encodeTempBreak
  YY_PROTO((YYDebugData *YY_CONST debugDataP,
	    YY_CONST Arg *YY_CONST argP));
static YY_VOID interact
  YY_PROTO((YYDebugData *YY_CONST debugDataP,
	    int actN,
	    YYXTerm tok,
	    int yyChar,
	    YY_VOIDP yylvalP,
	    YYXState gotoState,
	    YY_VOIDP sp));
static YYBoolean isInteract
  YY_PROTO((YYDebugData *YY_CONST debugDataP,
	    YYBoolean isShift,
	    YYXTerm tok,
	    YYXRuleN ruleN,
	    YYXNonTerm lhsNonTerm));
static YYBoolean isDisplay
  YY_PROTO((YYDebugData *YY_CONST debugDataP,
	    YYBoolean isShift,
	    YYXTerm tok,
	    YYXRuleN ruleN,
	    YYXNonTerm lhsNonTerm));


/* Return argument from line lineP for command with index cmdIndex in 
 * *argP.  If argument valid, return pointer to character after argument
 * in line, else return NULL.
 */
static YY_CONST char *
getArg(debugDataP, lineP, argP, cmdIndex)
  YY_CONST YYDebugData *YY_CONST debugDataP;
  YY_CONST char *YY_CONST lineP;
  Arg *YY_CONST argP;
  YYUInt cmdIndex;
{
  YY_CONST char cmd= cmds[cmdIndex].cmd;
  YY_CONST char *p;
  argP->argType= NO_ARG;
  argP->flags= 0;
  for (p= lineP; *p == ' ' || *p == '\t'; p++) ;
  if (cmd == 'h') { /* special processing for help command */
    if (*p != '\n') {
      YYUInt i;
      for (i= 0; i < sizeof(cmds)/sizeof(Cmd); i++) {
	if (*p == cmds[i].cmd) {
	  argP->argType= HELP_ARG; argP->num= i; p++;
	  break;
	}
      } /* for (i= 0; i < sizeof(cmds)/sizeof(Cmd); i++) */
    }
  } /* if (cmd == 'h' && *p != '\n') */
  else { /* not help command */
    YY_CONST YYBoolean isDisplayCmd= tolower(cmds[cmdIndex].cmd) == 'd';
    if (*p == '%') {
      switch (p[1]) {
	case 'n':
	  argP->argType= ALL_NON_TERMS_ARG;
	  argP->flags= 
	    (isDisplayCmd) ? FLAG(REDUCE_DISPLAY_F) : FLAG(REDUCE_BREAK_F);
	  break;
	case 't':
	  argP->argType= ALL_TERMS_ARG;
	  argP->flags= 
	    (isDisplayCmd) ? FLAG(SHIFT_DISPLAY_F) : FLAG(SHIFT_BREAK_F);
	  break;
	default:
	  fprintf(debugDataP->err, "%sinvalid specifier after %%\n", 
		  ERR_PREFIX);
	  return NULL;
      } /* switch p[1] */
      p+= 2;
    }
    else if (*p == '*') {
      argP->argType= ANY_ACT_ARG; 
      argP->flags= 
	(isDisplayCmd) ? (FLAG(REDUCE_DISPLAY_F) | FLAG(SHIFT_DISPLAY_F))
	: (FLAG(REDUCE_BREAK_F) | FLAG(SHIFT_BREAK_F));
      p++;
    }
    else if (isdigit(*p)) {
      argP->argType= NUM_ARG;
      argP->flags= 
	(isDisplayCmd) ? FLAG(RULE_DISPLAY_F) : FLAG(RULE_BREAK_F);
      argP->num= 0;
      for (; isdigit(*p); p++) argP->num= 10*argP->num + (*p - '0');
    }
    else if (isalpha(*p) || *p == '\'' || *p == '_' || *p == '$' 
	     || *p == '<') {
      YY_CONST char *YY_CONST p0= p;
      YYUInt len;
      YYUInt i;
      if (*p == '\'') {
	for (p= p + 1; *p != '\'' && *p != '\n'; p++) {
	  if (*p == '\\') p++;
	}
	if (*p == '\'') {
	  p++;
	}
	else {
	  fprintf(debugDataP->err, "%sunterminated literal name\n", 
		  ERR_PREFIX);
	  return NULL;
	}
      }
      else {
	for (p= p + 1; isalnum(*p) || *p == '_' || *p == '>'; p++) ;
      }
      len= p - p0;
      for (i= 0; i < debugDataP->nTerms; i++) { /* search terminals */
	if (strncmp(debugDataP->termNames[i], p0, len) == 0) {
	  argP->argType= TERM_ARG; argP->num= i;
	  argP->flags=
	    (isDisplayCmd) ? FLAG(TERM_DISPLAY_F) : FLAG(TERM_BREAK_F);
	  break;
	}
      }
      if (argP->argType == NO_ARG && *p0 != '\'') { /* search non-terms */  
	for (i= 0; i < debugDataP->nNonTerms; i++) {
	  if (strncmp(debugDataP->nonTermNames[i], p0, len) == 0) {
	    argP->argType= NON_TERM_ARG; argP->num= i;
	    argP->flags=
	      (isDisplayCmd) ? FLAG(NON_TERM_DISPLAY_F) 
	                     : FLAG(NON_TERM_BREAK_F);
	    break;
	  }
	}
	if (argP->argType == NO_ARG) {
	  fprintf(debugDataP->err, "%sunknown name %.*s\n", 
		  ERR_PREFIX, (int)len, p0);
	  return NULL;
	}
      } /* if (argP->argType == NO_ARG */
    } /* else if (isalpha(*p) ... ) */
  } /* else not a help command */
  if ((cmds[cmdIndex].argTypes & argP->argType) == argP->argType) {
    return p;
  }
  else {
    fprintf(debugDataP->err, "%sinvalid argument\n", ERR_PREFIX);
    return NULL;	
  }
}

static YY_VOID
setResetFlag(debugDataP, argP, doSet)
  YYDebugData *YY_CONST debugDataP;
  YY_CONST Arg *YY_CONST argP;
  YYUInt doSet;
{
  YYUChar mask= (argP->flags);
  YYUChar *target;
  switch (argP->argType) {
    case NON_TERM_ARG:
    case NUM_ARG:
    case TERM_ARG:
      target= &debugDataP->breaks[argP->num];
      break;
    case ANY_ACT_ARG:
    case ALL_TERMS_ARG:
    case ALL_NON_TERMS_ARG:
      target= &debugDataP->flags;
      break;
    default:
      assert(0);
      break;
  }
  if (doSet) {
    *target|= mask;
  }
  else {
    *target&= ~mask;
  }
  YY_VOID_RET();
}

static YY_VOID
helpCmd(debugDataP, argP)
  YY_CONST YYDebugData *YY_CONST debugDataP;
  YY_CONST Arg *YY_CONST argP;
{
  if (argP->argType == NO_ARG) {
    YYUInt i;
    for (i= 0; i < sizeof(cmds)/sizeof(Cmd); i++) {
      YY_CONST char *YY_CONST msg= cmds[i].helpMsg;
      YY_CONST char *YY_CONST p= strstr(msg, "\n\n");
      YYUInt len= (p) ? p - msg + 1 : strlen(msg);
      fprintf(debugDataP->out, "%.*s", (int)len, msg);
    }
  }
  else {
    fprintf(debugDataP->out, "%s", cmds[argP->num].helpMsg);
  }
  YY_VOID_RET();
}

static YY_VOID
debugInit(debugDataP)
  YYDebugData *YY_CONST debugDataP;
{
  debugDataP->flags= 0;
  if (!yyRemoteDebug(debugDataP)) {
    debugDataP->in= stdin;
    debugDataP->out= stdout;
    debugDataP->err= stderr;
  }
  debugDataP->flags|= FLAG(DEBUG_INIT_F);
  debugDataP->flags|= (FLAG(REDUCE_DISPLAY_F) | FLAG(SHIFT_DISPLAY_F))
                      | FLAG(VERBOSE_DISPLAY_F);
  fprintf(debugDataP->out, "%szyacc debugger.  'h' for help.\n", OUT_PREFIX);
}

static YY_CONST char *
getSymName(debugDataP, sym) 
  YYDebugData *YY_CONST debugDataP;
  YYUInt sym;
{
  return (YY_SYM_TYPE(sym) == YY_NON_TERM_SYM) 
	 ? debugDataP->nonTermNames[YY_SYM_NUM(sym)]
	 : debugDataP->termNames[YY_SYM_NUM(sym)];

}

static YYUInt
ruleLength(debugDataP, ruleN)
  YY_CONST YYDebugData *YY_CONST debugDataP;
  YYXRuleN ruleN;
{
  YYUInt len= 0;
  YY_CONST YYXSym *YY_CONST syms= debugDataP->syms;
  YYUInt i;
  for (i= debugDataP->rhs[ruleN]; YY_SYM_TYPE(syms[i]) != YY_RULE_SYM; i++) {
    len++;
  }
  return len;
}

static YY_VOID
outSrc(debugDataP)
  YY_CONST YYDebugData *YY_CONST debugDataP;
{
  enum { MAX_BUF= 256 };
  YY_CONST char *srcbaseEnv= (YY_CONST char*)getenv(SRCBASE_ENV_VAR);
  YY_CONST char *srcbase= (srcbaseEnv) ? srcbaseEnv : ".";
  FILE *out= debugDataP->out;
  if (strlen(srcbase) + 1 + strlen(debugDataP->srcName) + 1 > MAX_BUF) {
    fprintf(out, "Source file path length %s/%s too long\n", 
	    srcbase, debugDataP->srcName);
  }
  else { /* name not too long */
    char buf[MAX_BUF];
    sprintf(buf, "%s/%s", srcbase, debugDataP->srcName);
    { /* open file */
      FILE *srcF= fopen(buf, "r");
      if (srcF == NULL) {
	fprintf(out, "could not open source file %s/%s\n", 
		srcbase, debugDataP->srcName);
      }
      else { /* source file is open */
	enum { MAX_LINE= 128 };
	char line[MAX_LINE];
	YYBoolean lastIsNL= TRUE;
	while (fgets(line, MAX_LINE, srcF)) {
	  if (lastIsNL && line[0] == '.') fputs(".", out);
	  fputs(line, out);
	  lastIsNL= (line[strlen(line) - 1] == '\n');
	}
	if (!lastIsNL) fputs("\n", out);
	fclose(srcF);
      } /* else source file is open */
    } /* open file */
  } /* else name not too long */ 
  fputs(".\n", out);
}

static YY_VOID 
outRule(debugDataP, ruleN, isInLine) 
  YYDebugData *YY_CONST debugDataP;
  YYUInt ruleN;
  YYBoolean isInLine;
{
  FILE *out= debugDataP->out;
  YY_CONST char *YY_CONST lhsNonTermName= 
    debugDataP->nonTermNames[debugDataP->lhs[ruleN]];
  unsigned i;
  if (isInLine) {
    fprintf(out, "%s: ", lhsNonTermName);
  }
  else {
    enum { RULE_NO_COL= 24 };
    fprintf(out, "%s%s:%*s/* rule %d */\n", OUT_PREFIX,
	  lhsNonTermName, RULE_NO_COL - strlen(lhsNonTermName), "", ruleN);
  }
  if (!isInLine && 
      YY_SYM_TYPE(debugDataP->syms[debugDataP->rhs[ruleN]]) == YY_RULE_SYM) {
    fprintf(out, "%s  /* empty */\n", OUT_PREFIX);
  }
  for (i= debugDataP->rhs[ruleN]; 
       YY_SYM_TYPE(debugDataP->syms[i]) != YY_RULE_SYM; i++) {
    fprintf(debugDataP->out, "%s%s%s%s",  isInLine ? "" : OUT_PREFIX,
	    isInLine ? " " : "  ", 
	    getSymName(debugDataP, debugDataP->syms[i]),
	    isInLine ? "" : "\n");
  }
  YY_VOID_RET();
}

#define	IS_NORMAL_TOK_START_CHAR(c) 					\
  (isalpha(c) || (c) == '_' || (c) == '\'')


static YY_VOID
display(debugDataP, actN, tok, yyChar, yylvalP, gotoState, sp)
  YYDebugData *YY_CONST debugDataP;
  YYUInt actN;
  YYXTerm tok;
  int yyChar;
  YY_VOIDP yylvalP;
  YYXState gotoState;
  YY_VOIDP sp;
{
  typedef enum { SHIFT, REDUCE, ERROR, ACCEPT } ActType;
  ActType actType= 
      (actN < debugDataP->nStates) ? SHIFT
    : ( actN == debugDataP->nStates + debugDataP->errRuleN ||
        tok == debugDataP->errTok
      ) 
      ? ERROR
    : (actN == debugDataP->nStates + 0) ? ACCEPT
    : REDUCE;
  FILE *out= debugDataP->out;
  YY_CONST char *pLimit= (YY_CONST char *)sp;
  YY_CONST char *p= (debugDataP->maxDepth > 0) 
                    ? pLimit - debugDataP->maxDepth
                    : debugDataP->stkBase;
  if (debugDataP->flags & FLAG(VERBOSE_DISPLAY_F)) {
    fprintf(out, "%s[ ", OUT_PREFIX);
    for (p= p < (YY_CONST char *)debugDataP->stkBase 
	   ? debugDataP->stkBase 
	   : p; 
	 p < pLimit; 
	 p+= debugDataP->sizeStkEntry) {
      YY_CONST YYXState s= *(YYXState*)p;
      fprintf(out, "%d/%s ", s, getSymName(debugDataP, debugDataP->access[s]));
    }
    fprintf(out, "] %s", debugDataP->termNames[tok]);
    if (debugDataP->semFn && 
	IS_NORMAL_TOK_START_CHAR(debugDataP->termNames[tok][0])) {
      fputs(" ", out);
      (debugDataP->semFn)(out, yyChar, yylvalP);
    }
    fputs(" ", out);
    switch (actType) {
      case SHIFT:
	fprintf(out, "SHIFT %d", actN);
	break;
      case ERROR:
	fprintf(out, "ERROR");
	break;
      case ACCEPT:
	fprintf(out, "ACCEPT");
	break;
      case REDUCE: {
	YY_CONST YYXRuleN ruleN= actN - debugDataP->nStates;
	fprintf(out, "REDUCE %d; ", ruleN);
	fprintf(out, "GOTO %d; ", gotoState);
	fprintf(out, "LINE %d; ", debugDataP->lineNums[ruleN]);
	outRule(debugDataP, ruleN, TRUE); 
	break;
	default:
	  assert(0);
      } /* case REDUCE */
    } /* switch (actType) */
    fprintf(out, "\n");
  } /* if verbose */
  else { /* not verbose */
    fprintf(out, "%s%%K ", OUT_PREFIX);
    for (p= p < (YY_CONST char *)debugDataP->stkBase 
	   ? debugDataP->stkBase 
	   : p; 
	 p < pLimit; 
	 p+= debugDataP->sizeStkEntry) {
      YY_CONST YYXState s= *(YYXState*)p;
      fprintf(out, "%d/%s ", s, getSymName(debugDataP, debugDataP->access[s]));
    }
    fprintf(out, "; %%T %s; ", debugDataP->termNames[tok]);
    if (debugDataP->semFn && 
	IS_NORMAL_TOK_START_CHAR(debugDataP->termNames[tok][0])) {
      fputs("%V \'", out);       
      (debugDataP->semFn)(out, yyChar, yylvalP);
      fputs("\'; ", out);
    }
    switch (actType) {
      case SHIFT:
	fprintf(out, "%%A S; %%S %d; ", actN);
	break;
      case ERROR:
	fprintf(out, "%%A E; ");
	break;
      case ACCEPT:
	fprintf(out, "%%A A; ");
	break;
      case REDUCE: {
	YY_CONST YYXRuleN ruleN= actN - debugDataP->nStates;
	fprintf(out, "%%A R; %%R %d; %%S %d; ", ruleN, gotoState);
	fprintf(out, "%%L %d; ", debugDataP->lineNums[ruleN]);
	fprintf(out, "%%M %d; ", ruleLength(debugDataP, ruleN));
	fprintf(out, "%%N %s; ", 
		debugDataP->nonTermNames[debugDataP->lhs[ruleN]]);
	break;
	default:
	  assert(0);
      } /* case REDUCE */
    } /* switch (actType) */
    fprintf(out, "\n");
  } /* else not verbose */
  YY_VOID_RET();
}

#define MAX(a, b)		(((a) > (b)) ? (a) : (b))

static YY_VOID
allBreaks(debugDataP, doList, isDisplay)
  YYDebugData *YY_CONST debugDataP;
  YY_CONST YYBoolean doList;
  YY_CONST YYBoolean isDisplay;
{
  YYUInt i;
  FILE *out= debugDataP->out;
  YYUChar *breaks= debugDataP->breaks;
  YYUChar mask;
  if (doList) { /* list break/display points */
    mask= !isDisplay ? FLAG(TERM_BREAK_F) : FLAG(TERM_DISPLAY_F);
    for (i= 0; i < debugDataP->nTerms; i++) {
      if ((breaks[i] & mask) != 0) {
	fprintf(out, "%s%s\n", OUT_PREFIX, debugDataP->termNames[i]);
      }
    }
    mask= !isDisplay ? FLAG(NON_TERM_BREAK_F) : FLAG(NON_TERM_DISPLAY_F);
    for (i= 0; i < debugDataP->nNonTerms; i++) {
      if ((breaks[i] & mask) != 0) {
	fprintf(out, "%s%s\n", OUT_PREFIX, debugDataP->nonTermNames[i]);
      }
    }
    mask= !isDisplay ? FLAG(RULE_BREAK_F) : FLAG(RULE_DISPLAY_F);
    for (i= 0; i < debugDataP->nRules; i++) {
      if ((breaks[i] & mask) != 0) {
	fprintf(out, "%srule #%d\n", OUT_PREFIX, i);
      }
    }
    mask= !isDisplay ? FLAG(SHIFT_BREAK_F) : FLAG(SHIFT_DISPLAY_F);
    if ((debugDataP->flags & mask) != 0) {
      fprintf(out, "%sall shifts\n", OUT_PREFIX);
    }
    mask= !isDisplay ? FLAG(REDUCE_BREAK_F) : FLAG(REDUCE_DISPLAY_F);
    if ((debugDataP->flags & mask) != 0) {
      fprintf(out, "%sall reductions\n", OUT_PREFIX);
    }
  }
  else { /* clear all break/display points */
    YYUInt n= MAX(MAX(debugDataP->nTerms, debugDataP->nNonTerms), 
		  debugDataP->nRules);
    mask= ~(isDisplay 
	    ? (FLAG(NON_TERM_DISPLAY_F) | FLAG(TERM_DISPLAY_F) |
	       FLAG(RULE_DISPLAY_F))
	    : (FLAG(NON_TERM_BREAK_F) | FLAG(TERM_BREAK_F) |
	       FLAG(RULE_BREAK_F)));
    for (i= 0; i < n; i++) {
      breaks[i]&= mask;
    }
    mask= ~(isDisplay 
	    ? (FLAG(SHIFT_DISPLAY_F) | FLAG(REDUCE_DISPLAY_F))
	    : (FLAG(SHIFT_BREAK_F) | FLAG(REDUCE_BREAK_F))
	   );
    debugDataP->flags&= mask;
  } /* else clear all break/display points */
  YY_VOID_RET();
}

static YY_VOID
listCmd(debugDataP, argP)
  YYDebugData *YY_CONST debugDataP;
  YY_CONST Arg *YY_CONST argP;
{
  YYUInt i;
  if (argP->argType == NO_ARG) {
    for (i= 0; i < debugDataP->nRules; i++) {
      outRule(debugDataP, i, FALSE);
    }
  }
  if ((argP->argType & ALL_NON_TERMS_ARG) != 0) {
    for (i= 0; i < debugDataP->nNonTerms; i++) {
      fprintf(debugDataP->out, "%s%s\n", OUT_PREFIX, 
	      debugDataP->nonTermNames[i]);
    }
  }
  if ((argP->argType & ALL_TERMS_ARG) != 0) {
    for (i= 0; i < debugDataP->nTerms; i++) {
      fprintf(debugDataP->out, "%s%s\n", OUT_PREFIX, debugDataP->termNames[i]);
    }
  }
  if (argP->num < debugDataP->nRules && (argP->argType & NUM_ARG) != 0) {
    outRule(debugDataP, argP->num, FALSE);
  }
  YY_VOID_RET();
}

static YY_VOID
encodeTempBreak(debugDataP, argP)
  YYDebugData *YY_CONST debugDataP;
  YY_CONST Arg *YY_CONST argP;
{
  debugDataP->tempLocalFlags= debugDataP->tempGlobalFlags= 0;
  if ((argP->argType & (NON_TERM_ARG | TERM_ARG | NUM_ARG)) != 0) {
    debugDataP->tempLocalFlags= argP->flags;
  }
  if ((argP->argType & ANY_ACT_ARG) != 0) {
    debugDataP->tempGlobalFlags= argP->flags;
  }
  debugDataP->tempBreakN= argP->num;
  YY_VOID_RET();
}

static YY_VOID
interact(debugDataP, actN, tok, yyChar, yylvalP, gotoState, sp)
  YYDebugData *YY_CONST debugDataP;
  int actN;
  YYXTerm tok;
  int yyChar;
  YY_VOIDP yylvalP;
  YYXState gotoState;
  YY_VOIDP sp;
{
  enum { MAX_LINE_LEN= 256 };
  char line[MAX_LINE_LEN];
  while (1) {
    YYBoolean readDone= FALSE;
    YYUInt cmdIndex;
    YYUChar cmd;
    Arg arg;
    YY_CONST char *p;
    do {
      fprintf(debugDataP->out, "%s", 
	      ((debugDataP->flags & FLAG(VERBOSE_DISPLAY_F)) != 0) 
	      ? VERBOSE_PROMPT
	      : NON_VERBOSE_PROMPT);
      fflush(debugDataP->out);
      if (!fgets(line, MAX_LINE_LEN, debugDataP->in)) {
	fprintf(debugDataP->err, "!could not read input\n"); exit(1);
      }
      if (line[strlen(line) - 1] != '\n') {
	fprintf(debugDataP->err, "!input line too long\n");
      }
      else {
	readDone= TRUE;
      }
    } while (!readDone);
    for (p= line; *p == ' ' || *p == '\t'; p++) ;
    if (*p == '\n') {
      cmd= 's';
    }
    else {
      cmd= *p++;
    }
    for (cmdIndex= 0; cmdIndex < sizeof(cmds)/sizeof(Cmd); cmdIndex++) {
      if (cmds[cmdIndex].cmd == cmd) break;
    }
    if (cmdIndex >= sizeof(cmds)/sizeof(Cmd)) {
      fprintf(debugDataP->err, "%sinvalid command `%c'\n", ERR_PREFIX, cmd);
      continue; /* with outer loop */
    }
    if ((p= getArg(debugDataP, p, &arg, cmdIndex)) == NULL) continue;
    while (*p == ' ' || *p == '\t') p++;
    if (*p != '\n' && (*p != '\r' || p[1] != '\n')) {
      fprintf(debugDataP->err, "%sunrecognized command argument\n", 
	      ERR_PREFIX);
      continue;
    }
    switch (cmd) { /* command is ok; do it */
      case 'b':
      case 'B': 
      case 'd':
      case 'D': {
	YYBoolean doSet= islower(cmd);
	YYBoolean isDisplay= tolower(cmd) == 'd';
	if (arg.argType != NO_ARG) {
	  setResetFlag(debugDataP, &arg, doSet);
	}
	else {	
	  allBreaks(debugDataP, doSet, isDisplay);
	}
	break;
      }
      case 'f':
	outSrc(debugDataP);
	break;
      case 'h':
	helpCmd(debugDataP, &arg);
	break;
      case 'l':
	listCmd(debugDataP, &arg);
	break;
      case 'm':
	debugDataP->maxDepth= (arg.argType == NUM_ARG) ? arg.num : 0;
	break;
      case 'M':
	fprintf(debugDataP->out, "%s%d\n", OUT_PREFIX, 
		debugDataP->semFn ? (int)debugDataP->semMax : 0);
	break;
      case 's':
	arg.argType|= ALL_TERMS_ARG; 
	arg.flags|= FLAG(REDUCE_BREAK_F);
	/* no break */
      case 'n':
	arg.argType|= ALL_NON_TERMS_ARG;
	arg.flags|= FLAG(SHIFT_BREAK_F);
	/* no break */
      case 'c': 
	encodeTempBreak(debugDataP, &arg);
	return;
      case 'p':
	if (actN >= 0) {
	  display(debugDataP, actN, tok, yyChar, yylvalP, gotoState, sp);
	}
	break;
      case 'q':
	debugDataP->flags= FLAG(YY_DEBUG_IGNORE_F);
	return;
	break;
      case 'v':
	debugDataP->flags|= FLAG(VERBOSE_DISPLAY_F);
	break;
      case 'V':
	debugDataP->flags&= ~FLAG(VERBOSE_DISPLAY_F);
	break;
      default:
	assert(0);
    }
      
  } /* while (1) */
}


static YYBoolean 
isInteract(debugDataP, isShift, tok, ruleN, lhsNonTerm)
  YYDebugData *YY_CONST debugDataP;
  YYBoolean isShift;
  YYXTerm tok;
  YYXRuleN ruleN;
  YYXNonTerm lhsNonTerm;
{
  if (isShift) {
    return ( (debugDataP->flags & FLAG(SHIFT_BREAK_F)) != 0 ) ||
	   ( (debugDataP->tempGlobalFlags & FLAG(SHIFT_BREAK_F)) != 0 ) ||
	   ( (debugDataP->breaks[tok] & FLAG(TERM_BREAK_F)) != 0 ) ||
	   ( (debugDataP->tempLocalFlags & FLAG(TERM_BREAK_F)) != 0 &&
	     (debugDataP->tempBreakN == tok)
	   );
  }
  else {
    return ( (debugDataP->flags & FLAG(REDUCE_BREAK_F)) != 0 ) ||
	   ( (debugDataP->tempGlobalFlags & FLAG(REDUCE_BREAK_F)) != 0 ) ||
	   ( (debugDataP->breaks[lhsNonTerm] & 
	         FLAG(NON_TERM_BREAK_F)) != 0 ) ||
	   ( (debugDataP->breaks[ruleN] & FLAG(RULE_BREAK_F)) != 0 ) ||
	   ( (debugDataP->tempLocalFlags & FLAG(NON_TERM_BREAK_F)) != 0 &&
	     (debugDataP->tempBreakN == lhsNonTerm) 
	   ) ||
	   ( (debugDataP->tempLocalFlags & FLAG(RULE_BREAK_F)) != 0 &&
	     (debugDataP->tempBreakN == ruleN)
	   );
  }
}    

static YYBoolean 
isDisplay(debugDataP, isShift, tok, ruleN, lhsNonTerm)
  YYDebugData *YY_CONST debugDataP;
  YYBoolean isShift;
  YYXTerm tok;
  YYXRuleN ruleN;
  YYXNonTerm lhsNonTerm;
{
  if (isShift) {
    return ( (debugDataP->flags & FLAG(SHIFT_DISPLAY_F)) != 0 ) ||
	   ( (debugDataP->breaks[tok] & FLAG(TERM_DISPLAY_F)) != 0 );
  }
  else {
    return ( (debugDataP->flags & FLAG(REDUCE_DISPLAY_F)) != 0 ) ||
	   ( (debugDataP->breaks[lhsNonTerm] & 
	         FLAG(NON_TERM_DISPLAY_F)) != 0 ) ||
	   ( (debugDataP->breaks[ruleN] & FLAG(RULE_DISPLAY_F)) != 0 );
  }
}    

YY_VOID 
yyDebugHandler(debugDataP, actN, tok, yyChar, yylvalP, gotoState, sp) 
  YYDebugData *YY_CONST debugDataP;
  int actN;
  YYXTerm tok;
  int yyChar;
  YY_VOIDP yylvalP;
  YYXState gotoState;
  YY_VOIDP sp;
{
  YYBoolean isInit= actN < 0;
  YY_CONST YYBoolean isShift= (actN < debugDataP->nStates && !isInit);
  YY_CONST YYXRuleN ruleN= (isShift) ? 0 : actN - debugDataP->nStates;
  YY_CONST YYXNonTerm lhsNonTerm= 
    (isShift || isInit) ? 0 : debugDataP->lhs[ruleN];
  if (isInit && (debugDataP->flags & FLAG(DEBUG_INIT_F)) == 0) {
    debugInit(debugDataP);
  }
  if (!isInit && isDisplay(debugDataP, isShift, tok, ruleN, lhsNonTerm)) {
      display(debugDataP, actN, tok, yyChar, yylvalP, gotoState, sp);
  }
  if (isInit || isInteract(debugDataP, isShift, tok, ruleN, lhsNonTerm)) { 
    interact(debugDataP, actN, tok, yyChar, yylvalP, gotoState, sp);
  }
  YY_VOID_RET();
}
			      


