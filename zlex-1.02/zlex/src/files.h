/*

File:	 files.c
Purpose: Interface to files used.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _FILES_H
#define _FILES_H

#include "port.h"

typedef struct {
  FILE *dumpFile;
} Files;

extern Files files;

#endif /* ifndef _FILES_H */
