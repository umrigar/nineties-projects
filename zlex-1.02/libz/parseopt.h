/*

File:	 parseopt.h
Purpose: Table-driven parsing of command-line options.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _PARSEOPT_H
#define _PARSEOPT_H

#include "ccport.h"

/*

Table-driven command-line option processing.

Similar to getopt() and GNU's getopt_long(), but different interface
and tries to do more of the work.  Might land up with lot of little
functions scattered all over the program, rather than a central switch
which handles all options.

USER INTERFACE

Short option names consist of a single letter (any character except a
'-') and must occur in an option word starting with a '-'.  Long
option names can contain multiple letters (any alphanum or '_' or '-')
and must occur in an option word starting with "--".

If a short option has a required value, then the value can follow
immediately in the option word or in the next word provided it doesn't
begin with a '-'.  If a short option has an optional value, then the
value must be specified in the option word immediately after the
option letter.

Multiple short options which do not require any arguments may occur
within a single word.

If a long options has a required value, then the value can either be
in the next word (provided it doesn't start with a '-') or in the same
word, separated from the option name by an '='.  If long options have
an optional value, then when the value is specified, it must be in the
same word separated by an '=' from the option name.

Each long option specified on the command-line is required to only match
an unambiguous prefix of any of the available long-options.

Some of the predefined option functions allow unambiguous prefix matching
on option values too.

When an option has a required value, then if a value is not found
within the option word, then the next word is taken to be the option
value, even if it otherwise looks like a option.

-- ends all the options and starts the non-options arguments (even if
they look like options).

PROGRAMMER INTERFACE

The options are specified via an option table.  The option table
specifies option functions which can be used to check and set program
variables representing the option values.

The routines in this module provide facilities for parsing options
(both via an argc, argv[] interface and via a string interface),
routines for initializing the options to default values specified in
the options table, and routines for printing help messages.  It also
provides a large collection of typical option functions.

If the preprocessor symbol PO_OFFLINE_FMT is defined when this file is
included, then a special format is used for the tables and routines
for offline formatting of help messages become available.

*/


/*			OPTION TABLE ENTRIES				*/

typedef enum {	/* Describe option argument types. */
  /* The first 4 are mutually exclusive.  The INTERN flag may be combined
   * with any of the first 3.
   */
  NO_OPT_FLAG= 0,		/* No argument is permitted. */
  OPTIONAL_OPT_FLAG= 1,		/* A argument may or may not be provided. */
  REQUIRED_OPT_FLAG= 2,		/* A argument must be provided. */
  SYNONYM_OPT_FLAG= 3,		/* Used for synonymous options in table. */
  INTERN_OPT_FLAG= 4		/* Argument must be intern'd. */
} OptFlags;

typedef enum { 		/* Option error codes. */
  NO_OPT_ERR= 0,	/* No error. */
  CHECK_OPT_ERR= 1,	/* Invalid option value detected in OptFn(). */
  RANGE_VAL_OPT_ERR,	/* Option value not within specified range. */
  AMBIGUOUS_VAL_OPT_ERR,/* Ambiguous option value. */
  BAD_OPT_ERR,		/* Invalid option. */
  AMBIGUOUS_OPT_ERR,	/* Ambiguous option name. */
  EXPECTED_VAL_OPT_ERR,	/* Expected option value not found. */
  UNEXPECTED_VAL_OPT_ERR,/* Found an option value where one is not expected. */
  OPT_ERR_BIT= 5,	/* # of bits reserved for error-code in return val. */
  OPT_ZZ_ERR=
   1 << OPT_ERR_BIT	/* Programmer error #s must be < this. */
} OptErr;		


/* Typedef for option functions which are called by library routines
 * to check and set each command-line option argument: Each such
 * option function should return 0 if the option with user-defined id
 * and option argument argP (NULL if none) is ok.  It parses argP and
 * stores the resulting value in (a suitably cast) valP.  If checkP is
 * non-NULL, then checkP points to more information needed for
 * checking the option value: typically some kind of array of legal
 * option values.  For error return, it can return an appropriate
 * error code from the above list or an unassigned value (<
 * OPT_ZZ_ERR) different from the error codes above (if a user defined
 * error routine is used for printing error messages).  If it returns
 * non-zero, then that error-code is also returned as part of the
 * return value of the library routine.  The id argument allows
 * several different options to share a single OptFn() which performs
 * slightly different actions depending on the value of id.  id,
 * checkP and valP are simply the corresponding values in the OptInfo
 * entry and are passed unchanged to the optFn; their use can be
 * different from that outlined above provided the optFn understands
 * that use.  argP is a pointer to the character string identified as
 * the argument to the option (NULL if none).
 */
typedef Int (*OptFn) PROTO((Int id, VOIDP checkP, 
			    VOIDP valP, ConstString argP));

#ifdef PO_OFFLINE_FMT
/* Special format used for offline processing of help messages. */
/* 
 * The option table consists of the following OptInfo entries.
 * Duplicate names may be specified for an option by specifying the
 * flags as SYNONYM_OPT_FLAG: all fields except longOpt and shortOpt in
 * such an entry are ignored.  The values for the fields are found in
 * the first entry after the SYNONYM_OPT_FLAG whose flag field is not
 * SYNONYM_OPT_FLAG.
 */
typedef struct {		
  ConstString longOpt;	/* Long option name, NULL if none. */
  VOIDP checkP;		/* Address for extra checking, NULL if none. */
  ConstString valP;	/* Address where option value should be stored. */
  ConstString optFn;	/* Function to parse, check and store option value. */
  ConstString flagsText;/* Flags as a string. */
  ConstString checkPText;/* checkP as a string. */
  Char *doc;		/* Documentation for option --- see allOptsHelp(). */
  ConstString id;	/* User-defined ID --- used with optFn(). */
  Char shortOpt;	/* Short option name, '\0' if none. */
  Char flags;		/* Or of OptFlags with restrictions. */
} OptInfo;

/* Use this macro to statically initialize tables. */
#define OPT_ENTRY(longOpt, shortOpt, argType, 				\
		  id, checkP, valP, optFn, doc)				\
  { longOpt, (VOIDP)checkP, #valP, #optFn, #argType, #checkP, doc, 	\
    #id, shortOpt, argType }

#else /* !defined PO_OFFLINE_FMT */
/* 
 * The option table consists of the following OptInfo entries.
 * Duplicate names may be specified for an option by specifying the
 * flags as SYNONYM_OPT_FLAG: all fields except longOpt and shortOpt in
 * such an entry are ignored.  The values for the fields are found in
 * the first entry after the SYNONYM_OPT_FLAG whose flag field is not
 * SYNONYM_OPT_FLAG.
 */
typedef struct {		
  ConstString longOpt;	/* Long option name, NULL if none. */
  VOIDP checkP;		/* Address for extra checking, NULL if none. */
  VOIDP valP;		/* Address where option value should be stored. */
  OptFn optFn;		/* Function to parse, check and store option value. */
  char *doc;		/* Documentation for option --- see allOptsHelp(). */
  Int2 id;		/* User-defined ID --- used with optFn(). */
  Char shortOpt;	/* Short option name, '\0' if none. */
  Char flags;		/* Or of OptFlags with restrictions. */
} OptInfo;

/* Use this macro to statically initialize tables. */
#define OPT_ENTRY(longOpt, shortOpt, argType, 				\
		  id, checkP, valP, optFn, doc)				\
  { longOpt, (VOIDP)checkP, (VOIDP)valP, optFn, doc, id, 		\
    shortOpt, argType 							\
  }
#endif /* ifdef PO_OFFLINE_FMT */


/*			PARSE OPTION CONTEXT				*/

/* The following provides a context to option routines via a ptr.  If
 * the ptr provided to the routine is NULL, then the routines will use
 * the default values outlined below.
 */
typedef struct {

  /* FILE ptr in which to write help messages.  If 0, defaults to stdout. */
  FILE *helpFile;

  /* FILE ptr in which to write error messages.  If 0, defaults to stderr. */
  FILE *errFile;

  /* If non-NULL, then this function is used to intern the argument
   * for any option which has its INTERN_OPT_FLAG set in the option
   * table.  The intern'd argument is passed to the option function,
   * rather than the raw argument.  This is useful, if options are
   * provided by transient sources whose associated memory will
   * subsequently be reclaimed.  (Typically, when the options are
   * provided by the command-line argv[], then the argv[] memory
   * persists for the duration of the program and this function
   * is unnecessary).  Defaults to NULL.
   */
  ConstString (*internFn)PROTO((ConstString arg, Size argLen));

  /* If non-NULL, this function is called before any option function
   * is called.  The argument id is the id field from the options
   * table for the identified option and argP is the argument provided
   * for the option (NULL if none).  If this function returns
   * non-zero, then the option function is not called.  One use for
   * this is to implement non-sequential option priority (usually
   * options have sequential priority: an option specified later
   * overrides an option specified earlier).  Non-sequential option
   * priority can be used to achieve things like allowing command-line
   * options to override options specified in a program file, even
   * tho' the program file options are processed after the
   * command-line options.  Defaults to NULL.
   */
  Int (*preFn)PROTO((Int id, ConstString argP));

  /* If non-NULL, then this function is called after any option function
   * is called.  It can be used to factor out any common post-processing
   * of optFns.  Defaults to NULL.
   */
  VOID (*postFn)PROTO((Int id, ConstString argP));

} OptContext;

/* Set all values in *optContextP to defaults. */
VOID setOptContextDefaults PROTO((OptContext *CONST optContextP));


/*			    MAIN ROUTINES				*/	

/* Macros to decode value returned on error by parseOpt. */

/* Returns arg # in argv of option at which error was detected. */
#define ERR_RET_ARGN(errRetCode)					\
  (((UInt)(-(errRetCode))) >> (OPT_ERR_BIT + 1))

/* Returns 1 if error was detected in an option value in the next argument
 * from the option; 0 otherwise.
 */
#define ERR_RET_NEXTARG(errRetCode)					\
  ((((UInt)(-(errRetCode))) >> OPT_ERR_BIT) & 1)

/* Returns the OptErr code associated with the error. */
#define ERR_RET_ERRN(errRetCode)					\
  (((UInt)(-(errRetCode))) & ((1 << OPT_ERR_BIT) - 1))

/* Parse options in argv (with argc entries) starting from argv[argIndex],
 * according to table optTab having nEntries.  Options are parsed until
 * any one of the following conditions becomes true:  the end of argv[]
 * is encountered; an error is detected; a non-option argument is detected.
 * If there are no errors, then the return value is > 0 and is the number
 * of the first non-option argument (argc if none).  If an error
 * occurs, then a negative encoded value is returned: the ERR_RET_*
 * macros should be used to decode the return value.
 */
Int parseOptTillNonOpt PROTO((CONST OptContext *CONST optContextP,
		    Int argc, ConstString argv[], Int argIndex,
		    CONST OptInfo *CONST optTab, unsigned nEntries));



/* Parse options in argv (with argc entries) starting from argv[1],
 * according to table optTab having nEntries.  argv[] is reorganized
 * so that all non-option arguments are shifted to its hi end.  If
 * there are no errors, then the return value is > 0 and is the number
 * of the first non-option argument (argc if none).  If an error
 * occurs, then a negative encoded value is returned: the ERR_RET_*
 * macros should be used to decode the return value.
 */
Int parseOpt PROTO((CONST OptContext *CONST optContextP,
		    Int argc, ConstString argv[], 
		    CONST OptInfo *CONST optTab, unsigned nEntries));

/* Call optFns (without calling the pre and post functions) for all
 * options in table which have a default specified in the doc string
 * (see allOptsHelp()).  If the doc-field does not have a '\t' then
 * the OptFn is not called at all; if the doc-field has the '\t' as
 * its last character then the OptFn is called with a NULL argP; if
 * the '\t' is followed by a non-empty string then the OptFn is called
 * with argP pointing to that string.
 */
OptErr parseOptDefaults PROTO((CONST OptContext *CONST optContextP,
			       CONST OptInfo *CONST optTab, unsigned nEntries));

/* Print intro followed by help-documentation on options in optTab
 * (having nEntries entries).  Each doc-string is assumed to have
 * been preformatted offline (see doc under PO_OFFLINE_FMT): it will
 * be printed without any further formatting until its end or until
 * a '\t' is encountered (whichever occurs first).
 */
VOID allOptsHelp PROTO((CONST OptContext *CONST optContextP, 
			CONST char *intro, 
			CONST OptInfo *CONST optTab, unsigned nEntries));

/* Print help message for the option in optTab[indexX], as above. */
VOID optHelp PROTO((CONST OptContext *CONST optContextP,
		    CONST unsigned indexX, CONST OptInfo *optTab,
		    unsigned nEntries));

/* Print an appropriate error message corresponding to errRetValue.
 * If optHelp is non-NULL, then print help for the specified option
 * which is in error (if it can be identified), followed by the 
 * contents of optHelp followed by a newline.
 */
VOID parseOptErr PROTO((CONST OptContext *CONST optContextP,
			Int argc, ConstString argv[],
			Int errRetValue, ConstString optHelp, 
			CONST OptInfo *optTab, unsigned nEntries));

/* Convert words in string optStr to NUL-terminated strings pointed to
 * by the argv[] return value starting at argv[1].  Dynamically
 * allocated memory is used for the argv[] array and to store the
 * NUL-terminated strings.  argv[0] should not be touched by the user
 * of this routine.  A "word" is defined as a sequence of non-space
 * (as determined by isspace()) characters (note that '\n' is a valid
 * word separator).  If commDelim0 is non-NULL, then any contents of
 * optStr between an occurrence of the string pointed to by commDelim0
 * and the string pointed to by commDelim1 (which should then be
 * non-NULL) is treated as a comment and ignored.  Note that the
 * string pointed to by commDelim0 should not start with a isspace()
 * character.  If the closing comment delimiter is not found, then
 * NULL is returned.
 */
ConstString *string2argv PROTO((ConstString optStr, 
				ConstString commDelim0, ConstString commDelim1,
				Int *argcP));

/* Delete memory used by argv[] returned by string2argv(). */
VOID delArgv PROTO((ConstString *argv));


/*			    OFFLINE PROCESSING				*/

#ifdef PO_OFFLINE_FMT
/* The unformatted doc-string in the optTab entries should have the
 * following form:
 *
 *   "OptArgForm\tOptionDesc"
 * or
 *   "OptArgForm\tOptionDesc\tDefault
 *
 * The first '\t' delimiter is required even if the argument-forms are
 * empty.  OptArgForm is usually a single word describing the option
 * argument (if any).  The Default should be the value of the option
 * if the user does not specify it (if a default does not make sense,
 * the second '\t' should simply be omitted).  If a particular option
 * is a default among several options then the default for that option
 * should be empty (specified by the second '\t' at the end of the
 * doc-string), while the other options don't have a second '\t' at
 * all.  OptionDesc should be a short option description string
 * without any whitespace except for single blanks --- the
 * offline-formatting routines break lines at the single spaces.  The
 * help messages should not end with a `.'; the routine will print
 * one.  OptionDesc can contain the following format specifiers (see
 * the documentation for the OptFns to understand the format
 * specifiers which use checkP).
 *
 * %%:  Will be replaced by a single %.
 * %A:	Will be replaced by the OptArgForm.
 * %H:	Will be replaced by the upper-bound of the range pointed to by checkP.
 * %I:  Will be replaced by a comma-separated list of the set of integers
 *	pointed to by checkP.
 * %L:	Will be replaced by the lower-bound of the range pointed to by checkP.
 * %S:	Will be replaced by a comma-separated list of the set of string
 *	pointed to by checkP.
 */

/* This routine takes an option-table with unformatted help messages
 * and outputs an initializer for the option-table with formatted help
 * messages (using the OPT_ENTRY() macro) to helpFile.  The caller needs
 * to output the remaining superstructure around the initializer.
 */
VOID optTabFmt PROTO((FILE *outFile, CONST OptInfo *optTab, unsigned nEntries));

/* This routine is useful for automatically generating TeXInfo
 * documentation: It prints @table entries for all the nEntries
 * options in optInfo on stdout.  Only the option names, the argument
 * type and the doc entries are looked at.
 */
VOID optTab2TeXInfo PROTO((FILE *outFile,
			   CONST OptInfo *optTab, unsigned nEntries));


#endif /* ifdef PO_OFFLINE_FMT */


/* 			PREDEFINED OptFn's. 				*/

/* The following functions converts and stores the integer pointed to
 * by argP in the location pointed to by their third arguments
 * (returning OPT_CHECK_ERR if argP doesn't point to a good integer).
 * A 0 is stored if argP is NULL.  If checkP is non-NULL, then it must
 * point to a long array.If the first entry of the array is non-zero
 * then it must specify the number of entries in the array (including
 * itself) and the remaining entries giving the legal values for the
 * argument.  If the first entry is 0, then checkP points to a 3
 * element array with elements 1 and 2 containing the inclusive lower
 * and upper bounds for the argument.
 */
int charOptFn0 PROTO((int id, VOIDP checkP, VOIDP shortP, ConstString argP));
int shortOptFn0 PROTO((int id, VOIDP checkP, VOIDP shortP, ConstString argP));
int intOptFn0 PROTO((int id, VOIDP checkP, VOIDP intP, ConstString argP));
int longOptFn0 PROTO((int id, VOIDP checkP, VOIDP longP, ConstString argP));

/* The following functions converts and stores the integer pointed to
 * by argP in the location pointed to by their third arguments
 * (returning OPT_CHECK_ERR if argP doesn't point to a good integer).
 * A 1 is stored if argP is NULL.  If checkP is non-NULL, then it must
 * point to a long array.  If the first entry of the array is non-zero
 * then it must specify the number of entries in the array (including
 * itself) and the remaining entries giving the legal values for the
 * argument.  If the first entry is 0, then checkP points to a 3
 * element array with elements 1 and 2 containing the inclusive lower
 * and upper bounds for the argument.
 */
int charOptFn1 PROTO((int id, VOIDP checkP, VOIDP shortP, ConstString argP));
int shortOptFn1 PROTO((int id, VOIDP checkP, VOIDP shortP, ConstString argP));
int intOptFn1 PROTO((int id, VOIDP checkP, VOIDP intP, ConstString argP));
int longOptFn1 PROTO((int id, VOIDP checkP, VOIDP longP, ConstString argP));

/* The following function stores the single char pointed to by argP in
 * the location pointed to by its third argument when argP is
 * non-NULL.  If argP is NULL, '\0' is stored.  If checkP is non-NULL,
 * then it must point to an integer array.  If the first entry of the
 * array is non-zero then it must specify the number of entries in the
 * array (including itself) and the remaining entries giving the legal
 * values for the argument.  If the first entry is 0, then checkP
 * points to a 3 element array with elements 1 and 2 containing the
 * inclusive lower and upper bounds for the argument.
 */
int charOptFn PROTO((int id, VOIDP checkP, VOIDP charP, ConstString argP));

/* The following function stores the string pointed to by argP in
 * the location pointed to by its second argument when argP is non-NULL.
 * If argP is NULL, the string "" is stored.  checkP is ignored.
 */
int stringOptFn PROTO((int id, VOIDP checkP, VOIDP stringP, ConstString argP));

/* The following function stores the index in checkP of the string
 * pointed to by argP in the location pointed to by intP when argP is
 * non-NULL.  If argP is NULL, the value -1 is stored.  checkP should
 * point to a NULL-terminated array of strings, which specify possible
 * values for the argument.  The comparison with the strings in checkP
 * require only that the string pointed to by argP be a unique prefix of
 * one of the strings in checkP[].
 */
int stringValOptFn PROTO((int id, VOIDP checkP, VOIDP intP, ConstString argP));


/* The following function stores a 0 or 1 (1 if argP is NULL) in the
 * character pointed to by charP if the argP points to "no", "off",
 * "0", or "yes", "on", "1".
 */
int yesNoOptFn PROTO((int id, VOIDP checkP, VOIDP charP, ConstString argP));

#endif /* ifndef _PARSEOPT_H */
