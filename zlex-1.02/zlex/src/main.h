#ifndef _MAIN_H
#define _MAIN_H

#include "port.h"

extern ConstString dirs;
VOID skelHelp PROTO((FILE *helpFile));
Int versionFn PROTO((Int id, VOIDP checkP, VOIDP valP, ConstString argP));


#endif
