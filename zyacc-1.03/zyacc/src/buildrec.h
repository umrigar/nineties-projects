/*

File:	 buildrec.h
Purpose: Interface to build date, time and command.

Last Update Time-stamp: "97/07/12 16:18:31 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "port.h"

typedef struct {
  ConstString date;
  ConstString time;
  ConstString cmd;
} BuildRec;

extern BuildRec buildRec;
