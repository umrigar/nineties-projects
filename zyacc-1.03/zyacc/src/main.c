/*

File:	 main.c
Purpose: Main program for parser-generator.

Last Update Time-stamp: "97/07/31 22:42:58 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/* Local includes. */
#include "attribs.h"
#include "buildrec.h"
#include "gram.h"
#include "lalr.h"
#include "main.h"
#include "options.h"
#include "outfns.h"
#include "parseopt.h"
#include "scan.h"
#include "zparse.h"

/* Libz includes. */
#include "error.h"
#include "ids.h"
#include "memalloc.h"
#include "zutil.h"

/* System includes via port.h. */

/* Insert signature string into executable to permit distinguishing it
 * from other executables of the same name.
 */
char uniqueSignature[]= "Zerksis";

/* A little function to ensure that above signature doesn't get optimized
 * away by optimizing linkers.
 */
int 
ensureSig()
{
  int n= sizeof(uniqueSignature);
  int i;
  for (i= 0; i < n; i++) uniqueSignature[i]= (char)(2*i);
  return uniqueSignature[2];
}

/* 			STATIC PROTOTYPES. 				*/

static VOID setVersionInfo PROTO((VOID_ARGS));
static Count outVersionParams PROTO((FILE *outFile));
static Int getOptValue PROTO((ConstString argP, OptErr *errP));
static Count outOptParams PROTO((FILE *outFile));
static VOID genFileNames PROTO((ConstString srcName));
static Count outPrefixedNames PROTO((FILE *outFile));
static Count outParams PROTO((FILE *outFile));
static Count outSkelSec PROTO((FILE *outFile));
static VOID outParserFile PROTO((ConstString outFName, BooleanX doLine, 
			  Int argc, ConstString argv[]));
static VOID outDefsFile PROTO((ConstString defName));


/* 			   VERSION INFORMATION.				*/

#ifndef PRG_NAME
#define PRG_NAME	"zyacc"
#endif

/* VERSION should be defined on compiler command-line, but if not defined,
 * here's a default.
 */
#ifndef VERSION
#define VERSION "1.0"
#endif

typedef struct {
  ConstString name;
  Index2 major;
  Index2 minor;
} Version;

static Version version;

/* Set the version information using the value of the CPP macro VERSION
 * which should be set at the compiler command-line to a string of the
 * form "MAJOR.MINOR" (eg: "1.2").
 */
static VOID
setVersionInfo()
{
  ConstString name= VERSION;
  int major, minor;
  if (sscanf(name, "%d.%d", &major, &minor) != 2) {
    fatal("Could not extract version information on startup.");
  }
  version.name= name; version.major= major; version.minor= minor;
  VOID_RET();
}

static Count 
outVersionParams(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  CONST char *p= strchr(version.name, '.');
  if (!p || p[1] == '\0') {
    fprintf(outFile, "#define YY_ZYACC_MAJOR_VERSION %s\n", version.name);
    nLines++;
    fprintf(outFile, "#define YY_ZYACC_MINOR_VERSION 0\n");
    nLines++;
  }
  else {
    fprintf(outFile, "#define YY_ZYACC_MAJOR_VERSION %.*s\n", 
	    p - version.name, version.name);
    nLines++;
    fprintf(outFile, "#define YY_ZYACC_MINOR_VERSION %s\n", p + 1);
    nLines++;
  }
  return nLines;
}


Int 
versionFn(id, checkP, valP, argP)
  Int id; 
  VOIDP checkP;
  VOIDP valP;
  ConstString argP;
{
  fprintf(stderr, "zyacc version %s\n", version.name);
  exit(0);
  return 0;
}

Int 
buildFn(id, checkP, valP, argP)
  Int id; 
  VOIDP checkP;
  VOIDP valP;
  ConstString argP;
{
  fprintf(stderr, "built at %s on %s.\n", buildRec.time, buildRec.date);
  fprintf(stderr, "build command: %s.\n", buildRec.cmd);
  exit(0);
  return 0;
}


/* Return value for option passed in argP.  Set *errP if error. */
static Int
getOptValue(argP, errP)
  ConstString argP;
  OptErr *errP;
{
  char *endP;
  long v;
  *errP= NO_OPT_ERR;
  assert(argP);
  v= strtol(argP, &endP, 0);
  if (*endP != '\0') *errP= CHECK_OPT_ERR;
  return v;
}

static Count 
outOptParams(outFile)
  FILE *outFile;
{
  Count lineCount= 0;
  fprintf(outFile, "#define YY_CMD_LINE_DEBUG %d\n", optionsP->cmdLineDebug);
  lineCount++;
  fprintf(outFile, "#define YY_PREFIX %s\n", optionsP->extPrefix);
  lineCount++;
  return lineCount;
}

/*			FILE-NAME GENERATION. 				*/

/* File names. */
static struct {
  ConstString srcName;		/* Full source file name. */
  ConstString outName;		/* Generated parser file name. */
  ConstString defName;		/* Definitions file name. */
} fNames;

/* Generate file names based on names provided by the user on command
 * line: specifically based on the parser .y file srcName and the name
 * of the generated parser file (if specified using the --output
 * option).
 */
static VOID 
genFileNames(srcName)
  ConstString srcName;
{
  Size outLen;
  fNames.srcName= srcName;
  fNames.outName= optionsP->outName;

  /* Generate output file name. */
  if (fNames.outName) { /* Output file specified using --output. */
    outLen= strlen(fNames.outName);
  }
  else if (optionsP->doYacc) {
    fNames.outName= "y.tab.c"; outLen= 7;
  }
  else if (optionsP->filePrefix) { /* Form output file name from prefix. */
    CONST Size rootLen= strlen(optionsP->filePrefix);
    Char *outName;
    outLen= rootLen + 6;	/* Adding ".tab.c". */
    outName= MALLOC(outLen + 1);		/* Extra '\0'. */
    strncpy(outName, optionsP->filePrefix, rootLen);
    strcpy(outName + rootLen, ".tab.c");
    fNames.outName= outName;
  }
  else { /* Form output file name from input file name. */
    CONST Size srcLen= strlen(srcName);
    CONST Size prefixLen= srcLen - 2;    /* Remove ".y" suffix. */
    CONST Size 
      rootLen= (srcLen >= 2 && strcmp(srcName + prefixLen, ".y") == 0)
	       ? prefixLen
	       : srcLen;
    Char *outName;
    outLen= rootLen + 6;  			/* Adding ".tab.c". */
    outName= MALLOC(outLen + 1);	/* Extra '\0'. */
    strncpy(outName, srcName, rootLen);
    strcpy(outName + rootLen, ".tab.c");
    fNames.outName= outName;
  } /* Form output name. */
  if (optionsP->doDefs) { /* Generating a .h description file. */
    CONST Size prefixLen= outLen - 2; 	/* Try removing ".c". */
    CONST Size 
      rootLen= (outLen >= 2 && strcmp(fNames.outName + prefixLen, ".c")==0)
	       ? prefixLen
	       : outLen;
    Char *defName;
    defName= MALLOC(rootLen + 2 + 1); /* ".h" + '\0' */
    strncpy(defName, fNames.outName, rootLen);
    strcpy(defName + rootLen, ".h");
    fNames.defName= defName;
  }
  VOID_RET();
}


/*			OUTPUT PREFIXED NAMES.				*/

typedef struct {
  ConstString macroSuffix;
  ConstString nameSuffix;
} PrefixedNameInfo;

static PrefixedNameInfo prefixedNames[]= {
  { "CHAR", "char" },
  { "DEBUG", "debug" },
  { "ERROR", "error" },
  { "LEX", "lex" },
  { "LVAL", "lval" },
  { "NERRS", "nerrs" },
  { "PARSE", "parse" }
};

#define MACRO_PREFIX "YY_"

static Count 
outPrefixedNames(outFile)
  FILE *outFile;
{
  CONST Count n= sizeof(prefixedNames)/sizeof(PrefixedNameInfo);
  Count nLines= 0;
  Index i;
  for (i= 0; i < n; i++) {
    fprintf(outFile, "#ifndef %s%s\n", MACRO_PREFIX,
	    prefixedNames[i].macroSuffix);
    nLines++;
    fprintf(outFile, "#define %s%s %s%s\n", 
	    MACRO_PREFIX, prefixedNames[i].macroSuffix,
	    optionsP->extPrefix, prefixedNames[i].nameSuffix);
    nLines++;
    fputs("#endif\n\n", outFile); nLines+= 2;
  }
  return nLines;
}


/* 			PARAMETER OUTPUT ROUTINE.			*/

/* Output parser parameters to outFile.  Return # of lines output. */
static Count 
outParams(outFile)
  FILE *outFile;
{
  Count lineCount= 0;
  lineCount+= outAttribParams(outFile);
  lineCount+= outGramParams(outFile);
  lineCount+= outLALRParams(outFile);
  lineCount+= outOptParams(outFile);	
  lineCount+= outParseParams(outFile);	
  lineCount+= outScanParams(outFile);
  lineCount+= outVersionParams(outFile);
  return lineCount;
}


/*			SKELETON FILE.					*/

/* It would be preferable to define ZYACC_SEARCH_PATH as:
 *      "$ZYACC_SEARCH_PATH" ":." ":$HOME" STRINGIFY(:ZYACC_SKEL_PATH)
 * where ZYACC_SEARCH_PATH would be a macro defined on the command line.
 * Unfortunately catenation of adjacent strings and STRINGIFY is
 * not understood properly by brain-damaged ccs.  Hence we require
 * that the entire ZYACC_SEARCH_PATH to be defined on the command line
 * (we provide a default below).
 */

/* Directories searched for skeleton file. */
#ifndef ZYACC_SEARCH_PATH
#define ZYACC_SEARCH_PATH	"$ZYACC_SEARCH_PATH:.:$HOME:/usr/local/share"
#endif

ConstString dirs= ZYACC_SEARCH_PATH;

/* Name of skeleton file. */
#define	SKEL_NAME	"zyaccskl.c"

VOID
skelHelp(helpFile)
  FILE *helpFile;
{
  ConstString path;
  UInt pathLen;
  FILE *f= findFileDir(SKEL_NAME, "r", dirs, &path, &pathLen);
  if (!f) {
    error("no skeleton file found");
  }
  else {
    fprintf(helpFile, "Skeleton file= `%.*s%c%s'.\n", 
	    (Int)pathLen, path, DIR_SEPARATOR, SKEL_NAME);
    fclose(f);
  }
  VOID_RET();
}

/* Output the next skeleton section to outFile.  Return a count of the
 * # of lines output.
 */
static Count 
outSkelSec(outFile)
  FILE *outFile;
{
#define IS_DIRECTIVE_LINE(line)						\
  (line[0] == '/' && line[1] == '*' && line[2] == '!')
  static FILE *skelFile= 0;
  Count lineCount= 0;
  if (skelFile == 0) {
    if (!(skelFile= findFile(SKEL_NAME, "r",  dirs))) {
      FILE_ERROR(SKEL_NAME);
    }
  }
  while (!feof(skelFile)) {
    enum { MAX_LINE= 120 };
    Char line[MAX_LINE];
    line[MAX_LINE - 1]= '\n';
    fgets(line, MAX_LINE, skelFile);
    if (IS_DIRECTIVE_LINE(line)) break;
    fputs(line, outFile);
    lineCount+= line[MAX_LINE - 1] == '\n';
  }
  return lineCount;
#undef IS_DIRECTIVE_LINE
}


/*			  MAIN OUTPUT ROUTINES.				*/

/* This vector could be automatic within outParserFile(), but some cc's
 * don't allow initialization of auto-vectors. 
 */
typedef Count (*OutFn) PROTO((FILE *));
static OutFn outFns[]= { OUT_FNS_INIT };

/* Output generated parser file.  Intersperse skeleton sections with 
 * sections generated by functions called via outFns.
 */
static VOID 
outParserFile(outFName, doLine, argc, argv)
  ConstString outFName;
  BooleanX doLine;
  Int argc;
  ConstString argv[];
{
  const unsigned nFns= sizeof(outFns)/sizeof(OutFn);
  unsigned i;
  Count lineN= 1;
  FILE *f= safeOpen(outFName, PRG_NAME);
  fprintf(f, "/* Automatically generated from %s by %s version %s\n",
	  fNames.srcName, PRG_NAME, version.name);
  fprintf(f, " * using the command:\n * ");
  for (i= 0; i < argc; i++) { fprintf(f, "%s ", argv[i]); }
  fprintf(f, "\n */\n"); 
  lineN+= 4;

  for (i= 0; i < nFns; i++) {
    lineN+= outSkelSec(f);
    lineN+= outFns[i](f);
    if (doLine) { 
      fprintf(f, "#line %d \"%s\"\n", ++lineN, outFName);
    }
  }
  fclose(f);
  VOID_RET();
}

/* Output .h file. */ 
static VOID 
outDefsFile(defName)
  ConstString defName;
{
  FILE *f= safeOpen(defName, PRG_NAME);
  fprintf(f, "/* Automatically generated from %s by %s version %s. */\n",
	  fNames.srcName, PRG_NAME, version.name);
  fputs("#ifndef _YY_DEFS_H\n#define _YY_DEFS_H\n\n", f);
  outYYSTYPE(f);
  fputs("extern YYSTYPE yylval;\n", f);
  outToks(f);
  fputs("#endif /* ifndef _YY_DEFS_H */\n", f);
  fclose(f);
  VOID_RET();
}


/*			MAIN PROGRAM.					*/

Int main PROTO((Int argc, ConstString argv[]));


Int 
main(argc, argv)
  Int argc;
  ConstString argv[];
{
  Int err;
  Int nonOptArgN;
  setVersionInfo();
#ifdef MEM_DEBUG
  atexit(memAtExit);
#endif
  ensureSig();
  nonOptArgN= processOptions(argc, argv);
  if (nonOptArgN < 0) exit(1);
  if (nonOptArgN == argc) {
    helpFn(0, NULL, NULL, NULL);
    exit(1);
  }
  genFileNames(
    strcmp(argv[nonOptArgN], "-") == 0 ? "<stdin>" : argv[nonOptArgN]);
  initScan(argc, nonOptArgN, argv);
  initAttribs();
  doGram();
  err= nErrors();
  if (err > 0) {
    fprintf(stderr, "%d errors. No output produced.\n", err);
    exit(1);
  }
  else {
    if (optionsP->doGram) { outRefGram(stdout); exit(0); }
    doLALR();
    computeAttribs();
    outParserFile(fNames.outName, optionsP->doLine, argc, argv);
    if (optionsP->doDefs) outDefsFile(fNames.defName);
    if (optionsP->doVerbose) {
      doVerbose(fNames.srcName, fNames.outName);
    }
    if (optionsP->doHTML) {
      doHTML(fNames.srcName, fNames.outName);
    }
  }
  exit(0);	/* exit rather than return as some systems discard return val.*/
  return 0;
}
