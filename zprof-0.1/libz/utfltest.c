/*

File:	 utfltest.c
Purpose: Test program for find-file routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "zutil.h"
#include "osport.h"

#if MSDOS | _MSDOS | __MSDOS__ | __TURBOC__

static ConstString dirs[]= {
  ";../;$FIND_FILE_PATH",
  "../;;$FIND_FILE_PATH",
  "..;$FIND_FILE_PATH;",
  "..;$FIND_FILE_PATH;.",
  ""
};

#else /* Assume Unix conventions. */

static ConstString dirs[]= {
  ":../:/usr/bin:$FIND_FILE_PATH",
  "../::/usr/bin:$FIND_FILE_PATH",
  "..:/usr/bin:$FIND_FILE_PATH:",
  "..:/usr/bin:$FIND_FILE_PATH:.",
  ""
};

#endif 

int main() {
  while (1) {
    enum {LINE_LENGTH= 120};
    char fileName[LINE_LENGTH];
    ConstString succDir;
    Int succDirLen;
    Index i;
    printf("Enter file name: "); fflush(stdout);
    if (!gets(fileName)) break;
    puts(fileName);
    for (i= 0; i < N_ELEMENTS(dirs); i++) {
      FILE *f;
      printf("Finding `%s' with search list= `%s'.\n", fileName, dirs[i]);
      if ((f= findFileDir(fileName, "r", dirs[i], &succDir, &succDirLen))) {
	printf("\t%.*s%c%s\n", succDirLen, succDir, 
	       DIR_SEPARATOR, fileName);
	fclose(f);
      }
      else {
	printf("Not found.\n");
      }		    
    }
  }
  return 0;
}

