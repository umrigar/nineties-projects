/*

File:	 pohlpfmt.c
Purpose: Offline formatting of help messages.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#define PO_OFFLINE_FMT	1

#include "optp.h"
#include "memalloc.h"
#include "zutil.h"

/* Since this module is used offline, efficiency is not a major concern. */


struct HdrState;
struct DocState;

static Index initHdrState PROTO((Index i, 
				 CONST OptInfo *CONST optInfo, Count nEntries,
				 struct HdrState *hdrStateP));
static Boolean isHdrDone PROTO((CONST struct HdrState *hdrStateP));
static VOID nextHdrState PROTO((struct HdrState *hdrStateP));
static Count buildNextHdr PROTO((Char *text, Size maxWidth, BooleanX doForce,
				 struct HdrState *CONST hdrStateP));
static Count buildHdrLine PROTO((Char *text, Size maxWidth,
				 struct HdrState *hdrStateP));
static VOID initDocState PROTO((Index i, 
				CONST OptInfo *CONST optTab, Count nEntries,
				struct DocState *docStateP));
static Boolean isDocDone PROTO((CONST struct DocState *docStateP));
static Count buildDocLine PROTO((Char *text, Size maxWidth, 
				 struct DocState *docStateP));




/*			HELP HEADER PRINT				*/

typedef struct HdrState {/* Print state of a header. */
  CONST OptInfo *current;/* Current option-table header being printed. */
  CONST OptInfo *hi;	/* Last option-table header to be printed. */
  ConstString arg;	/* Point to argument. */
  UInt1 argLen;		/* # of characters in argument. */
  UInt1 indent;		/* # of chars subsequent header should be indented. */
  Boolean isLong;	/* TRUE if long-opt in current entry. */
  Boolean isOptional;	/* TRUE if argument is optional. */
} HdrState;

static Index
initHdrState(i, optInfo, nEntries, hdrStateP)
  Index i;
  CONST OptInfo *CONST optInfo;
  Count nEntries;
  HdrState *hdrStateP;
{
  Index lo, hi;
  for (lo= i; 
       lo > 0 && (optInfo[lo - 1].flags & ARG_TYPE_MASK) == SYNONYM_OPT_FLAG; 
       lo--) {
  }
  for (hi= i; (optInfo[hi].flags & ARG_TYPE_MASK) == SYNONYM_OPT_FLAG; i++) ;
  hdrStateP->current= &optInfo[lo];
  hdrStateP->hi= &optInfo[hi];
  hdrStateP->isLong= (optInfo[lo].longOpt != NULL);
  hdrStateP->indent= 0;
  hdrStateP->arg= optInfo[hi].doc;
  hdrStateP->argLen= strchr(optInfo[hi].doc, '\t') - optInfo[hi].doc;
  hdrStateP->isOptional= 
    (optInfo[hi].flags & ARG_TYPE_MASK) == OPTIONAL_OPT_FLAG;
  return hi;
}

static Boolean
isHdrDone(hdrStateP)
  CONST HdrState *hdrStateP;
{
  return hdrStateP->current == NULL;
}

/* Advance to the next header state.  Set current to NULL if done.
 * Assumes that each option table entry contains at least 1 long or
 * short option.
 */
static VOID
nextHdrState(hdrStateP)
  HdrState *hdrStateP;
{
  if (hdrStateP->isLong && hdrStateP->current->shortOpt != '\0') {
    hdrStateP->isLong= FALSE;
  }
  else if (hdrStateP->current == hdrStateP->hi) {
    hdrStateP->current= NULL;
  }
  else {
    hdrStateP->current++;
    hdrStateP->isLong= hdrStateP->current->longOpt != NULL;
  }
  hdrStateP->indent= 2;
}

/* Build next header in text, if it is possible to do so within
 * maxWidth, unless doForce is TRUE in which case maxWidth is ignored.
 * Return the # of characters output.
 */
static Count
buildNextHdr(text, maxWidth, doForce, hdrStateP)
  Char *text;
  Size maxWidth;
  BooleanX doForce;
  struct HdrState *CONST hdrStateP;
{
  Char shortOpt[2];	/* To convert shortOpt name to a string. */
  CONST OptInfo *CONST current= hdrStateP->current;
  CONST Boolean isLong= hdrStateP->isLong;
  CONST Boolean hasArg= hdrStateP->argLen > 0;
  CONST Boolean isOptional= hdrStateP->isOptional;
  CONST Size nameLen= isLong ? strlen(current->longOpt) : 1;
  ConstString optChars= isLong ? "--" : "-";
  ConstString optName= isLong ? current->longOpt : shortOpt;
  ConstString sep= (isOptional || !hasArg) ? "" : " ";
  ConstString leftOptional= (isOptional && hasArg) ? "[" : "";
  ConstString equals= (isOptional && isLong && hasArg) ? "=" : "";
  ConstString rightOptional= (isOptional && hasArg) ? "]" : "";
  ConstString end= 
    (hdrStateP->current != hdrStateP->hi 
     || (hdrStateP->isLong && current->shortOpt != '\0')
    )
    ? " | "
    : " ";
  CONST Size totalLen= 
    strlen(optChars) + nameLen + strlen(sep) + strlen(leftOptional) + 
    strlen(equals) + hdrStateP->argLen + strlen(rightOptional) + strlen(end);
  assert(hdrStateP->current != NULL);
  if (doForce || totalLen < maxWidth) {
    if (!isLong) {
      shortOpt[0]= current->shortOpt; shortOpt[1]= '\0';
    }
    sprintf(text, "%s%s%s%s%s%.*s%s%s", optChars, optName, sep, leftOptional, 
	   equals, hdrStateP->argLen, hdrStateP->arg, rightOptional, end);
    return totalLen;
  }
  else {
    return 0;
  }
}

static Count
buildHdrLine(text, maxWidth, hdrStateP)
  Char *text;
  Size maxWidth;
  HdrState *hdrStateP;
{
  Boolean doForce= TRUE;
  Char *limit= text + maxWidth;
  Char *dest= text;
  if (hdrStateP->current) {
    sprintf(dest, "%*s", hdrStateP->indent, ""); 
    dest+= hdrStateP->indent;
    do {
      CONST Count nOut= 
	buildNextHdr(dest, limit - dest, doForce, hdrStateP);
      doForce= FALSE;
      if (nOut == 0) break;  	/* Could not fit within maxWidth. */
      dest+= nOut;
      nextHdrState(hdrStateP);
    } while (hdrStateP->current);
  }
  return dest - text;
}


/*			  OUTPUT DOC STRING				*/

typedef enum {
  ARG_HELP,		/* Need to process %A format. */
  DEFAULT_HELP,		/* Need to output default value. */
  HI_BOUND_HELP,	/* Need to process %H format. */
  INT_VAL_HELP,		/* In %I format. */
  LO_BOUND_HELP,	/* Need to process %L format. */
  NORMAL_HELP,		/* Within doc string, not within fmt, arg or def. */
  STRING_VAL_HELP	/* In %S format. */
} HelpState;

typedef struct DocState {
  ConstString doc;	/* Current position in doc. string; NULL when done. */
  ConstString arg;	/* Argument. */
  VOIDP checkP;		/* check entry. */
  UInt1 checkPIndex;	/* Index within checkP[] array. */
  UInt1 argLen;
  UInt1 helpState;
} DocState;


static VOID 
initDocState(i, optTab, nEntries, docStateP)
  Index i;
  CONST OptInfo *CONST optTab;
  Count nEntries;
  DocState *docStateP;
{
  ConstString doc= optTab[i].doc;
  VOIDP checkP= optTab[i].checkP;
  ConstString argEnd= strchr(doc, '\t');
  assert(i < nEntries);
  docStateP->doc= argEnd + 1;
  docStateP->arg= doc; docStateP->argLen= argEnd - doc;
  docStateP->checkP= checkP; docStateP->checkPIndex= 0;
  docStateP->helpState= NORMAL_HELP;
  VOID_RET();
}

static Boolean
isDocDone(docStateP)
  CONST DocState *docStateP;
{
  return docStateP->doc == NULL;
}


/* We assume here that the length of a "word" is < the LINE_WIDTH - HDR_WIDTH.
 * This ensures that if it doesn't fit on the current line it is guaranteed
 * to fit on the next line.
 */
static Count
buildDocLine(text, maxWidth, docStateP)
  Char *text;
  Size maxWidth;
  DocState *docStateP;
{
  Char *dest= text;
  if (docStateP->doc != NULL) {
    CONST Char *CONST limit= text + maxWidth;
    Boolean done= FALSE;
    while (!done) { 
      switch (docStateP->helpState) {
	case ARG_HELP: {
	  Size totalLen= docStateP->argLen;
	  if (dest + docStateP->argLen < limit) {
	    sprintf(dest, "%.*s", docStateP->argLen, docStateP->arg);
	    docStateP->helpState= NORMAL_HELP; dest+= totalLen;
	  }
	  else done= TRUE;
	  break;
	}
	case DEFAULT_HELP: {
	  ConstString def= docStateP->doc;
	  Size defLen= strlen(def);
	  Size totalLen= (defLen)
	    /* ( default ": "  `    def     '   )   . */
	    ? (1 +  7 +   2  + 1 + defLen + 1 + 1 + 1)
	    /*  ( default )   . */
	    : ( 1 +  7  + 1 + 1);
	  if (dest + totalLen < limit) {
	    if (defLen) {
	      sprintf(dest, "(default: `%s').\t%s", def, def);
	    }
	    else {
	      sprintf(dest, "(default).\t");
	    }
	    dest+= totalLen; docStateP->doc= NULL;
	  }
	  done= TRUE;
	  break;
	}
	case LO_BOUND_HELP:
	case HI_BOUND_HELP: {
	  CONST Long *ranges= (CONST Long *)docStateP->checkP;
	  CONST Long val= ranges[docStateP->checkPIndex];
	  CONST Size totalLen= 1 + longPrintLen(val, 10) + 1;
	  assert(ranges[0] == 0);
	  assert(docStateP->checkPIndex == 1 || docStateP->checkPIndex == 2);
	  /* 	       lo-bound				hi-bound 	*/
	  if (dest + totalLen < limit) {
	    sprintf(dest, "`%ld'", val);
	    dest+= totalLen; docStateP->helpState= NORMAL_HELP;
	  }
	  else done= TRUE;
	  break;
	}
	case INT_VAL_HELP: {
	  CONST Long *values= (CONST Long *)docStateP->checkP;
	  CONST Long nValues= values[0] - 1;
	  CONST Long val= values[docStateP->checkPIndex];
	  CONST Boolean isSecondLast= (docStateP->checkPIndex == nValues - 1);
	  CONST Boolean isLast= (docStateP->checkPIndex == nValues);
	  CONST Size totalLen= 1 + longPrintLen(val, 10) + 1 + 
	                       (isLast ? 0 : (isSecondLast) ? 4 : 2);
	  assert(nValues > 1);
	  assert(docStateP->checkPIndex < nValues + 1);
	  if (dest + totalLen < limit) {
	    sprintf(dest, "`%ld'%s", val, 
		    isLast ? "" : (isSecondLast ? " or " : ", "));
	    dest+= totalLen; 
	    docStateP->checkPIndex++;
	    if (isLast) docStateP->helpState= NORMAL_HELP;
	  }
	  else done= TRUE;
	  break;
	}
	case NORMAL_HELP: {
	  ConstString src= docStateP->doc;
	  ConstString srcCut= src;
	  Char *destCut= dest;
	  for (src= docStateP->doc;
	       *src != '\0' && *src != '\t' && *src != '%' && dest < limit;
	       src++) {
	    if (*src == '"' || *src == '\\') {
	      if (dest < limit - 1) {
		*dest++= '\\';
	      }
	      else break;
	    }
	    if (*src == ' ') { destCut= dest; srcCut= src; }
	    *dest++= *src;
	  }
	  if (*src == '\0') { /* This may exceed limit by 1. */
	    *dest++= '.'; docStateP->doc= NULL; done= TRUE;
	  }
	  else if (*src == '\t') {
	    *dest++= ' ';	/* This may exceed limit by 1. */
	    docStateP->doc= src + 1; docStateP->helpState= DEFAULT_HELP;
	  }
	  else if (*src == '%') {
	    docStateP->doc= src + 2;
	    switch (src[1]) {
	      case 'A':
		docStateP->helpState= ARG_HELP; break;
	      case 'H':
		docStateP->checkPIndex= 2; docStateP->helpState= HI_BOUND_HELP; 
		break;
	      case 'I':
		docStateP->checkPIndex= 1; docStateP->helpState= INT_VAL_HELP; 
		break;
	      case 'L':
		docStateP->checkPIndex= 1; docStateP->helpState= LO_BOUND_HELP;
		break;
	      case 'S':
		docStateP->checkPIndex= 0; 
		docStateP->helpState= STRING_VAL_HELP;
		break;
	      case '%':	
		*dest++= '%'; break;	/* May exceed limit by 1. */
	    }
	  } /* else if (*src == '%') */
	  else { /* Must have had limit exceeded. */
	    docStateP->doc= srcCut + 1;
	    dest= destCut;
	    done= TRUE;
	  }
	  done|= (dest >= limit);
	  break;
	} /* case NORMAL_HELP: */
	case STRING_VAL_HELP: {
	  ConstString *values= (ConstString *)docStateP->checkP;
	  ConstString val= values[docStateP->checkPIndex];
	  CONST Boolean isSecondLast= 
	    values[docStateP->checkPIndex + 2] == NULL;
	  CONST Boolean isLast= values[docStateP->checkPIndex + 1] == NULL;
	  CONST Size totalLen= 1 + strlen(val) + 1 + 
	    (isLast ? 0 : (isSecondLast ? 4 : 2));
	  assert(values[0] != NULL && values[1] != NULL);
	  if (dest + totalLen < limit) {
	    sprintf(dest, "`%s'%s", val, 
		    isLast ? "" : (isSecondLast ? " or " : ", "));
	    dest+= totalLen; 
	    docStateP->checkPIndex++;
	    if (isLast) docStateP->helpState= NORMAL_HELP;
	  }
	  else done= TRUE;
	  break;
	}
      } /* switch (docStateP->helpState) */
    } /* while (!done) */
  } /* if (docStateP->doc != NULL) */
  return dest - text;
}


/*			      TOP-LEVEL ROUTINE.			*/


enum {
  HDR_WIDTH= 26,
  LINE_WIDTH= 72
};


/* This routine takes an option-table with unformatted help messages
 * and outputs an initializer for the option-table with formatted help
 * messages (using the OPT_ENTRY() macro) to helpFile.  The caller needs
 * to output the remaining superstructure around the initializer.
 */
VOID optTabFmt(outFile, optTab, nEntries) 
  FILE *outFile;
  CONST OptInfo *optTab; 
  unsigned nEntries;
{
  enum { DOC_INC= 512 };	/* Increment for doc dynamic memory. */
  Char *doc= NULL;
  Size docSize= 0;
  Index i;
  for (i= 0; i < nEntries; i++) {
    CONST OptInfo *p= &optTab[i];
    fprintf(outFile, "  OPT_ENTRY(\n");
    if (p->longOpt) {
      fprintf(outFile, "    /* longOpt */\t\"%s\",\n", p->longOpt);
    }
    else {
      fprintf(outFile, "    /* longOpt */\tNULL,\n");
    }
    if (p->shortOpt == '\0') {
      fprintf(outFile, "    /* shortOpt */\t'\\0',\n");
    }
    else {
      fprintf(outFile, "    /* shortOpt */\t'%c',\n", p->shortOpt);
    }
    fprintf(outFile, "    /* argType */\t%s,\n", p->flagsText);
    fprintf(outFile, "    /* id */\t\t%s,\n", p->id);
    fprintf(outFile, "    /* checkP */\t\t%s,\n", p->checkPText);
    fprintf(outFile, "    /* valP */\t\t%s,\n", p->valP);
    fprintf(outFile, "    /* optFn */\t\t%s,\n", p->optFn);
    if (p->doc == NULL) {
      fprintf(outFile, "    /* doc */\t\tNULL\n");
    }
    else {
      Index docIndex= 0;
      HdrState hdrState;
      DocState docState;
      ConstString docP;
      initHdrState(i, optTab, nEntries, &hdrState);
      initDocState(i, optTab, nEntries, &docState);
      while (!isDocDone(&docState) || !isHdrDone(&hdrState)) {
	Index lineEndIndex= docIndex + LINE_WIDTH;
	Index docStartIndex= docIndex + HDR_WIDTH;
	if (docIndex + LINE_WIDTH + 2 >= docSize) { /* 1 for slack; 2 for \n */
	  doc= (Char*)REALLOC(doc, docSize+= DOC_INC);
	}
	docIndex+= buildHdrLine(&doc[docIndex], HDR_WIDTH, &hdrState);
	while (docIndex < docStartIndex) doc[docIndex++]= ' ';
	docIndex+= buildDocLine(&doc[docIndex], lineEndIndex - docIndex - 2,
				&docState);
	doc[docIndex++]= '\\'; doc[docIndex++]= 'n';
	
      }
      doc[docIndex]= '\0';
      fprintf(outFile, "    /* doc */\t\t\"\\\n");
      docP= doc; 
      while (*docP != '\0') {
	ConstString nlP1= strstr(docP, "\\n") + 2;
	fprintf(outFile, "%.*s\\\n", nlP1 - docP, docP);
	docP= nlP1;
      }; 
      assert(optTab[i].doc);
      assert(strchr(optTab[i].doc, '\t'));
      if ((docP= strchr(strchr(optTab[i].doc, '\t') + 1, '\t')) != NULL) {
	fprintf(outFile, "\\t%s\\\n", docP+1);
      }
      fprintf(outFile, "\"\n");
    }
    fprintf(outFile, "  )%s\n", (i == nEntries - 1) ? "" : ",");
  }
  FREE(doc);
  VOID_RET();
}

