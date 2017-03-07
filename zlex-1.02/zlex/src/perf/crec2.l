%{
/*

Performance test for just recognizing C-tokens.  Comments handled using
start-conditions.  We should really use exclusive start-conditions, but
we have to live with the lowest-common denominator (lex).

*/

%}
digit		[0-9]
nonzero		[1-9]
octet		[0-7]
hexet		[0-9a-fA-F]
alpha		[a-zA-Z_]
alnum		({alpha}|{digit})
long		[lL]
unsigned	[uU]
ulong		({long}{unsigned}|{unsigned}{long})
fraction	({digit}*\.{digit}+|{digit}+\.)
exp		([eE][+-]?{digit}+)
real		({fraction}{exp}?|{digit}+{exp})
float		[fF]
ppnum		"."?{digit}({alnum}|"."|[eE][-+])*
esc		\\.
space		[ \t\v\f]
newline		\n

%%
{alpha}{alnum}*		;
{nonzero}{digit}* 		;
{nonzero}{digit}*{long} 	;
{nonzero}{digit}*{unsigned} 	;
{nonzero}{digit}*{ulong}  	;
0{octet}* 			;
0{octet}*{long}	 	;
0{octet}*{unsigned} 		;
0{octet}*{ulong} 		;
0x{hexet}+ 			;
0x{hexet}+{long} 		;
0x{hexet}+{unsigned} 		;
0x{hexet}+{ulong} 		;
{real} 			;
{real}{long}		 	;
{real}{float}		 	;
{ppnum} 			;
\"([^"\n\\]|{esc})*\" 		;
L\"([^"\n\\]|{esc})*\"		;
'([^'\n\\]|{esc})*' 		;
L'([^'\n\\]|{esc})*'	 	;
\"([^"\n\\]|{esc})*\n 		;
L\"([^"\n\\]|{esc})*\n		;
'([^'\n\\]|{esc})*\n 		;
L'([^'\n\\]|{esc})*\n 		;
{space}+ 			;
{newline} 			;
^{space}*#{space}*		;
"->" 				;
"++" 				;
"--" 				;
"<<" 				;
">>" 				;
"<=" 				;
">=" 				;
"==" 				;
"!=" 				;
"&&" 				;
"||" 				;
"*=" 				;
"/=" 				;
"%=" 				;
"+=" 				;
"-=" 				;
"<<=" 				;
">>=" 				;
"&=" 				;
"^=" 				;
"|=" 				;
"..." 				;
"/*" 				{ char ch0, ch1= ' ';
				  do {
				    ch0= ch1; ch1= input();
				  } while (ch0 != '*' || ch1 != '/');
                                }
. 				;
%%

int main() {
  while (yylex()) ;
  return 0;
}

