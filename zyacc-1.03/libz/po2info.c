/*

File:	 parseopt.h
Purpose: Generate TeXInfo documentation from options table.

Last Update Time-stamp: <97/07/28 12:22:49 zdu>

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#define PO_OFFLINE_FMT 1

#include "optp.h"
#include "memalloc.h"
#include "zutil.h"


/* TeXInfo directives used. */
#define ITEM_DIR	"@item"
#define ITEMX_DIR	"@itemx"
#define VAR_DIR		"@var"
#define CODE_DIR	"@code"

static VOID outOptDoc PROTO((FILE *outFile, 
			     CONST OptInfo *optTab, Count nEntries,
			     Index i));
static Index outOptItems PROTO((FILE *outFile,
				CONST OptInfo *CONST optTab, Count nEntries,
				Index i));

/* Output option items for opt-table entries starting at i until non-synonym
 * option entry found.  Return its index.
 */
static Index
outOptItems(outFile, optTab, nEntries, i)
  FILE *outFile;
  CONST OptInfo *CONST optTab;
  Count nEntries;
  Index i;
{
  Index i1;	/* i1 will be index of non-SYNONYM entry. */
  for (i1= i; (optTab[i1].flags & ARG_TYPE_MASK) == SYNONYM_OPT_FLAG; i1++) ;
  {
    ConstString doc= optTab[i1].doc;
    ConstString tab1= strchr(doc, '\t');	/* Argument tab. */
    CONST Size argLen= tab1 - doc;
    ConstString arg= (tab1 == doc) ? NULL : toLowerStringN(doc, argLen);
    Count nItems= 0;
    CONST Boolean isOptional=
      (optTab[i1].flags & ARG_TYPE_MASK) == OPTIONAL_OPT_FLAG;
    Index j;
    for (j= i; j <= i1; j++) { /* Print items. */
      Char s[2];	/* For converting short option char to a string. */
      Index k;
      s[1]= '\0';
      for (k= 0; k < 2; k++) { /* k == 0: longOpt; k == 1: shortOpt. */
	ConstString opt= (k == 0) ? optTab[j].longOpt 
	  : (optTab[j].shortOpt == '\0' ? NULL : s);
	if (opt) {
	  if (k) s[0]= optTab[j].shortOpt;
	  fprintf(outFile, "%s %c%s%s", (nItems) ? ITEMX_DIR : ITEM_DIR, 
		  OPT_CHAR, (k) ? "" : OPT_CHAR_STRING, opt);
	  nItems++;
	  if (arg) {
	    fprintf(outFile, "%c", isOptional ? '[' : ' ');
	    if (isOptional && k == 0) fprintf(outFile, "=");
	    if (isalpha(arg[0])) {
	      fprintf(outFile, "%s{%s}", VAR_DIR, arg);
	    }
	    else {
	      fprintf(outFile, "%s", arg);
	    }
	    if (isOptional) fprintf(outFile, "%c", ']');
	  }
	  fprintf(outFile, "\n");
	}
      }
    } /* for (j= i; j <= i1; j++) */
  }
  return i1;
}


/* Output document string for optTab[i] to outFile substituting format
 * specifiers with suitable texinfo directives.  Will break lines at
 * ' ' or at format specifiers.
 */
static VOID 
outOptDoc(outFile, optTab, nEntries, i)
  FILE *outFile;
  CONST OptInfo *optTab;
  unsigned nEntries;
  Index i;
{
  enum { LINE_WIDTH= 72 };
  typedef enum {
    ARG_HELP,		/* Need to process %A format. */
    DEFAULT_HELP,	/* Need to output default value. */
    HI_BOUND_HELP,	/* Need to process %H format. */
    INT_VAL_HELP,	/* In %I format. */
    LO_BOUND_HELP,	/* Need to process %L format. */
    NORMAL_HELP,	/* Within doc string, not within fmt, arg or def. */
    STRING_VAL_HELP	/* In %S format. */
  } HelpState;
  CONST Size varDirLen= strlen(VAR_DIR) + 2; /* +2 for { and }. */
  CONST Size codeDirLen= strlen(CODE_DIR) + 2; /* +2 for { and }. */
  ConstString doc= optTab[i].doc;
  ConstString tab1= strchr(doc, '\t');	/* Argument tab. */
  CONST Int argLen= tab1 - doc;
  ConstString arg= (tab1 == doc) ? NULL : toLowerStringN(doc, argLen);
  HelpState helpState= NORMAL_HELP;
  ConstString p= tab1 + 1;
  VOIDP checkP= optTab[i].checkP;
  Index checkPIndex;
  assert(doc != NULL);
  while (*p != '\0') {	/* Print multiple lines until done. */
    Count len= 0;	/* Length of current line. */
    Boolean done= FALSE;
    while (!done) {	/* Print until one line done. */
      switch (helpState) {
	case ARG_HELP: {
	  Size totalLen= argLen + varDirLen;
	  if (len + totalLen < LINE_WIDTH) {
	    fprintf(outFile, "%s{%.*s}", VAR_DIR, argLen, arg);
	    helpState= NORMAL_HELP; len+= totalLen;
	  }
	  else done= TRUE;
	  break;
	}
	case DEFAULT_HELP: {
	  ConstString def= p;
	  Size defLen= strlen(def);
	  Size totalLen= defLen 
	    /* ( default ": "  @code          def      )   . */
	    ? (1 +  7 +   2  + codeDirLen +  defLen  + 1 + 1)
	    /*  ( default )   . */
	    : ( 1 +  7  + 1 + 1);
	  if (len + totalLen < LINE_WIDTH) {
	    if (defLen) {
	      fprintf(outFile, "(default: %s{%s}).", CODE_DIR, def);
	    }
	    else {
	      fprintf(outFile, "(default).");
	    }
	    p+= defLen; len+= totalLen;
	  }
	  done= TRUE;
	  break;
	}
	case LO_BOUND_HELP:
	case HI_BOUND_HELP: {
	  CONST Long *ranges= (CONST Long *)checkP;
	  CONST Long val= ranges[checkPIndex];
	  CONST Size totalLen= codeDirLen + longPrintLen(val, 10);
	  assert(ranges[0] == 0);
	  assert(checkPIndex == 1 || checkPIndex == 2);
	        /* loBound		hiBound		*/
	  if (len + totalLen < LINE_WIDTH) {
	    fprintf(outFile, "%s{%ld}", CODE_DIR, val);
	    len+= totalLen; helpState= NORMAL_HELP;
	  }
	  else done= TRUE;
	  break;
	}
	case INT_VAL_HELP: {
	  CONST Long *values= (CONST Long *)checkP;
	  CONST Long nValues= values[0] - 1;
	  CONST Long val= values[checkPIndex];
	  CONST Boolean isSecondLast= (checkPIndex == nValues - 1);
	  CONST Boolean isLast= (checkPIndex == nValues);
	  CONST Size totalLen= 1 + codeDirLen + longPrintLen(val, 10) + 1 + 
	    (isLast ? 0 : (isSecondLast) ? 4 : 2);
	  assert(nValues > 1);
	  assert(checkPIndex < nValues + 1);
	  if (len + totalLen < LINE_WIDTH) {
	    fprintf(outFile, "%s{%ld}%s", CODE_DIR, val, 
		    isLast ? "" : (isSecondLast ? " or " : ", "));
	    len+= totalLen; 
	    checkPIndex++;
	    if (isLast) helpState= NORMAL_HELP;
	  }
	  else done= TRUE;
	  break;
	}
	case NORMAL_HELP: {
	  ConstString srcCut= p;
	  ConstString src;
	  for (src= p;
	       *src != '\0' && *src != '\t' && *src != '%' 
		 && len + (src - p) < LINE_WIDTH;
	       src++) {
	    if (*src == ' ') { srcCut= src; }
	  }
	  if (*src == '\0') { /* This may exceed limit by 1. */
	    fprintf(outFile, "%.*s.", src - p, p); 
	    len+= src - p; p= src; done= TRUE; 
	  }
	  else if (*src == '\t') {
	    fprintf(outFile, "%.*s ", src - p, p); /* May overflow line by 1. */
	    len+= src - p + 1; p= src + 1; helpState= DEFAULT_HELP;
	  }
	  else if (*src == '%') {
	    fprintf(outFile, "%.*s", src - p, p);
	    len+= src - p; p= src + 2;
	    switch (src[1]) {
	      case 'A':
		helpState= ARG_HELP; break;
	      case 'H':
		checkPIndex= 2; helpState= HI_BOUND_HELP; 
		break;
	      case 'I':
		checkPIndex= 1; helpState= INT_VAL_HELP; 
		break;
	      case 'L':
		checkPIndex= 1; helpState= LO_BOUND_HELP;
		break;
	      case 'S':
		checkPIndex= 0; 
		helpState= STRING_VAL_HELP;
		break;
	      case '%':	
		fprintf(outFile, "%%"); 	/* May exceed limit by 1. */
		break;	
	    }
	  } /* else if (*src == '%') */
	  else { /* Must have had limit exceeded. */
	    fprintf(outFile, "%.*s", srcCut - p, p);
	    len+= srcCut - p; p= srcCut + 1;
	    done= TRUE;
	  }
	  done|= (len >= LINE_WIDTH);
	  break;
	} /* case NORMAL_HELP: */
	case STRING_VAL_HELP: {
	  ConstString *values= (ConstString *)checkP;
	  ConstString val= values[checkPIndex];
	  CONST Boolean isSecondLast= 
	    values[checkPIndex + 2] == NULL;
	  CONST Boolean isLast= values[checkPIndex + 1] == NULL;
	  CONST Size totalLen= 1 + codeDirLen + strlen(val) + 1 + 
	    (isLast ? 0 : (isSecondLast ? 4 : 2));
	  assert(values[0] != NULL && values[1] != NULL);
	  if (len + totalLen < LINE_WIDTH) {
	    fprintf(outFile, "%s{%s}%s", CODE_DIR, val, 
		    isLast ? "" : (isSecondLast ? " or " : ", "));
	    len+= totalLen; 
	    checkPIndex++;
	    if (isLast) helpState= NORMAL_HELP;
	  }
	  else done= TRUE;
	  break;
	}
      } /* switch (helpState) */
    } /* while (!done) */
    fprintf(outFile, "\n");
  } /* while (*p != '\0') */
  if (optTab[i].longOpt) fprintf(outFile, "@xref{%s}.\n", optTab[i].longOpt);
  VOID_RET();
} 

/* This routine is useful for automatically generating TeXInfo
 * documentation: It prints @table entries for all the nEntries
 * options in optInfo on stdout.  Only the option names, the argument
 * type and the doc entries are looked at.  A bit of a mess, but only
 * used offline.
 */
VOID 
optTab2TeXInfo(outFile, optTab, nEntries)
  FILE *outFile;
  CONST OptInfo *optTab;
  unsigned nEntries;
{
  Index i;
  for (i= 0; i < nEntries; i++) {
    i= outOptItems(outFile, optTab, nEntries, i);
    outOptDoc(outFile, optTab, nEntries, i);
    fprintf(outFile, "\n");
  }
  VOID_RET();
}
