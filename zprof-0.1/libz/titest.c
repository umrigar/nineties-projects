/*

File:	 titest.c
Purpose: Test program for TextID routines.

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "errorz.h"
#include "hashtab.h"
#include "textidp.h"
#include "strspace.h"

int main PROTO((int argc, ConstString argv[]));

static int 
sizeCmp(CONST VOIDP s1P, CONST VOIDP s2P)
{
  Size *size1P= (Size *) s1P;
  Size *size2P= (Size *) s2P;
  return (*size1P < *size2P) ? -1 : ((*size1P > *size2P) ? 1 : 0);
}

/* Hash lines from command-line files.  Use files like /usr/dict/words
 * for interactive testing.  If the first argument starts with a digit
 * then it is not regarded as a file name: instead the routine does
 * not report lookup actions and at the end hash-table statistics are
 * dumped.
 */
int 
main(argc, argv)
  int argc; 
  ConstString argv[];
{
  int i;
  TextID textID= newTextID(FALSE);
  TextIDP textIDP= (TextIDStruct *) textID;
  StrSpace ss= initStrSpace(FALSE);
  Boolean hashDump= argc > 1 && isdigit(argv[1][0]) != 0;
  Boolean doStdin= argc == hashDump + 1;
  for (i= 1 + hashDump; i < argc || doStdin; i++) {
    FILE *f= doStdin ? stdin : fopen(argv[i], "r");
    if (!f) {
      libzError(ER_SYS|ER_FATAL|ER_NL, "Could not open file `%s'.", argv[i]);
    }
    printf("%s\n", doStdin ? "stdin" : argv[i]);
    doStdin= FALSE;
    while (!feof(f)) {
      enum { LINE_LEN= 120 };
      char line[LINE_LEN]; 
      line[LINE_LEN - 1]= '\0';
      if (!fgets(line, LINE_LEN, f)) break;
      { Size len= strlen(line);
	Index id;
	line[len - 1]= '\0';	/* Clobber '\n'. */
	id= getTextID(textID, line, len);
	if (id == NIL) {
	  id= addTextID(textID, addStrSpace(ss, line, len), len);
	  if (!hashDump) {
	    printf("Added `%s' as ID %d.\n", line, id); 
	  }
	}
	if (id == getTextID(textID, line, len)) { 
	  if (!hashDump) {
	    printf("Retrieved `%s' as ID %d.\n", line, id);
	  }
	}
	else {
	  libzError(ER_FATAL, "Could not retrieve `%s'.\n", line);
	}
      }
    }
  }
  if (hashDump) {
    statsHashTab(textIDP->hashTab);
  }
  delTextID(textID); delStrSpace(ss);
  return 0;
}
