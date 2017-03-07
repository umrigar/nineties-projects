/*

File:	 optcheck.c
Purpose: Options check tables.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


static ConstString compressOpts[]= { 
  "no", 
  "comb", 
  "iterative", 
  NULL 
};
static ConstString defaultOpts[]= { 
  "echo", 
  "error", 
  "fatal", 
  "ignore", 
  NULL 
};
static ConstString tableOpts[]= { 
  "address", 
  "difference", 
  "state", 
  NULL 
};
static int colWasteOpts[]= { 
  0, 
  0, 
  100 
};

