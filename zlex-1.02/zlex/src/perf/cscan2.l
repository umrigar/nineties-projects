%{
/*

Performance test for delivering C-tokens to a caller.  Comments handled using
input().

*/
typedef enum {FALSE, TRUE} Boolean;
Boolean isPPDir= FALSE;
int yylineno;

enum {
  ID_TOK= 256,
  INT_TOK,
  REAL_TOK,
  PPNUM_TOK,
  STRING_TOK,
  ERR_TOK,
  PTR_TOK,
  INC_TOK,
  DEC_TOK,
  SHL_TOK,
  SHR_TOK,
  LE_TOK,
  GE_TOK,
  EQ_TOK,
  NE_TOK,
  LOG_AND_TOK,
  LOG_OR_TOK,
  MULT_ASSGN_TOK,
  DIV_ASSGN_TOK,
  MOD_ASSGN_TOK,
  ADD_ASSGN_TOK,
  SUB_ASSGN_TOK,
  SHL_ASSGN_TOK,
  SHR_ASSGN_TOK,
  AND_ASSGN_TOK,
  OR_ASSGN_TOK,
  XOR_ASSGN_TOK,
  DOT_DOT_TOK
};
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
{alpha}{alnum}*			return ID_TOK;
{nonzero}{digit}* 		return INT_TOK;
{nonzero}{digit}*{long} 	return INT_TOK;
{nonzero}{digit}*{unsigned} 	return INT_TOK;
{nonzero}{digit}*{ulong}  	return INT_TOK;
0{octet}* 			return INT_TOK;
0{octet}*{long}		 	return INT_TOK;
0{octet}*{unsigned} 		return INT_TOK;
0{octet}*{ulong} 		return INT_TOK;
0x{hexet}+ 			return INT_TOK;
0x{hexet}+{long} 		return INT_TOK;
0x{hexet}+{unsigned} 		return INT_TOK;
0x{hexet}+{ulong} 		return INT_TOK;
{real} 				return REAL_TOK;
{real}{long}		 	return REAL_TOK;
{real}{float}		 	return REAL_TOK;
{ppnum} 			return PPNUM_TOK;
\"([^"\n\\]|{esc})*\" 		return STRING_TOK;
L\"([^"\n\\]|{esc})*\"		return STRING_TOK;
'([^'\n\\]|{esc})*' 		return INT_TOK;
L'([^'\n\\]|{esc})*'	 	return INT_TOK;
\"([^"\n\\]|{esc})*\n 		return ERR_TOK;
L\"([^"\n\\]|{esc})*\n		return ERR_TOK;
'([^'\n\\]|{esc})*\n 		return ERR_TOK;
L'([^'\n\\]|{esc})*\n 		return ERR_TOK;
{space}+ 			;
{newline} 			{ yylineno++; isPPDir= FALSE; }
^{space}*#{space}*		isPPDir= TRUE; 
"->" 				return PTR_TOK;
"++" 				return INC_TOK;
"--" 				return DEC_TOK;
"<<" 				return SHL_TOK;
">>" 				return SHR_TOK;
"<=" 				return LE_TOK;
">=" 				return GE_TOK;
"==" 				return EQ_TOK;
"!=" 				return NE_TOK;
"&&" 				return LOG_AND_TOK;
"||" 				return LOG_OR_TOK;
"*=" 				return MULT_ASSGN_TOK;
"/=" 				return DIV_ASSGN_TOK;
"%=" 				return MOD_ASSGN_TOK;
"+=" 				return ADD_ASSGN_TOK;
"-=" 				return SUB_ASSGN_TOK;
"<<=" 				return SHL_ASSGN_TOK;
">>=" 				return SHR_ASSGN_TOK;
"&=" 				return AND_ASSGN_TOK;
"^=" 				return XOR_ASSGN_TOK;
"|=" 				return OR_ASSGN_TOK;
"..." 				return DOT_DOT_TOK;
"/*" 				{ char ch0, ch1= ' ';
				  do {
				    ch0= ch1; ch1= input();
				  } while (ch0 != '*' || ch1 != '/');
				}
. 				return yytext[0];
%%

int main() {
  while (yylex()) ;
  return 0;
}

