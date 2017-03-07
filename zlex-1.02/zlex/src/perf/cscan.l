%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define	ADDEQ	257
#define	ANDAND	258
#define	ANDEQ	259
#define	ARRAY	260
#define	ASM	261
#define	AUTO	262
#define	BREAK	263
#define	CASE	264
#define	CHAR	265
#define	CONST	266
#define	CONTINUE	267
#define	DECR	268
#define	DEFAULT	269
#define	DEREF	270
#define	DIVEQ	271
#define	DO	272
#define	DOUBLE	273
#define	ELLIPSIS	274
#define	ELSE	275
#define	ENUM	276
#define	EQL	277
#define	EXTERN	278
#define	FCON	279
#define	FLOAT	280
#define	FOR	281
#define	FUNCTION	282
#define	GEQ	283
#define	GOTO	284
#define	ICON	285
#define	ID	286
#define	IF	287
#define	INCR	288
#define	INT	289
#define	LEQ	290
#define	LONG	291
#define	LSHIFT	292
#define	LSHIFTEQ	293
#define	MODEQ	294
#define	MULEQ	295
#define	NEQ	296
#define	OREQ	297
#define	OROR	298
#define	POINTER	299
#define	REGISTER	300
#define	RETURN	301
#define	RSHIFT	302
#define	RSHIFTEQ	303
#define	SCON	304
#define	SHORT	305
#define	SIGNED	306
#define	SIZEOF	307
#define	STATIC	308
#define	STRUCT	309
#define	SUBEQ	310
#define	SWITCH	311
#define	TYPEDEF	312
#define	UNION	313
#define	UNSIGNED	314
#define	VOID	315
#define	VOLATILE	316
#define	WHILE	317
#define	XOREQ	318
#define	EOI	319


#define	RET(i)	{ return i;}

%}

O	[0-7]
D	[0-9]
L	[a-zA-Z_]
H	[a-fA-F0-9]
E	[Ee][+-]?{D}+
FS	[fFlL]
IS	[uUlL]*
ESC	[\\]([abfnrtv?'"\\]|"x"{H}+|{O}+)

%X	COMMENT

%%

"/*"			{ BEGIN COMMENT; }
	
"auto"			{ RET(AUTO); }
"break"			{ RET(BREAK); }
"case"			{ RET(CASE); }
"char"			{ RET(CHAR); }
"const"			{ RET(CONST); }
"continue"		{ RET(CONTINUE); }
"default"		{ RET(DEFAULT); }
"do"			{ RET(DO); }
"double"		{ RET(DOUBLE); }
"else"			{ RET(ELSE); }
"enum"			{ RET(ENUM); }
"extern"		{ RET(EXTERN); }
"float"			{ RET(FLOAT); }
"for"			{ RET(FOR); }
"goto"			{ RET(GOTO); }
"if"			{ RET(IF); }
"int"			{ RET(INT); }
"long"			{ RET(LONG); }
"register"		{ RET(REGISTER); }
"return"		{ RET(RETURN); }
"short"			{ RET(SHORT); }
"signed"		{ RET(SIGNED); }
"sizeof"		{ RET(SIZEOF); }
"static"		{ RET(STATIC); }
"struct"		{ RET(STRUCT); }
"switch"		{ RET(SWITCH); }
"typedef"		{ RET(TYPEDEF); }
"union"			{ RET(UNION); }
"unsigned"		{ RET(UNSIGNED); }
"void"			{ RET(VOID); }
"volatile"		{ RET(VOLATILE); }
"while"			{ RET(WHILE); }

{L}({L}|{D})*			{ RET(ID); }

("0"[xX]{H}+{IS}?)|("0"{D}+{IS}?)|({D}+{IS}?) |
([']({ESC}|[^\n\\'])*['])
			{ RET(ICON); }

({D}+{E}{FS}?)|({D}*"."{D}+{E}?{FS}?)|({D}+"."{D}*{E}?{FS}?)
			{ RET(FCON); }
	
(["]({ESC}|[^\n\\"])*["])
				{ RET(SCON); }
	
"..."                   { RET(ELLIPSIS); }
">>="			{ RET(RSHIFTEQ); }
"<<="			{ RET(LSHIFTEQ); }
"+="			{ RET(ADDEQ); }
"-="			{ RET(SUBEQ); }
"*="			{ RET(MULEQ); }
"/="			{ RET(DIVEQ); }
"%="			{ RET(MODEQ); }
"&="			{ RET(ANDEQ); }
"^="			{ RET(XOREQ); }
"|="			{ RET(OREQ); }
">>"			{ RET(RSHIFT); }
"<<"			{ RET(LSHIFT); }
"++"			{ RET(INCR); }
"--"			{ RET(DECR); }
"->"			{ RET(DEREF); }
"&&"			{ RET(ANDAND); }
"||"			{ RET(OROR); }
"<="			{ RET(LEQ); }
">="			{ RET(GEQ); }
"=="			{ RET(EQL); }
"!="			{ RET(NEQ); }
";"			{ RET(';'); }
"{"			{ RET('{'); }
"}"			{ RET('}'); }
","			{ RET(','); }
":"			{ RET(':'); }
"="			{ RET('='); }
"("			{ RET('('); }
")"			{ RET(')'); }
"["			{ RET('['); }
"]"			{ RET(']'); }
"."			{ RET('.'); }
"&"			{ RET('&'); }
"!"			{ RET('!'); }
"~"			{ RET('~'); }
"-"			{ RET('-'); }
"+"			{ RET('+'); }
"*"			{ RET('*'); }
"/"			{ RET('/'); }
"%"			{ RET('%'); }
"<"			{ RET('<'); }
">"			{ RET('>'); }
"^"			{ RET('^'); }
"|"			{ RET('|'); }
"?"			{ RET('?'); }


[ \t\v\f]+	

"\n"

"#"			{ RET('#'); }
.			{ printf("unexpected character: %c\n", yytext[0]); }

<COMMENT>"*/"		{ BEGIN INITIAL; }
<COMMENT>"\n"
<COMMENT>.		

%%

int
main()
{
  int t;
  while((t = yylex()) != 0) {
  }
  return 0;
}
