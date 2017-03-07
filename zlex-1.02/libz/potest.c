/*

File:	 potest.c
Purpose: Test program for command-line options parsing routines.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/


#include "memalloc.h"
#include "parseopt.h"

static int intFlag0, intFlag1;
static int intVal;
static char charFlag;
static char charVal;
static char *stringFlag;
static char *stringVal;
static int rangeVal;
static short primeVal;
static int nameVal;

#include "potstchk.c"
#include "potstopt.c"

static VOID initVals() 
{
  parseOptDefaults(NULL, optTab, N_ELEMENTS(optTab));
}

ConstString
internText(ConstString s, Size sLen)
{
  Char *z= MALLOC(sLen + 1);
  strcpy(z, s);
  return z;
}
  

int main()
{
  CONST Count nEntries= N_ELEMENTS(optTab);
  allOptsHelp(NULL, "Usage: parseOpt [Options] [RestArgs]", 
	      optTab, nEntries);
  while (1) {
    enum { LINE_LENGTH= 120 };
    Char line[LINE_LENGTH];
    printf("Enter dummy command line: "); fflush(stdout);
    initVals();
    if (!gets(line)) {
      break;
    }
    else { /* Not EOF. */
      OptContext optContext;
      ConstString *argv;
      Int lineLen= strlen(line);
      Int argc;
      Int nonOptArgN;
      setOptContextDefaults(&optContext);
      puts(line);
      line[lineLen]= '\n'; line[lineLen + 1]= '\0';
      optContext.internFn= internText;
      argv= string2argv(line, "#", "\n", &argc);
      nonOptArgN= parseOpt(&optContext, argc, argv, optTab, nEntries); 
      if (nonOptArgN < 0) {
	parseOptErr(&optContext, argc, argv, nonOptArgN, "No help option", 
		    optTab, nEntries);
      }
      delArgv(argv);
      if (nonOptArgN >= 0) {
	printf("intFlag0= %d; intFlag1= %d; intVal= %d\n", 
	       intFlag0, intFlag1, intVal);
        printf("charFlag= `%c'; charVal= `%c'\n", charFlag, charVal);
	printf("stringFlag= \"%s\"; stringVal= \"%s\"\n", 
	       stringFlag, stringVal);
	printf("rangeVal= %d; primeVal= %d; names= %d\n", 
	       rangeVal, primeVal, nameVal);
	printf("Non-option args: ");
	for (; nonOptArgN < argc; nonOptArgN++) { 
	  printf("%s ", argv[nonOptArgN]); 
	}
	printf("\n");
      }
    } /* else not EOF. */
  } /* while (1) */
  return 0;
}

