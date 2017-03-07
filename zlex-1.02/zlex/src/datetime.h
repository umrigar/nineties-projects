/*

File:	 datetime.h
Purpose: Interface to compilation date and time.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "port.h"

typedef struct {
  ConstString date;
  ConstString time;
} DateTime;

extern DateTime dateTime;
