/*

File:	 opttab0.c
Purpose: Unformatted Option table.

Last Update Time-stamp: "97/07/11 15:35:58 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "parseopt.h"

OptInfo optTab[]= {
  /* LongOpt Name, ShortOpt Name, Argument Type, User ID, CheckP, ValP,	
   * OptFn, Doc								
   */									
  OPT_ENTRY("build-display", '\0', NO_OPT_FLAG, BUILD_DISPLAY_OPT,
	    NULL, NULL, buildFn,
    "\tDisplay parameters used to build zyacc and exit"),
  OPT_ENTRY("debug", 't', OPTIONAL_OPT_FLAG, DEBUG_OPT, NULL,
	    (VOIDP) &options.cmdLineDebug, yesNoOptFn,
	    "1|0\tAllow tracing of generated parser\t0"
  ),
  OPT_ENTRY("defines", 'd', OPTIONAL_OPT_FLAG, DEFINES_OPT, NULL,
	    (VOIDP) &options.doDefs, yesNoOptFn, 
	    "1|0\tOutput a .h definitions file\t0" 
  ),
  OPT_ENTRY("file-prefix", 'b', REQUIRED_OPT_FLAG, FILE_PREFIX_OPT, NULL,
	    (VOIDP) &options.filePrefix, stringOptFn, 
	    "PREFIX\tSpecify prefix to be used for all output files\tyy"
  ),
  OPT_ENTRY("grammar", 'g', OPTIONAL_OPT_FLAG, GRAMMAR_OPT, NULL,
	    (VOIDP) &options.doGram, yesNoOptFn,
	    "1|0\tOutput a reference grammar file to <stdout> and exit\t0"
  ),
  OPT_ENTRY("help", 'h', NO_OPT_FLAG, HELP_OPT, NULL,
	    NULL, helpFn,
	    "\tPrint summary of options and exit"
  ),
  OPT_ENTRY("html", 'H', OPTIONAL_OPT_FLAG, HTML_OPT, NULL,
	    (VOIDP) &options.doHTML, yesNoOptFn,
	    "1|0\tWrite html parser description in .html file\t0"
  ),
  OPT_ENTRY("lines", '\0', OPTIONAL_OPT_FLAG, LINES_OPT, NULL,
	    (VOIDP) &options.doLine, yesNoOptFn,
	    "1|0\tOutput #line directives in generated parser file\t1" 
  ),
  OPT_ENTRY("longer-rule-prefer", '\0', OPTIONAL_OPT_FLAG, RR_LONGER_OPT, NULL,
	    (VOIDP) &options.doRRLonger, yesNoOptFn, 
	    "1|0\tWhen resolving a reduce-reduce conflict "
	    "prefer the longer rule\t0" 
  ),
  OPT_ENTRY("output-file", 'o', REQUIRED_OPT_FLAG, OUTPUT_FILE_OPT, NULL,
	    (VOIDP) &options.outName, stringOptFn,
	    "outFile\tSpecify the name outFile for the parser file" 
  ),
  OPT_ENTRY("name-prefix", 'p', REQUIRED_OPT_FLAG, NAME_PREFIX_OPT, NULL,
	    (VOIDP) &options.extPrefix, stringOptFn, 
	    "PREFIX\tSpecify prefix to be used for all external symbols\tyy"
  ),
  OPT_ENTRY("term-suffix", '\0', REQUIRED_OPT_FLAG, TERM_SUFFIX_OPT, NULL,
	    (VOIDP) &options.termSuffix, stringOptFn, 
  "SUFFIX\tSpecify suffix string to be appended to all external terminal names\t"
  ),
  OPT_ENTRY("verbose", 'v', OPTIONAL_OPT_FLAG, VERBOSE_OPT, NULL,
	    (VOIDP) &options.doVerbose, yesNoOptFn,
	    "1|0\tWrite verbose parser description in .output file\t0"
  ),
  OPT_ENTRY("version", 'V', NO_OPT_FLAG, VERSION_OPT, NULL,
	    NULL, versionFn,
	    "\tPrint version number and exit"
  ),
  OPT_ENTRY("yacc", 'y', OPTIONAL_OPT_FLAG, YACC_OPT, NULL,
	    (VOIDP) &options.doYacc, yesNoOptFn,
	    "1|0\tName output files like YACC\t0"
  )
};
