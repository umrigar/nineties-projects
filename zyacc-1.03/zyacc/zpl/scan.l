/*			   PROLOG SCANNER.				*/

/*

Requires zlex because of spacing and comments.

Specified as per Quintus Release 3.0 manual.
   
For a practical system used as part of a Prolog compiler, the only
major addition required would be to add a buffer-stack to handle
nested consults.

We use the doEsc flag to goto different start-states with different
handling of escape-sequences.

*/


/*			   LEX MACROS.					*/

digit		[0-9]
octet		[0-7]
hexet		[0-9a-fA-F]				
uAlpha		[A-Z]
lAlpha		[a-z]
alpha		({uAlpha}|{lAlpha})
alphaNum 	({alpha}|{digit})
lay		[\x01-\x20]			/* Layout char. */
layNotNL	[\x01-\x09\x0B-\x20]		/* Non-'\n' layout char. */
nonLay		[\x21-\x7E]			/* Non-layout char. */
soloChar	[!;]
eQChar		[\x21-\x26\x28-\x7E]		/* Char in quote with esc. */
octEsc		(\\{octet}{1,3})		/* Octal esc-seq. */
hexEsc		(\\x{hexet}{hexet}?)		/* Hex esc-seq. */
escChar		(\\(.|\n))			/* Escaped char. */
ctlEsc		(\\\^(.|\n))			/* Control-escape. */
layEsc		(\\c{lay}*)			/* Layout escape. */
escSeq		({escChar}|{octEsc}|{hexEsc}|	/* Possible escape sequence */
		 {ctlEsc})			/* alternatives (not layout). */
qChar		([^\']|\'\')			/* !Esc quoted name char. */	
sChar		([^\"]|\"\")			/* String char (no esc). */
symChar		[-\\+^<>=~:.?@$&/*]		/* Symbol char. */
symCharNotSlash	[-\\+^<>=`~:.?@$&*]		/* Symbol char but not '/. */
symCharNotStar	[-\\+^<>=~:.?@$&/]		/* Symbol char. */
symbol		(\/ |				/* A single slash. */
		 \/{symCharNotStar}{symChar}* | /* Slash non-star sym-char*. */
		 {symCharNotSlash}{symChar}*	/* Non-slash, sym-char*. */
		)				/* A symbol. */
idName		({lAlpha}({alphaNum}|_)*)	/* Unquoted identifier name. */
var		(({alpha}|_)({alphaNum}|_)*)	/* Variable name. */
natNum		({digit}+)			/* Natural number. */
realNum		({digit}+\.{digit}+		/* A simple float */
		 ((e|E){digit}+)?		/* followed by optional exp */
		)				/* is a real number. */
		 

/*			EXCLUSIVE START STATES.				*/

%x COMMENT		/* Within a /* comment. */
%x NUM_ESC_OFF		/* Within a 0x number with escape-chars flag off. */
%x NUM_ESC_ON		/* Within a 0'x number with escape-chars flag on. */
%x QUOTE_ESC_OFF	/* Within a quoted name with escape-chars flag off. */
%x QUOTE_ESC_ON		/* Within a quoted name with escape-chars flag on. */
%x STR_ESC_OFF		/* Within a string with escape-chars flag off. */
%x STR_ESC_ON		/* Within a string with escape-chars flag on. */
%x BASED_INT		/* Within a based integer. */


/*			C DECLARATIONS FOR SECTION 1. 			*/
%{

#include "scan.h"

#include "error.h"
#include "strspace.h"
#include "textid.h"

static struct {
  StrSpace ss;		/* Space for name-strings from strSpace.h. */
  TextID nameTab;	/* Name-table from textID.h. */
  Count2 lineNum;	/* Current line # in source file. */
  Count2 startLineN;	/* Line # where last multi-line token started. */
  Boolean doEsc;	/* TRUE if escape sequences are to be processed. */
} globs;

#define G globs
#define VAL yylval
#define T yytext

static VOID eofError(ConstString construct);
static Int escChar(Char c);
static Int escSeq(ConstString *escP);
static ID quotedNameEsc(String text, Count len);
static ID quotedNameNoEsc(String text, Count len);
static ConstString layEsc(ConstString text);
static int basedInt(unsigned base, char *text);

#define CTL_ESC(c)	((c) % 32)
#define IS_OCTET(c)	('0' <= c && c <= '7')
#define IS_HEXET(c)	(isdigit(c) || ('a' <= tolower(c) && tolower(c) <= 'f'))

#ifdef	 TEST_SCAN

#define TOK(X) ( (int) #X)
#define CTOK(X) ( (int) (X))

union {
  struct { Index2 id; } name;
  struct { long num; } natNum;
  struct { double num; } realNum;
} VAL;

#else

#include "parse.h" /* for yylval and token declarations. */

#define TOK(X) X ## _TOK
#define CTOK(X) X

#endif

/* Field selectors for semantic value. */

#define NAME_S	name.id
#define NAT_S	natNum.num
#define REAL_S	realNum.num

%}


/*			      LEX PATTERNS.				*/

%%

/* Local variables within yylex(). */

  unsigned base;		/* Base for nn'NUM type based-numbers. */


\n			{ G.lineNum++; }
{layNotNL}+		/* Ignore layout-chars (except for newline). */ 
^"("			{ return TOK(WS_LPAREN); }
{layNotNL}+"("		{ return TOK(WS_LPAREN); }
"%".+			/* Ignore % to end-of-line comments. */
{idName}		|
{symbol}		|
{soloChar}		|
"{}"			|
"[]"		        { VAL.NAME_S= getName(T, yyleng); return TOK(NAME); }
{var}			{ VAL.NAME_S= getName(T, yyleng); return TOK(VAR); }
{natNum}		{ sscanf(T, "%ld", &VAL.NAT_S); return TOK(NAT_NUM); }
{realNum}		{ sscanf(T, "%lg", &VAL.REAL_S); return TOK(REAL_NUM); }
"."{lay}		{ return TOK(FULL_STOP); }
"-"/{digit}             { return TOK(NUM_MINUS); }
0\'			{ G.startLineN= G.lineNum; 
			  BEGIN ((G.doEsc) ? NUM_ESC_ON : NUM_ESC_OFF); 
			}
{natNum}\'		{ sscanf(T, "%d", &base); BEGIN BASED_INT; }
\'			{ G.startLineN= G.lineNum; 
			  BEGIN ((G.doEsc) ? QUOTE_ESC_ON : QUOTE_ESC_OFF); 
			}
\"			{ G.startLineN= G.lineNum; 
			  BEGIN ((G.doEsc) ? STR_ESC_ON : STR_ESC_OFF); 
			}
"/*"			{ G.startLineN= G.lineNum; BEGIN COMMENT; }
.			{ return CTOK(T[0]); }

<BASED_INT>{alphaNum}+	{ VAL.NAT_S= basedInt(base, T); 
			  BEGIN INITIAL; return TOK(NAT_NUM); 
			}
<BASED_INT>.		{ error("Invalid based int at %c.", T[0]);
			  BEGIN INITIAL;
			}

<COMMENT>"*/"		{ BEGIN INITIAL; }
<COMMENT>"/"/"*"[^/]	{ warn("/* within comment."); }
<COMMENT>\n		{ G.lineNum++; }
<COMMENT>"*"+/[^/]	/* Allow *'s if not followed by /. */
<COMMENT>"/"+/[^*]	/* Allow /'s if not followed by *. */
<COMMENT>[^\n*/]+
<COMMENT><<EOF>>	{ eofError("comment"); return 0; }

<NUM_ESC_OFF>.		{ BEGIN INITIAL; VAL.NAT_S= T[0]; return TOK(NAT_NUM); }
<NUM_ESC_OFF>\n		{ BEGIN INITIAL; G.lineNum++;
			  VAL.NAT_S= T[0]; return TOK(NAT_NUM); 
			}
<NUM_ESC_OFF><<EOF>>	{ eofError("quoted character"); }

<NUM_ESC_ON>( {nonLay} | [ \t] )
			{ BEGIN INITIAL; VAL.NAT_S= T[0]; return TOK(NAT_NUM); }
<NUM_ESC_ON>{layEsc}	{ layEsc(&T[2]); }
<NUM_ESC_ON>{escSeq}	{ ConstString p= T; BEGIN INITIAL;
			  VAL.NAT_S= escSeq(&p); return TOK(NAT_NUM); 
			}
<NUM_ESC_ON>.		{ error("Invalid quoted character %c.", T[0]); 
			  BEGIN INITIAL; 
			}
<NUM_ESC_ON>\n		{ error("Invalid newline quoted character."); 
			  G.lineNum++; BEGIN INITIAL; 
			}
<NUM_ESC_ON><<EOF>>	{ eofError("quoted character"); }

<QUOTE_ESC_OFF>\'	{ BEGIN INITIAL; }
<QUOTE_ESC_OFF>( [^\'] | \'\' )+
  			{ VAL.NAME_S= quotedNameNoEsc(T, yyleng);
			  return TOK(NAME);
			}
<QUOTE_ESC_OFF><<EOF>>	{ eofError("quoted name"); }

<QUOTE_ESC_ON>\'	{ BEGIN INITIAL; }
<QUOTE_ESC_ON>( {eQChar} | {layEsc} | {escSeq} | [ \t] | \'\'  )+
                        { VAL.NAME_S= quotedNameEsc(T, yyleng); 
			  return TOK(NAME); 
			}
<QUOTE_ESC_ON>.		{ error("Invalid character %c within quoted name.", 
				T[0]); 
			  BEGIN(INITIAL); return TOK(LEX_ERR); 
			}
<QUOTE_ESC_ON>\n       	{ error("Invalid newline within quoted name."); 
			  G.lineNum++; BEGIN(INITIAL); return TOK(LEX_ERR); 
			}
<QUOTE_ESC_ON><<EOF>>	{ eofError("quoted name"); }

<STR_ESC_OFF>\"		{ BEGIN INITIAL; }
<STR_ESC_OFF>\"\"	{ VAL.NAT_S= '\"'; return TOK(STR_CHAR); }
<STR_ESC_OFF>.		{ VAL.NAT_S= T[0]; return TOK(STR_CHAR); }
<STR_ESC_OFF>\n		{ G.lineNum++; VAL.NAT_S= '\n'; return TOK(STR_CHAR); }
<STR_ESC_OFF><<EOF>>	{ eofError("string"); }
  
<STR_ESC_ON>\"		{ BEGIN INITIAL; }
<STR_ESC_ON>\"\"	{ VAL.NAT_S= '\"'; return TOK(STR_CHAR); }
<STR_ESC_ON>{layEsc}	{ layEsc(&T[2]); }
<STR_ESC_ON>{escSeq}	{ ConstString p= T;
			  VAL.NAT_S= escSeq(&p); return TOK(STR_CHAR); 
			}
<STR_ESC_ON>( {nonLay} | [ \t] )
			{ VAL.NAT_S= T[0]; return TOK(STR_CHAR); }
<STR_ESC_ON>.		{ error("Invalid character %c within string.", T[0]);
			  BEGIN INITIAL; return TOK(LEX_ERR);
			}
<STR_ESC_ON>\n		{ error("Invalid newline within string.");
			  G.lineNum++; BEGIN INITIAL; return TOK(LEX_ERR);
			}
<STR_ESC_ON><<EOF>>	{ eofError("string"); }
	       
%%

Index getName(ConstString text, Count len)
/* Return ID from G.nameTab corresponding to text.  Add if not present. */
{
  Count len1= len + 1; /* Account for '\0' terminator. */
  ID id= getTextID(G.nameTab, text, len1);
  if (id == NIL) { /* Not present. */
    id= addTextID(G.nameTab, addStrSpace(G.ss, text, len1), len1);
  }
  return id;
}

ConstString getNameString(Index id)
{
  return idText(G.nameTab, id);
}

static VOID eofError(ConstString construct)
{
  error("EOF within %s starting at line %d.", construct, G.startLineN);
}

static Int escChar(Char c)
/* Translate escape char c to its escape code. */
{
  switch (c) { /* Switch rather than table to facilitate non-Ascii chars. */
    case 'a': return '\a';	/* Alert. */
    case 'b': return '\x08';	/* Backspace. */
    case 'c': INTERNAL_ERROR();	/* Continue escape handled elsewhere. */
    case 'd': return '\x7F';	/* Delete. */
    case 'e': return '\x1B';	/* Escape. */
    case 'f': return '\f';	/* Form-feed. */
    case 'n': return '\n';	/* Newline. */
    case 'r': return '\r'; 	/* Carriage-return. */
    case 't': return '\t';	/* Horizontal tab. */
    case 'v': return '\v';	/* Vertical tab. */
    case '\n': G.lineNum++; 
    /* No break. */
    default: return c;
  }
}

static ConstString layEsc(ConstString text)
{
  ConstString p;
  for (p= text; 0 < *p && *p <= ' '; p++) {
    if (*p == '\n') G.lineNum++;
  }
  return p;
}

static Int escSeq(ConstString *escP)
/* Process the escape sequence whose address is in escP.  Update *escP
 * to point just after the escape sequence.   Return the value of the
 * sequence.  This should not be used for layout escapes "\c".
 */
{
  ConstString src= *escP;
  Char c= src[1];
  Index v;
  assert(*src == '\\');
  assert(src[1] != 'c');	/* Not a layout escape. */
  if (IS_OCTET(c)) {
    Count n;
    v= 0;
    for (src= src + 1, n= 0; IS_OCTET(*src) && n < 3; src++, n++) {
      v= v * 8 + (*src - '0');
    }
  }
  else if (c == 'x' && IS_HEXET(src[2])) {
    Count n;
    v= 0;
    for (src= src + 2, n= 0; IS_HEXET(*src) && n < 2; src++, n++) {
      v= v*16 + toupper(*src) - (isdigit(*src) ? '0' : 'A' - 10);
    }
  }
  else if (c == '^') {
    v= CTL_ESC(src[2]); src+= 3;
  }
  else {
    v= escChar(c); src+= 2;
  }
  *escP= src; return v;
}

static ID quotedNameEsc(String text, Count len)
{
  ConstString src;
  String dest;
  ConstString end= text + len;
  for (src= dest= text; src < end;) {
    Char c= *src;
    switch (c) {
      case '\'':
        assert(*(src + 1) == '\''); *dest++= *src; src+= 2;
	break;
      case '\\': 
	if (src[1] == 'c') { /* Layout escape "\c". */
	  src= layEsc(&src[2]);
	} 
	else {
	  *dest++= escSeq(&src);
	}
	break;
      default:
	*dest++= *src++;
	break;
    }
  }
  *dest= '\0';
  return getName(text, dest - text);
}

static ID quotedNameNoEsc(String text, Count len)
{
  String src, dest;
  String end= text + len;
  for (src= dest= text; src < end;) {
    Char c= *src;
    switch (c) {
      case '\'':
        assert(*(src + 1) == '\''); *dest++= *src; src+= 2;
	break;
      case '\n': 
	G.lineNum++;
	/* No break */
      default:
	*dest++= *src++;
	break;
    }
  }
  *dest= '\0';
  return getName(text, dest - text);
}

static int basedInt(unsigned base, char *text) {
  Char *p;
  Int num= 0;
  if (base == 0 || base > 36) {
    error("Invalid base %d in number %s.", base, text);
  }
  else {
    for (p= text; *p != '\0'; p++) {
      Int v= toupper(*p) - (isdigit(*p) ? '0' : 'A' - 10);
      if (v >= base) {
	error("Digit %c in number %s greater than base %d.", *p, text, base);
	break;
      }
      else
	num= num * base + v;
    }
  }
  return num;
}


/*				PARSE ERROR.				*/

VOID yyerror(ConstString s)
{
  printf("%s near `%s'.", s, yytext);
}

/*			INITIALIZATION. 				*/

static unsigned errorLineN(VOID_ARGS)
{
  return G.lineNum;
}

static ConstString errorFile(VOID_ARGS)
{
  return "<stdin>";
}


VOID initScan(VOID_ARGS)
{
  G.lineNum= 1;
  G.ss= initStrSpace(FALSE);
  G.nameTab= newTextID(FALSE);
  initError(NULL, NIL, NULL, errorFile, errorLineN, NULL);
}


/*			TEST PROGRAM.					*/

#ifdef TEST_SCAN

#include <string.h>

int main(int argc) {
/* Test for yylex.  Tries to print token, VAL and lexeme.  Processes
 * escapes if argc > 1.
 */

  int t;
  initScan();
  G.doEsc= (argc > 1);

  while ( (t = yylex()) ) {
    if (t < 256)  /* Assume that string pointers > 256. */
      printf("Token = %c\n", t);
    else {
      char *ct= (char *)t;
      if (strcmp(ct, "NAT_NUM") == 0 || strcmp(ct, "STR_CHAR") == 0) 
	printf("Token = %s; Value= %ld.\n", ct, VAL.NAT_S);
      else if (strcmp(ct, "NAME") == 0 || strcmp(ct, "VAR") == 0) 
	printf("Token = %s; ID= %d; Stored text= %s.\n", ct, VAL.NAME_S, 
	       idText(G.nameTab, VAL.NAME_S));
      else if (strcmp(ct, "REAL_NUM") == 0)
	printf("Token = REAL; Value= %g.\n", VAL.REAL_S);
      else
	printf("Token = %s.\n",  ct);
    }
  }
  return 0;
}

#endif

