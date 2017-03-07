/*

File:	 potest.c
Purpose: Unformatted options table for parseopt.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "potstchk.c"

static OptInfo optTab[]= {
  /* LongOpt Name, ShortOpt Name, Argument Type, User ID, CheckP, ValP,  
   * OptFn, Doc 
   */
  OPT_ENTRY("intFlag0", '0', NO_OPT_FLAG, 0, NULL, &intFlag0, 
	    intOptFn0, "\tSet integer flag 0 to 0\t1"),
  OPT_ENTRY("intFlag1", '1', NO_OPT_FLAG, 0, NULL, &intFlag1, 
	    intOptFn1, "\tSet integer flag 1 to 1\t0"),
  OPT_ENTRY("intVal", 'V', REQUIRED_OPT_FLAG, 0, NULL, &intVal, 
	    intOptFn0, "VALUE\tSet integer value to VALUE.\t0"),
  OPT_ENTRY("charAltFlag", 'C', SYNONYM_OPT_FLAG, 0, 0, NULL, 
	    NULL, NULL),
  OPT_ENTRY("charFlag", 'c', NO_OPT_FLAG, 0, NULL, &charFlag,
	    charOptFn, "\tReset char flag.\t1"),
  OPT_ENTRY("charVal", 'v', OPTIONAL_OPT_FLAG, 0, NULL, &charVal, charOptFn,
	    "Char\tSet charVal to Char.  Continuing on with this \
description... This character option has a very long description. \
It continues for more than one line.  It goes on and on and on and on \
and on and on and on...\tA"),
  OPT_ENTRY("stringFlag", '\0', OPTIONAL_OPT_FLAG|INTERN_OPT_FLAG, 
	    0, NULL, &stringFlag,
	    stringOptFn, "VALUE\tSet string flag to %A\t "),
  OPT_ENTRY("stringAltVal", 'S', SYNONYM_OPT_FLAG, 0, NULL, NULL,
	    NULL, NULL), 
  OPT_ENTRY("stringVal", 's', REQUIRED_OPT_FLAG|INTERN_OPT_FLAG, 
	    0, NULL, &stringVal, 
	    stringOptFn, "String\tSet string value to %A\t"), 
  OPT_ENTRY("rangeVal", 'r', REQUIRED_OPT_FLAG, 0, rangeSpec, &rangeVal, 
	    intOptFn0, "Int\tRange option between %L and %H\t3"), 
  OPT_ENTRY("primeVal", 'p', REQUIRED_OPT_FLAG, 0, primeSpec, &primeVal, 
	    shortOptFn0, "Int\tPrime # %I\t7"), 
  OPT_ENTRY("names", '\0', REQUIRED_OPT_FLAG, 0, nameSpec, &nameVal, 
	    stringValOptFn, 
	    "Name\tName must be one of %S\tmary")
};

