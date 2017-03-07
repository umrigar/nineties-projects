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

%s COMMENT

%%
<INITIAL>{alpha}{alnum}*		;
<INITIAL>{nonzero}{digit}* 		;
<INITIAL>{nonzero}{digit}*{long} 	;
<INITIAL>{nonzero}{digit}*{unsigned} 	;
<INITIAL>{nonzero}{digit}*{ulong}  	;
<INITIAL>0{octet}* 			;
<INITIAL>0{octet}*{long}	 	;
<INITIAL>0{octet}*{unsigned} 		;
<INITIAL>0{octet}*{ulong} 		;
<INITIAL>0x{hexet}+ 			;
<INITIAL>0x{hexet}+{long} 		;
<INITIAL>0x{hexet}+{unsigned} 		;
<INITIAL>0x{hexet}+{ulong} 		;
<INITIAL>{real} 			;
<INITIAL>{real}{long}		 	;
<INITIAL>{real}{float}		 	;
<INITIAL>{ppnum} 			;
<INITIAL>\"([^"\n\\]|{esc})*\" 		;
<INITIAL>L\"([^"\n\\]|{esc})*\"		;
<INITIAL>'([^'\n\\]|{esc})*' 		;
<INITIAL>L'([^'\n\\]|{esc})*'	 	;
<INITIAL>\"([^"\n\\]|{esc})*\n 		;
<INITIAL>L\"([^"\n\\]|{esc})*\n		;
<INITIAL>'([^'\n\\]|{esc})*\n 		;
<INITIAL>L'([^'\n\\]|{esc})*\n 		;
<INITIAL>{space}+ 			;
<INITIAL>{newline} 			;
<INITIAL>^{space}*#{space}*		;
<INITIAL>"->" 				;
<INITIAL>"++" 				;
<INITIAL>"--" 				;
<INITIAL>"<<" 				;
<INITIAL>">>" 				;
<INITIAL>"<=" 				;
<INITIAL>">=" 				;
<INITIAL>"==" 				;
<INITIAL>"!=" 				;
<INITIAL>"&&" 				;
<INITIAL>"||" 				;
<INITIAL>"*=" 				;
<INITIAL>"/=" 				;
<INITIAL>"%=" 				;
<INITIAL>"+=" 				;
<INITIAL>"-=" 				;
<INITIAL>"<<=" 				;
<INITIAL>">>=" 				;
<INITIAL>"&=" 				;
<INITIAL>"^=" 				;
<INITIAL>"|=" 				;
<INITIAL>"..." 				;
<INITIAL>"/*" 				BEGIN COMMENT;
<COMMENT>[^\n*]+ 			;
<COMMENT>[*]+[^/]	 		;
<COMMENT>\n 				;
<COMMENT>"*/"				BEGIN INITIAL;
<INITIAL>. 				;
%%

int main() {
  while (yylex()) ;
  return 0;
}

