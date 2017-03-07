/*

File:	 attribs.c
Purpose: Front and back-end for attribute processing.

Last Update Time-stamp: "97/07/30 20:21:14 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

/* Local includes. */
#include "attribs.h"
#include "gcolor.h"
#include "gram.h"
#include "lalr.h"
#include "scan.h"

/* Libz includes. */
#include "area.h"
#include "iset.h"
#include "error.h"
#include "ids.h"
#include "memalloc.h"

/* System includes via port.h. */

/*

Terminal attributes are declared in section 1 using:

%token Term(%out TypePrefix0 $Attr0 TypeSuffix0, ...)

This module ensures that all terminal attributes are %out.  Multiple
declaration of terminal attributes implies multiple declaration of the
terminal which will be detected by another module.  Hence this module
does not signal multiple declarations of terminal attributes as errors.

Non-terminal attributes are not allowed to be declared in section 1.
Instead they are declared in the LHS of rules.

NonTerm(InOut TypePrefix0 $Attr0 TypeSuffix0, ...) : ...

where InOut is either %in or %out.  All rules for a non-terminal which
has named attributes MUST share the same LHS.

Note that usually the InOut specifier for both terminal and
non-terminal attribute declarations is optional (defaults to %out).
The only situation in which a InOut specifier cannot be omitted is
immediately after the left parenthesis of a rule LHS, if the previous
rule (if there was one) was not terminated by a semi-colon.

Given that the attribute-flow is required to be L-attributed, it is
possible to do a consistency check between the occurrence of an attribute 
variable and the declared in/out type of that attribute.  

a) A defining attribute position on the RHS (a position declared %out
for the corresponding RHS symbol) can only consist of a single
attribute variable which must be the first occurrence of that
attribute variable in the rule (not counting variables declared %out
in the LHS).

b) An applied attribute position on the RHS (a position declared %in
for the RHS symbol) can only contain attribute variables which have had
previous occurrences in the rule (not counting vars declared %out in
the LHS).

c) Each attribute variable can occur in only a single defining
position (a defining position is a position declared %in on the LHS or
%out on the RHS).  This implies that variables declared %out in the
LHS can only have a single occurrence in a %out position on the RHS.

We ensure that the synthesized attributes for the LHS have their first
occurrence in the final rule action.  For example:

N(%out int $a, %out int $b): N1($a) N2($b);

is converted to: 

N(%out int $a, %out int $b): N1($a1) N2($b1) { $a= $a1; $b= $b1; }

As a non-terminal can be used before its rules appear, this module
allows forward references to non-terminals.  When such a forward
reference appears, the module records the number of attributes and
their implied in/out pattern, which is checked with the declaration
when it finally appears.

Actions are allowed to use a attribute-variable with its first (defining)
occurrences within that action, provided that attribute variable has
a subsequent copy-occurrences in a %in position in a subsequent RHS
symbol in the same rule (allowing inferring the types of such a
variable).  Specifically, something like:

N(%in int $a, %out int $b):
  { $c= f(a); }
  M($c, $b);

M(%in float $a, int $b): 
  { $b= g($a); }
  ...

Here the type of $c for the internal action of the rule for N is
inferred to be float from its subsequent use in M. 

This module infers the types of the attributes of the nullable
non-terminal created for an action which is properly within a rule,
from the types of the corresponding attributes when they are used
elsewhere in the rule.  For example,

N(%in int $a): { f($a, $b); } M($b);
M(%in float $a): ...

will create a nullable non-terminal say _3 for the internal action of the
type:
_3(%in int $a, %out float $b): ;



IMPLEMENTATION NOTES

All attribute data is stored here --- this coupled with the need for
quick lookup implies that this module maintains several AREA arrays
which operate in parallel with those in the gram module.  The code
assumes that the arrays are indeed parallel with the other arrays
(checked by asserting that new indexes created within this module are
equal to external indexes).

The tokens (type AttribTok) comprising attribute expressions and
attribute declarations are stored in array ATTRIB_TOKS[].  The limits
in ATTRIB_TOKS[] of each attribute expression or declaration (with
each entry delimiting one expression) is stored in array
ATTRIB_SEQS[].

The formal attributes of each terminal and non-terminal symbol
(including the dummy non-terminals created for internal actions) are
stored in the area ID_VARS[].  Each IDVar will point to a type
declaration in ATTRIB_TOKS[] via a ATTRIB_SEQ[] entry.  These formal
declarations in ID_VARS[] are indexed by TERM_ATTRIBS[] (for
terminals) and NON_TERM_ATTRIBS[] (for non-terminals).  If a symbol
(terminal or non-terminal) has arity attributes, then the
corresponding one of the above two arrays store the limits of a
sequence of arity declarations in ID_VARS[] for each symbol. We assume
that the terminal/non-terminal whose attributes being are being
declared are the last ones to be declared within their respective
arrays: new terminal and non-terminal indexes are just beyond the end
of these arrays.

Finally each symbol which occurs on the RHS has its attribute
expressions in ATTRIB_SEQS[] (the number of expressions corresponding
to its arity) delimited by entries of type RHSAttrib in array
RHS_ATTRIBS[].  The last array is indexed by the RHS index (item
number).

An auxiliary data structure ATTRIB_SYMS[] is an array used as a
linearly-searched symbol table to store attributes as a rule is being
parsed.  Named attribute variables are inserted into the symbol table
with their name-id, defining position and corresponding formal IDVar
for that occurrence.

To check for consistent L-attribution, the following method is used:

We maintain the current position within the current-rule in a global
data-structure.  This consists of three coordinates symNum (0 =
LHS, 1 = 1st RHS symbol, etc), argNum (0 = 1st argument, 1 = 2nd
argument, etc) within each argument, and tokNum (starting from 0)
within each argument.  These coordinates are reset at the end of each
rule, symbol, argument respectively.

When a named attribute-variable is found, it is looked up in the
symbol table.  If it is not already there and it is not a %out
variable on the LHS, its current occurrence is assumed to be a
defining occurrence and it is entered into the symbol-table. 

At the point after a RHS symbol is completed, its attribute
expressions are processed.  Each attribute expression is classified as
either a COPY_EXP, DEF_EXP or COMPOUND_EXP.  A COPY_EXP consists of a
single attribute variable which has had a defining occurrence in a
previous symbol.  A DEF_EXP consists of a single attribute expression
which has not had any previous occurrences earlier in the rule (not
counting %out declarations on the LHS).  A COMPOUND_EXP is an
attribute expression with all contained attribute variables having had
defining occurrences in previous symbols.  If an expression cannot be
classified as one of the above, then it is inconsistently attributed
and results in a error.

As each RHS attribute expression is checked, it is checked for
consistency with any previous declaration for that attribute.  A
DEF_EXP requires that the corresponding attribute has been declared
%out, a COPY_EXP or COMPOUND_EXP requires that the corresponding
attribute has been declared %in.  If the RHS symbol has not been
previously declared, but has had a previous forward-reference, then
the forward-referenced declarations are checked for consistency with
previously inferred %in/%out.  If the RHS symbol has never been
seen previously, then a forward-referenced declaration (to be
backpatched later) is created, and the inferred %in/%out noted.

As far as this module is concerned, the only type of attribute tokens
which can be contained within an action are named attribute
variables.  When the end of the action is reached, a routine in this
module is called.  This routine goes thru all the attribute variables
in the action and eliminates duplicates.  It remembers the fact that
there is an unprocessed action.

When a attribute token or RHS symbol is to be inserted, the insertion
routine first checks whether there is an unprocessed action.  If there
is, a new nullable non-terminal is created as usual.  The attributes
for the new non-terminal are the unique attribute variables in the
action.  Attribute variables in the action with defining occurrences
in earlier symbols are classified as %in, the others are classified as
%out.  The type for %in attributes are inferred from the type of the
corresponding defining attribute occurrence if it has been declared.
If the defining occurrence was within another internal action, then
the type of this attribute variable is inserted onto the backpatch
chain of that defining occurrence.  If it hasn't yet been declared,
then the unknown type is remembered on the back-patch chain to be
back-patched when the symbol is declared.  Subsequently if there is a
COPY occurrence of that variable in a symbol in the rule, two
possibilities arise: If the symbol's attribute types have been
declared then the backpatch chain is filled in with the relevant type;
if the symbol's attribute types have not been declared, then the
backpatch chain is appended to the backpatch chain for the
corresponding attribute.  It is an error if there is no such COPY
occurrence.

At the end of each rule, the module checks the sanity of attribute
flow within the rule.  Specifically, it checks that each %out
attribute on the LHS occurs on the RHS.  If the defining occurrence of
such an attribute is not in the final rule action, then this module
produces assignments which are inserted just after the final rule
action. 

*/


/*		    GLOBAL VARS USED IN THIS MODULE.			*/

/* The globs.hasAttribs flag is set to true as soon as any attribute
 * token appears.  It is used to prevent allocation of memory for
 * those source files which do not use named attributes.  When the
 * flag is set true, the necessary memory is allocated and data
 * structures initialized.  
 */

static struct {		/* Mainly properties of current rule. */
  Index2 symNum;	/* Number of current symbol: 0 == LHS, etc. */
  Count2 argNum;	/* Current argument number within current tuple. */	
  Count2 tokNum;	/* Token number within current exp. */	
  Count2 lhsNonTerm;	/* LHS Non-terminal for last user rule. */
  Count2 lhsArity;	/* # of attrib-args of LHS non-term of current rule. */
  Index2 lastActN;	/* If last sym was act, then act # else NIL. */
  Count2 nExtraRules;	/* # of extra rules to be created by current rule. */
  Boolean isOut;	/* TRUE if LHS attrib-var is %out. */
  Boolean hasAttribs;	/* TRUE if we have seen at least 1 attr tok. */
  Boolean hasParseError;/* TRUE if parse error in last attrib. sequence. */
} globs;

#define G globs


/*			  STATIC PROTOTYPES.				*/

struct AttribTuple;

static Index newInVar PROTO((Index idVar));
static Index newIDVar PROTO((Index id, Index sym, Index argN, Index typeExp, 
			     BooleanX isOut));
static VOID makeIDVarIn PROTO((Index idVar));
static VOID resolveForwardIDVar PROTO((Index idVar, Index id, Index typeExp));
static Count outAttribsOutType PROTO((FILE *outFile));
static Count outType PROTO((FILE *outFile, Index typeExp, 
			    ConstString idString));
static VOID doFirstAttrib PROTO((VOID_ARGS));
static VOID internalActAttribs PROTO((VOID_ARGS));
static Count outExp PROTO((FILE *outFile, Index exp));
static Count outAttribTok PROTO((FILE *outFile, Index tokIndex));
static Index getAttribSym PROTO((Index id));
static Index getSymID PROTO((Index symIndex));
static Index getSymNum PROTO((Index symIndex));
static VOID setSymNum PROTO((Index symIndex, Index symNum));
static Index getSymIDVar PROTO((Index symIndex));
static VOID setSymIDVar PROTO((Index symIndex, Index idVar));
static Boolean getSymIsUntyped PROTO((Index symIndex));
static VOID setSymIsUntyped PROTO((Index symIndex, BooleanX isUntyped));
static VOID resetSymsToLHS PROTO((VOID_ARGS));
static VOID checkLHSOutSyms PROTO((VOID_ARGS));
static VOID checkInternalActAttribs PROTO((VOID_ARGS));
static VOID clearSyms PROTO((VOID_ARGS));
static Count outExp PROTO((FILE *outFile, Index exp));
static Count outType PROTO((FILE *outFile, Index typeExp, 
			    ConstString idString));
static Boolean equalTypes PROTO((Index typeExp1, Index typeExp2));
static VOID installNewDec PROTO((Index sym, struct AttribTuple *CONST tupleP));
static VOID installInferredDec PROTO((Index sym, 
				      struct AttribTuple *CONST tupleP));
static VOID processCopyExp PROTO((Index symIndex, Index idVar0, 
				  Index idVar1, BooleanX isUntyped1));
static Index getExpType PROTO((Index seqsIndex, Index idVar, 
			       BooleanX isActExp));
static VOID checkNewUse PROTO((Index sym, struct AttribTuple *CONST tupleP));
static VOID checkDeclaredUse 
  PROTO((Index sym, CONST struct AttribTuple *CONST tupleP));
static VOID checkOldDeclaration PROTO((Index nonTerm));
static Boolean hasInAttribs PROTO((Index nonTerm));
static VOID ensureLastActHasAttrib PROTO((Index id));
static VOID fixLastAction PROTO((Index ruleN));
static VOID internalActAttribs PROTO((VOID_ARGS));
static Count outDefUndef PROTO((FILE *outFile, Index reductN, BooleanX isDef));
static VOID doFirstAttrib PROTO((VOID_ARGS));
static Count hashType PROTO((Index typeExp));
static VOID buildInTypes PROTO((VOID_ARGS));
static Index getInExpIndex PROTO((CONST Index state, CONST Index idVar));
static Boolean hasClosureIDVars PROTO((Index exp));
static Index evaluateExp PROTO((CONST Index state, CONST Index item, 
				CONST Index exp));
static Boolean equalExps PROTO((Index exp1, Index exp2));
static VOID processItemIns PROTO((CONST Index state, CONST Index item));
static VOID processTestExp PROTO((Index exp));
static VOID setClosureOrder PROTO((Index2 items[], CONST Count nItems));
static VOID computeStateIns PROTO((VOID_ARGS));
static NodePairs buildColorPreferences PROTO((VOID_ARGS));
static Graph buildInterferenceGraph PROTO((VOID_ARGS));
static VOID processColoring PROTO((Coloring c));
static VOID eliminateRedundantCopies PROTO((VOID_ARGS));
static Count outInAttribNames PROTO((FILE *outFile, Index class, 
				     Index typeNum));
static Count outAttribsInType PROTO((FILE *outFile));
static VOID storeInAttribs PROTO((VOID_ARGS));




/*			    IN VARS.					*/

typedef struct {	/* Extra information for inherited IDVars. */
  Index2 idVar;		/* Index of corresponding IDVar. */
  Index2 typeNum;	/* The type number for this attribute. */
  union {
    Index2 class;	/* The eq-class for this attribute. */
    Index2 typeChain;	/* Chain of identical types. */
  } u;
} InVar;

static AREA (inVars, InVar, 6, UINT2_MAX);
#define IN_VARS(i) A_ACCESS(inVars, InVar, i)

static Index 
newInVar(idVar)
  Index idVar;
{ 
  CONST Index j= A_NEXT(inVars);
  InVar *CONST jP= &IN_VARS(j);
  jP->idVar= idVar; 
  jP->typeNum= jP->u.typeChain= NIL; /* Fill in later. */
  return j;
}

Count 
nInIDVars()
{
  return A_NENTRIES(inVars);
}



/*			    ID VARS.					*/

/* In the following struct, id == NIL -> this IDVar corresponds to an
 * attribute for a non-terminal which hasn't yet been defined.  In
 * this case, typeExp points to a list of IDVars whose type depends on
 * the type of this idVar (these will be %out attributes of internal
 * actions whose type is inferred from this undefined non-terminal).
 */
typedef struct {	/* Property of each formal attribute variable. */
  Index2 id;		/* Formal name of attribute variable. */
  Sym sym;		/* The symbol for which this ident. is an attrib. */
  Index2 argN;		/* The argument index within symbol. */
  Index2 typeExp;	/* Index in ATTRIB_SEQS[] for type.  Also backpatch 
			 * chain for idVars of same but unknown type. */
  Index2 inVar;		/* NIL if %out; else index in IN_VARS[]. */
} IDVar;

static AREA (idVars, IDVar, 7, UINT2_MAX);
#define ID_VARS(i) A_ACCESS(idVars, IDVar, i)

/* Create a new IDVar with specified fields.  If !isOut, then create a
 * new InVar entry as well.
 */
static Index 
newIDVar(id, sym, argN, typeExp, isOut)
  Index id; 
  Index sym; 
  Index argN; 
  Index typeExp; 
  BooleanX isOut;
{
  CONST Index i= A_NEXT(idVars);
  IDVar *CONST iP= &ID_VARS(i);
  iP->id= id; iP->sym= sym; iP->argN= argN; 
  iP->typeExp= typeExp; 
  iP->inVar= (isOut) ? NIL : newInVar(i);
  return i;
}

/* Make the IDVar idVar refer to a inherited attribute. */
static VOID 
makeIDVarIn(idVar)
  Index idVar;
{
  assert(idVar < A_NENTRIES(idVars));
  assert(ID_VARS(idVar).inVar == NIL);
  ID_VARS(idVar).inVar= newInVar(idVar);
  VOID_RET();
}

/* This routine is called when a non-terminal which has already
 * occurred on the RHS occurs for the first time on the LHS.  It
 * updates the formal attribute name to id.  It also goes thru all
 * IDVars chained on the typeExp field, changing their typeExp fields
 * to typeExp.
 */
static VOID 
resolveForwardIDVar(idVar, id, typeExp)
  Index idVar;
  Index id; 
  Index typeExp;
{
  IDVar *CONST iP= &ID_VARS(idVar);
  Index i;
  assert(iP->id == NIL);
  iP->id= id;
  i= idVar;
  do {
    CONST Index i1= ID_VARS(i).typeExp;
    ID_VARS(i).typeExp= typeExp;
    i= i1;
  } while (i != NIL);
  VOID_RET();
}

Boolean
hasTermIDVars()
{
  CONST Index nIDVars= A_NENTRIES(idVars);
  Index v;
  for (v= 0; v < nIDVars; v++) {
    CONST Index sym= ID_VARS(v).sym;
    CONST SymType symType= SYM_TYPE(sym);
    if (symType == TERM_SYM) return TRUE;
  }
  return FALSE;
}

#define IN_VAR_MACRO			"YY_IN_VAR"
#define TERM_VAR_MACRO			"YY_TERM_VAR"
#define NON_TERM_VAR_MACRO		"YY_NON_TERM_VAR"
#define CLASS_SELECTOR_PREFIX 		"yyC_"
#define TYPE_NUM_SELECTOR_PREFIX	"yyT_"
#define NON_TERM_SELECTOR_PREFIX	"yy_"

/* Output idVar to outFile.  The output is OutMacro(offset, Selector),
 * where OutMacro is either YY_IN_VAR, YY_TERM_VAR, or YY_NON_TERM_VAR
 * offset is the argument to the function and Selector is a field
 * selector within the semantic value.  If the idVar is for a
 * terminal, then (the idVar must have been %out) the selector is
 * simply the <type> declared for the terminal.  If idVar is %out for
 * a non-terminal then the selector is a prefix yy_ catenated with the
 * non-terminal name.  Else it is a non-terminal %in idVar and the
 * selector is of the form yyC_Class.yyT_Type where Class and Type are
 * integers.
 */
VOID 
outIDVar(outFile, idVar, offset)
  FILE *outFile;
  Index idVar;
  Int offset;
{
  CONST IDVar *CONST iP= &ID_VARS(idVar);
  assert(idVar < A_NENTRIES(idVars));
  if (iP->inVar == NIL) { /* %out attribute. */
    Index symNum= SYM_NUM(iP->sym);
    if (SYM_TYPE(ID_VARS(idVar).sym) == TERM_SYM) {
      fprintf(outFile, "%s(%d, %s.%s)", TERM_VAR_MACRO, offset, 
	      getIDString(getTermType(symNum)), getIDString(iP->id));
    }
    else {
      fprintf(outFile, "%s(%d, %s%s.%s)", NON_TERM_VAR_MACRO, offset,
	      NON_TERM_SELECTOR_PREFIX, getNonTermString(symNum),
	      getIDString(iP->id));
    }
  }
  else { /* %in attribute. */
    CONST InVar *CONST inP= &IN_VARS(iP->inVar);
    assert(iP->inVar < A_NENTRIES(inVars));
    fprintf(outFile, "%s(%d, %s%d.%s%d)", IN_VAR_MACRO, offset,
	    CLASS_SELECTOR_PREFIX, inP->u.class,
	    TYPE_NUM_SELECTOR_PREFIX, inP->typeNum);
  }
  VOID_RET();
}

/* Output semantic struct declaration for %out attributes of terminal
 * symbols to outFile.  Returns # of lines output.
 */
Count 
outAttribsTermTypes(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  CONST Index nIDVars= A_NENTRIES(idVars);
  ISet termTypeIDs= newISet(); /* Ensures terminal-types output once. */
  Index v;
  for (v= 0; v < nIDVars; ) {
    CONST Index sym= ID_VARS(v).sym;
    CONST SymType symType= SYM_TYPE(sym);
    CONST Index symNum= SYM_NUM(sym);
    CONST Index termTypeID= symType == TERM_SYM ? getTermType(symNum) : NIL;
    assert(symType == NON_TERM_SYM || termTypeID != NIL);
    if (termTypeID == NIL || inISet(termTypeID, termTypeIDs)) {
      while(v < nIDVars && ID_VARS(v).sym == sym) v++;
    }
    else { /* new terminal type */
      fputs("  struct {\n", outFile); nLines++;
      addISet(termTypeIDs, termTypeID);
      while(v < nIDVars && ID_VARS(v).sym == sym) {
	if (ID_VARS(v).inVar == NIL) { /* Output this %out. */
	  fputs("    ", outFile);
	  nLines+= outType(outFile, ID_VARS(v).typeExp, 
			   getIDString(ID_VARS(v).id));
	  fputs(";\n", outFile); nLines++;
	}
	v++;
      } /* while(v < nIDVars && ID_VARS(v).sym == sym) */
      assert(v == nIDVars || ID_VARS(v).sym != sym);
      fprintf(outFile, "  } %s;\n", getIDString(termTypeID)); nLines++;
    } /* else new terminal type */
  } /* for (v= 0; v < nIDVars; ) */
  delISet(termTypeIDs);
  return nLines;
}

/* Output typedef for YYOut semantic struct to outFile.  Returns # of
 * lines output.
 */
static Count 
outAttribsOutType(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  CONST Index nIDVars= A_NENTRIES(idVars);
  Index v;
  fputs("typedef union {\n  YYSTYPE yySType;\n", outFile); nLines+= 2;
  for (v= 0; v < nIDVars; ) {
    CONST Index sym= ID_VARS(v).sym;
    CONST SymType symType= SYM_TYPE(sym);
    if (symType == TERM_SYM) {
      while(v < nIDVars && ID_VARS(v).sym == sym) v++;
    }
    else { /* non-terminal symbol */
      Boolean hasOut= FALSE;
      Index v0= v;
      for (; v < nIDVars && ID_VARS(v).sym == sym; v++) {
	if (ID_VARS(v).inVar == NIL) { /* non-term does have a %out */
	  hasOut= TRUE; break;
	}
      }
      if (hasOut) {
	v= v0;
	fputs("  struct {\n", outFile); nLines++;
	while(v < nIDVars && ID_VARS(v).sym == sym) {
	  if (ID_VARS(v).inVar == NIL) { /* Output this %out. */
	    fputs("    ", outFile);
	    nLines+= outType(outFile, ID_VARS(v).typeExp, 
			     getIDString(ID_VARS(v).id));
	    fputs(";\n", outFile); nLines++;
	  }
	  v++;
	} /* while(v < nIDVars && ID_VARS(v).sym == sym) */
	fprintf(outFile, "  } %s%s;\n", NON_TERM_SELECTOR_PREFIX,
		getNonTermString(SYM_NUM(sym))); 
	nLines++;
      } /* if (hasOut) */
    } /* else non-terminal symbol */
  } /* for (v= 0; v < nIDVars; ) */
  fputs("} YYOut;\n", outFile); nLines++;
  return nLines;
}

#define ATTRIB_NAME_PREFIX	"yyYY"

/* Output attribName to outFile with a yyYY prefix.  The outFile will
 * define them as macros, making 1 pass attribute processing possible.
 * (Basically, the second pass is done by cpp when the user compiles
 * the generated parser).
 */
VOID 
outAttribVar(outFile, attribName)
  FILE *outFile;
  ConstString attribName;
{
  fputs(ATTRIB_NAME_PREFIX, outFile); fputs(attribName, outFile);
  VOID_RET();
}



/*			ATTRIBUTE TOKENS.				*/

/* ATokType typedef'd in attribs.h. */

enum {				/* Extend ATokType for internal use. */
  ID_VAR_DEC_ATOK= ZZ_ATOK,	/* An ID_VAR in a attribute declaration. */
  ID_VAR_XUSE_ATOK,		/* A RHS ID_VAR when it first comes in. */
  ID_VAR_USE_ATOK,		/* A RHS ID_VAR after it has been resolved. */
  NESTED_EXP_ATOK		/* The index of a nested expression. */
};

typedef struct {	/* An LHS ID_VAR which is being declared. */
  Index2 id;		/* The formal-name of the attribute-variable. */
  Boolean isOut;	/* TRUE if its %out. */
} IDDec;

typedef struct {	/* A RHS ID_VAR when it first comes in. */
  Index2 id;		/* The name of the attribute-variable. */
} IDXUse;

typedef struct {	/* A RHS ID_VAR after it has been resolved. */
  Index2 id;		/* The actual name used for this attribute variable. */
  Int2 offset;		/* The offset of the defining symbol from this use. */
  Index2 idVar;		/* Index in ID_VARS[] of formal attribute. */
} IDUse;

typedef struct {
  union {
    ConstString text;		/* type == TEXT_ATOK. */
    char shortText		/* type == SHORT_TEXT_ATOK. */
      [sizeof(ConstString)];
    NumVar numVar;		/* type == NUM_VAR_ATOK. NumVar in "gram.h". */
    IDDec idDec;		/* type == ID_VAR_DEC_ATOK. */
    IDXUse idXUse;		/* type == ID_VAR_XUSE_ATOK. */
    IDUse idUse;		/* type == ID_VAR_USE_ATOK. */
    Index2 exp;			/* type == NESTED_EXP_ATOK. */
  } u;
  UChar type;			/* ATokType. */
} AttribTok;

static AREA (attribToks, AttribTok, 10, UINT2_MAX);
#define ATTRIB_TOKS(i) A_ACCESS(attribToks, AttribTok, i)

/* Add attribute token of specified type to ATTRIB_TOKS[], with
 * properties specified by *infoP.  IN_ATOK and OUT_ATOK are not
 * added, but merely set the globs.isOut flag.  ID_VAR_ATOK are
 * converted to either a ID_VAR_DEC_ATOK or ID_VAR_XUSE_ATOK depending
 * respectively on whether G.symNum == 0 (a LHS or token declaration)
 * or G.symNum != 0 (a RHS use).
 */
VOID 
addAttribTok(type, infoP)
  Index type;
  CONST VOIDP infoP;
{
  if (!G.hasAttribs) doFirstAttrib(); 
  if (G.lastActN != NIL) internalActAttribs();
  if (type == IN_ATOK) { /* No store --- just set flag. */
    G.isOut= FALSE;
  }
  else if (type == OUT_ATOK) { /* No store --- just set flag. */
    G.isOut= TRUE;
  }
  else { /* We have a token which needs to be stored in ATTRIB_TOKS[]. */
    CONST Index i= A_NEXT(attribToks);
    AttribTok *CONST tP= &ATTRIB_TOKS(i); /* Where we are storing. */
    tP->type= type;	/* Will be changed later if type == ID_VAR_ATOK. */
    switch (type) {
      case TEXT_ATOK:
        tP->u.text= (ConstString)infoP;
	break;
      case SHORT_TEXT_ATOK: { /* Copy short text string. */
	Index i;
	CONST char *CONST srcP= (CONST char *)infoP;
	char *CONST destP= tP->u.shortText;
	for (i= 0; i < sizeof(ConstString); i++) destP[i]= srcP[i];
	break;	
      }
      case NUM_VAR_ATOK:
        tP->u.numVar= *(NumVar *)infoP;
        break;
      case ID_VAR_ATOK: {
	Index id= *(Index *)infoP;
	if (G.symNum == 0) { /* Declaration of an attribute. */
	  tP->type= ID_VAR_DEC_ATOK;
	  tP->u.idDec.id= id; tP->u.idDec.isOut= G.isOut;
	}
	else { /* Use of an attribute on the RHS. */
	  tP->type= ID_VAR_XUSE_ATOK; tP->u.idXUse.id= id;
	}
	break;
      }
      default:
        INTERNAL_ERROR();
        break;
    } /* switch (type) */
    G.tokNum++;
  } /* else we have a token which needs to be stored in ATTRIB_TOKS[]. */
  VOID_RET();
}

/* Output the attribute token in ATTRIB_TOKS(tokIndex) to outFile and
 * return the # of newlines output.  It must be a processed token (no
 * ID_VAR_XUSE_ATOKs).  If it is a NESTED_EXP_ATOK, then it is output
 * surrounded by parentheses.
 */
static Count 
outAttribTok(outFile, tokIndex)
  FILE *outFile;
  Index tokIndex;
{
  Count nLines= 0;
  CONST AttribTok *CONST tP= &ATTRIB_TOKS(tokIndex);
  assert(tokIndex < A_NENTRIES(attribToks));
  switch (tP->type) {
    case TEXT_ATOK:
      fputs(tP->u.text, outFile);
      break;
    case SHORT_TEXT_ATOK:
      fputs(tP->u.shortText, outFile); 
      nLines+= (tP->u.shortText[0] == '\n'); /* NL stored this way. */
      break;
    case ID_VAR_USE_ATOK:
      outIDVar(outFile, tP->u.idUse.idVar, tP->u.idUse.offset);
      break;
    case NESTED_EXP_ATOK:
      fputs("(\n", outFile); nLines++;
      nLines+= outExp(outFile, tP->u.exp);
      fputs(")", outFile);
      break;
    default:
      INTERNAL_ERROR();
      break;
  } /* switch (tP->type) */
  return nLines;
}


/*			    SYMBOL TABLE.			       	*/

/* 

Linear symbol-table.  First globs.lhsArity entries correspond to the 
formal attributes of the LHS symbol.  New entries are added at the
end.

*/

typedef struct {	/* Struct for each symbol-table entry. */
  Index2 id;		/* The name of the attribute variable. */
  Index2 symNum;	/* The defining symbol position.
			 * (0-> LHS; NIL-> undefined LHS %out attrib). */
  Index2 idVar;		/* The declaration of the corr. formal attrib. */
  Boolean isUntyped;	/* TRUE if defined in internal act with no type yet. */
} IDSym;

static AREA (idSyms, IDSym, 5, UINT2_MAX);
#define ID_SYMS(i) A_ACCESS(idSyms, IDSym, i)


/* Return index of symbol-table entry for identifier id.  If there is
 * no entry, then create one with the current symbol number and a NIL
 * pointer for its formal IDVar parameter.  If an entry is found and
 * its defining symNum is NIL (indicating a %out LHS declaration),
 * then change its defining occurrence to the current symbol number.
 */
static Index 
getAttribSym(id)
  Index id;
{
  Index i;
  CONST Count nSyms= A_NENTRIES(idSyms);
  for (i= 0; i < nSyms; i++) {
    if (ID_SYMS(i).id == id) {
      if (G.symNum == 0) { /* We are declaring LHS attributes. */
	error("duplicate declaration of attribute %s", getIDString(id));
      }
      else if (ID_SYMS(i).symNum == NIL) { /* Was declared %out on LHS. */
	assert(i < G.lhsArity);
	ID_SYMS(i).symNum= G.symNum;	/* This is defining occurrence. */
      }
      return i;	/* Index of previously entered symbol. */
    }
  }
  i= A_NEXT(idSyms);	/* Create a new entry. */
  assert(i == nSyms);
  ID_SYMS(i).id= id; ID_SYMS(i).symNum= G.symNum; 
  ID_SYMS(i).idVar= NIL;
  ID_SYMS(i).isUntyped= FALSE;
  return i;
}

static Index 
getSymID(symIndex)
  Index symIndex;
{
  assert(symIndex < A_NENTRIES(idSyms));
  return ID_SYMS(symIndex).id;
}

static Index 
getSymNum(symIndex)
  Index symIndex;
{
  assert(symIndex < A_NENTRIES(idSyms));
  return ID_SYMS(symIndex).symNum;
}

static VOID 
setSymNum(symIndex, symNum)
  Index symIndex;
  Index symNum;
{
  assert(symIndex < A_NENTRIES(idSyms));
  ID_SYMS(symIndex).symNum= symNum;
  VOID_RET();
}

static Index 
getSymIDVar(symIndex)
  Index symIndex;
{
  assert(symIndex < A_NENTRIES(idSyms));
  return ID_SYMS(symIndex).idVar;
}

static VOID 
setSymIDVar(symIndex, idVar)
  Index symIndex;
  Index idVar;
{
  assert(symIndex < A_NENTRIES(idSyms));
  ID_SYMS(symIndex).idVar= idVar;
  VOID_RET();
}

static Boolean 
getSymIsUntyped(symIndex)
  Index symIndex;
{
  assert(symIndex < A_NENTRIES(idSyms));
  return ID_SYMS(symIndex).isUntyped;
}

static VOID 
setSymIsUntyped(symIndex, isUntyped)
  Index symIndex; 
  BooleanX isUntyped;
{
  assert(symIndex < A_NENTRIES(idSyms));
  ID_SYMS(symIndex).isUntyped= isUntyped;
  VOID_RET();
}

/* Reset symbol-table to what it was at the neck of the rule (just
 * after the LHS).
 */
static VOID 
resetSymsToLHS()
{
  Index i;
  for (i= 0; i < G.lhsArity; i++) {
    if (ID_SYMS(i).symNum > 0) { /* Must have been a LHS %out variable. */
      ID_SYMS(i).symNum= ID_SYMS(i).idVar= NIL;
    }
  }
  A_CUT(idSyms, i); /* Forget all symbols with only RHS occurrences. */
  VOID_RET();
}

/* Check that all %out LHS symbols had defining occurrences on the
 *  RHS. 
 */
static VOID 
checkLHSOutSyms() 
{
  Index i;
  for (i= 0; i < G.lhsArity; i++) {
    if (ID_SYMS(i).symNum == NIL) {
      warn("%%out attribute $%s on LHS not defined on RHS.", 
	   getIDString(ID_SYMS(i).id));
    }
  }
  VOID_RET();
}

/* Ensure that all %out attributes of internal actions had types which
 * could be inferred by subsequent COPY occurrences in a RHS symbol.
 */
static VOID 
checkInternalActAttribs() 
{
  Index i;
  CONST Count n= A_NENTRIES(idSyms);
  for (i= 0; i < n; i++) {
    if (ID_SYMS(i).isUntyped) {
      error("could not infer type for attribute $%s of internal action",
	    getIDString(ID_SYMS(i).id));
    }
  }
  VOID_RET();
}

/* Forget all symbols in the symbol-table. */
static VOID 
clearSyms()
{
  CONST Index i= 0;
  A_CUT(idSyms, i);
  VOID_RET();
}


/*			ATTRIBUTE TOKEN SEQUENCES.			*/

typedef struct {	/* Sequence of attribute tokens. */
  Index2 lo;		/* Index of sequence start in ATTRIB_TOKS[]. */
  Index2 hi;		/* Index one beyond end of sequence in ATTRIB_TOKS[]. */
  Index2 lineN;		/* The approx. line # at which the sequence starts. */
} AttribSeq;

static AREA (attribSeqs, AttribSeq, 7, UINT2_MAX);
#define ATTRIB_SEQS(i) A_ACCESS(attribSeqs, AttribSeq, i)

/* Remember that a parse error has occurred, so that subsequent addAttribSeq() 
 * becomes a NOP.
 */
VOID 
setAttribParseError()
{
  G.hasParseError= TRUE;
}

/* Create a new ATTRIB_SEQS[] entry remembering the last G.tokNum
 * tokens. 
 */
VOID 
addAttribSeq(lineN)
  Index lineN;
{
  if (G.hasParseError) {
    G.hasParseError= FALSE;
  }
  else {
    CONST Index i= A_NEXT(attribSeqs);
    AttribSeq *CONST sP= &ATTRIB_SEQS(i);
    CONST Index hi= A_NENTRIES(attribToks);
    CONST Index lo= hi - G.tokNum;
    sP->lo= lo; sP->hi= hi; sP->lineN= lineN;
    G.argNum++;
  }
  G.tokNum= 0; 
  VOID_RET();
}

/* Output the expression given by ATTRIB_SEQS(exp).  Indirect
 * expressions are output surrounded by extra parentheses.
 */
static Count 
outExp(outFile, exp)
  FILE *outFile; 
  Index exp;
{
  Count nLines= 0;
  CONST Index hi= ATTRIB_SEQS(exp).hi;
  Index t;
  assert(exp < A_NENTRIES(attribSeqs));
  nLines+= lineDir(outFile, ATTRIB_SEQS(exp).lineN);
  for (t= ATTRIB_SEQS(exp).lo; t < hi; t++) {
    nLines+= outAttribTok(outFile, t);
  }
  return nLines;
}


/*			TYPE ATTRIBUTE TOKEN SEQUENCES.			*/

/* Output type declaration given by typeExp.  Note that the id output
 * is that given by id and not the id in typeExp, since some types are
 * equivalenced, even tho' the formal names of the attribute variables
 * need not be the same.  Assumes that the type declaration is ok. 
 */
static Count 
outType(outFile, typeExp, idString)
  FILE *outFile; 
  Index typeExp;
  ConstString idString;
{
  Count nLines= 0;
  CONST Index hi= ATTRIB_SEQS(typeExp).hi;
  Index t;
  assert(typeExp < A_NENTRIES(attribSeqs));
  nLines+= lineDir(outFile, ATTRIB_SEQS(typeExp).lineN);
  for (t= ATTRIB_SEQS(typeExp).lo; t < hi; t++) {
    if (ATTRIB_TOKS(t).type == ID_VAR_DEC_ATOK)
      fputs(idString, outFile);
    else {
      nLines+= outAttribTok(outFile, t);
    }
  }
  return nLines;
}

#define IS_SPACE_TOK(i)							\
  (ATTRIB_TOKS(i).type == SHORT_TEXT_ATOK &&				\
   (ATTRIB_TOKS(i).u.shortText[0] == ' ' ||				\
    ATTRIB_TOKS(i).u.shortText[0] == '\n'))

/* Return TRUE iff the types in ATTRIB_SEQS[] specified by typeExp1
 * and typeExp2 are equal modulo spelling of white-space.  Assumes
 * that typeExp1 and typeExp2 are valid types, and whitespace occurs
 * as SHORT_TEXT tokens containing either " " or "\n".
 */
static Boolean 
equalTypes(typeExp1, typeExp2)
  Index typeExp1;
  Index typeExp2;
{
  CONST Index hi1= ATTRIB_SEQS(typeExp1).hi;
  CONST Index hi2= ATTRIB_SEQS(typeExp2).hi;
  Index i1= ATTRIB_SEQS(typeExp1).lo;
  Index i2= ATTRIB_SEQS(typeExp2).lo;
  assert(typeExp1 < A_NENTRIES(attribSeqs));
  assert(typeExp2 < A_NENTRIES(attribSeqs));
  while (i1 < hi1 && i2 < hi2) {
    if (ATTRIB_TOKS(i1).type != ATTRIB_TOKS(i2).type) return FALSE;
    switch (ATTRIB_TOKS(i1).type) {
      case TEXT_ATOK:
        if (strcmp(ATTRIB_TOKS(i1).u.text, ATTRIB_TOKS(i2).u.text) != 0) {
	  return FALSE;
	}
	i1++; i2++; 
	break;
      case SHORT_TEXT_ATOK: {
	CONST char *p1= ATTRIB_TOKS(i1).u.shortText;
	CONST char *p2= ATTRIB_TOKS(i2).u.shortText;
	if (p1[0] == ' ' || p1[0] == '\n') { /* We have whitespace. */
	  assert(p1[1] == '\0');	/* Represented by 1-char. string. */
	  if (p2[0] != ' ' && p2[0] != '\n') return FALSE;
	  assert(p2[1] == '\0');
	  /* Skip following whitespace. */
	  do { i1++; } while (i1 < hi1 && IS_SPACE_TOK(i1));
	  do { i2++; } while (i2 < hi2 && IS_SPACE_TOK(i2));
	}
	else { /* Non-whitespace short-text token. */
	  if (strcmp(p1, p2) != 0) return FALSE;
	  i1++; i2++;
	}
	break;
      } /* case SHORT_TEXT_ATOK */
      case ID_VAR_DEC_ATOK:
	i1++; i2++;	/* Ignore spelling of formal identifier. */
	break;
      default:
	INTERNAL_ERROR();
	break;
    } /* switch (ATTRIB_TOKS(i1).type) */
  } /* while (i1 < hi1 && i2 < hi2) */
  return i1 == hi1 && i2 == hi2;
}



/*			    ATTRIBUTE TUPLES.				*/

typedef struct AttribTuple {	/* Limits in ATTRIB_SEQS[] or ID_VARS[]. */
  Index2 lo;			/* Index first AttribSeq or IDVar. */
  Index2 hi;			/* 1 beyond last AttribSeq or IDVar. */
} AttribTuple;


/*			INSTALL NEW DECLARATION.			*/

/* Install the new declaration given by the last G.argNum entries in
 * ATTRIB_SEQS[] into ID_VARS[], returning the bounds of the installed
 * entries in *tupleP.
 */
static VOID 
installNewDec(sym, tupleP)
  Index sym;
  AttribTuple *CONST tupleP;
{
  CONST Index seqsHi= A_NENTRIES(attribSeqs);
  CONST Index seqsLo= seqsHi - G.argNum;
  Index i;
  assert(G.symNum == 0); /* Must be rule LHS or terminal declaration. */
  tupleP->lo= A_NENTRIES(idVars);
  for (i= seqsLo; i < seqsHi; i++) {
    CONST Index hi= ATTRIB_SEQS(i).hi;
    Index j;
    for (j= ATTRIB_SEQS(i).lo; j < hi; j++) { /* Search for IDVar token. */
      if (ATTRIB_TOKS(j).type == ID_VAR_DEC_ATOK) break;
    }
    assert(j < hi); /* Must have had a IDVar declaration. */
    { CONST IDDec *CONST dP= &ATTRIB_TOKS(j).u.idDec;
      CONST Index symIndex= getAttribSym(dP->id);
      CONST Boolean isOut= (SYM_TYPE(sym) == TERM_SYM) || dP->isOut;
      Index k= newIDVar(dP->id, sym, seqsLo - i, i, isOut);
      assert(getSymNum(symIndex) == 0);
      if (isOut) { /* Reset symbol symNum and idVar to NIL. */
	setSymNum(symIndex, NIL); setSymIDVar(symIndex, NIL); 
      }
      else {
	setSymIDVar(symIndex, k);
      }
      if (SYM_TYPE(sym) == TERM_SYM && !dP->isOut) {
	error("attribute $%s of terminal %s cannot be declared %%in", 
	      getIDString(dP->id), getTermString(SYM_NUM(sym)));
      }
    }
  }
  tupleP->hi= A_NENTRIES(idVars);
  VOID_RET();
}


/*			INSTALL INFERRED DECLARATION.			*/

/* Check that the declaration given by the last G.argNum entries in 
 * ATTRIB_SEQS[] is consistent with that inferred in the ID_VARS[] 
 * pointed to by tupleP, updating the id and type components of the
 * idVars.
 */
static VOID 
installInferredDec(sym, tupleP)
  Index sym;
  AttribTuple *CONST tupleP;
{
  assert(SYM_TYPE(sym) == NON_TERM_SYM);
  if ((tupleP->hi - tupleP->lo) != G.argNum) {
    error("# of attributes for %s does not agree with previous use",
	  getNonTermString(SYM_NUM(sym)));
    installNewDec(sym, tupleP); /* Believe declaration is correct! */
  }
  else { /* Inferred arity agrees with declaration. */
    CONST Index seqsHi= A_NENTRIES(attribSeqs);
    Index i, j;
    for (i= seqsHi - G.argNum, j= tupleP->lo; i < seqsHi; i++, j++) {
      Index k;
      CONST Index hi= ATTRIB_SEQS(i).hi;
      for (k= ATTRIB_SEQS(i).lo; k < hi; k++) { /* Search for a idVar. */
	if (ATTRIB_TOKS(k).type == ID_VAR_DEC_ATOK) break;
      }
      assert(k < hi); /* Declaration must contain a idVar. */
      { CONST IDDec *CONST dP= &ATTRIB_TOKS(k).u.idDec;
	CONST Index symIndex= getAttribSym(dP->id);
	assert(getSymNum(symIndex) == 0);
	setSymIDVar(symIndex, j);
	if ((ID_VARS(j).inVar == NIL) != dP->isOut) {
	  CONST char *inOutNames[2];
	  inOutNames[0]= "%in"; inOutNames[1]= "%out";
	  error("\
attribute $%s of non-terminal %s declared %s, \
but previously inferred to be %s", 
		getIDString(dP->id), getNonTermString(SYM_NUM(sym)), 
		inOutNames[dP->isOut], inOutNames[ID_VARS(j).inVar == NIL]);
	}
	else if (dP->isOut) { 
	  setSymNum(symIndex, NIL); setSymIDVar(symIndex, NIL); 
	}
	resolveForwardIDVar(j, dP->id, i);
      } /* CONST IDDec *CONST dP= */
    } /* for (i= seqsHi - G.argNum, j= tupleP->lo; i < seqsHi; i++, j++) */
  } /* else inferred arity agrees with declaration. */
  VOID_RET();
}


/*			  GET EXPRESSION TYPE.				*/

typedef enum {
  DEF_EXP,
  COPY_EXP,
  COMPOUND_EXP
} ExpType;

/* Process the copy expression defined by the variable given by
 * symIndex in the symbol table with declaration given by idVar0.
 * idVar1 is the declaration for the position of the expression.
 * isUntyped1 is TRUE if the type of idVar1 can only be determined
 * from other occurrences within the rule (for an internal act). 
 */
static VOID 
processCopyExp(symIndex, idVar0, idVar1, isUntyped1)
  Index symIndex; 
  Index idVar0;
  Index idVar1;
  BooleanX isUntyped1;
{
  CONST Boolean isUntyped0= getSymIsUntyped(symIndex);
  CONST Index code= (isUntyped0 | (isUntyped1 << 1)); /* switch combine. */
  switch (code) {
    case 0: { /* !isUntyped1 && !isUntyped0. */
      break;
    }
    case 1: { /* !isUntyped1 && isUntyped0. */
      /* We have resolved the type of an internal act %out variable 0. */
      setSymIsUntyped(symIndex, FALSE); /* Record type resolved. */
      if (ID_VARS(idVar1).id == NIL) { 
	/* Forward-ref. to undefined non-terminal.  Append type-chains. */
	Index i;
	for (i= idVar0; ID_VARS(i).typeExp != NIL; i= ID_VARS(i).typeExp) {
	  /* Find end of type-exp. chain for variable 0. */
	}
	ID_VARS(i).typeExp= ID_VARS(idVar1).typeExp; /* append. */
	ID_VARS(idVar1).typeExp= idVar0;
      }
      else { /* Fill in the type for internal action variables. */
	Index i, succ;
	CONST Index typeExp1= ID_VARS(idVar1).typeExp;
	for (i= idVar0; i != NIL; i= succ) {
	  succ= ID_VARS(i).typeExp; ID_VARS(i).typeExp= typeExp1;
	}
      }
      break;
    }
    case 2: { /* isUntyped1 && !isUntyped0. */
      if (ID_VARS(idVar0).id == NIL) { /* Forward-ref. undef. non-term. */
	ID_VARS(idVar1).typeExp= ID_VARS(idVar0).typeExp; 
	ID_VARS(idVar0).typeExp= idVar1;
      }
      else {
	ID_VARS(idVar1).typeExp= ID_VARS(idVar0).typeExp;
      }
      break;
    }
    case 3: { /* isUntyped0 && isUntyped1. */
    /* Var1 must be %in act-var; Var0 must be %out act-var; 
     * Chain idVar1 type onto chain for idVar0. */
      ID_VARS(idVar1).typeExp= ID_VARS(idVar0).typeExp;
      ID_VARS(idVar0).typeExp= idVar1;
      break;
    }
    default:
      INTERNAL_ERROR();
  } /* switch (code) */
  VOID_RET();
}


/* Return the expression-type for the attribute expression given by
 * seqsIndex.  Update all the idVar tokens in the expression from
 * ID_VAR_XUSE_ATOK to ID_VAR_USE_ATOK.  If the attribute expression
 * consists of a single token which is a attribute variable which has
 * never occurred earlier in the rule, then it is a DEF_EXP; in that
 * case idVar is entered into its symbol-table entry.  If the
 * attribute expression consists of a single token which is a
 * attribute variable which has occurred earlier in the rule, then it
 * is a COPY_EXP.  Otherwise the attribute expression is a
 * COMPOUND_EXP (and for consistent attribution all its variables
 * should have occurred earlier in the rule). 
 */
static Index 
getExpType(seqsIndex, idVar, isActExp)
  Index seqsIndex;
  Index idVar;
  BooleanX isActExp;
{
  assert(seqsIndex < A_NENTRIES(attribSeqs));
  if ((ATTRIB_SEQS(seqsIndex).hi - ATTRIB_SEQS(seqsIndex).lo) == 1 &&
      ATTRIB_TOKS(ATTRIB_SEQS(seqsIndex).lo).type == ID_VAR_XUSE_ATOK) {
    /* Expression consists of a single attribute variable. */
    CONST Index id= ATTRIB_TOKS(ATTRIB_SEQS(seqsIndex).lo).u.idXUse.id;
    CONST Index idSym= getAttribSym(id);
    IDUse *CONST uP= &ATTRIB_TOKS(ATTRIB_SEQS(seqsIndex).lo).u.idUse;
    ATTRIB_TOKS(ATTRIB_SEQS(seqsIndex).lo).type= ID_VAR_USE_ATOK;
    uP->id= id;
    if (getSymNum(idSym) == G.symNum) { 
      /* Attribute variable defined within current sym. */
      if (getSymIDVar(idSym) != NIL) {
	error("\
attribute variable $%s defined and used within the same grammar symbol", 
	      getIDString(id));
      }
      setSymIDVar(idSym, idVar);  setSymIsUntyped(idSym, isActExp);
      uP->offset= 0; uP->idVar= idVar;
      return DEF_EXP;
    }
    else { /* Single attrib. var. with a previous occurrence. */
      assert(getSymIDVar(idSym) != NIL);
      uP->offset= getSymNum(idSym) - G.symNum;
      uP->idVar= getSymIDVar(idSym);
      processCopyExp(idSym, uP->idVar, idVar, isActExp);
      return COPY_EXP;
    }
  }
  else { /* We have a compound expression. */
    CONST Index hi= ATTRIB_SEQS(seqsIndex).hi;
    Index i;
    for (i= ATTRIB_SEQS(seqsIndex).lo; i < hi; i++) {
      assert(i < A_NENTRIES(attribToks));
      if (ATTRIB_TOKS(i).type == ID_VAR_XUSE_ATOK) {
	CONST IDXUse *CONST xP= &ATTRIB_TOKS(i).u.idXUse;
	IDUse *CONST uP= &ATTRIB_TOKS(i).u.idUse;
	CONST Index idSym= getAttribSym(xP->id);
	CONST Index symNum= getSymNum(idSym);
	CONST Index symIDVar= getSymIDVar(idSym);
	ATTRIB_TOKS(i).type= ID_VAR_USE_ATOK;
	if (symNum == G.symNum) {
	  if (symIDVar == NIL) {
	    error("attribute $%s used without being defined", 
		  getIDString(xP->id));
	    setSymIDVar(idSym, idVar);
	  }
	  else {
	    error("\
attribute variable $%s defined and used within the same grammar symbol", 
		  getIDString(xP->id));
	  }
	} /* if (symNum == G.symNum) */
	uP->offset= symNum - G.symNum; uP->idVar= symIDVar;
      } /* if (ATTRIB_SEQS(i).type == ID_VAR_XUSE_ATOK) */
    } /* for (i= ATTRIB_SEQS(seqsIndex).lo; i < hi; i++) */
    return COMPOUND_EXP;
  } /* else we have a compound expression. */
}


/*			CHECK NEW USE.					*/

/* This routine is called for a forward reference to a sym on the RHS,
 * when that sym has never been seen before.  It infers the %in/%out
 * pattern of its attributes, creates appropriate new entries in 
 * ID_VARS[] returned via tupleP. 
 */
static VOID 
checkNewUse(sym, tupleP)
  Index sym;
  AttribTuple *CONST tupleP;
{
  CONST Index hi= A_NENTRIES(attribSeqs);
  CONST Index lo= hi - G.argNum;
  Index i;
  tupleP->lo= A_NENTRIES(idVars);
  for (i= lo; i < hi; i++) {
    CONST Index j= newIDVar(NIL, sym, i - lo, NIL, TRUE);
    CONST ExpType expType= getExpType(i, j, FALSE);
    if (expType != DEF_EXP) makeIDVarIn(j);
  }
  tupleP->hi= A_NENTRIES(idVars);
  VOID_RET();
}


/*			CHECK DECLARED USE.				*/

/* Check that the last G.argNum expressions in ATTRIB_SEQS[] are consistent
 * with the declarations in ID_VARS[] pointed to by tupleP.  Convert all
 * IDVars in the expressions from ID_VAR_XUSE_ATOK to ID_VAR_USE_ATOK.
 */
static VOID 
checkDeclaredUse(sym, tupleP)
  Index sym;
  CONST AttribTuple *CONST tupleP;
{
  CONST Index hi= A_NENTRIES(attribSeqs);
  Index i, j;
  if (G.argNum != tupleP->hi - tupleP->lo) {
    CONST Index symNum= SYM_NUM(sym);
    ConstString symName= (SYM_TYPE(sym) == TERM_SYM) 
                         ? getTermString(symNum) 
			 : getNonTermString(symNum);
    error("# of attributes for %s does not agree with previous use", 
	  symName);
  }
  for (i= hi - G.argNum, j= tupleP->lo; i < hi; i++, j++) {
    CONST ExpType expType= getExpType(i, j, FALSE);
    if ((ID_VARS(j).inVar == NIL) != (expType == DEF_EXP)) {
      CONST Index symNum= SYM_NUM(sym);
      ConstString symName= (SYM_TYPE(sym) == TERM_SYM) 
	? getTermString(symNum) 
	: getNonTermString(symNum);
      error("\
attribute %d for %s is not consistent with previous %%in/%%out use", 
	    j - tupleP->lo, symName);
    }
  }
  VOID_RET();
}


/*			    TERMINAL ATTRIBUTES.			*/

static AREA (termAttribs, AttribTuple, 6, UINT2_MAX);
#define TERM_ATTRIBS(i) A_ACCESS(termAttribs, AttribTuple, i)

/* Record the last nDec declarations in ATTRIB_SEQS[] as the
 * attributes for terminal termID, after passing the rest of the 
 * information onto the grammar module to declare the terminal.
 */
Index 
dclTermAttribs(termID, litID, litVal, val, assocPrec, typeID, nDec, nowarn)
  Index termID;
  Index litID;
  Index litVal;
  Index val;
  Index assocPrec;
  Index typeID;
  Count nDec;
  BooleanX nowarn;
{
  CONST Index term0= 
    dclTerm(termID, litID, litVal, val, assocPrec, typeID, nowarn);
  /* term0 could be NIL for a multiple declaration. */
  assert(G.symNum == 0);
  if (G.hasAttribs) {
    CONST Index i= A_NENTRIES(termAttribs);
    clearSyms();
    if (i == term0) { /* New terminal, not a multiple declaration. */
      Index term= A_NEXT(termAttribs);
      assert(term == term0);
      G.argNum= nDec;
      installNewDec(MAKE_SYM(TERM_SYM, term), &TERM_ATTRIBS(term));
    }
    else {
      /* We don't recover storage as it could be required by another
       * terminal in the same declaration.
       */
    }
  }
  G.argNum= 0;
  return term0;
}


/*			    NON-TERMINAL ATTRIBUTES.			*/

static AREA (nonTermAttribs, AttribTuple, 6, UINT2_MAX);
#define NON_TERM_ATTRIBS(i) A_ACCESS(nonTermAttribs, AttribTuple, i)

/*			CHECK PREVIOUS DECLARATION.			*/

/* Check whether the current non-terminal declaration is identical
 * (modulo spelling of whitespace) with an earlier non-terminal
 * declaration.  This is used to allow the same non-terminal to occur
 * on the LHS of multiple rules.
 */
static VOID
checkOldDeclaration(nonTerm)
  Index nonTerm;
{
  CONST AttribTuple *CONST tupleP= &NON_TERM_ATTRIBS(nonTerm);
  CONST Index seqsHi= A_NENTRIES(attribSeqs);
  CONST Index seqsLo= seqsHi - G.argNum;
  Index i, j;
  if (G.argNum != tupleP->hi - tupleP->lo) {
    error("\
# of arguments for LHS non-terminal `%s' does not agree with \
previous declaration", 
	  getNonTermString(nonTerm));
  }
  else { /* Equal arity. */
    for (i= seqsLo, j= tupleP->lo; i < seqsHi; i++, j++) {
      Index k;
      CONST Index hi= ATTRIB_SEQS(i).hi;
      CONST Boolean jIsOut= ID_VARS(j).inVar == NIL;
      for (k= ATTRIB_SEQS(i).lo; k < hi; k++) { /* Search for a idVar. */
	if (ATTRIB_TOKS(k).type == ID_VAR_DEC_ATOK) break;
      }
      assert(k < hi);	/* Declaration must contain a idVar. */
      if (ATTRIB_TOKS(k).u.idDec.isOut != jIsOut 
	  || !equalTypes(i, ID_VARS(j).typeExp)) {
	error("\
declared type for `%s' does not agree with previous \
declaration", 
	      getIDString(ATTRIB_TOKS(k).u.idDec.id));
      }
      { CONST IDDec *CONST dP= &ATTRIB_TOKS(k).u.idDec;
      CONST Index symIndex= getAttribSym(dP->id);
      assert(getSymNum(symIndex) == 0);
      if (dP->isOut) { /* Reset symbol symNum and idVar to NIL. */
	setSymNum(symIndex, NIL); setSymIDVar(symIndex, NIL); 
      }
      else {
	setSymIDVar(symIndex, j);
      }
      } /* CONST IDDec *CONST dP= ... */
    } /* for (i= seqsHi - G.argNum, ... */
  } /* else equal arity. */
  A_CUT(attribToks, ATTRIB_SEQS(seqsLo).lo);
  A_CUT(attribSeqs, seqsLo); /* Forget declaration. */  
  VOID_RET();
}

/* Record the last nDec declarations in ATTRIB_SEQS[] as the
 * attributes for non-terminal nonTermID, after declaring the
 * non-terminal in the grammar module.  Note that if the non-term has
 * attributes, then <type> must be NIL.
 */
Index 
dclNonTermAttribs(nonTermID, type)
  Index nonTermID;
  Index type;
{
  CONST Index nonTerm0= (type == NIL) ? ruleLHS(nonTermID) 
                                      : dclNonTerm(nonTermID, type);
  assert(G.symNum == 0);
  if (G.hasAttribs) {
    clearSyms();
    if (type != NIL) {		/* We have a %type declaration. */
      assert(G.argNum == 0);
      if (nonTerm0 == A_NENTRIES(nonTermAttribs)) { /* No error. */
	CONST Index nonTerm= A_NEXT(nonTermAttribs); 
	assert(nonTerm == nonTerm0);
	NON_TERM_ATTRIBS(nonTerm).lo= NON_TERM_ATTRIBS(nonTerm).hi= 
	  A_NENTRIES(idVars);
      }
    }
    else {			/* We have a rule LHS. */
      if (nonTerm0 < A_NENTRIES(nonTermAttribs)) { /* Previously declared. */
	if (G.argNum > 0 && ID_VARS(NON_TERM_ATTRIBS(nonTerm0).lo).id != NIL) {
	  checkOldDeclaration(nonTerm0);
	}
	else {
	  installInferredDec(MAKE_SYM(NON_TERM_SYM, nonTerm0),
			     &NON_TERM_ATTRIBS(nonTerm0));
	}
      }
      else {
	CONST Index nonTerm= A_NEXT(nonTermAttribs);
	assert(nonTerm0 == nonTerm);
	installNewDec(MAKE_SYM(NON_TERM_SYM, nonTerm),
		      &NON_TERM_ATTRIBS(nonTerm));
      }
    } /* else we have a rule LHS. */
  }
  G.lhsArity= G.argNum;   G.lhsNonTerm= nonTerm0;
  G.argNum= 0;
  return nonTerm0;
}

/* Returns TRUE if nonTerm has %in attributes. */
static Boolean 
hasInAttribs(nonTerm)
  Index nonTerm;
{
  Index i;
  CONST Index hi= NON_TERM_ATTRIBS(nonTerm).hi;
  assert(nonTerm < A_NENTRIES(nonTermAttribs));
  for (i= NON_TERM_ATTRIBS(nonTerm).lo; i < hi; i++) {
    if (ID_VARS(i).inVar != NIL) return TRUE;
  }
  return FALSE;
}

/*			RHS ATTRIBUTE EXPRESSIONS.			*/

static AREA (rhsAttribs, AttribTuple, 8, UINT2_MAX);
#define RHS_ATTRIBS(i) A_ACCESS(rhsAttribs, AttribTuple, i)

Index 
addRHSAttribs(id, val)
  Index id;
  Index val;
{
  if (G.lastActN != NIL) internalActAttribs();
  { Index rhsIndex0= addBodySym(id, val);
    if (G.hasAttribs) {
      CONST Sym sym= getRHSSym(rhsIndex0);
      CONST SymType symType= SYM_TYPE(sym);
      CONST Index symNum= SYM_NUM(sym);
      CONST Boolean isNew= 
	(symType == TERM_SYM) ? A_NENTRIES(termAttribs) <= symNum
	                      : A_NENTRIES(nonTermAttribs) <= symNum;
      CONST Index rhsIndex= A_NEXT(rhsAttribs);
      assert(G.symNum > 0);
      assert(rhsIndex0 == rhsIndex);
      RHS_ATTRIBS(rhsIndex).lo= A_NENTRIES(attribSeqs) - G.argNum;
      RHS_ATTRIBS(rhsIndex).hi= A_NENTRIES(attribSeqs);
      if (isNew) {
	CONST Index j= (symType == TERM_SYM) ? A_NEXT(termAttribs) 
	                                     : A_NEXT(nonTermAttribs);
	AttribTuple *CONST tupleP= 
	  (symType == TERM_SYM) ? &TERM_ATTRIBS(j) : &NON_TERM_ATTRIBS(j);
	assert(j == symNum);
	if (symType == TERM_SYM && G.argNum != 0) { /* New terminal. */
	  error("attributes of terminal %s must be declared in section 1", 
		getIDString(id));
	}
	checkNewUse(sym, tupleP);
      }
      else { /* !isNew. */
	AttribTuple *CONST tupleP= 
	  (symType == TERM_SYM) ? &TERM_ATTRIBS(symNum) 
	                        : &NON_TERM_ATTRIBS(symNum);
	checkDeclaredUse(sym, tupleP);
      }
    } /* if (G.hasAttribs) */
    G.symNum++; G.argNum= 0;
    return rhsIndex0;
  }
}


/*			    RULE ATTRIBUTES.				*/

VOID 
beginRuleAttribs() 
{
  beginRule();
  if (G.hasAttribs) {
    resetSymsToLHS(); 
  }
  G.symNum= 1;
  VOID_RET();
}

/* Assumes that the last entry in RHS_ATTRIBS[] is a action.  It
 * checks to see if id is among the attribute variables there, if not
 * it adds it.
 */
static VOID 
ensureLastActHasAttrib(id)
  Index id;
{
  CONST Index actIndex= A_NENTRIES(rhsAttribs) - 1;
  CONST Index hi= RHS_ATTRIBS(actIndex).hi;
  Index i;
  for (i= RHS_ATTRIBS(actIndex).lo; i < hi; i++) {
    CONST Index j= ATTRIB_SEQS(i).lo;
    CONST Index jID= ATTRIB_TOKS(j).u.idUse.id;
    assert(ATTRIB_TOKS(j).type == ID_VAR_USE_ATOK);
    assert(ATTRIB_SEQS(i).hi - ATTRIB_SEQS(i).lo == 1);
    if (jID == id) break;
  }
  if (i >= hi) { /* Not found. */
    CONST Index idSym= getAttribSym(id);
    Index s= A_NEXT(attribSeqs);
    Index t= A_NEXT(attribToks);
    IDUse *CONST uP= &ATTRIB_TOKS(t).u.idUse;
    assert(s == hi);
    ATTRIB_SEQS(s).lo= t; ATTRIB_SEQS(s).hi= t + 1;
    RHS_ATTRIBS(actIndex).hi= A_NENTRIES(attribSeqs);
    ATTRIB_TOKS(t).type= ID_VAR_USE_ATOK; 
    uP->id= id;	uP->offset= getSymNum(idSym) - G.symNum;
    uP->idVar= getSymIDVar(idSym);
  }
  VOID_RET();
}


/* Go thru last action (if any), ensuring that any attributes defined
 * in it are %out for LHSs.  Change uses from XUSE to USE.  If any LHS
 * %out attributes are defined before the last action, add copying
 * assignments to the last action. 
 */
static VOID 
fixLastAction(ruleN)
  Index ruleN;
{
  CONST Index rhsActIndex= A_NENTRIES(rhsAttribs) - (G.lastActN != NIL);
  CONST Index lhsIDVar0= NON_TERM_ATTRIBS(G.lhsNonTerm).lo;
  Index i;
  if (G.lastActN == NIL) { /* No action at the end of the rule. */
    Index i= A_NEXT(rhsAttribs);
    assert(i == rhsActIndex);
    RHS_ATTRIBS(i).lo= RHS_ATTRIBS(i).hi= A_NENTRIES(attribSeqs);
  }
  else { /* We had an action at the end of the rule. */
    Index hi= RHS_ATTRIBS(rhsActIndex).hi;
    for (i= RHS_ATTRIBS(rhsActIndex).lo; i < hi; i++) {
      CONST Index j= ATTRIB_SEQS(i).lo;
      CONST Index id= ATTRIB_TOKS(j).u.idXUse.id;
      CONST Index idSym= getAttribSym(id);
      IDUse *CONST uP= &ATTRIB_TOKS(j).u.idUse;
      assert(ATTRIB_TOKS(j).type == ID_VAR_XUSE_ATOK);
      assert(ATTRIB_SEQS(i).hi - ATTRIB_SEQS(i).lo == 1);
      ATTRIB_TOKS(j).type= ID_VAR_USE_ATOK;  uP->id= id;
      if (getSymNum(idSym) == G.symNum) { 
	/* Attribute variable defined within current sym. */
	Index idVar= NON_TERM_ATTRIBS(G.lhsNonTerm).lo + idSym;
	if (idSym >= G.lhsArity) {
	  error("Unused attribute variable $%s.", getIDString(id));
	  idVar= 0;
	}
	else {
	  assert(getSymIDVar(idSym) == NIL);
	}
	setSymIDVar(idSym, idVar);
	uP->offset= 0; uP->idVar= idVar;
      }
      else { /* Attrib. var. with a previous occurrence. */
	assert(getSymIDVar(idSym) != NIL);
	uP->offset= getSymNum(idSym) - G.symNum;
	uP->idVar= getSymIDVar(idSym);
      }
    }
  } /* else (G.lastActN != NIL) */

  for (i= 0; i < G.lhsArity; i++) {
    Index defSymNum= getSymNum(i);
    if (defSymNum > 0 && defSymNum < G.symNum) { 
      /* LHS %out attribute defined before last action. */
      Index id= getSymID(i);
      if (G.lastActN == NIL) {	/* No action at end of rule. */
	Index actN= newAct();	/* New action. */
	addBodyAct(actN, errorLineN());
	G.lastActN= actN; setActReduction(actN, ruleN);
      }
      ensureLastActHasAttrib(id);
      addActAttribAssign(ruleN, lhsIDVar0 + i, 0, id);
    }
  } /* for (i= 0; i < G.lhsArity; i++) */
  VOID_RET();
}

VOID 
endRuleAttribs(nRHS)
  Count nRHS;
{
  CONST Index ruleN= endRule(nRHS);
  if (G.hasAttribs) {
    Index i;
    CONST Count nDummyRHS= G.nExtraRules;
    fixLastAction(ruleN);
    for (i= 0; i < nDummyRHS; i++) { /* Make dummy RHS entries for rules. */
      CONST Index dummyRuleN= ruleN + i + 1;
      CONST Index dummyNonTerm= ruleNonTerm(dummyRuleN);
      CONST Index dummyNonTermOnRHS= nonTermRHSIndex(dummyNonTerm);
      CONST Index j= A_NEXT(rhsAttribs); 
      assert(dummyNonTermOnRHS != NIL);
      assert(dummyNonTermOnRHS < j);
      RHS_ATTRIBS(j)= RHS_ATTRIBS(dummyNonTermOnRHS);
    }
    checkLHSOutSyms(); checkInternalActAttribs();
  }
  G.symNum= G.argNum= G.tokNum= 0; 
  G.nExtraRules= 0; G.lastActN= NIL;
  VOID_RET();
}


/*			    ACTION ATTRIBUTES.				*/

/* Remember the distinct attribute variables in
 * ATTRIB_TOKS[]. Remember the limits of these distinct attribute
 * tokens in the these ATTRIB_SEQS[] entries in the next entry in
 * RHS_ATTRIBS[].
 */
VOID 
addActAttribs(actN, lineNum)
  Index actN;
  Index lineNum;
{ 
  if (G.lastActN != NIL) internalActAttribs();
  addBodyAct(actN, lineNum);
  if (G.hasAttribs) {
    CONST Index hi= A_NENTRIES(attribToks);
    CONST Index lo= hi - G.tokNum;
    CONST Index rhsIndex= A_NEXT(rhsAttribs);
    Index dest= lo;
    Index src;
    assert(G.argNum == 0);
    RHS_ATTRIBS(rhsIndex).lo= A_NENTRIES(attribSeqs);
    for (src= lo; src < hi; src++) { /* Kill duplicate attribute vars. */
      Index i;
      assert(ATTRIB_TOKS(src).type == ID_VAR_XUSE_ATOK);
      assert(dest <= src);
      for (i= lo; i < dest; i++) {
	if (ATTRIB_TOKS(i).u.idXUse.id == ATTRIB_TOKS(src).u.idXUse.id) break;
      }
      if (i == dest) {		/* Found a new attribute variable. */
	CONST Index exp= A_NEXT(attribSeqs);
	ATTRIB_SEQS(exp).lo= dest; ATTRIB_SEQS(exp).hi= dest + 1; 
	ATTRIB_SEQS(exp).lineN= 0;
	ATTRIB_TOKS(dest)= ATTRIB_TOKS(src); dest++;
      }
    }
    A_CUT(attribToks, dest); /* Forget duplicates. */
    RHS_ATTRIBS(rhsIndex).hi= A_NENTRIES(attribSeqs);
    G.tokNum= 0; 
  }
  G.lastActN= actN;
  VOID_RET();
}

/* Create a dummy non-term for the internal action.  Fill in the types
 * of its attribute variables from previous occurrences of these
 * variables (if any), or remember that these types need to be
 * inferred from subsequent COPY occurrences.
 */
static VOID 
internalActAttribs()
{
  CONST Index rhsIndex0= internalActRule(); 
  assert(G.lastActN != NIL);
  assert(G.argNum == 0);
  if (G.hasAttribs) {
    CONST Index newNonTerm= A_NEXT(nonTermAttribs);
    CONST Sym sym= MAKE_SYM(NON_TERM_SYM, newNonTerm);
    CONST Index rhsIndex= A_NENTRIES(rhsAttribs) - 1;
    CONST Index lo= RHS_ATTRIBS(rhsIndex).lo;
    CONST Index hi= RHS_ATTRIBS(rhsIndex).hi;
    Index exp;
    assert(rhsIndex0 == rhsIndex);
    NON_TERM_ATTRIBS(newNonTerm).lo= A_NENTRIES(idVars);
    for (exp= lo; exp < hi; exp++) {
      CONST Index i= ATTRIB_SEQS(exp).lo;
      CONST Index id= ATTRIB_TOKS(i).u.idXUse.id;
      CONST Index idVar= newIDVar(id, sym, exp - lo, NIL, TRUE);
      CONST ExpType expType= getExpType(exp, idVar, TRUE);
      assert(ATTRIB_TOKS(i).type == ID_VAR_USE_ATOK);
      assert(ATTRIB_SEQS(exp).hi - ATTRIB_SEQS(exp).lo == 1);
      assert(expType == DEF_EXP || expType == COPY_EXP);
      if (expType != DEF_EXP) makeIDVarIn(idVar);
    }
    NON_TERM_ATTRIBS(newNonTerm).hi= A_NENTRIES(idVars);
  } /* if (G.hasAttribs) */
  G.tokNum= 0;  G.symNum++;
  G.nExtraRules++;
  G.lastActN= NIL; 
  VOID_RET();
}


/*			    TEST ATTRIBUTES.				*/

static VOID 
processTestExp(exp)
  Index exp;
{
  CONST Index hi= ATTRIB_SEQS(exp).hi;
  Index t;
  for (t= ATTRIB_SEQS(exp).lo; t < hi; t++) {
    if (ATTRIB_TOKS(t).type == ID_VAR_XUSE_ATOK) {
      Index id= ATTRIB_TOKS(t).u.idXUse.id;
      IDUse *CONST uP= &ATTRIB_TOKS(t).u.idUse;
      CONST Index idSym= getAttribSym(id);
      CONST Index symNum= getSymNum(idSym);
      CONST Index symIDVar= getSymIDVar(idSym);
      ATTRIB_TOKS(t).type= ID_VAR_USE_ATOK;
      if (getSymNum(idSym) == G.symNum) {
	error("attribute $%s used without being defined",
	      getIDString(id));
      }
      uP->offset= symNum - G.symNum; uP->idVar= symIDVar;
    }
  } /* for (t= ATTRIB_SEQS(exp).lo; t < hi; t++) */
  VOID_RET();
}

VOID 
addTestAttribs()
{
  assert(G.hasAttribs); /* Because of test-expression. */
  assert(G.argNum == 1);
  assert(G.tokNum == 0);
  if (G.lastActN != NIL) internalActAttribs();
  addBodyTest();
  { CONST Index newNonTerm= A_NEXT(nonTermAttribs);
    AttribTuple *CONST tupleP= &NON_TERM_ATTRIBS(newNonTerm);
    CONST Index rhsIndex= A_NEXT(rhsAttribs);
    CONST Index testExp= A_NENTRIES(attribSeqs) - G.argNum;
    processTestExp(testExp);
    RHS_ATTRIBS(rhsIndex).lo= testExp;
    RHS_ATTRIBS(rhsIndex).hi= A_NENTRIES(attribSeqs);
    tupleP->hi= tupleP->lo= 0;
    G.nExtraRules++;
  }
  G.argNum= 0;  G.symNum++;
  VOID_RET();
}

/* Output test expression testExp for nonTerm to outFile, returning # 
 * of lines output. 
 */
Count 
outTest(outFile, testExp, nonTerm)
  FILE *outFile;
  Index testExp;
  Index nonTerm;
{
  Count nLines= 0;
  CONST Index rhsRuleIndex= nonTermRHSRule(nonTerm);
  CONST Index r= SYM_NUM(RHS_SYM(rhsRuleIndex));
  assert(testExp != NIL);
  assert(RHS_SUCC(rhsRuleIndex) == NIL);
  assert(SYM_TYPE(RHS_SYM(rhsRuleIndex)) == RULE_SYM);
  assert(RULE_RHS_HI(r) - RULE_RHS_LO(r) == 0);
  assert(RHS_ATTRIBS(RULE_RHS_HI(r)).hi - RHS_ATTRIBS(RULE_RHS_HI(r)).lo == 1);
  nLines+= outExp(outFile, RHS_ATTRIBS(RULE_RHS_HI(r)).lo);
  return nLines;
}


/* Output all test expression as part of a case statement. */
Count 
outTestCases(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  if (nTests()) {
    CONST Count nNonTerms= gramNNonTerms();
    Index nonTerm;
    for (nonTerm= 0; nonTerm < nNonTerms; nonTerm++) {
      CONST Index t= nonTermTest(nonTerm);
      if (t != NIL) {
	fprintf(outFile, "\t  case %d:\n\t    yyResult= ", t); nLines++;
	nLines+= outTest(outFile, t, nonTerm);
	fputs(";\n\t  break;\n", outFile); nLines+= 2;
      }
    }
  } /* if (nTests()) */
  return nLines;
}


/*			OUT ATTRIBUTE DEF/UNDEF.			*/

static Count 
outDefUndef(outFile, reductN, isDef)
  FILE *outFile;
  Index reductN;
  BooleanX isDef;
{
  Count lineCount= 0;
  if (G.hasAttribs) {
    CONST Index ruleRHSEnd= RULE_RHS_HI(reductN);
    CONST Index hi= RHS_ATTRIBS(ruleRHSEnd).hi;
    Index i;
    for (i= RHS_ATTRIBS(ruleRHSEnd).lo; i < hi; i++) {
      CONST Index tokI= ATTRIB_SEQS(i).lo;
      ConstString idName= getIDString(ATTRIB_TOKS(tokI).u.idUse.id);
      assert(ATTRIB_SEQS(i).hi - ATTRIB_SEQS(i).lo == 1);
      assert(ATTRIB_TOKS(tokI).type == ID_VAR_USE_ATOK);
      fputs((isDef) ? "#define " : "#undef ", outFile);
      outAttribVar(outFile, idName);
      if (isDef) {
	CONST Index idVar= ATTRIB_TOKS(tokI).u.idUse.idVar;
	CONST Int offset= ATTRIB_TOKS(tokI).u.idUse.offset;
	assert(idVar < A_NENTRIES(idVars));
	fputs(" ", outFile); outIDVar(outFile, idVar, offset);
      }
      fputs("\n", outFile); lineCount++;
    }
    assert(lineCount == hi - RHS_ATTRIBS(ruleRHSEnd).lo);
  }
  return lineCount;
}

Count 
outAttribActPrefix(outFile, ruleN)
  FILE *outFile;
  Index ruleN;
{
  return outDefUndef(outFile, ruleN, TRUE);
}

Count 
outAttribActSuffix(outFile, ruleN)
  FILE *outFile;
  Index ruleN;
{
  return outDefUndef(outFile, ruleN, FALSE);
}


/*		INITIALIZE AFTER FIRST ATTRIBUTE.			*/	

static VOID 
doFirstAttrib()
{
  Index i;
  Count n;
  
  assert(G.hasAttribs == FALSE);
  n= gramNTerms();
  for (i= 0; i < n; i++) {
    Index j= A_NEXT(termAttribs);
    TERM_ATTRIBS(j).hi= TERM_ATTRIBS(j).lo= 0;
  }

  n= gramNNonTerms();
  for (i= 0; i < n; i++) {
    Index j= A_NEXT(nonTermAttribs);
    NON_TERM_ATTRIBS(j).hi= NON_TERM_ATTRIBS(j).lo= 0;
  }

  n= gramNRHSSyms();
  for (i= 0; i < n; i++) {
    Index j= A_NEXT(rhsAttribs);
    RHS_ATTRIBS(j).hi= RHS_ATTRIBS(j).lo= 0;
  }
  G.hasAttribs= TRUE;
  VOID_RET();
}


/*		    TYPE TABLE FOR INHERITED ATTRIBUTES.		*/

#define TYPES_HASH_SIZE 37
#define LG2_TYPE_AREA_INC 5

typedef struct {
  Index2 typeSeq;	/* Index of type declaration. */
  Index2 inVarsChain;	/* Chain of inVars having this type. */
} InType;

static AREA (inTypes, InType, LG2_TYPE_AREA_INC, UINT2_MAX);
#define IN_TYPES(i) A_ACCESS(inTypes, InType, i)

/* Return sum of characters of type specified by typeExp modulo
 * TYPES_HASH_SIZE, where all whitespace within is collapsed into a
 * single ' '.  Assumes that typeExp1 and typeExp2 are valid types,
 * and whitespace occurs as SHORT_TEXT tokens containing either " " or
 * "\n".
 */
static Count 
hashType(typeExp)
  Index typeExp;
{
  Count sum= 0;
  CONST Index hi= ATTRIB_SEQS(typeExp).hi;
  Index i= ATTRIB_SEQS(typeExp).lo;
  while (i < hi) {
    switch (ATTRIB_TOKS(i).type) {
      case ID_VAR_DEC_ATOK:
        i++;
        break;
      case TEXT_ATOK: {
	CONST char *p= ATTRIB_TOKS(i).u.text;
	while (*p != '\0') { sum+= *p++; }
	i++;
	break;
      }
      case SHORT_TEXT_ATOK: {
	CONST char *p= ATTRIB_TOKS(i).u.shortText;
	if (p[0] == ' ' || p[0] == '\n') {
	  assert(p[1] == '\0');
	  sum+= ' ';
	  do { i++; } while (i < hi && IS_SPACE_TOK(i));
	}
	else {
	  while (*p != '\0') { sum+= *p++; }
	  i++;
	}
	break;
      }
      default:
        INTERNAL_ERROR();
        break;
    } /* switch (ATTRIB_TOKS(i).type) */
  } /* while (i < hi) */
  return sum%TYPES_HASH_SIZE;
}

/* Fill the Area IN_TYPES[] with the type-expressions for all the unique
 * types for all attributes declared %in.  Replace the type-field in
 * the IN_VARS[] entry for all inherited attribute by the index of the
 * corresponding type in IN_TYPES[].  After this routine has been called,
 * equal types will have equal indexes in IN_TYPES[].
 */
static VOID 
buildInTypes()
{
  CONST Count nInVars= A_NENTRIES(inVars);
  if (nInVars > 0) {
    Index2 typesHashTab[TYPES_HASH_SIZE];
    Area typeSuccs; /* Parallel with IN_TYPES[]. */
#define TYPE_SUCCS(i) A_ACCESS(typeSuccs, Index2, i)
    Index i;
    INIT_AREA(typeSuccs, Index2, LG2_TYPE_AREA_INC, UINT2_MAX);
    for (i= 0; i < TYPES_HASH_SIZE; i++) typesHashTab[i]= NIL;
    for (i= 0; i < nInVars; i++) {
      CONST Index type= ID_VARS(IN_VARS(i).idVar).typeExp;
      CONST Index h= hashType(type);
      Index j;
      for (j= typesHashTab[h]; j != NIL; j= TYPE_SUCCS(j)) {
	if (equalTypes(type, IN_TYPES(j).typeSeq)) break;
      }
      if (j != NIL) { /* Found --- chain this idVar onto type-chain. */
	IN_VARS(i).u.typeChain= IN_TYPES(j).inVarsChain;
	IN_TYPES(j).inVarsChain= i;
      }
      else  { /* Not found --- create new entry. */
	CONST Index k= A_NEXT(typeSuccs);
	j= A_NEXT(inTypes);
	assert(k == j);
	IN_TYPES(j).typeSeq= type; IN_TYPES(j).inVarsChain= NIL;
	TYPE_SUCCS(k)= typesHashTab[h]; typesHashTab[h]= k;
      } /* if (j == NIL) */
      IN_VARS(i).typeNum= j;
    } /* for (i= 0; i < nInVars; i++) */
    delArea(&typeSuccs);
#undef TYPE_SUCCS
  } /* if (nInVars > 0) */
  VOID_RET();
}


/*			ATTRIBUTE COMPUTATION.				*/

typedef struct {/* Inherited attribute expressions. */	
  Index2 state;	/* The state this expression applies to. */
  Index2 idVar;	/* The index in ID_VARS[] of the inherited attribute. */
  Index2 exp;	/* Index in ATTRIB_SEQS[] of expression. */
  Index2 succ;	/* Next entry in hash-chain. */
} InExp;

static AREA (inExps, InExp, 9, UINT2_MAX);
#define IN_EXPS(i) A_ACCESS(inExps, InExp, i)

typedef struct {
  Index2 lo;		/* Low index in IN_EXPS for a state. */
  Index2 hi;		/* 1 beyond last index in IN_EXPS[] for a state. */
} StateIns;

static StateIns *stateIns;
static Index2 *inExpsHashTab;
#define IN_EXPS_HASH_SIZE 64
#define HASH_IN_EXP(state, idVar)	((state + idVar)%IN_EXPS_HASH_SIZE)

/* Search the inExps hash-table for the attribute for idVar in state. 
 * Return its index in IN_EXPS[].  If not found, create with a new
 * entry with the exp field set to NIL.  
 */
static Index 
getInExpIndex(state, idVar)
  CONST Index state;
  CONST Index idVar;
{
  CONST Index h= HASH_IN_EXP(state, idVar);
  Index i;
  assert(ID_VARS(idVar).inVar != NIL); /* !isOut. */
  for (i= inExpsHashTab[h]; i != NIL; i= IN_EXPS(i).succ) {
    if (IN_EXPS(i).state == state && IN_EXPS(i).idVar == idVar) break;
  }
  if (i == NIL) { /* Need to create a new entry. */
    i= A_NEXT(inExps);
    IN_EXPS(i).state= state; IN_EXPS(i).idVar= idVar;
    IN_EXPS(i).exp= NIL; 
    IN_EXPS(i).succ= inExpsHashTab[h]; inExpsHashTab[h]= i;
  }
  return i;  
}

/* Return TRUE iff the expression exp contains a idVar which is in the
 * current closure (identified as one with a offset == -1).
 */
static Boolean 
hasClosureIDVars(exp)
  Index exp;
{
  CONST Index hi= ATTRIB_SEQS(exp).hi;
  Index t;
  assert(exp < A_NENTRIES(attribSeqs));
  for (t= ATTRIB_SEQS(exp).lo; t < hi; t++) {
    CONST Index type= ATTRIB_TOKS(t).type;
    assert(type == TEXT_ATOK || type == SHORT_TEXT_ATOK || type == ID_VAR_USE_ATOK);
    if (type == ID_VAR_USE_ATOK && ATTRIB_TOKS(t).u.idUse.offset == -1) {
      return TRUE;
    }
  }
  return FALSE;
}

/* Return an expression which is similar to expression except that all
 * closure attributes may have been replaced by indirect pointers to
 * their expressions.  If expression consists of a single closure
 * attribute, then the expression associated currently with that closure
 * attribute is returned.  If exp contains no closure attributes, then
 * exp is returned unchanged.
 */
static Index 
evaluateExp(state, item, exp)
  CONST Index state;
  CONST Index item;
  CONST Index exp;
{
  if (!IS_CLOSURE_ITEM(item) || !hasClosureIDVars(exp)) {
    return exp;
  }
  else if (ATTRIB_SEQS(exp).hi - ATTRIB_SEQS(exp).lo == 1) {
    /* exp is a copy of a closure attribute variable. */
    CONST Index t= ATTRIB_SEQS(exp).lo;
    CONST Index tExpI= getInExpIndex(state, ATTRIB_TOKS(t).u.idUse.idVar);
    CONST Index tExp= IN_EXPS(tExpI).exp;
    assert(ATTRIB_TOKS(t).type == ID_VAR_USE_ATOK);
    assert(ATTRIB_TOKS(t).u.idUse.offset == -1);
    assert(tExp != NIL);
    return tExp;    
  }
  else { /* Compound expression which contains closure attributes. */
    CONST Index newExp= A_NEXT(attribSeqs);
    CONST Index hi= ATTRIB_SEQS(exp).hi;
    Index i;
    ATTRIB_SEQS(newExp).lo= A_NENTRIES(attribToks);
    ATTRIB_SEQS(newExp).lineN= ATTRIB_SEQS(exp).lineN;
    for (i= ATTRIB_SEQS(exp).lo; i < hi; i++) {
      CONST Index t= A_NEXT(attribToks);
      if (ATTRIB_TOKS(i).type == ID_VAR_USE_ATOK &&
	  ATTRIB_TOKS(i).u.idUse.offset == -1) { /* Closure attribute. */
	CONST Index inExpI= getInExpIndex(state, ATTRIB_TOKS(i).u.idUse.idVar);
	assert(IN_EXPS(inExpI).exp != NIL);
	ATTRIB_TOKS(t).type= NESTED_EXP_ATOK; 
	ATTRIB_TOKS(t).u.exp= IN_EXPS(inExpI).exp;
      }
      else { /* Simply make a copy. */
	ATTRIB_TOKS(t)= ATTRIB_TOKS(i);
      }
    } /* for (i= ATTRIB_SEQS(exp).lo; i < hi; i++) */
    ATTRIB_SEQS(newExp).hi= A_NENTRIES(attribToks);
    return newExp;
  }
}

/* Return TRUE iff exp1 and exp2 refer to the same expression or exp1
 * and exp2 consist of single identical attribute vars at identical
 * offsets. 
 */
static Boolean 
equalExps(exp1, exp2)
  Index exp1;
  Index exp2;
{
  CONST Index t1= ATTRIB_SEQS(exp1).lo;
  CONST Index t2= ATTRIB_SEQS(exp2).lo;
  return 
    exp1 == exp2 ||  
    ( (ATTRIB_SEQS(exp1).hi - t1 == 1) &&
      (ATTRIB_SEQS(exp2).hi - t2 == 1) &&
      (ATTRIB_TOKS(t1).type == ID_VAR_USE_ATOK) &&
      (ATTRIB_TOKS(t2).type == ID_VAR_USE_ATOK) &&
      (ATTRIB_TOKS(t1).u.idUse.idVar == ATTRIB_TOKS(t2).u.idUse.idVar) &&
      (ATTRIB_TOKS(t1).u.idUse.offset == ATTRIB_TOKS(t2).u.idUse.offset)
    );
}

/* Process all the inherited attributes in item in state.  Create
 * expressions in ID_EXPS[] for all the inherited attributes for item.
 */
static VOID 
processItemIns(state, item)
  CONST Index state;
  CONST Index item;
{
  CONST Index nonTerm= SYM_NUM(RHS_SYM(item));
  assert(IS_NON_TERM_ITEM(item));
  if (hasInAttribs(nonTerm)) {
    CONST Index lo= NON_TERM_ATTRIBS(nonTerm).lo;
    CONST Index hi= NON_TERM_ATTRIBS(nonTerm).hi;
    Index v;
    assert(RHS_ATTRIBS(item).hi - RHS_ATTRIBS(item).lo == hi - lo);
    for (v= lo; v < hi; v++) {
      if (ID_VARS(v).inVar != NIL) { /* Inherited attribute. */
	CONST Index2 rhsExp= 
	  evaluateExp(state, item, RHS_ATTRIBS(item).lo + v - lo);
	CONST Index2 inExpI= getInExpIndex(state, v);
	CONST Index2 inExp= IN_EXPS(inExpI).exp;
	assert(inExpI < A_NENTRIES(inExps));
	if (inExp == NIL) {
	  IN_EXPS(inExpI).exp= rhsExp;
	}
	else if (!equalExps(rhsExp, inExp)) {
	  message("\
%s:%d: Attribute conflict for attribute %s of non-terminal %s in state %d.\n",
		  errorSrc(), ATTRIB_SEQS(rhsExp).lineN,
	       getIDString(ID_VARS(v).id), getNonTermString(nonTerm), state);
	}
      } /* if (ID_VARS(v).inVar != NIL) */
    } /* for (v= NON_TERM_ATTRIBS(nonTerm).lo; v < hi; v++) */
  } /* if (hasInAttribs(nonTerm)) */
  VOID_RET();
}

/* Reorder the nItems in items[] so that they are in closure-order.
 * Specifically an initial segment of items[] which contain kernel
 * items is not touched.  The remaining closure items are reordered so
 * that after this routine if there is a closure item items[i] which
 * is A-> . alpha, then there is a previous item j with j < i, such
 * that items[j] is X-> beta . A gamma for some symbol strings alpha,
 * beta, gamma. 
 */
static VOID 
setClosureOrder(items, nItems)
  Index2 items[];
  CONST Count nItems;
{
  ISet processedNonTerms= newISet();
  Index i;
  for (i= 0; i < nItems && !IS_CLOSURE_ITEM(items[i]); i++) { 
    /* Kernel items at beginning initially and keep them there. */
    addISet(processedNonTerms, SYM_NUM(RHS_SYM(items[i])));
  }
  for (; i < nItems; i++) {
    assert(IS_CLOSURE_ITEM(items[i]));
    if (!inISet(ruleNonTerm(getItemRuleN(items[i])), processedNonTerms)) {
      /* Scan for an item which can be inserted at i. */
      Index j;
      Index tempItem;
      for (j= i + 1; j < nItems; j++) {
	if (inISet(ruleNonTerm(getItemRuleN(items[j])), processedNonTerms)) 
	  break;
      }
      assert(j < nItems); /* Else it wouldn't be in the closure. */
      tempItem= items[j]; items[j]= items[i]; items[i]= tempItem;
    }
    addISet(processedNonTerms, SYM_NUM(RHS_SYM(items[i])));
  }
  delISet(processedNonTerms);
  VOID_RET();
}

static VOID 
computeStateIns()
{
  CONST Count nStates= lalrNStates();
  CONST Count maxNI= maxNonTermItems();
  Index2 *itemsCache= (Index2 *)MALLOC(maxNI * sizeof(Index2));
  Index s, i;
  inExpsHashTab= (Index2 *)MALLOC(IN_EXPS_HASH_SIZE * sizeof(Index2));
  stateIns= (StateIns *)MALLOC(nStates * sizeof(StateIns));
  for (i= 0; i < IN_EXPS_HASH_SIZE; i++) inExpsHashTab[i]= NIL;
  for (s= 0; s < nStates; s++) {
    Count nItems= 0;
    Index cursor= NIL;
    Index item;
    stateIns[s].lo= A_NENTRIES(inExps);
    for (item= nextNonTermItem(s, &cursor); item != NIL;
	 item= nextNonTermItem(s, &cursor)) {
      /* Don't try to filter non-terms with no %ins here, as it can
       * screw up closure-order computation.
       */
      assert(IS_NON_TERM_ITEM(item));
      itemsCache[nItems++]= item;
    }
    if (nItems > 0) {
      Index i;
      setClosureOrder(itemsCache, nItems);
      for (i= 0; i < nItems; i++) processItemIns(s, itemsCache[i]);
    } /* if (nItems > 0) */
    stateIns[s].hi= A_NENTRIES(inExps);
  } /* for (s= 0; s < nStates; s++) */
  FREE(itemsCache); FREE(inExpsHashTab);
  VOID_RET();
}

VOID 
printInAttribExps(outFile, s)
  FILE *outFile;
  Index s;
{
  if (stateIns && stateIns[s].hi > stateIns[s].lo) {
    CONST Index hi= stateIns[s].hi;
    Index i;
    fprintf(outFile, "Inherited attribute assignments:\n");
    for (i= stateIns[s].lo; i < hi; i++) {
      if (IN_EXPS(i).idVar != NIL) { /* Not redundant. */
        assert(SYM_TYPE(ID_VARS(IN_EXPS(i).idVar).sym) == NON_TERM_SYM);
	fprintf(outFile, "%s.%s= ", 
		getNonTermString(SYM_NUM(ID_VARS(IN_EXPS(i).idVar).sym)),
		getIDString(ID_VARS(IN_EXPS(i).idVar).id));
	outExp(outFile, IN_EXPS(i).exp);
	fputs("\n", outFile);
      }
    }
  }
  VOID_RET();
}

Count 
outInAttribExps(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  if (A_NENTRIES(inVars) > 0) {
    CONST Count nStates= lalrNStates();
    Index s;
    for (s= 0; s < nStates; s++) {
      if (stateIns[s].hi > stateIns[s].lo) {
	CONST Index hi= stateIns[s].hi;
	Index i;
	fprintf(outFile, "\tcase %d:\n", s); nLines++;
	for (i= stateIns[s].lo; i < hi; i++) {
	  if (IN_EXPS(i).idVar != NIL) { /* Not redundant. */
	    outIDVar(outFile, IN_EXPS(i).idVar, -1);
	    fputs("= ", outFile);
	    nLines+= outExp(outFile, IN_EXPS(i).exp);
	    fputs(";\n", outFile); nLines++;
	  }
	} /* for (i= stateIns[s].lo; i < hi; i++) */
	fputs("\tbreak;\n", outFile); nLines++;
      } /* if (stateIns[s].hi > stateIns[s].lo) */
    } /* for (s= 0; s < nStates; s++) */
  } /* if (A_NENTRIES(inVars) > 0) */
  return nLines;
}


/*			COLORING PREFERENCES.				*/

/* Build preferences for coloring.  Two inherited attributes are preferred
 * to have the same color iff they have the same type. 
 */
static NodePairs 
buildColorPreferences()
{
  CONST Count nTypes= A_NENTRIES(inTypes);
  NodePairs sameTypes= newPairs();
  Index i;
  for (i= 0; i < nTypes; i++) {
    Index j;
    for (j= IN_TYPES(i).inVarsChain; j != NIL; j= IN_VARS(j).u.typeChain) {
      Index k;
      for (k= IN_VARS(j).u.typeChain; k != NIL; k= IN_VARS(k).u.typeChain) {
	addPair(sameTypes, j, k);
      }
    }
  } /* for (i= 0; i < nTypes; i++) */
  return sameTypes;
}


/*			INTERFERENCE GRAPH.				*/

/* Build interference graph for inherited attributes.  There is an
 * edge between two inherited attributes iff they are assigned
 * "unequal" expressions in a single state.
 */
static Graph 
buildInterferenceGraph()
{
  CONST Count nStates= lalrNStates();
  CONST Count nInVars= A_NENTRIES(inVars);
  Graph g= newGraph(nInVars);
  Index i;
  for (i= 0; i < nStates; i++) {
    Index j;
    CONST Index hi= stateIns[i].hi;
    for (j= stateIns[i].lo; j < hi; j++) {
      Index k;
      for (k= j + 1; k < hi; k++) {
	if (!equalExps(IN_EXPS(j).exp, IN_EXPS(k).exp) &&
	    IN_EXPS(j).idVar != IN_EXPS(k).idVar) {
	  addEdge(g, ID_VARS(IN_EXPS(j).idVar).inVar, 
		  ID_VARS(IN_EXPS(k).idVar).inVar);
	}
      } /* for (k= j + 1; k < hi; k++) */
    } /* for (j= stateIns[i].lo; j < hi; j++) */
  } /* for (i= 0; i < nStates; i++) */
  return g;
}

/*		    COLORING TO CLASS COMPUTATION.			*/	

static Coloring coloring; /* The coloring for the interference graph. */

typedef struct {
  Index2 lo;		/* 1st type in class-types. */
  Index2 hi;		/* 1 beyond last index in class-types. */
} Class;

typedef struct { /* Tracks the unique types assigned to each class. */
  Index2 type;
} ClassType;

static Class *classes;
static ClassType *classTypes;
static Count2 nClasses;

static VOID 
processColoring(c)
  Coloring c;
{
  CONST Count nC= nColors(c);
  Index i;
  Index classTypesIndex= 0;
  CONST Count nInVars= A_NENTRIES(inVars);
  nClasses= nC;
  classes= (Class *)MALLOC(nC * sizeof(Class));
  classTypes= (ClassType *)MALLOC(nInVars * sizeof(ClassType));
  for (i= 0; i < nC; i++) {
    Index j;
    CONST Index lo= classTypesIndex;
    classes[i].lo= lo;
    for (j= nextColoredNode(c, i, NIL); j != NIL; 
	 j= nextColoredNode(c, i, j)) {
      CONST Index type= IN_VARS(j).typeNum;
      Index k;
      IN_VARS(j).u.class= i;
      for (k= lo; k < classTypesIndex; k++) {
	if (classTypes[k].type == type) break;
      }
      if (k == classTypesIndex) { /* New type for this class. */
	classTypes[classTypesIndex++].type= type;
      }
    } /* for (j= nextColoredNode(c, i, NIL); j != NIL; ...) */
    assert(classTypesIndex > lo); /* Must have at least 1 type. */
    classes[i].hi= classTypesIndex;
  } /* for (i= 0; i < nC; i++) */
  assert(classTypesIndex <= nInVars);
  classTypes= (ClassType *)
    REALLOC((VOIDP)classTypes, classTypesIndex * sizeof(ClassType));
  VOID_RET();
}


/*		   ELIMINATE REDUNDANT ASSIGNMENTS.			*/

/* Eliminate redundant copies of inherited attributes which share the
 * same storage.  This is signalled by setting the idVar of the IN_EXP[]
 * entry to NIL.
 */
static VOID 
eliminateRedundantCopies()
{
  CONST Count nStates= lalrNStates();
  Index s;
  if (nClasses < A_NENTRIES(inVars)) { /* Possible redundant assignments. */
    for (s= 0; s < nStates; s++) {
      CONST Index hi= stateIns[s].hi;
      Index i;
      for (i= stateIns[s].lo; i < hi; i++) {
	if (IN_EXPS(i).idVar != NIL) { /* Not previously made redundant. */
	  Index inVarI= ID_VARS(IN_EXPS(i).idVar).inVar;
	  Index j;
	  assert(inVarI != NIL); /* Must be an inherited attribute. */
	  for (j= i + 1; j < hi; j++) {
	    if (IN_EXPS(j).idVar != NIL) { /* Not yet redundant. */
	      Index inVarJ= ID_VARS(IN_EXPS(j).idVar).inVar;
	      assert(inVarJ != NIL); /* Must be an inherited attribute. */
	      if (IN_VARS(inVarI).typeNum == IN_VARS(inVarJ).typeNum &&
		  IN_VARS(inVarI).u.class == IN_VARS(inVarJ).u.class) { 
		/* j shared storage with i; eliminate j's assignment. */
		assert(equalExps(IN_EXPS(i).exp, IN_EXPS(j).exp));
		IN_EXPS(j).idVar= NIL;
	      }
	    } /* if (IN_EXPS(j).idVar != NIL) */
	  } /* for (j= i + 1; j < hi; j++) */
	} /* if (IN_EXPS(i).idVar != NIL) */
      } /* for (i= stateIns[s].lo; i < hi; i++) */
    } /* for (s= 0; s < nStates; s++) */
  } /* if (nClasses < A_NENTRIES(inVars)) */
  VOID_RET();
}


/*		    OUTPUT INHERITED ATTRIB STRUCT			*/

/* Output a comment to outFile containing the names of all the inherited
 * attribute variables having equiv-class class and type-num typeNum. 
 */
static Count 
outInAttribNames(outFile, class, typeNum)
  FILE *outFile;
  Index class;
  Index typeNum;
{
  enum { LIMIT= 72 };
  Count nLines= 0;
  Count nChars= 0;
  Index i;
  fprintf(outFile, "    /* "); nChars+= 7;
  for (i= nextColoredNode(coloring, class, NIL); i != NIL; 
       i= nextColoredNode(coloring, class, i)) {
    if (IN_VARS(i).typeNum == typeNum) {
      Index idVar= IN_VARS(i).idVar;
      Sym sym= ID_VARS(idVar).sym;
      ConstString nonTermName= getNonTermString(SYM_NUM(sym));
      ConstString attribName= getIDString(ID_VARS(idVar).id);
      CONST Count len= strlen(nonTermName) + 1 + strlen(attribName) + 1;
      assert(idVar <= A_NENTRIES(idVars));
      assert(SYM_TYPE(sym) == NON_TERM_SYM);
      if (nChars + len >= LIMIT) {
	fputs("\n     * ", outFile); nLines++; nChars= 7;
      }
      fprintf(outFile, "%s.%s ", nonTermName, attribName); nChars+= len;
    }
  } /* for (i= nextColoredNode(coloring, class, NIL); ... */
  if (nLines > 0) { 
    fputs("\n     */\n", outFile); nLines+= 2; 
  }
  else {
    fputs("*/\n", outFile); nLines++; 
  }
  return nLines;
}

static Count 
outAttribsInType(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  Index i;
  if (A_NENTRIES(inVars) > 0) {
    fputs("typedef struct {\n", outFile); nLines++;
    for (i= 0; i < nClasses; i++) {
      Index j;
      CONST Index hi= classes[i].hi;
      fputs("  union {\n", outFile); nLines++;
      for (j= classes[i].lo; j < hi; j++) {
	CONST Index t= classTypes[j].type;
	Index typeSeq= IN_TYPES(t).typeSeq;
	char typeID[10];
	nLines+= outInAttribNames(outFile, i, t);
	fputs("    ", outFile);
	sprintf(typeID, "%s%d", TYPE_NUM_SELECTOR_PREFIX, t);
	nLines+= outType(outFile, typeSeq, typeID);
	fputs(";\n", outFile); nLines++;
      }
      fprintf(outFile, "  } %s%d;\n", CLASS_SELECTOR_PREFIX, i); nLines++;
    }
    fputs("} YYIn;\n", outFile); nLines++;
  }
  return nLines;
}


/*		INHERITED ATTRIBUTES STORAGE COMPUTATION.		*/

static VOID 
storeInAttribs()
{
  NodePairs preferences= buildColorPreferences();
  Graph intGraph= buildInterferenceGraph();
  coloring= colorGraph(intGraph, preferences);
  freeGraph(intGraph); freeNodePairs(preferences);
  processColoring(coloring);
  VOID_RET();
}


/*			COMPUTE ATTRIBUTES.				*/

VOID 
computeAttribs()
{
  CONST Count nInVars= A_NENTRIES(inVars);
  if (nInVars > 0) {
    buildInTypes();
    computeStateIns();
    storeInAttribs();
    eliminateRedundantCopies();
  } /* if (nInVars > 0) */
  VOID_RET();
}


/*			OUTPUT SEMANTIC TYPES.				*/

Count 
outSemanticTypes(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  nLines+= outYYSTYPE(outFile);
  nLines+= outAttribsOutType(outFile);
  nLines+= outAttribsInType(outFile);
  return nLines;
}


/*		   OUTPUT ATTRIBUTE PARAMETERS. 			*/

Count 
outAttribParams(outFile)
  FILE *outFile;
{
  Count nLines= 0;
  fprintf(outFile, "#define YY_HAS_IN_ATTRIBS %d\n", 
	  A_NENTRIES(inVars) > 0);
  nLines++;
  return nLines;
}


/*			    TEST ROUTINES.				*/

#ifdef TEST_ATTRIBS

static Boolean isValidTypeExp PROTO((Index typeExp));
static VOID dumpDec PROTO((Index sym));
static VOID dumpAttribs PROTO((VOID_ARGS));
static VOID dumpRHS PROTO((Index rhs));
static VOID dumpRules PROTO((VOID_ARGS));
static VOID dumpInStruct PROTO((VOID_ARGS));
static VOID dumpTests PROTO((VOID_ARGS));


static Boolean 
isValidTypeExp(typeExp)
  Index typeExp;
{
  Index i;
  if (typeExp >= A_NENTRIES(attribSeqs)) return FALSE;
  for (i= ATTRIB_SEQS(typeExp).lo; i < ATTRIB_SEQS(typeExp).hi; i++) {
    CONST Int t= ATTRIB_TOKS(i).type;
    if (t != TEXT_ATOK && t != SHORT_TEXT_ATOK && t != ID_VAR_DEC_ATOK)
      return FALSE;
  }
  return TRUE;
}

/* Dump the declaration for idVar. */
static VOID 
dumpDec(sym)
  Index sym;
{
  CONST SymType symType= SYM_TYPE(sym);
  CONST Index symNum= SYM_NUM(sym);
  CONST AttribTuple *tupleP= 
    (symType == TERM_SYM) ? &TERM_ATTRIBS(symNum) : &NON_TERM_ATTRIBS(symNum);
  CONST Index hi= tupleP->hi;
  printf("%s", (symType == TERM_SYM) 
	        ? getTermString(symNum) 
	        : getNonTermString(symNum));
  if (hi > tupleP->lo) {
    Index idVar;
    printf("(");
    for (idVar= tupleP->lo; idVar < hi; idVar++) {
      if (ID_VARS(idVar).id != NIL) { /* Some sort of declaration. */
	CONST Index typeExp= ID_VARS(idVar).typeExp;
	if (isValidTypeExp(typeExp)) {
	  outType(stdout, typeExp, getIDString(ID_VARS(idVar).id));
	}
	else {
	  printf("<UNKNOWN TYPE> %s", getIDString(ID_VARS(idVar).id));
	}
      }
      if (idVar + 1 != hi) printf(", ");
    }
    printf(")");
  }
  VOID_RET();
}

static VOID 
dumpAttribs() 
{
  Index i;
  for (i= 0; i < A_NENTRIES(termAttribs); i++) {
    dumpDec(MAKE_SYM(TERM_SYM, i));
    printf("\n");
  }
  for (i= 0; i < A_NENTRIES(nonTermAttribs); i++) {
    dumpDec(MAKE_SYM(NON_TERM_SYM, i));
    printf("\n");
  }
  VOID_RET();
}

static VOID 
dumpRHS(rhs)
  Index rhs;
{
  CONST Sym rhsSym= RHS_SYM(rhs);
  CONST SymType symType= SYM_TYPE(rhsSym);
  CONST Index symNum= SYM_NUM(rhsSym);
  ConstString symName= 
    (symType == TERM_SYM) 
    ? getTermString(symNum)
    : ( (symType == NON_TERM_SYM) ? getNonTermString(symNum) : "YY_RULE" );
  CONST Index hi= RHS_ATTRIBS(rhs).hi;
  printf("%s", symName);
  if (hi > RHS_ATTRIBS(rhs).lo) {
    Index i;
    printf("(");
    for (i= RHS_ATTRIBS(rhs).lo; i < hi; i++) {
      outExp(stdout, i);
      if (i + 1 != hi) printf(", ");
    }
    printf(")");
  }
  VOID_RET();
}

static VOID 
dumpRules()
{
  CONST Count nRules= gramNRules();
  Index r;
  for (r= 0; r < nRules; r++) {
    CONST Index lhsNonTerm= RULE_NON_TERM(r);
    Index i;
    dumpDec(MAKE_SYM(NON_TERM_SYM, lhsNonTerm));
    printf(" :\n");
    for (i= RULE_RHS_LO(r); i <= RULE_RHS_HI(r); i++) {
      printf("  ");
      dumpRHS(i);
      printf("\n");
    }
  }
  VOID_RET();
}

static VOID 
dumpInStruct()
{
  outInStruct(stdout);
  VOID_RET();
}

static VOID 
dumpTests()
{
  outTestExps(stdout);
  VOID_RET();
}

#endif


/*				INITIALIZATION.				*/

VOID 
initAttribs()
{
  G.lastActN= NIL;
#ifdef TEST_ATTRIBS
  atexit(dumpAttribs);
  atexit(dumpRules);
  atexit(dumpInStruct);
  atexit(dumpTests);
#endif
  VOID_RET();
}
