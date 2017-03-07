/*

File:	 postrarg.c
Purpose: Utility routine to convert a string into a argv[] array.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "optp.h"

#include "memalloc.h"

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
 * NULL is returned.  Makes two passes over strings, computing needed
 * memory on first pass.
 */
ConstString *
string2argv(optStr, commDelim0, commDelim1, argcP)
  ConstString optStr;
  ConstString commDelim0;
  ConstString commDelim1;
  Int *argcP;
{
  CONST ConstString firstCommStart= 
    (commDelim0) ? strstr(optStr, commDelim0) : NULL;
  ConstString commStart= firstCommStart;
  CONST Size commDelim0Len= (commDelim0) ? strlen(commDelim0) : 0;
  CONST Size commDelim1Len= (commDelim0) ? strlen(commDelim1) : 0;
  ConstString p= optStr;
  Count argc= 1;
  Size textSize= 0;
  ConstString *argv;
  while (*p) {	/* 1st pass to compute memory requirements. */
    if (p == commStart) { /* Start of comment. */
      p= strstr(p + commDelim0Len, commDelim1);
      if (!p) return NULL;
      p+= commDelim1Len;
      commStart= strstr(p, commDelim0);
    }
    else if (isspace(*p)) { /* Skip space. */
      do { p++; } while (isspace(*p));
    }
    else { /* We have a word. */
      ConstString wordStart= p;
      do { p++; } while (!isspace(*p) && *p != '\0' && p != commStart);
      textSize+= p - wordStart + 1; argc++;
    }
  } /* while (*p) */

  argv= MALLOC_VEC(argc, ConstString);
  if (textSize == 0) {
    argv[0]= NULL;
  }
  else {
    Char *text= (Char*)MALLOC(textSize);
    Char *dest= text;
    argv[0]= text;
    p= optStr; argc= 1; commStart= firstCommStart;
    while (*p) {	/* 2nd pass to copy words. */
      if (p == commStart) { /* Start of comment. */
	p= strstr(p + commDelim0Len, commDelim1);
	assert(p);	/* Else we'd have returned NULL on 1st pass. */
	p+= commDelim1Len;
	commStart= strstr(p, commDelim0);
      }
      else if (isspace(*p)) { /* Skip space. */
	do { p++; } while (isspace(*p));
      }
      else { /* We have a word. */
	argv[argc++]= dest;
	do { 
	  *dest++= *p++; 
	} while (!isspace(*p) && *p != '\0' && p != commStart);
	*dest++= '\0';
      }
    } /* while (*p) */
    assert(dest == text + textSize);
  }
  *argcP= argc;
  return argv;
}

/* Delete memory used by argv[] returned by string2argv(). */
VOID 
delArgv(argv)
  ConstString *argv;
{
  FREE(argv[0]);
  FREE(argv);
  VOID_RET();
}

