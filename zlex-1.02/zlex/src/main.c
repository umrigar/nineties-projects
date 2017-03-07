/*

File:	 main.c
Purpose: Main program for scanner.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "chrclass.h"
#include "codescan.h"
#include "datetime.h"
#include "dfa.h"
#include "files.h"
#include "main.h"
#include "options.h"
#include "outdfa.h"
#include "outfns.h"
#include "patterns.h"
#include "scan.h"
#include "zparse.h"

#include "error.h"
#include "ids.h"
#include "memalloc.h"
#include "zutil.h"

/*			      STATIC PROTOTYPES.			*/

static VOID setVersionInfo PROTO((VOID_ARGS));
static Count outVersionParams PROTO((FILE *outFile));
static Count outOptParams PROTO((FILE *outFile));
static Count outParams PROTO((FILE *outFile));
static Count outPrefixedNames PROTO((FILE *outFile));
static Count outSkelSec PROTO((FILE *outFile));
#ifdef DO_TRACE
static FILE *setupDumpFile PROTO((VOID_ARGS));
#endif
static VOID outScannerFile PROTO((ConstString outFName, Int argc, 
				  ConstString argv[]));


/* Insert signature string into executable to permit distinguishing it
 * from other executables of the same name.
 */
char uniqueSignature[]= "Zerksis";

typedef struct {
  ConstString name;
  Index2 major;
  Index2 minor;
} Version;

static Version version;

/* VERSION should be defined on compiler command-line, but if not defined,
 * here's a default.
 */
#ifndef VERSION
#define VERSION "1.0"
#endif

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
  fprintf(outFile, "#define YY_ZLEX_MAJOR_VERSION %d\n", version.major);
  nLines++;
  fprintf(outFile, "#define YY_ZLEX_MINOR_VERSION %d\n", version.minor);
  nLines++;
  return nLines;
}

Int 
versionFn(id, checkP, valP, argP)
  Int id; 
  VOIDP checkP;
  VOIDP valP;
  ConstString argP;
{
  fprintf(stderr, "zlex version %s; built %s on %s.\n", 
	  version.name, dateTime.time, dateTime.date);
  exit(0);
  return 0;
}


/*			SCANNER PARAMETERS OUTPUT ROUTINE		*/

static Count
outOptParams(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  fprintf(outFile, "#define YY_ALIGN %d\n", optionsP->align);
  nLines++;
  fprintf(outFile, "#define YY_ARRAY %d\n", optionsP->doArray);
  nLines++;
  fprintf(outFile, "#define YY_BACKUP_OPTIMIZE %d\n", optionsP->backupOptimize);
  nLines++;
  fprintf(outFile, "#define YY_CHAR_SET_SIZE %d\n", optionsP->charSetSize);
  nLines++;
  fprintf(outFile, "#define YY_COMPRESS %d\n", optionsP->compress); 
  nLines++;
  fprintf(outFile, "#define YY_DO_CODE %d\n", optionsP->doCode);
  nLines++;
  fprintf(outFile, "#define YY_DO_DEBUG %d\n", optionsP->doDebug);
  nLines++;
  fprintf(outFile, "#define YY_DO_REJECT %d\n", optionsP->doReject);
  nLines++;
  fprintf(outFile, "#define YY_DO_STDIO %d\n", optionsP->doStdio);
  nLines++;
  fprintf(outFile, "#define YY_SENTINEL_CHAR %d\n", optionsP->sentinelChar);
  nLines++;
  fprintf(outFile, "#define YY_DEFAULT_ACTION %d\n", optionsP->defaultAct);
  nLines++;
  fprintf(outFile, "#define YY_TABLE_ENTRY %d\n", optionsP->tableEntry);
  nLines++;
  return nLines;
}

static Count
outParams(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  nLines+= outCompDFAParams(outFile);
  nLines+= outDFAParams(outFile);
  nLines+= outEqClassParams(outFile);
  nLines+= outOptParams(outFile);
  nLines+= outVersionParams(outFile);
  nLines+= outPatternParams(outFile);
  nLines+= outScanParams(outFile);
  return nLines;
}


/*			OUTPUT PREFIXED NAMES.				*/

typedef struct {
  ConstString macroSuffix;
  ConstString nameSuffix;
} PrefixedNameInfo;

static PrefixedNameInfo prefixedNames[]= {
  { "CURRENT_BUFFER", "_current_buffer" },
  { "DATA_P", "dataP" },
  { "ERR", "err" },
  { "IN", "in" },
  { "LENG", "leng" },
  { "LEX", "lex" },
  { "LINENO", "lineno" },
  { "OUT", "out" },
  { "TEXT", "text" },
  { "WRAP", "wrap" },
  { "ZLEX_DEBUG", "_zlex_debug" }
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
	    optionsP->prefix, prefixedNames[i].nameSuffix);
    nLines++;
    fputs("#endif\n\n", outFile); nLines+= 2;
  }
  return nLines;
}

/*			SKELETON FILE.					*/

/* It would be preferable to define ZLEX_SEARCH_PATH as:
 * 	"." ":$ZLEX_SEARCH_PATH" STRINGIFY(:ZLEX_SKEL_PATH)
 * where ZLEX_SKEL_PATH would be a macro defined on the command line.
 * Unfortunately catenation of adjacent strings and STRINGIFY is
 * not understood properly by brain-damaged ccs.  Hence we require
 * that the entire ZLEX_SEARCH_PATH to be defined on the command line
 * (we provide a default below).
 */

/* Directories searched for skeleton file. */
#ifndef ZLEX_SEARCH_PATH
#define ZLEX_SEARCH_PATH	"$ZLEX_SEARCH_PATH:.:$HOME:/usr/local/share"
#endif

ConstString dirs= ZLEX_SEARCH_PATH;

/* Name of skeleton file. */
#define	SKEL_NAME	"zlexskl.c"

VOID
skelHelp(helpFile)
  FILE *helpFile;
{
  ConstString path;
  Int pathLen;
  FILE *f= findFileDir(SKEL_NAME, "r", dirs, &path, &pathLen);
  if (!f) {
    error("no skeleton file found");
  }
  else {
    fprintf(helpFile, "Skeleton file= `%.*s%c%s'.\n", 
	    pathLen, path, DIR_SEPARATOR, SKEL_NAME);
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
    if (!(skelFile= findFile(SKEL_NAME, "r", dirs))) {
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


/*			     TRACE ROUTINE.				*/

Files files;

#ifdef DO_TRACE

static FILE *
setupDumpFile()
{
  FILE *dumpFile;
  ConstString extensions[2];
  extensions[0]= ".l"; extensions[1]= NULL;
  {
    CONST Boolean buildName= strlen(optionsP->traceFileName) == 0;
    ConstString dumpName= (buildName)
                          ? chStrSuffix(srcName(), extensions, ".trc")
                          : optionsP->traceFileName;
    dumpFile= safeOpen(dumpName, "zlex");
    fputs("\t\t\tTRACE OUTPUT FROM zlex\n\n", dumpFile);
    if (buildName) FREE(dumpName);
  }
  return dumpFile;
}

#else
static FILE *
setupDumpFile()
{
  warn("Tracing not supported.");
  message("For tracing to be supported, zlex needs to be compiled with the\n");
  message("cpp symbol DO_TRACE defined.\n");
  return NULL;
}

#endif /* ifdef DO_TRACE */


/*			  MAIN OUTPUT ROUTINES.				*/

/* This vector could be automatic within outScannerFile(), but some cc's
 * don't allow initialization of auto-vectors. 
 */
typedef Count (*OutFn)(FILE *);
static OutFn outFns[]= { OUT_FNS_INIT };

/* Output generated scanner file.  Intersperse skeleton sections with 
 * sections generated by functions called via outFns.
 */
static VOID 
outScannerFile(outFName, argc, argv)
  ConstString outFName;
  Int argc;
  ConstString argv[]; 
{
  const unsigned nFns= sizeof(outFns)/sizeof(OutFn);
  unsigned i;
  Count lineN= 1;
  FILE *f= (optionsP->toStdOut) ? stdout : safeOpen(outFName, "zlex");

  /* Output zlex header and version. */
  fprintf(f, 
          "/* This scanner was automatically generated by zlex Version %s\n", 
	  version.name); 
  fprintf(f, " * using the command:\n * ");
  for (i= 0; i < argc; i++) { fprintf(f, "%s ", argv[i]); }
  fprintf(f, "\n */\n"); 
  lineN+= 4;

  for (i= 0; i < nFns; i++) {
    lineN+= outSkelSec(f);
    lineN+= outFns[i](f);
    lineN+= lineDir(f, lineN + 1, outFName);
    if (ferror(f)) fatal("output file error.");
  }
  if (!optionsP->toStdOut) fclose(f);
  VOID_RET();
}

#ifdef DEBUG_MEM

static VOID memLog PROTO((VOID_ARGS));

static VOID
memLog()
{
  MEM_ALLOCATIONS();
}

#endif

int main PROTO((int argc, ConstString argv[]));

int 
main(argc, argv)
  int argc;
  ConstString argv[]; 
{
  int nonOptArgN;
  initError(NULL, NIL, NULL, srcName, lineN, NULL);
  initISet(); 
  initID(FALSE);
  setVersionInfo();
#ifdef DEBUG_MEM
  atexit(memLog);
#endif
  nonOptArgN= processOptions(argc, argv);
  if (nonOptArgN < 0) exit(1);
  if (nonOptArgN == argc && !optionsP->lexCompat) {
    helpFn(0, NULL, NULL, NULL);
    exit(1);
  }
  initScan(argc, nonOptArgN, argv);
  if (optionsP->traceFileName) { files.dumpFile= setupDumpFile(); }
  initParse();
  initChrClass();
  initPatterns();
  yyparse();
  if (nErrors()) {;
    exit(1);
  }
  forcedOptions();
  computeClasses(); 
  buildDFA();
  terminateLeaves();
  if (optionsP->doCode) { initCode(); }
  else { compressDFA(); }
  outScannerFile(optionsP->outFileName, argc, argv);
  delDFA();
  terminatePatterns();
  terminateScan();
  terminateISet();
  terminateID();
  if (files.dumpFile) {
    if (ferror(files.dumpFile)) fatal("output file error.");  
    fclose(files.dumpFile);
  }
  exit(0);
  return 0;
}


