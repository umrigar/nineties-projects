/*

File:	 osport.h
Purpose: Porting definitions to work-around OS differences.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


/* Character used to separate directory and path components. */
#if MSDOS | _MSDOS | __MSDOS__ | __TURBOC__

#ifndef DIR_SEPARATOR
#define DIR_SEPARATOR	'\\'
#endif

#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR 	';'
#endif 

#ifndef IS_ABSOLUTE_PATH
#define IS_ABSOLUTE_PATH(n)						\
  ( n[0] == DIR_SEPARATOR 						\
  || (strlen(n) > 2 && isalpha(n[0]) && n[1] == ':'))
#endif

#else /* Not an MSDOG system -- assume UNIX. */

#ifndef DIR_SEPARATOR
#define DIR_SEPARATOR	'/'
#endif

#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR	':'
#endif

#ifndef IS_ABSOLUTE_PATH
#define IS_ABSOLUTE_PATH(n)	(n[0] == DIR_SEPARATOR)
#endif

#endif	/* else not an MSDOG system. */


