%{
static char *strings[]= {
 "This is string 1\n",
 "This is string 2\n",
 "This is string 3\n",
 "This is string 4\n",
 "This is string 5\n"
};

static int index= 0;


%}
%%
<<EOF>> 	{ if (index < sizeof(strings)/sizeof(char *)) {
                    char *s= strings[index++];
		    yy_delete_buffer(YY_CURRENT_BUFFER);
		    yy_switch_to_buffer(yy_scan_string(s));
                  }
                }

%%

int main()
{
  yy_switch_to_buffer(yy_scan_string(strings[index++]));
  return yylex();
}

