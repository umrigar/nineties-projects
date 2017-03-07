%{
typedef enum {FALSE, TRUE} Boolean;
Boolean isPPDir= FALSE;
int yylineno;
#define ECHOX ECHO
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
exp		([eE][-+]?{digit}+)
real		({fraction}{exp}?|{digit}+{exp})
float		[fF]
ppnum		"."?{digit}({alnum}|"."|[eE][-+])*
esc		\\.
space		[ \t\v\f]
newline		\n
%%
{alpha}{alnum}*	{		/* Identifier.				*/
  ECHOX;
  }
{nonzero}{digit}* {		/* Decimal int, no suffix.		*/
  ECHOX;
  }
{nonzero}{digit}*{long} {	/* Decimal int, long suffix.		*/
  ECHOX;
  }
{nonzero}{digit}*{unsigned} {	/* Decimal int, unsigned suffix.	*/
  ECHOX;
  }
{nonzero}{digit}*{ulong}  {	/* Decimal int, unsigned & long suffix.	*/
  ECHOX;
  }
0{octet}* {			/* Octal int, no suffix.		*/
  ECHOX;
  }
0{octet}*{long} {		/* Octal int, long suffix.		*/
  ECHOX;
  }
0{octet}*{unsigned} {		/* Octal int, unsigned suffix.		*/
  ECHOX;
  }
0{octet}*{ulong} {		/* Octal int, unsigned & long suffix.	*/
  ECHOX;
  }
0x{hexet}+ {			/* Hex int, no suffix.			*/
  ECHOX;
  }
0x{hexet}+{long} {		/* Hex int, long suffix.		*/
  ECHOX;
  }
0x{hexet}+{unsigned} {		/* Hex int, unsigned suffix.		*/
  ECHOX;
  }
0x{hexet}+{ulong} {		/* Hex int, unsigned & long suffix.	*/
  ECHOX;
  }
{real} {			/* Real number, no suffix.		*/
  ECHOX;
  }
{real}{long} {			/* Real number, long suffix.		*/
  ECHOX;
  }
{real}{float} {			/* Real number, float suffix.		*/
  ECHOX;
  }
{ppnum} {			/* Preprocessor #, not C #.		*/
  ECHOX;
  }
\"([^"\n\\]|{esc})*\" {		/* String.				*/
  ECHOX;
  }
L\"([^"\n\\]|{esc})*\" {	/* Wide-char string.			*/
  ECHOX;
  }
'([^'\n\\]|{esc})*' {		/* Character constant.			*/
  ECHOX;
  }
L'([^'\n\\]|{esc})*' {		/* Wide-character constant.		*/
  ECHOX;
  }
\"([^"\n\\]|{esc})*\n {		/* Unterminated string.			*/
  ECHOX;
  }
L\"([^"\n\\]|{esc})*\n {	/* Unterminated wide-char string.	*/
  ECHOX;
  }
'([^'\n\\]|{esc})*\n {		/* Unterminated character constant.	*/
  ECHOX;
  }
L'([^'\n\\]|{esc})*\n {		/* Unterminated wide-character constant.*/
  ECHOX;
  }
{space}+ {
  ECHOX;
  }
{newline} {
  yylineno++; isPPDir= FALSE; ECHOX;
  }
^{space}*#{space}* { 
  isPPDir= TRUE; ECHOX;
  }
"/*" {                          /* Comment. */
  register int c0= input();
  register int c1;
  ECHO;
  do {
    c1= c0; fputc(c0, yyout);
    if (c0 == '\n') yylineno++;
    else if (c0 == EOF) yyterminate(); 
  } while ((c0= input()) != '/' || c1 != '*');
  fputc(c0, yyout);
}
"->" {
  ECHOX;
  }
"++" {
  ECHOX;
  }
"--" {
  ECHOX;
  }
"<<" {
  ECHOX;
  }
">>" {
  ECHOX;
  }
"<=" {
  ECHOX;
  }
">=" {
  ECHOX;
  }
"==" {
  ECHOX;
  }
"!=" {
  ECHOX;
  }
"&&" {
  ECHOX;
  }
"||" {
  ECHOX;
  }
"*=" {
  ECHOX;
  }
"/=" {
  ECHOX;
  }
"%=" {
  ECHOX;
  }
"+=" {
  ECHOX;
  }
"-=" {
  ECHOX;
  }
"<<=" {
  ECHOX;
  }
">>=" {
  ECHOX;
  }
"&=" {
  ECHOX;
  }
"^=" {
  ECHOX;
  }
"|=" {
  ECHOX;
  }
"..." {
  ECHOX;
  }
. {
  ECHOX;
  }
%%
int main() {
  return yylex();
}
