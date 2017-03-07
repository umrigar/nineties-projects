/*

File:	 idsp.h
Purpose: Private interface to non-reentrant id routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _IDSP_H
#define _IDSP_H

#include "ids.h"
#include "strspace.h"
#include "textid.h"

typedef struct {
  StrSpace ss;		/* String-space. */
  TextID idTab;		/* Table of identified strings. */
} Globs;

extern Globs _ZidsGlobs;
#define G _ZidsGlobs

#endif /* ifndef _IDSP_H */
