/* !declBegin! */
%{
#include <ctype.h>
#include <math.h>
#include <stdio.h>

int yylex(void);
void yyerror(const char *errMsg);

/* Typedef typical unary <math.h> function. */
typedef double (*MathFnP)(double input);

/* Interface to symbol table. */
static double getIDVal(const char *name);
static MathFnP getIDFn(const char *name);
static void setIDVal(const char *name, double val);
static void setIDFn(const char *name, MathFnP fnP);

%}
%union {
  double val;	  	/* For returning numbers. */
  const char *id;  	/* For returning identifiers. */
}

%token <val>  NUM_TOK  	/* Double precision number */
%token <id>   ID_TOK	/* Identifiers. */
%type  <val>  exp

%right '='
%left '-' '+'
%left '*' '/'
%left NEG		/* Negation--unary minus */
%right '^'    		/* Exponentiation */

/* Grammar follows */
%%
/* !declEnd! */
/* !gramBegin! */
input
  : /* empty */
  | input line
  ;

line
  : '\n'
  | exp '\n'   		{ printf ("\t%.10g\n", $1); }
  | error '\n' 		{ yyerrok; }
  ;

/* !gramXBegin! */
exp
  : ID_TOK 		{ $$= getIDVal($1); }
  | ID_TOK '=' exp   	{ setIDVal($1, $3); $$= $3; }
  | ID_TOK '(' exp ')' 	{ $$= (*(getIDFn($1)))($3); }
/* !gramXEnd! */
  | NUM_TOK
  | exp '+' exp   	{ $$= $1 + $3; }
  | exp '-' exp   	{ $$= $1 - $3; }
  | exp '*' exp   	{ $$= $1 * $3; }
  | exp '/' exp   	{ $$= $1 / $3; }
  | '-' exp  %prec NEG 	{ $$= -$2; }
  | exp '^' exp   	{ $$= pow($1, $3); }
  | '(' exp ')'   	{ $$= $2; }
  ;
/* End of grammar */
%%
/* !gramEnd! */
/* !strBegin! */
/* String space ADT to map identifiers into IDNums. */
typedef struct Ident {
  const char *name;	/* NUL-terminated chars of identifier. */
  struct Ident *succ;	/* Next entry in linear chain. */
} Ident;

/* The string space is a chain of Ident's. */
static Ident *strSpace;

static const char *
getID(const char *name)
{
  Ident *p;
  for (p= strSpace; p != NULL; p= p->succ) {
    if (strcmp(name, p->name) == 0) break;
  }
  if (!p) {
    p= malloc(sizeof(Ident));
    p->name= name;
    p->succ= strSpace; strSpace= p;
  }
  return p->name;
}

/* !strEnd! */
/* !symTypeBegin! */
/* Symbol table ADT. */

/* Possible types for symbols. */
typedef enum { VAR_SYM, FN_SYM } SymType;

typedef struct Sym {
  const char *name;	/* Name of symbol. */
  SymType type;		/* Type of symbol. */
  union {
    double var; 	/* Value of a VAR_SYM. */
    MathFnP fn;		/* Value of a FN_SYM. */
  } value;
  struct Sym *succ;  	/* Link field. */
} Sym;

/* The symbol table: a chain of Sym's.*/
static Sym *symTab;

/* !symTypeEnd! */
/* !getSymBegin! */
/* Search symTab for name.  If doCreate, then create an entry for
 * it if it is not there.  Return pointer to Sym entry.
 */
static Sym *
getSym(const char *name, unsigned doCreate) 
{
  Sym *p;
  for (p= symTab; p != NULL && p->name != name; p= p->succ) ;
  if (p == NULL && doCreate) {
    p= malloc(sizeof(Sym)); 
    p->name= name; p->succ= symTab; symTab= p;
  }
  return p;
}
/* !getSymEnd! */

/* !getValBegin! */
/* Get value associated with name; signal error if not ok. */
static double 
getIDVal(const char *name)
{
  const Sym *p= getSym(name, 0);
  double val= 1.0; /* A default value. */
  if (!p) fprintf(stderr, "No value for %s.\n", name);
  else if (p->type != VAR_SYM) 
    fprintf(stderr, "%s is not a variable.\n", name);
  else val= p->value.var;
  return val;
}

/* Get function associated with name; signal error if not ok. */
static MathFnP 
getIDFn(const char *name)
{
  const Sym *p= getSym(name, 0);
  MathFnP fn= sin; /* A default value. */
  if (!p) fprintf(stderr, "No value for %s.\n", name);
  else if (p->type != FN_SYM) 
    fprintf(stderr, "%s is not a function.\n", name);
  else fn= p->value.fn;
  return fn;
}
/* !getValBegin! */

/* !setSymBegin! */
/* Unconditionally set name to a VAR_SYM with value val. */
static void 
setIDVal(const char *name, double val)
{
  Sym *p= getSym(name, 1);
  p->type= VAR_SYM; p->value.var= val;
}

/* Unconditionally set name to a FN_SYM with fn ptr fnP. */
static void 
setIDFn(const char *name, MathFnP fnP)
{
  Sym *p= getSym(name, 1);
  p->type= FN_SYM; p->value.fn= fnP;
}
/* !setSymEnd! */

/* !initSymBegin! */
/* Initial functions. */
struct { 
  const char *name; 	/* Name of function. */
  MathFnP fn;		/* Corresponding <math.h> function. */
} initFns[]= {
  { "sin", sin },
  { "cos", cos },
  { "atan", atan },
  { "ln", log },
  { "exp", exp },
  { "sqrt", sqrt }
};

static void
initSyms(void)
{
  const unsigned n= sizeof(initFns)/sizeof(initFns[0]);
  unsigned i;
  for (i= 0; i < n; i++) {
    setIDFn(getID(initFns[i].name), initFns[i].fn);
  }
}
/* !initSymEnd! */

/* !readIDBegin! */
/* Read alphanumerics from stdin into a buffer.  Check
 * if identical to previous ident: if so return pointer
 * to previous, else return pointer to new buffer.
 * Assumes char after ident is not an EOF.
 */
static const char *
readID(void)
{
  enum { SIZE_INC= 40 };
  unsigned size= SIZE_INC;
  char *buf= malloc(size);
  unsigned i= 0;
  int c;	
  const char *ident;
    
  do { /* Accumulate stdin into strSpace. */
    c= getchar();
    if (i >= size) buf= realloc(buf, size*= 2);
    buf[i++]= c;
  } while (isalnum(c));

  ungetc(c, stdin); buf[i - 1]= '\0'; /* Undo extra read. */
  buf= realloc(buf, i); /* Resize buf to be only as big as needed. */

  ident= getID(buf);	/* Search string-space. */

  if (ident != buf) free(buf); /* Previously existed. */
  return ident;
}
/* !readIDEnd! */

int
yylex(void)
{
  int c;

  /* Ignore whitespace, get first nonwhite character.  */
  while ((c= getchar ()) == ' ' || c == '\t') ;

  if (c == EOF)
    return 0;

  /* Char starts a number => parse the number.    */
  if (c == '.' || isdigit (c)) {
    ungetc (c, stdin);
    scanf ("%lf", &yylval.val);
    return NUM_TOK;
  }

/* !idLexBegin! */
  /* Char starts an identifier => read the name.  */
  if (isalpha(c)) {
    ungetc(c, stdin);
    yylval.id= readID();
    return ID_TOK;
  }
/* !idLexEnd! */

  /* Any other character is a token by itself.   */
  return c;
}

/* Called by yyparse on error */
void
yyerror(const char *s)  
{
  printf ("%s\n", s);
}

/* !mainBegin! */
int main()
{
  initSyms();
  return yyparse();
}
/* !mainEnd! */


