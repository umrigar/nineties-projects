%{
/*

Performance test for delivering C-tokens to a caller.  Comments handled using
start-conditions.  We should really use exclusive start-conditions, but
we have to live with the lowest-common denominator (lex).

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

%s COMMENT

%%
<INITIAL>{alpha}{alnum}*		return ID_TOK;
<INITIAL>{nonzero}{digit}* 		return INT_TOK;
<INITIAL>{nonzero}{digit}*{long} 	return INT_TOK;
<INITIAL>{nonzero}{digit}*{unsigned} 	return INT_TOK;
<INITIAL>{nonzero}{digit}*{ulong}  	return INT_TOK;
<INITIAL>0{octet}* 			return INT_TOK;
<INITIAL>0{octet}*{long}	 	return INT_TOK;
<INITIAL>0{octet}*{unsigned} 		return INT_TOK;
<INITIAL>0{octet}*{ulong} 		return INT_TOK;
<INITIAL>0x{hexet}+ 			return INT_TOK;
<INITIAL>0x{hexet}+{long} 		return INT_TOK;
<INITIAL>0x{hexet}+{unsigned} 		return INT_TOK;
<INITIAL>0x{hexet}+{ulong} 		return INT_TOK;
<INITIAL>{real} 			return REAL_TOK;
<INITIAL>{real}{long}		 	return REAL_TOK;
<INITIAL>{real}{float}		 	return REAL_TOK;
<INITIAL>{ppnum} 			return PPNUM_TOK;
<INITIAL>\"([^"\n\\]|{esc})*\" 		return STRING_TOK;
<INITIAL>L\"([^"\n\\]|{esc})*\"		return STRING_TOK;
<INITIAL>'([^'\n\\]|{esc})*' 		return INT_TOK;
<INITIAL>L'([^'\n\\]|{esc})*'	 	return INT_TOK;
<INITIAL>\"([^"\n\\]|{esc})*\n 		return ERR_TOK;
<INITIAL>L\"([^"\n\\]|{esc})*\n		return ERR_TOK;
<INITIAL>'([^'\n\\]|{esc})*\n 		return ERR_TOK;
<INITIAL>L'([^'\n\\]|{esc})*\n 		return ERR_TOK;
<INITIAL>{space}+ 			;
<INITIAL>{newline} 			{ yylineno++; isPPDir= FALSE; }
<INITIAL>^{space}*#{space}*		isPPDir= TRUE; 
<INITIAL>"->" 				return PTR_TOK;
<INITIAL>"++" 				return INC_TOK;
<INITIAL>"--" 				return DEC_TOK;
<INITIAL>"<<" 				return SHL_TOK;
<INITIAL>">>" 				return SHR_TOK;
<INITIAL>"<=" 				return LE_TOK;
<INITIAL>">=" 				return GE_TOK;
<INITIAL>"==" 				return EQ_TOK;
<INITIAL>"!=" 				return NE_TOK;
<INITIAL>"&&" 				return LOG_AND_TOK;
<INITIAL>"||" 				return LOG_OR_TOK;
<INITIAL>"*=" 				return MULT_ASSGN_TOK;
<INITIAL>"/=" 				return DIV_ASSGN_TOK;
<INITIAL>"%=" 				return MOD_ASSGN_TOK;
<INITIAL>"+=" 				return ADD_ASSGN_TOK;
<INITIAL>"-=" 				return SUB_ASSGN_TOK;
<INITIAL>"<<=" 				return SHL_ASSGN_TOK;
<INITIAL>">>=" 				return SHR_ASSGN_TOK;
<INITIAL>"&=" 				return AND_ASSGN_TOK;
<INITIAL>"^=" 				return XOR_ASSGN_TOK;
<INITIAL>"|=" 				return OR_ASSGN_TOK;
<INITIAL>"..." 				return DOT_DOT_TOK;
<INITIAL>"/*" 				BEGIN COMMENT;
<COMMENT>[^\n*]+ 			;
<COMMENT>[*]+[^/]	 		;
<COMMENT>\n 				;
<COMMENT>"*/"				BEGIN INITIAL;
<INITIAL>. 				return yytext[0];
%%

int main() {
  while (yylex()) ;
  return 0;
}

