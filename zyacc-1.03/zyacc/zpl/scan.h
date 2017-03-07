#ifndef _SCAN_H
#define _SCAN_H

#include "ccport.h"

Int yylex(void);
Index getName(ConstString text, Count len);
ConstString getNameString(Index id);
VOID yyerror(ConstString s);
VOID initScan(VOID_ARGS);

#endif /* ifndef _SCAN_H */
