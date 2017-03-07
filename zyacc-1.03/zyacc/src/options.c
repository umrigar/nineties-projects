/*

File:	 options.c
Purpose: Option processing.

Last Update Time-stamp: "97/07/30 11:23:11 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "main.h"
#include "options.h"

#include "error.c"
#include "ids.h"
#include "memalloc.h"
#include "parseopt.h"
#include "zutil.h"

static int optPre PROTO((Int id, ConstString argP));

/*			COMMAND-LINE OPTIONS.				*/

enum {
  BUILD_DISPLAY_OPT,
  DEBUG_OPT,
  DEFINES_OPT,
  FILE_PREFIX_OPT,
  GRAMMAR_OPT,
  HELP_OPT,
  HTML_OPT,
  LINES_OPT,
  NAME_PREFIX_OPT,
  OUTPUT_FILE_OPT,
  RR_LONGER_OPT,
  TERM_SUFFIX_OPT,
  VERBOSE_OPT,
  VERSION_OPT,
  YACC_OPT,
  ZZZ_OPT		/* must be last. */
};

static Options options;		/* Options defined in "options.h". */
CONST Options *CONST optionsP= &options;

#include "opttab.c"


#define ERR_HELP_MSG 		  "Use `zyacc -h' for help"

static Boolean optProtect[ZZZ_OPT];
static Boolean isCmdLine;

/* Returns 0 if its a command-line option or the option is not protected.
 * Protects the option if its a command line option.  This routine is
 * called by the libz parseopt routine before calling option functions 
 * (the option function being called only when this routine returns 0).
 */
static Int 
optPre(id, argP)
  Int id;
  ConstString argP;
{
  CONST int retVal= !(isCmdLine || !optProtect[id]);
  assert(id < sizeof(optProtect)/sizeof(Boolean));
  if (isCmdLine) optProtect[id]= TRUE;
  return retVal;
}


/* Certain options force other options.  The following routine enforces
 * these restrictions.
 */
VOID
forcedOptions()
{
}



#define OPT_FILE_NAME	"zyacc.opt"
#define OPT_ENV_VAR	"ZYACC_OPTIONS"

Int 
helpFn(id, checkP, valP, argP)
  Int id; 
  VOIDP checkP;
  VOIDP valP;
  ConstString argP;
{
  ConstString path;
  UInt pathLen;
  FILE *optFile= findFileDir(OPT_FILE_NAME, "r", dirs, &path, &pathLen);
  ConstString optEnv= (ConstString)getenv(OPT_ENV_VAR);
  OptContext optContext;
  OptContext *CONST optContextP= &optContext;
  setOptContextDefaults(optContextP);
  allOptsHelp(optContextP, "Usage: zyacc [options] lex-file.", 
	      optTab, N_ELEMENTS(optTab));
  if (optFile) {
    ConstString fileContents= readFile(optFile);
    fprintf(optContextP->helpFile, 
	    "Above defaults overridden by file %.*s%c%s with contents:\n%s\n",
	    (int)pathLen, path, DIR_SEPARATOR, OPT_FILE_NAME, fileContents);
    FREE(fileContents);
    fclose(optFile);
  }
  if (optEnv) {
    fprintf(optContextP->helpFile, 
    "Above defaults overridden by environment variable %s with contents:\n%s\n",
	    OPT_ENV_VAR, optEnv);
  }
  fprintf(optContextP->helpFile, "Data search path is %s.\n", dirs);
  skelHelp(optContextP->helpFile);
  exit(0);
  return 0;
}

Int 
processOptions(argc, argv)
  Int argc;
  ConstString argv[];
{
  FILE *optFile= findFile(OPT_FILE_NAME, "r", dirs);
  CONST UInt nOptions= N_ELEMENTS(optTab);
  ConstString optEnv= (ConstString)getenv(OPT_ENV_VAR); 
  OptContext optContext;
  OptContext *CONST optContextP= &optContext;
  Int poRet;
  setOptContextDefaults(optContextP);
  if (parseOptDefaults(optContextP, optTab, nOptions) != NO_OPT_ERR) {
    INTERNAL_ERROR();
  }
  optContextP->internFn= internText;
  isCmdLine= FALSE;
  if (optFile) {
    ConstString fileContents= readFile(optFile);
    Int argc0;
    ConstString *argv0= string2argv(fileContents, "/*", "*/", &argc0);
    fclose(optFile);
    poRet= parseOpt(optContextP, argc0, argv0, optTab, nOptions);
    FREE(fileContents);
    if (poRet < 0) {
      error("option error in options file %s.", OPT_FILE_NAME);
      parseOptErr(optContextP, argc0, argv0, poRet, 
		  ERR_HELP_MSG, 
		  optTab, N_ELEMENTS(optTab));
    }
    delArgv(argv0);
    if (poRet < 0) return poRet;
  }
  if (optEnv) {
    Int argc0;
    ConstString *argv0= string2argv(optEnv, NULL, NULL, &argc0);
    poRet= parseOpt(optContextP, argc0, argv0, optTab, nOptions);
    if (poRet < 0) {
      error("option error in environment.");
      parseOptErr(optContextP, argc0, argv0, poRet,
		  ERR_HELP_MSG, 
		  optTab, N_ELEMENTS(optTab));
    }
    delArgv(argv0);
    if (poRet < 0) return poRet;
  }
  isCmdLine= TRUE;
  optContextP->internFn= NULL;
  optContextP->preFn= optPre;
  poRet= parseOpt(optContextP, argc, argv, optTab, nOptions);
  if (poRet < 0) {
    parseOptErr(optContextP, argc, argv, poRet, 
		ERR_HELP_MSG, 
		optTab, nOptions);
  }
  return poRet;
}

VOID 
setOptions(optStr)
  ConstString optStr;
{
  Int argc;
  ConstString *argv= string2argv(optStr, NULL, NULL, &argc);
  OptContext optContext;
  OptContext *CONST optContextP= &optContext;
  Int poRet;

  setOptContextDefaults(optContextP);
  optContextP->internFn= internText; optContextP->preFn= optPre;
  isCmdLine= FALSE;
  poRet= parseOpt(optContextP, argc, argv, optTab, N_ELEMENTS(optTab));

  if (poRet != argc) {	/* No non-option arguments allowed. */
    error("option error.");
    parseOptErr(optContextP, argc, argv, poRet,
		ERR_HELP_MSG, 
		optTab, N_ELEMENTS(optTab));
  }
  delArgv(argv);
  VOID_RET();
}


