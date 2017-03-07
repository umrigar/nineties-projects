/* Test for buffer creation, switching and deletion routines by processing 
 * #include directives. Simply `#include File' at start of a line (with no
 * delimiters around File.  Example essentially from flex manual.
 */
%{
enum { MAX_INCL_DEPTH= 3 };

static YY_BUFFER_STATE inclStk[MAX_INCL_DEPTH];
static unsigned inclSP= 0;

static void beginIncl(char *fName);

%}

fileName	[0-9a-zA-Z./]+

%x INCLUDE
%%
^[ \t]*#[ \t]*include	YY_BEGIN(INCLUDE);
<INCLUDE>{fileName}	beginIncl(YY_TEXT); 
<INCLUDE>[ \t]+
<INCLUDE>\n		YY_BEGIN(INITIAL);
<<EOF>>			{ if (inclSP == 0) 
			    YY_TERMINATE();
			  else {
			    YY_SWITCH_TO_BUFFER(inclStk[--inclSP]);
			    YY_BEGIN(INCLUDE);
			  }
			}
%%

static void beginIncl(char *fName) {
  if (inclSP == MAX_INCL_DEPTH) {
    fprintf(stderr, "Includes nested too deeply.\n");
    return;
  }
  inclStk[inclSP++]= YY_CURRENT_BUFFER;
  yyin= fopen(fName, "r");
  if (!yyin) {
    fprintf(stderr, "Could not open %s.\n", fName); exit(1);
  }
  YY_SWITCH_TO_BUFFER(YY_CREATE_BUFFER(yyin, YY_BUF_SIZE));
  YY_BEGIN(INITIAL);
}

