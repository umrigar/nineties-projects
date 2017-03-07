/*

File:	 out.c
Purpose: Output routines

Last Update Time-stamp: <97/07/27 15:55:09 zdu>

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "options.h"
#include "out.h"
#include "scan.h"

#include "area.h"
#include "error.h"
#include "zutil.h"

/*			     OUTPUT ROUTINES				*/


typedef struct {
  ConstString fName;	/* source file name in which counter resides */
  Index lineNum;	/* line number at which counter resides */
} CounterInfo;

static struct {
  AreaX counterInfo;	/* contains information for all counters */
  FILE *outFile;
  ConstString ext;
  CounterType counter;
} globs;
#define G globs

#define COUNTER_INFO(i)	AX_ACCESS(G.counterInfo, CounterInfo, i)

VOID 
initOut() 
{
  INIT_AREAX(G.counterInfo, CounterInfo, 10, 0xFFFF);
}

VOID 
terminateOut() 
{
  delAreaX(&G.counterInfo);
}

/* Source Names */
#define CNTR_TYPE	"unsigned"	/* C type used for counters */
#define FNAME_TYPE	"FNameType"	/* type used for file-names array */
#define LINE_NUM_TYPE	"unsigned"	/* C type used for line #s */
#define PROF_TYPE	"ProfType"	/* type used for global info */
#define CNTRS		"Cntrs"		/* name of counter array */
#define LINE_NUMS	"LineNums"	/* name of line # array */
#define FILE_NAMES	"FNames"	/* name of file name breaks array */
#define ATEXIT_FN	"AtExit"	/* name of function to call at exit */
#define PROF_VAR	"Prof"
#define LIB_OUT_FN	"_bbProfOut"	/* runtime profile output function */

static FILE *
openOutputFile(fName)
  ConstString fName;
{
  ConstString outName= optionsP->output;
  enum { MAX_BUF= 256 };
  char buf[MAX_BUF];
  if (outName == NULL) {
    CONST Count fNameLen= strlen(fName);
    ConstString dotP= strrchr(fName, '.');	/* look for extension */
    CONST Int rootLen= (dotP) ? dotP - fName : fNameLen;
    if (fNameLen + strlen(optionsP->prefix) + 1 > MAX_BUF) {
      fatal("output file name too long");
    }
    else {
      sprintf(buf, "%.*s%s%s", rootLen, fName, 
	      optionsP->prefix, fName + rootLen);
    }
    outName= buf;
  }
  if ((G.outFile= safeOpen(outName, "zprof")) == NULL) {
    fatal("could not open file %s", buf);
  }
  return G.outFile;
}

#ifndef VERSION
#define VERSION	"0.1"
#endif


VOID
beginOutFile(fName)
  ConstString fName;
{
  FILE *outFile= openOutputFile(fName);

  fprintf(outFile, "/* Profiled version generated from %s by zprof %s */\n",
	  fName, VERSION);
  fprintf(outFile, "typedef struct {\n");
  fprintf(outFile, "  const unsigned hi;\n"); 
  fprintf(outFile, "  const char *const fName;\n");
  fprintf(outFile, "} %s%s;\n\n", optionsP->prefix, FNAME_TYPE);

  fprintf(outFile, "typedef struct {\n");
  fprintf(outFile, "  const %s *const counters;\n", CNTR_TYPE);
  fprintf(outFile, "  const %s%s *const fNames;\n", 
	  optionsP->prefix, FNAME_TYPE);
  fprintf(outFile, "  const %s *const lineNums;\n", LINE_NUM_TYPE);
  fprintf(outFile, "  const unsigned nCounters;\n");
  fprintf(outFile, "  unsigned isInit;\n");
  fprintf(outFile, "} %s%s;\n\n", optionsP->prefix, PROF_TYPE);

  fprintf(outFile, "static %s%s %s%s;\n", optionsP->prefix, PROF_TYPE, 
	  optionsP->prefix, PROF_VAR);
  fprintf(outFile, "static %s %s%s[];\n", CNTR_TYPE, optionsP->prefix, CNTRS);
  fprintf(outFile, "static %s%s %s%s[];\n", optionsP->prefix, FNAME_TYPE, 
	  optionsP->prefix, FILE_NAMES);
  fprintf(outFile, "static %s %s%s[];\n", 
	  LINE_NUM_TYPE, optionsP->prefix, LINE_NUMS);
  fprintf(outFile, "static void %s%s();\n", optionsP->prefix, ATEXIT_FN);

  fprintf(outFile, "int atexit();\n\n");
  
  fprintf(outFile, "# 1 \"%s\"\n", fileName());
}

VOID
endOutFile()
{
  CONST Count nCounters= AX_NENTRIES(G.counterInfo);
  FILE *outFile= G.outFile;
  if (nCounters > 0) {
    fprintf(outFile, "static %s %s%s[%d];\n", 
	    CNTR_TYPE, optionsP->prefix, CNTRS, nCounters);

    {
      enum { N_FILE_NAMES_PER_LINE= 4 };
      int i, c;
      ConstString fName= COUNTER_INFO(0).fName;
      fprintf(outFile, "static %s%s %s%s[]= {\n  ", 
	      optionsP->prefix, FNAME_TYPE, optionsP->prefix, FILE_NAMES);
      for (i=0, c= 0; c < nCounters; c++) {
	if (i == N_FILE_NAMES_PER_LINE) {
	  fputs("\n  ", outFile); i= 0;
	}
	if (COUNTER_INFO(c).fName != fName) { /* ok, since intern'd */
	  fprintf(outFile, "{ %d, \"%s\" }, ", c, fName);
	  fName= COUNTER_INFO(c).fName; i++;
	}
      }
      fprintf(outFile, "%s{ %d, \"%s\" }\n};\n\n", 
	      (i == N_FILE_NAMES_PER_LINE) ? "\n  " : "", 
	      c, fName);
    }

    {
      enum { N_LINE_NUMS_PER_LINE= 8 };
      int c;
      fprintf(outFile, "static %s %s%s[]= {", 
	      LINE_NUM_TYPE, optionsP->prefix, LINE_NUMS);
      for (c= 0; c < nCounters; c++) {
	if (c % N_LINE_NUMS_PER_LINE == 0) {
	  fputs("\n  ", outFile);
	}
	fprintf(outFile, "%d%s ", COUNTER_INFO(c).lineNum, 
		(c < nCounters - 1) ? ", " : "");
      }
      fputs("\n};\n\n", outFile);
    }

    fprintf(outFile, "static %s%s %s%s= {\n", 
	    optionsP->prefix, PROF_TYPE, optionsP->prefix, PROF_VAR);
    fprintf(outFile, " &%s%s[0], &%s%s[0], &%s%s[0], %d, 0\n",
	    optionsP->prefix, CNTRS,
	    optionsP->prefix, FILE_NAMES,
	    optionsP->prefix, LINE_NUMS,
	    nCounters
           );
    fputs("};\n\n", outFile);

    fprintf(outFile, "extern void %s(%s%s *);\n",
	    LIB_OUT_FN, optionsP->prefix, PROF_TYPE);
    fprintf(outFile, "static void %s%s(void) { %s(&%s%s); }\n",
	    optionsP->prefix, ATEXIT_FN,
	    LIB_OUT_FN,
	    optionsP->prefix, PROF_VAR);
    AX_CUT(G.counterInfo, 0);
  } /* if (nCounters > 0) */
  fclose(G.outFile); G.outFile= NULL;
  VOID_RET();   
}

/* Output text of current token in tokSemP.  After token whitespace
 * but before actual token lexeme insert appropriate counter code, if
 * G.counter specifies a counter.
 */
VOID 
putOut(tokSemP)
  CONST TokSem *CONST tokSemP;
{
  if (G.counter == NO_COUNTER) {
    fwrite(tokSemP->text, sizeof(Char), tokSemP->len, G.outFile);
  }
  else {
    Index c= AX_NEXT(G.counterInfo);
    COUNTER_INFO(c).fName= tokSemP->fName; 
    COUNTER_INFO(c).lineNum= tokSemP->lineNum;
    fwrite(tokSemP->text, sizeof(Char), tokSemP->len1, G.outFile);
    switch (G.counter) {
      case COMPOUND_COUNTER:
	fprintf(G.outFile, "%s%s[%d]++; ", optionsP->prefix, CNTRS, c);
	break;
      case COND_COUNTER:
	if (optionsP->isCond) {
	  fprintf(G.outFile, "( %s%s[%d]++ , ", optionsP->prefix, CNTRS, c);
	}
	break;
      case FN_COUNTER:
	fprintf(G.outFile, 
	    "int %sInit= %s%s.isInit || ((%s%s.isInit= 1) && atexit(%s%s)); ",
		optionsP->prefix, optionsP->prefix, PROF_VAR, 
		optionsP->prefix, PROF_VAR, optionsP->prefix, ATEXIT_FN);
	fprintf(G.outFile, "int %sCntrInc= %s%s[%d]++; ", optionsP->prefix, 
		optionsP->prefix, CNTRS, c);
	break;
      case STMT_COUNTER:
	fprintf(G.outFile, "{ %s%s[%d]++; ", optionsP->prefix, CNTRS, c);
	break;
      default:
	assert(0);
    }
    fwrite(tokSemP->text + tokSemP->len1, sizeof(Char), 
	   tokSemP->len - tokSemP->len1, G.outFile);
    G.counter= NO_COUNTER;
  }
  VOID_RET();
}

/* Since we want the counter to be associated with the *next* token,
 * we merely set a flag here.  Then when we output the next token,
 * we make sure we output the appropriate counter increment only
 * after any whitespace preceeding the next token.
 */
VOID 
beginCounter(counterType)
  CounterType counterType;
{
  G.counter= counterType;
  VOID_RET();
}

VOID 
endCounter(counterType)
  CounterType counterType;
{
  switch (counterType) {
    case COND_COUNTER:
      if (optionsP->isCond) {
	fputs(" ) ", G.outFile);
      }
      break;
    case STMT_COUNTER:
      fputs(" } ", G.outFile);
      break;
    default:
      break;
  }
  VOID_RET();
}

