/*

File:	 textidp.h
Purpose: Private interface for text-id routines.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _STRNGIDP_H
#define _STRNGIDP_H

#include "area.h"
#include "hashtab.h"
#include "memalloc.h"
#include "textid.h"

typedef Index2 TabIndex;

typedef struct {
  CONST char *text;	/* The stored text. */
  Size2 len;		/* # of character in stored stored text. */
} TabEntry;

typedef struct {
  AreaX tab;		/* Where TabEntry s are stored. */
  HashTab hashTab;	/* Hash table. */
  Boolean ignoreCase;  	/* TRUE if case is to be ignored. */
} TextIDStruct;

typedef TextIDStruct *TextIDP;

#define TAB(i)	(AX_ACCESS(textIDP->tab, TabEntry, i))

#endif /* ifndef _STRNGIDP_H */
