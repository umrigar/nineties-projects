/*
A filter which adds markup to a C, lex or yacc program to make it look
reasonable under texinfo or latex.

The program can have its lines numbered and labels defined to refer to
those lines.  A label looks like an identifier enclosed within label-chars
(currently '!') (the exact syntax of a label is given by the lex
macro {label}). 

Labels can occur anywhere in the source and are stripped from the output;
the rest of the line remains unless the line is a label-line, where a 
label-line is a line starting with a slash-star, single space, label, 
single-space, star-slash, optional whitespace, newline.  Such label-lines
are totally deleted from the output.

Command line options allow specifying begin and end labels: if these options
are specified, then the corresponding labels must occur only within
label-lines.  Other options allow choosing the output format, whether line
numbering should be done, starting line number, # of digits to be printed
for line numbers.

LaTeX is not currently implemented.  Adding it in should involve adding
an entry to the formats[] array.  If a tabbing environment is going to
be used, then there will have to be a pattern which converts line
indentation to suitable tab-stops.  Such a pattern may have some
complications with the pattern which recognizes a possibly indented cpp 
directive.

*/

keyword		[[:lower:]]+
ws		[ \t]
label		"!"[[:alpha:][:digit:]_:.]+"!"

%s	YACC	/* Seeing a yacc file. */
%s	LEX	/* Seeing a lex file. */
%x	STR	/* Within a (possibly multi-line) string. */
%x	COMM	/* Within a (possibly multi-line) comment. */
%x	CHAR	/* Within a (possibly multi-line) character constant. */
%x	SKIP	/* Skipping initial portion until begin label. */

%{

#include "area.h"
#include "ccport.h"
#include "error.h"
#include "ids.h"
#include "parseopt.h"

/*			    TARGET FORMATS				*/

typedef enum { TEXINFO_TARGET, LATEX_TARGET } Target;

struct { /* printf() style format strings to specially format certain things. */
  ConstString envBegin;		/* Constant format to start listing. */
  ConstString envEnd;		/* Constant format to end listing. */
  ConstString keyword;		/* %s format to emphasize keyword. */
  ConstString brace;		/* %c format to quote a brace char. */
  ConstString backslash;	/* Constant format to quote a backslash. */
  ConstString defLabel;		/* %s %d format to define a label. */
  ConstString commBegin;	/* Constant format to start a comment. */
  ConstString commEnd;		/* Constant format to end a comment. */
  ConstString commStar;		/* %s format for '*' at start of comm. line. */
  ConstString dQuote;		/* Constant format to print a \". */
  ConstString sQuote;		/* Constant format to print a \'. */
  ConstString at;		/* Constant format to print a @. */
}
 formats[]= {
  { /* TeXInfo */
    "@example\n",		/* Constant format to start listing. */
    "@end example\n",		/* Constant format to end listing. */
    "@b{%s}",			/* %s format to emphasize keyword. */
    "@%c",			/* %c format to quote a brace char. */
    "\\",			/* Constant format to quote a backslash. */
    "@set %s %d\n",		/* %s %d format to define a label. */
    "/*@r{",			/* Constant format to start a comment. */
    "}*/",			/* Constant format to end a comment. */
    "@t{%s}",			/* %s format for '*' at start of comm. line. */
    "\"",			/* Constant format to print a \". */
    "\'",			/* Constant format to print a \'. */
    "@@"			/* Constant format to print a @. */
  }
};



/* Character used to surround labels in source.  If changing, will also
 * need to change in lex macro {label}.
 */
#define LABEL_CHAR '!'

enum { /* Flags for languages handled. */
  C_FLAG= 1,		/* C keyword flag. */
  CPP_FLAG= 2,		/* cpp keyword flag. */
  LEX_FLAG= 4,		/* Lex keyword flag. */
  YACC_FLAG= 8		/* Yacc keyword flag. */
};

typedef enum {
  NO_LANG,		/* Used for initialization. */
  C_LANG,		/* Basic C language. */
  LEX_LANG,		/* C language + lex. */
  YACC_LANG,		/* C language + yacc. */
} Lang;

static VOID initGlobs PROTO((VOID_ARGS));
static VOID keyword PROTO((ConstString text, unsigned langFlags));
static int defLabel PROTO((char *text));
static VOID outLineNum PROTO((VOID_ARGS));

typedef struct {
  Target target;	/* Target format. */
  ConstString srcName;	/* Name of source file from command-line. */
  Index srcLineNum;	/* Source line number. */
  Index outLineNum;	/* Output line number. */
  Count nDigits;	/* # of digits to output for each line number. */
  Index beginID;	/* ID of label at which output should begin. */
  Index endID;		/* ID of label at which output should end. */
  Lang lang;		/* Top-level language of input: c, yacc, lex. */
  int start;		/* Start-state after begin label seen. */
} Globs;
static Globs globs;
#define G globs

static VOID
initGlobs()
{
  G.target= TEXINFO_TARGET; G.srcName= NULL;
  G.srcLineNum= 1; G.outLineNum= G.nDigits= NIL;
  G.beginID= G.endID= NIL;
  G.start= 0;
  G.lang= NO_LANG;
  VOID_RET();
}

#define LOWER		"abcdefghijklmnopqrstuvwxyz"
#define P(s)		fputs(s, yyout)
#define COMMENT(x)	fprintf(yyout, formats[G.target].x, yytext)
#define SPECIAL(x)	fprintf(yyout, formats[G.target].x, yytext[0])

%}

%%
<YACC>"%"{keyword}		{ P("%"); 
				  keyword(&yytext[1], C_FLAG|YACC_FLAG); 
				}
<LEX>"%"{keyword}		{ P("%");
				  keyword(&yytext[1], C_FLAG|YACC_FLAG); 
				}
^{ws}*"#"{ws}*{keyword}		{ char *keyStart= strpbrk(yytext, LOWER);
				  char save= *keyStart;
				  *keyStart= '\0'; P(yytext); *keyStart= save;
				  keyword(keyStart, C_FLAG|CPP_FLAG); 
				}
{keyword}			{ keyword(yytext, C_FLAG); }
^"/* "{label}" */"{ws}*\n	{ if (defLabel(&yytext[3])) { /* wind up */
				    P("\n"); return 0; 
				  }
				}
{label}				{ defLabel(yytext); }
"/*"				{ COMMENT(commBegin); yy_push_state(COMM); }
\"				{ SPECIAL(dQuote); yy_push_state(STR); }
\'				{ SPECIAL(sQuote); yy_push_state(CHAR); }
"{"				|
<COMM,STR,CHAR>"{"		|
"}"				|
<COMM,STR,CHAR>"}"		{ SPECIAL(brace); }
\\				|
<COMM,STR,CHAR>\\		{ SPECIAL(backslash); }
\@				|
<COMM,STR,CHAR>\@		{ SPECIAL(at); }
\n/(.|\n)			|	
<COMM,STR,CHAR>\n/(.|\n)	{ G.srcLineNum++; ECHO; outLineNum(); }
<STR>\\\"			{ SPECIAL(backslash); SPECIAL(dQuote); }
<STR>\"				{ SPECIAL(dQuote); yy_pop_state(); }
<CHAR>\\\'			{ SPECIAL(backslash); SPECIAL(sQuote); }
<CHAR>\'			{ SPECIAL(sQuote); yy_pop_state(); }
<COMM>"*/"			{ COMMENT(commEnd); yy_pop_state(); }
<COMM>^{ws}*"* "		{ COMMENT(commStar); }
<COMM>{label}			{ defLabel(yytext); }
<SKIP>^"/* "{label}" */"{ws}*\n	{ if (defLabel(&yytext[3])) { /* begin label */
				    BEGIN G.start;
				  }
				  G.srcLineNum++;
				}
<SKIP>.
<SKIP>\n			{ G.srcLineNum++; }
				
%%

/*			STATIC DECLARATIONS				*/
static VOID initKeywords PROTO((VOID_ARGS));
static VOID spewLabels PROTO((VOID_ARGS));
static int targetOptFn PROTO((int id, VOIDP checkP, 
			      VOIDP stringP, ConstString argP));
static int langOptFn PROTO((int id, VOIDP checkP,
			    VOIDP stringP, ConstString argP));
static int labelOptFn PROTO((int id, VOIDP checkP, 
			     VOIDP stringP, ConstString argP));
static ConstString errorFile PROTO((VOID_ARGS));
static unsigned errorLine PROTO((VOID_ARGS));
static FILE *checkCommandLine PROTO((int argc, ConstString argv[]));


/*			   KEYWORDS AND LABELS				*/

typedef union {
  unsigned langs;	/* Which languages a keyword belongs to. */
  unsigned lineNum;	/* Defining output line number for a label. */
} IDVal;

static AREA(idVals, IDVal, 7, 0xFFFF);
#define ID_VALS(i) A_ACCESS(idVals, IDVal, i)

typedef struct {
  ConstString keyword;
  unsigned langs;
} InitKeyword;

static InitKeyword keywords[]= {
  { "auto", C_FLAG },				
  { "break", C_FLAG },				
  { "case", C_FLAG },				
  { "char", C_FLAG },				
  { "const", C_FLAG },				
  { "continue", C_FLAG },			
  { "default", C_FLAG },				
  { "do", C_FLAG },				
  { "double", C_FLAG },  
  { "else", C_FLAG|CPP_FLAG },    
  { "enum", C_FLAG },    
  { "extern", C_FLAG },  
  { "float", C_FLAG },   
  { "for", C_FLAG },     
  { "goto", C_FLAG },    
  { "if", C_FLAG|CPP_FLAG },      
  { "int", C_FLAG },     
  { "long", C_FLAG },    
  { "register", C_FLAG },
  { "return", C_FLAG },  
  { "short", C_FLAG },   
  { "signed", C_FLAG },  
  { "sizeof", C_FLAG },  
  { "static", C_FLAG },  
  { "struct", C_FLAG },   
  { "switch", C_FLAG },   
  { "typedef", C_FLAG },  
  { "union", C_FLAG|YACC_FLAG },    
  { "unsigned", C_FLAG }, 
  { "void", C_FLAG },     
  { "volatile", C_FLAG }, 
  { "while", C_FLAG },    
  { "define", CPP_FLAG },
  { "elif", CPP_FLAG }, 
  { "endif", CPP_FLAG }, 
  { "error", CPP_FLAG }, 
  { "include", CPP_FLAG }, 
  { "ifdef", CPP_FLAG }, 
  { "ifndef", CPP_FLAG },  
  { "line", CPP_FLAG|YACC_FLAG },
  { "pragma", CPP_FLAG }, 
  { "undef", CPP_FLAG }, 
  { "s", LEX_FLAG },
  { "x", LEX_FLAG },
  { "expect", YACC_FLAG },
  { "in", YACC_FLAG },
  { "left", YACC_FLAG },
  { "lexbegin", YACC_FLAG },
  { "lexdefs", YACC_FLAG },
  { "lexend", YACC_FLAG },
  { "nolook", YACC_FLAG }, 
  { "nonassoc", YACC_FLAG },
  { "option", YACC_FLAG },
  { "out", YACC_FLAG },
  { "prec", YACC_FLAG },
  { "pure_parser", YACC_FLAG },
  { "right", YACC_FLAG },
  { "start", YACC_FLAG },
  { "token", YACC_FLAG },
  { "test", YACC_FLAG },
  { "type", YACC_FLAG },
};

#define N_KEYWORDS	N_ELEMENTS(keywords)

static VOID
initKeywords()
{
  Count n= N_KEYWORDS;
  Index i;
  initID(FALSE);
  for (i= 0; i < n; i++) {
    Index j= getID(keywords[i].keyword, strlen(keywords[i].keyword));
    Index k= A_NEXT(idVals);
    assert(i == j); 
    assert(i == k);
    ID_VALS(k).langs= keywords[i].langs;
  }
  VOID_RET();
}

/* Define label lab (surrounded by LABEL_CHARs).  Return 1 if label is 
 * special begin or end label.
 */
static int
defLabel(lab)
  char *lab;
{
  char *label= &lab[1];
  char *p= strchr(label, LABEL_CHAR);
  Index id;
  assert(lab[0] == LABEL_CHAR);
  assert(p);
  *p= '\0';
  id= getID(label, p - label);
  if (id < N_KEYWORDS) {
    error("Keyword %s used as label.", label);
  }
  else {  
    if (id >= A_NENTRIES(idVals)) {
      assert(id == A_NENTRIES(idVals));
      A_NEXT(idVals);
    }
    ID_VALS(id).lineNum= G.outLineNum;
  }
  return (id == G.beginID || id == G.endID);
}

static VOID
keyword(text, langs)
  ConstString text;
  unsigned langs;
{
  Index id= getIDFail(text, strlen(text));
  fprintf( yyout, 
	   (id != NIL && id < N_KEYWORDS && (langs & ID_VALS(id).langs) != 0) 
	   ? formats[G.target].keyword
           : "%s",
           text
         );
  VOID_RET();
}

static VOID
spewLabels()
{
  if (G.outLineNum != NIL) {
    CONST Count n= A_NENTRIES(idVals);
    Index i;
    for (i= N_KEYWORDS; i < n; i++) {
      fprintf(yyout, formats[G.target].defLabel, 
	      getIDString(i), ID_VALS(i).lineNum);
    }		
  }
  VOID_RET();
}


/*			OUTPUT LINE NUMBERS  				*/

static VOID
outLineNum()
{
  if (G.outLineNum != NIL || G.nDigits != NIL) {
    G.outLineNum++;
    if (G.nDigits != NIL) {
      fprintf(yyout, "%0*d ", G.nDigits, G.outLineNum);
    }
    else {
      fprintf(yyout, "%d ", G.outLineNum);
    }
  }
  VOID_RET();
}


/*				ERROR FUNCTIONS				*/
static ConstString 
errorFile()
{
  return G.srcName;
}

static unsigned 
errorLine()
{
  return G.srcLineNum;
}


/*			COMMAND-LINE PROCESSING				*/
	
enum {
  BEGIN_OPT,
  DIGITS_OPT,
  END_OPT,
  LANG_OPT,
  NUMBER_OPT,
  TARGET_OPT
};

static OptInfo optTab[]= {
  OPT_ENTRY("begin", 'b', REQUIRED_OPT_FLAG, BEGIN_OPT, NULL, 
	    NULL, labelOptFn,
	    "\
--begin LABEL |   Start listing file from line after LABEL.\n\
 -b LABEL\n\
"),
  OPT_ENTRY("digits", 'd', REQUIRED_OPT_FLAG, DIGITS_OPT, NULL,
	    &G.nDigits, intOptFn1,
	    "\
--digits NUM |    Number of digits to output for line numbers.\n\
 -d NUM\n\
"),
  OPT_ENTRY("end", 'e', REQUIRED_OPT_FLAG, END_OPT, NULL, 
	    NULL, labelOptFn,
	    "\
--end LABEL |     List file only until line containing LABEL.\n\
 -e LABEL\n\
"),
  OPT_ENTRY("lang", 'l', REQUIRED_OPT_FLAG, LANG_OPT, NULL, 
	    NULL, langOptFn,
	    "\
--lang LANG |     Input file is written in language LANG (c, lex, yacc).\n\
 -l LANG          If no file name is specified, then the default is c, else\n\
                  it depends on the file extension.\n\
"),
  OPT_ENTRY("number", 'n', OPTIONAL_OPT_FLAG, NUMBER_OPT, NULL,
	    &G.outLineNum, intOptFn1,
	    "\
--number[=NUM] |  Number output lines starting at NUM (which defaults\n\
 -n[NUM]          to 1).\n\
"),
  OPT_ENTRY("target", 't', REQUIRED_OPT_FLAG, TARGET_OPT, NULL, 
	    NULL, targetOptFn,
	    "\
--target=TARGET | Specify TARGET: texinfo (default) or latex.\n\
 -t TARGET\n\n
"),

};

static int 
targetOptFn(id, checkP, stringP, argP)
  int id; 
  VOIDP checkP; 
  VOIDP stringP; 
  ConstString argP;
{
  if (strcmp(argP, "latex") == 0) {
    G.target= LATEX_TARGET;
    return 0;
  }
  else if (strcmp(argP, "texinfo") == 0) {
    G.target= TEXINFO_TARGET;
    return 0;
  }
  else return 1;  		
}

static int 
langOptFn(id, checkP, stringP, argP)
  int id; 
  VOIDP checkP; 
  VOIDP stringP; 
  ConstString argP;
{
  if (strcmp(argP, "c") == 0) {
    G.lang= C_LANG;
    return 0;
  }
  else if (strcmp(argP, "lex") == 0) {
    G.lang= LEX_LANG;
    return 0;
  }
  else if (strcmp(argP, "yacc") == 0) {
    G.lang= YACC_LANG;
    return 0;
  }
  else return 1;  		
}

static int 
labelOptFn(optID, checkP, stringP, argP)
  int optID; 
  VOIDP checkP; 
  VOIDP stringP; 
  ConstString argP;
{
  Index id;
  assert(optID == BEGIN_OPT || optID == END_OPT);
  if (!argP) return 1;
  id= getID(argP, strlen(argP));
  if (id < A_NENTRIES(idVals)) {
    error("Invalid %s label %s.", optID == BEGIN_OPT ? "begin" : "end", argP);
    return 1;
  }
  A_NEXT(idVals);
  if (optID == BEGIN_OPT) {
    G.beginID= id;
  }
  else {
    G.endID= id;
  }
  return 0;
}

static FILE *
checkCommandLine(argc, argv) 
  int argc;
  ConstString argv[];
{
  CONST Count nOpts= N_ELEMENTS(optTab);
  int err= 0;
  int fileArgN;
  FILE *f= stdin;
  fileArgN= parseOpt(NULL, argc, argv, optTab, nOpts);
  if (fileArgN < argc - 1) {
    if (fileArgN < 0) {
      int optN= ERR_RET_ARGN(fileArgN);
      error("Bad option %s%s.", ERR_RET_NEXTARG(fileArgN) ? "value for " : "",
	    argv[optN]);
    }
    else if (fileArgN < argc - 1) {
      error("Only one file name can be specified.");
    }
    allOptsHelp(NULL, "usage: c2tex [OPTIONS] [FILE]", 
		optTab, N_ELEMENTS(optTab));
    exit(1);
  }

  /* Setup outLineNum properly: start it at 1 if --digits specified. */
  if (G.nDigits != NIL && G.outLineNum == NIL) G.outLineNum= 1;
  if (G.outLineNum != NIL) G.outLineNum--; /* Overcome initial increment. */

  if (fileArgN == argc - 1) { /* File name specified. */
    ConstString srcName= argv[fileArgN];
    if (G.lang == NO_LANG && strlen(srcName) > 2) {
      Size srcNameLen= strlen(srcName);
      CONST Char cLast= srcName[srcNameLen - 1];
      CONST Char cLast1= srcName[srcNameLen - 2];
      G.lang=   (cLast == 'y' && cLast1 == '.' ) ? YACC_LANG
	      : (cLast == 'l' && cLast1 == '.' ) ? LEX_LANG
	      : C_LANG;
    }
    f= fopen(srcName, "r");
    if (!f) FILE_ERROR(srcName);
  }
  if (G.lang == NO_LANG) G.lang= C_LANG;
  if (G.lang != C_LANG) {
    G.start= G.lang == LEX_LANG ? LEX : YACC;
  }
  BEGIN (G.beginID == NIL) ? G.start : SKIP;
  return f;
}

int main PROTO((int argc, ConstString argv[]));
int main(argc, argv) 
  int argc;
  ConstString argv[];
{
  initGlobs();
  initError(NULL, NIL, NULL, errorFile, errorLine, NULL);
  initKeywords();
  yyin= checkCommandLine(argc, argv);
  yyout= stdout;

  fputs(formats[G.target].envBegin, yyout);
  outLineNum();
  yylex();
  fputs(formats[G.target].envEnd, yyout);
  spewLabels();
  return 0;
}
