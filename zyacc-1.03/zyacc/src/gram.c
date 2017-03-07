/*

File:	 gram.c
Purpose: Parsing helper and grammar maintenance routines.

Last Update Time-stamp: "97/07/30 20:22:41 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifdef DEBUG_GRAM 	/* Define to check run-time assertions. */
#undef NDEBUG
#endif

/* Local includes. */
#include "attribs.h"
#include "gram.h"
#include "options.h"
#include "scan.h"
#include "zparse.h"

/* Libz includes. */
#include "area.h"
#include "iset.h"
#include "error.h"
#include "ids.h"
#include "memalloc.h"

/* System includes via port.h. */

/*			MAIN DATA STRUCTURES.	

Each terminal and non-terminal symbol are indexes into a TERMS[] 
and NON_TERMS[] arrays respectively.  These arrays contain the relevant
information for these symbols.  Each rule is an index into a RULES[]
array.  The entry for a rule in RULES[] bounds the indexes of its RHS in
a RHS_SYMS[] array.  A RHS_SYMS[] entry is of three types:

TERM_SYM:	For terminal RHS symbols.
NON_TERM_SYM:	For non-terminal RHS symbols.
RULE_SYM:	Sentinels put at the end of each RHS giving the rule #
                of the RHS just ended.

In addition to this symbol information, each RHS_SYMS[] entry also maintains
a successor chain.  The purpose of the chain depends on the type of symbol
stored at the RHS_SYMS[] entry:

TERM_SYM:	A chain rooted in TERMS[] which chains together all RHS
                occurrences of that terminal.
NON_TERM_SYM:	A chain rooted in NON_TERMS[] which chains together all RHS
                occurrences of that non-terminal.
RULE_SYM:	A chain rooted in NON_TERMS[] which chains together all rules
                having that non-terminal as their LHS non-terminal.

Occurrence of symbols in these chains are in the reverse order from their
order in the source file.

*/

/*			GLOBAL VARIABLES.				*/

static struct {
  char scratch[16];	/* Scratch area for printing names. */
  Index2 nextTokVal;	/* Value for next token with no explicit value. */
  Index2 maxTokVal;	/* Max. of all assigned token values so far. */
  ISet termVals;	/* Values which have been assigned to terminals. */
  ISet startIDs;	/* Start non-terminal IDs. */
  Index2 badTok;	/* Token number which always causes a error. */
  Index2 emptyTok;	/* Token number to indicate no tokens. */
  Index2 errTok;	/* Token number for error token. */
  Index2 errTokVal;	/* Value associated with error token. */
  Size2 maxRuleLen;	/* Max. length of rules seen. */
  Count2 nActs;		/* # of end-rule and inter-rule actions. */
  Boolean isTyped;	/* TRUE if %union or <type>s used. */
} globs;

#define G globs

VOID
dclTyped()
{
  G.isTyped= TRUE;
}


/*			  STATIC PROTOTYPES.				*/

static VOID initTerms PROTO((VOID_ARGS));
static Index newTerm PROTO((VOID_ARGS));
static VOID termLinkHash PROTO((Index id, BooleanX isLit, Index term));
static Index getTerm PROTO((Index id, BooleanX doCreate, BooleanX isLit));
static VOID checkTerms PROTO((VOID_ARGS));
static VOID initNonTerms PROTO((VOID_ARGS));
static Index getNonTerm PROTO((Index id, BooleanX doCreate));
static Boolean checkLitAsCID PROTO((ConstString litName));
static Index dclTermLo PROTO((Index2 ids[]));
static Index setTermValue PROTO((Index term, Index val));
static Index getValTerm PROTO((Index val));
static Index newRule PROTO((Index nonTerm));
static Index addRHSSym PROTO((Index symType, Index num, Index succ));
static Index newTest PROTO((Index ruleN));
static Index addBodyID PROTO((Index id));
static Index addBodyLit PROTO((Index litID, Index val));
static Index completeRule PROTO((Index nonTerm, Index ruleN, Count nRHS));
static Count reachableTraverse PROTO((Index n, Boolean stat[]));
static VOID checkReachableNonTerms PROTO((VOID_ARGS));
static VOID checkTerminalNonTerms PROTO((VOID_ARGS));
static Index makeStartRules PROTO((VOID_ARGS));
static VOID completeGram PROTO((VOID_ARGS));
static VOID initGram PROTO((VOID_ARGS));
static Boolean isErrorRule PROTO((Index ruleN));
static VOID outRefGramRuleRHS PROTO((FILE *f, Index ruleN, Index indent));
static VOID outRefNonTermRules PROTO((FILE *f, Index n));



/* 			OUTPUT GRAMMAR PARAMETERS.			*/

Count 
outGramParams(outFile)
  FILE *outFile;
{
  Count lineCount= 0;

  fprintf(outFile, "#define YY_BAD_TOK %d\n", G.badTok); 
  lineCount++;

  fprintf(outFile, "#define YY_EMPTY_TOK %d\n", G.emptyTok); 
  lineCount++;

  fprintf(outFile, "#define YY_ERR_TOK %d\n", G.errTok); 
  lineCount++;

  fprintf(outFile, "#define YY_ERR_TOK_VAL %d\n", G.errTokVal); 
  lineCount++;

  fprintf(outFile, "#define YY_HAS_MULTI_START %d\n", 
	  G.startIDs != BAD_ISET && sizeISet(G.startIDs) > 1); 
  lineCount++;

  fprintf(outFile, "#define YY_N_TESTS %d\n", nTests()); 
  lineCount++;

  fprintf(outFile, "#define YY_MAX_TOK_VAL %d\n", G.maxTokVal); 
  lineCount++;

  fprintf(outFile, "#define YY_MAX_RULE_LEN %d\n", G.maxRuleLen); 
  lineCount++;

  fprintf(outFile, "#define YY_N_NON_TERMS %d\n", gramNNonTerms()); 
  lineCount++;

  fprintf(outFile, "#define YY_N_TERMS %d\n", gramNTerms()); 
  lineCount++;

  fprintf(outFile, "#define YY_N_RHS %d\n", gramNRHSSyms()); 
  lineCount++;

  fprintf(outFile, "#define YY_N_RULES %d\n", gramNRules()); 
  lineCount++;

  fprintf(outFile, "#define YY_N_TOKS %d\n", gramNTerms()); 
  lineCount++;

  return lineCount;
}


/*			TERMINAL INFORMATION.				*/

/* We must have ID_NAME, LIT_NAME = 0, 1 respectively, as we use a
 * boolean isLit to index arrays.
 */
typedef enum {		/* Terminal name types. */
  ID_NAME,		/* A terminal name which is an identifier. */
  LIT_NAME, 		/* A terminal name which is a literal '...'. */
  N_NAMES		/* # of terminal names. */
} TermNameType;

typedef enum {
  PREC_USED_TF= 0x1,	/* Set if used in %prec directive. */
  NOWARN_TF= 0x2,	/* Set if no use of term on RHS should not warn. */
  LIT_NAME_TF= 0x4	/* Set if literal name used as ext. token name. */
} TermFlags;

typedef struct {	/* Terminal information. */
  Index2 ids[N_NAMES];	/* The names for this terminal. */
  Index2 succ[N_NAMES];	/* Next terminal in terminal hash-chain. */
  Index2 val;		/* The token value. */
  Index2 typeID;	/* <type> identifier.  NIL if none. */
  Index2 assocPrec;	/* assoc/precedence of this terminal. */
  Index2 rhsChain;	/* List head for all RHS occurrences. */
  Index2 flags;		/* TermFlags. */
} TermInfo;

static AREA (terms, TermInfo, 7, UINT2_MAX);
#define TERMS(i) A_ACCESS(terms, TermInfo, i)

#define TERMS_HASH_SIZE 64

static Index2 termsHashTab[TERMS_HASH_SIZE][N_NAMES];

#define ERR_TOK_VAL 256		/* Value associated with error token. */

Index
getErrorTerm()
{
  return G.errTok;
}

#define IS_LIT(id)		(getIDString(id)[0] == '\'')

static VOID 
initTerms()
{
  Index i;
  for (i= 0; i < TERMS_HASH_SIZE; i++) {
    Index n;
    for (n= ID_NAME; n < N_NAMES; n++) {
      termsHashTab[i][n]= NIL;
    }
  }
  G.termVals= newISet();
  VOID_RET();
}

static Index
newTerm()
{
  Index t= A_NEXT(terms);
  TERMS(t).ids[ID_NAME]= TERMS(t).ids[LIT_NAME]= TERMS(t).val= 
      TERMS(t).typeID= TERMS(t).assocPrec= TERMS(t).rhsChain= 
      TERMS(t).succ[ID_NAME]= TERMS(t).succ[LIT_NAME]= NIL; 
  TERMS(t).flags= 0;
  return t;
}

/* Link terminal term into hash-chain isLit for id. */
static VOID
termLinkHash(id, isLit, term)
  Index id; 
  BooleanX isLit;
  Index term;
{
  CONST Index h= id % TERMS_HASH_SIZE;
  assert(term < A_NENTRIES(terms));
  TERMS(term).succ[isLit]= termsHashTab[h][isLit];
  termsHashTab[h][isLit]= term;
  VOID_RET();
}

/* Return the terminal index for id, creating it if not found and
 * doCreate is TRUE.  If id is NIL, then don't link new entry into
 * hash-table.  If the entry is created, then its id is set to NIL.
 * This routine can only be used to create terminals which have a
 * single name. If !doCreate and !found return NIL.
 */
static Index 
getTerm(id, doCreate, isLit)
  Index id; 
  BooleanX doCreate;
  BooleanX isLit;
{
  Index t;
  CONST Index h= id % TERMS_HASH_SIZE;
  if (id != NIL) {
    for (t= termsHashTab[h][isLit]; t != NIL; t= TERMS(t).succ[isLit]) {
      if (TERMS(t).ids[isLit] == id) return t;
    }
  }
  /* Not found. */
  if (!doCreate) {
    return NIL;
  }
  else { /* Create a new entry. Link into hash-chain.  */	
    t= newTerm();
    if (id != NIL) {
      termLinkHash(id, isLit, t);
    }
    return t;
  }
}

/* Return the string for terminal with index term; literal name if possible. */
ConstString 
getTermString(term)
  Index term;
{
  Index2 *ids= TERMS(term).ids;
  CONST Index id= ids[LIT_NAME] == NIL ? ids[ID_NAME] : ids[LIT_NAME];
  assert(ids[ID_NAME] != NIL || ids[LIT_NAME] != NIL);
  return getIDString(id);
}

/* Return the string and length for terminal with index term; literal name
 * if possible.
 */
ConstString 
getTermStringWithLen(term, lenP)
  Index term;
  Size *lenP;
{
  Index2 *ids= TERMS(term).ids;
  CONST Index id= ids[LIT_NAME] == NIL ? ids[ID_NAME] : ids[LIT_NAME];
  assert(TERMS(term).ids[ID_NAME] != NIL || TERMS(term).ids[LIT_NAME] != NIL);
  *lenP= getIDLen(id);
  return getIDString(id);
}

/* Return the terminal with value val.  Must be the value of a
 * previously declared terminal.  Uses a linear search, since only
 * used in error-printing.
 */
static Index 
getValTerm(val)
  Index val;
{
  Index i;
  Count n= A_NENTRIES(terms);
  for (i= 0; i < n; i++) {
    if (TERMS(i).val == val) return i;
  }
  assert(0);
  return 0;
}

Index 
getTermType(term)
  Index term;
{
  return TERMS(term).typeID;
}

/* Output #define statements for all terminals with ID_NAME names or
 * multiple-character literal names. 
 * Also output #define statements for each start non-terminal if there
 * is more than 1.
 */
Count 
outToks(outFile)
  FILE *outFile;
{
  unsigned lineN= 0;
  unsigned i;
  CONST Count nTerms= A_NENTRIES(terms);
  for (i= 0; i < nTerms; i++) {
    if (TERMS(i).ids[ID_NAME] != NIL) {
      CONST char *tokName= getIDString(TERMS(i).ids[ID_NAME]);
      if ((isalpha(tokName[0]) || tokName[0] == '_')  
	  && TERMS(i).val != ERR_TOK_VAL) {
	fprintf(outFile, "#define %s%s %d\n", tokName, optionsP->termSuffix, 
		TERMS(i).val);
	lineN++;
      }
    }
    else if (TERMS(i).flags & LIT_NAME_TF) {
      Size len;
      ConstString litName= getTermStringWithLen(i, &len);
      fprintf(outFile, "#define %.*s%s %d\n", (int)len - 2, &litName[1], 
		optionsP->termSuffix, TERMS(i).val);
      lineN++;
    }
  } /* for (i= 0; i < nTerms; i++) */
  if (G.startIDs != BAD_ISET && sizeISet(G.startIDs) > 1) {
    /* The state values output here assume that the LR0 machine
     * is constructed in a breath-first manner.
     */
    Index i= 1;
    Index id;
    ISetIter isIter;
    fprintf(outFile, "\n/*%10sSTARTING NON-TERMINALS.%10s*/\n", "", "");
    lineN+= 2;
    FOR_EACH_IN_ISET(id, G.startIDs, isIter) {
      fprintf(outFile, "#define %s %d\n", getIDString(id), i++);
      lineN++;
    }
  }
  return lineN;
}

/* Output warnings for any terminal which is not used on the RHS of
 * at least 1 rule.
 */
static VOID
checkTerms()
{
  CONST Count nTerms= A_NENTRIES(terms);
  Index i;
  for (i= 0; i < nTerms; i++) {
    if (TERMS(i).rhsChain == NIL && 
	((TERMS(i).flags & (NOWARN_TF|PREC_USED_TF)) == 0)) {
      warn("Terminal %s not used within any rule.", getTermString(i));
    }
  }
}


/*			    TERMINAL TRANSLATIONS.			*/

/* Construct token translation table in tabs[XLAT_TAB]. */
CONST Index2 *
computeTranslateTab(nEntriesP)
  Index2 *nEntriesP;
{
  CONST Count nEntries= G.maxTokVal + 1;
  Index2 *xlatTab= (Index2 *)MALLOC(nEntries * sizeof(Index2));
  Index i;
  for (i= 0; i < nEntries; i++) xlatTab[i]= G.badTok;
  for (i= 0; i < A_NENTRIES(terms); i++) {
    assert(TERMS(i).val < nEntries);
    xlatTab[TERMS(i).val]= i;
  }
  *nEntriesP= nEntries;
  return xlatTab;
}

VOID 
freeTranslateTab(xlatTab)
  CONST Index2 *xlatTab;
{
  FREE((VOIDP)xlatTab);
  VOID_RET();
}




/*			NON-TERMINAL INFORMATION.			*/

typedef struct {
  Index2 id;		/* The identifier associated with this nonTerm. */
  Index2 typeID;	/* The <type> for this nonTerm.  NIL if none. */
  Index2 succ;		/* Next nonTerm in nonTerm hash-chain. */
  Index2 rulesChain;	/* Index first RULE_SYM on RHS for this nonTerm. */
  Index2 rhsChain;	/* Head of all RHS occurrences of this nonTerm. */
  Index2 testNum;	/* Test #. NIL if none. */
} NonTermInfo;

static AREA (nonTerms, NonTermInfo, 7, UINT2_MAX);
#define NON_TERMS(i) A_ACCESS(nonTerms, NonTermInfo, i)

#define NON_TERMS_HASH_SIZE 64

Index2 nonTermsHashTab[NON_TERMS_HASH_SIZE];

/*

The auxiliary structure above is used to chain the rules together in
the order in which they occur in the source, as well as separate out
rules which have a non-terminal as the first symbol on the RHS. 

*/
typedef struct { /* Indexes into RHS_SYMS[] (RULE_SYMs) to chain rules. */
  /* Element 1 used for rules whose RHS starts with a non-term, 
   * 0 for others. 
   */
  Index2 rulesStart[2];	/* First rule with nonTerm 1st RHS sym. */
  Index2 rulesEnd[2];	/* Last rule with nonTerm 1st RHS sym. */
} XNonTerm;

static AREA (xNonTerms, XNonTerm, 7, UINT2_MAX);
#define X_NON_TERMS(i) A_ACCESS(xNonTerms, XNonTerm, i)

static VOID 
initNonTerms()
{
  Index i;
  for (i= 0; i < NON_TERMS_HASH_SIZE; i++) {
    nonTermsHashTab[i]= NIL;
  }
  VOID_RET();
}

/* Return the non-terminal index for id, creating it if not found and
 * doCreate is TRUE.  If id is NIL then do not link into hash-table.
 * If the entry is created, then its id is set to NIL. If !doCreate
 * and !found return NIL.
 */
static Index 
getNonTerm(id, doCreate)
  Index id;
  BooleanX doCreate;
{
  Index i;
  CONST Index h= id % NON_TERMS_HASH_SIZE;
  if (id != NIL) {
    for (i= nonTermsHashTab[h]; i != NIL; i= NON_TERMS(i).succ) {
      if (NON_TERMS(i).id == id) return i;
    }
  }
  /* Not found. */
  if (!doCreate) {
    return NIL;
  }
  else { /* Create a new entry. Link into hash-chain.  */	
    Index j= A_NEXT(xNonTerms);
    XNonTerm *CONST p= &X_NON_TERMS(j);
    p->rulesStart[0]= p->rulesStart[1]= p->rulesEnd[0]= p->rulesEnd[1]= NIL;
    i= A_NEXT(nonTerms);
    assert(i == j);		/* Parallel arrays. */
    NON_TERMS(i).id= NON_TERMS(i).typeID= NON_TERMS(i).testNum=
      NON_TERMS(i).rulesChain= NON_TERMS(i).rhsChain= NIL; 
    if (id == NIL) {
      NON_TERMS(i).succ= NIL;
    }
    else {
      NON_TERMS(i).succ= nonTermsHashTab[h]; nonTermsHashTab[h]= i; 
    }
    return i;
  }
}

/* Return number of %test associated with nonTerm (NIL if none). */
Index 
nonTermTest(nonTerm)
  Index nonTerm;
{
  assert(nonTerm < A_NENTRIES(nonTerms));
  return NON_TERMS(nonTerm).testNum;
}

/* Return the string for non-terminal with index nonTerm.  If the id for
 * nonTerm is NIL, then the value returned is "_N" with nonTerm substituted 
 * for N.  
 * The return value is only valid only till the next call to getTermString()
 * or getNonTermString().
 */
ConstString 
getNonTermString(nonTerm)
  Index nonTerm;
{
  if (NON_TERMS(nonTerm).id == NIL) {
    Index testNum= nonTermTest(nonTerm);
    if (testNum != NIL) {
      sprintf(G.scratch, "YYTest_%d", testNum);
    }
    else {
      sprintf(G.scratch, "YY_%d", nonTerm); 
    }
    return G.scratch;
  }
  else {
    return getIDString(NON_TERMS(nonTerm).id);
  }
}

/* Return the string for non-terminal with index nonTerm.  If the id for
 * nonTerm is NIL, then the value returned is "_N" with nonTerm substituted 
 * for N.  
 * The return value is only valid only till the next call to getTermString()
 * or getNonTermString().
 */
ConstString 
getNonTermStringWithLen(nonTerm, lenP)
  Index nonTerm;
  Size *lenP;
{
  ConstString s;
  if (NON_TERMS(nonTerm).id != NIL) {
    s= getIDString(NON_TERMS(nonTerm).id);
    *lenP= getIDLen(NON_TERMS(nonTerm).id);
  }
  else {
    s= getNonTermString(nonTerm);
    *lenP= strlen(s);
  }
  return s;
}


/*			DECLARE TERMINAL.				*/

/* Check if the litName (enclosed within single quotes) is ok as a C
 * identifier.
 */
static Boolean
checkLitAsCID(litName)
  ConstString litName;
{
  Boolean isCID= isalpha(litName[1]) || litName[1] == '_';
  assert(litName[0] == '\'');
  if (isCID) {
    ConstString p;
    for (p= &litName[2]; (isalnum(*p) || *p == '_'); p++) ;
    isCID= (*p == '\'');
  }
  if (!isCID) {
    warn("#define for %s will result in a C error.", litName);
  }
  return isCID;
}

/* Declare terminal whose names are given by ids[].  Output error
 * message if any non-nil name was previously declared.  If any
 * non-nil name not previously declared, then create a terminal for
 * it.  Fill in both (if applicable) names into newly created terminal
 * and link them into hash-chain.  Return index of terminal if one is
 * created, NIL otherwise.
 */
static Index
dclTermLo(ids)
  Index2 ids[];
{
  CONST Index nt= ids[ID_NAME] == NIL ? NIL : getNonTerm(ids[ID_NAME], FALSE);
  Index t= NIL;
  assert(ids[ID_NAME] != NIL || ids[LIT_NAME] != NIL);
  if (nt != NIL) { /* ID_NAME declared to be a non-terminal. */
    error("`%s' previously declared as a non-terminal", 
	  getIDString(ids[ID_NAME]));
  }
  else if (ids[ID_NAME] == NIL || ids[LIT_NAME] == NIL) {
    /* Only a single name is being declared. */
    CONST Boolean isLit= ids[LIT_NAME] != NIL;
    t= getTerm(ids[isLit], TRUE, isLit);
    if (TERMS(t).ids[isLit] == NIL) {
      TERMS(t).ids[isLit]= ids[isLit];
    }
    else {
      error("multiply declared terminal `%s'", getIDString(ids[isLit]));
      t= NIL;
    }
  }
  else { /* Both names are being declared. */
    TermNameType n;
    for (n= ID_NAME; n < N_NAMES; n++) {
      assert(ids[n] != NIL);
      if (getTerm(ids[n], FALSE, n == LIT_NAME) == NIL) {
	if (t == NIL) t= newTerm(); 
	TERMS(t).ids[n]= ids[n]; termLinkHash(ids[n], n == LIT_NAME, t);
      }
      else {
	error("multiply declared terminal `%s'", getIDString(ids[n]));
      }
    } /* for (n= ID_NAME; n < N_NAMES; n++) */
  }
  return t;      
}

/* Set the value of term to val.  If val is NIL, compute a new value.  If
 * val is NIL and term has a literal name but no id name, then record the
 * fact that the literal was assigned a value by zlex.
 */
static Index
setTermValue(term, val)
  Index term; 
  Index val;
{
#if 0
  if (TERMS(term).ids[LIT_NAME] != NIL && val == NIL 
      && TERMS(term).ids[ID_NAME] == NIL) {
    /* Must be a multi-charactered literal with no assigned value. */
    TERMS(term).ids[ID_NAME]= 0; /* Remember the fact. */
  }
#endif
  if (val == NIL) {
    do { val= G.nextTokVal++; } while (inISet(val, G.termVals)); 
    addISet(G.termVals, val);
  }
  else if (inISet(val, G.termVals)) {
    error("value %d previously assigned to terminal `%s'", 
	  val, getTermString(getValTerm(val)));
  }
  TERMS(term).val= val;
  if (val > G.maxTokVal) G.maxTokVal= val;
  return val; 
}

/* Declare terminal id, with attributes given by remaining arguments.
 * Error if previously declared.  Check that val is unique.
 */
Index 
dclTerm(id, litID, litVal, val, assocPrec, typeID, nowarn)
  Index id;
  Index litID;
  Index litVal;
  Index val; 
  Index assocPrec;
  Index typeID;
  BooleanX nowarn;
{
  Index2 ids[2];
  Index t;
  ids[ID_NAME]= id; ids[LIT_NAME]= litID;
  assert(id != NIL || litID != NIL);
  t= dclTermLo(ids);
  if (t != NIL) {
    TermInfo * CONST p= &TERMS(t);
    val= setTermValue(t, val != NIL ? val : litVal);
    if (id == NIL && litVal == NIL && checkLitAsCID(getIDString(litID))) {
      p->flags|= LIT_NAME_TF;
    }
    if (nowarn) p->flags|= NOWARN_TF;
    p->typeID= typeID; p->assocPrec= assocPrec;
  }
  G.isTyped= G.isTyped || (typeID != NIL);
  return t;
}


/*			DECLARE NON-TERMINAL.				*/

/* Declare non-terminal id, with <type> given by typeID. Error if 
 * previously declared.
 */
Index 
dclNonTerm(id, typeID)
  Index id;
  Index typeID;
{
  Index i= getTerm(id, FALSE, FALSE);
  if (i != NIL) {
    error("`%s' previously declared as a terminal", getIDString(id));
  }
  else {
    i= getNonTerm(id, TRUE); assert(i != NIL);
    if (NON_TERMS(i).id != NIL) {
      error("multiply-declared non-terminal `%s'", getIDString(id));
    }
    else { /* Fill in values. */
      NonTermInfo *CONST p= &NON_TERMS(i);
      p->id= id; p->typeID= typeID;
    }
  }
  G.isTyped= G.isTyped || (typeID != NIL);
  return i;
}

VOID 
addStartNonTerm(nonTermID)
  Index nonTermID;
{
  if (G.startIDs == BAD_ISET) G.startIDs= newISet();
  addISet(G.startIDs, nonTermID);
  VOID_RET();
}


/*			    RULE INFORMATION.				*/

#if 0 /* Defined in "gram.h". Retained here for documentation. */

typedef struct {
  Index2 lhsNonTerm;	/* Non-terminal on LHS. */
  Index2 rhsIndexLo;	/* Index of 1st RHS symbol. */
  Index2 rhsIndexHi;	/* 1 beyond last RHS symbol (points to RULE_SYM). */
  Index2 assocPrec;	/* Encoded associativity & precedence of rule. */
  Index2 actN;		/* If non-NIL, then end-of-rule action #. */
  Index2 lineNum;	/* source line # for this rule */
  Index1 look;		/* 0 if %nolook; 1 if %look; 2 otherwise. */
} Rule;	

#endif

static AREA (rules, Rule, 8, UINT2_MAX);
#define RULES(i) A_ACCESS(rules, Rule, i)

static struct {		/* Information for current rule. */
  Index2 assocPrec;	/* Associativity of the rule. */
  Index2 lastActN;	/* If last RHS element is act, then act # else NIL. */
  Index2 lastTerm;	/* Last terminal added to rule RHS. */
  Index2 lhsNonTerm;	/* Non-terminal at LHS of current rule. */
  Index2 ruleN;		/* Rule number of current rule. */
  Index2 lastActLineNum;/* Line # for lastAct */
  Index1 look;		/* 0 if %nolook; 1 if %look; 2 otherwise. */
} currentRule= {
  NIL, NIL, NIL, NIL, NIL, FALSE
};

/* Create a new rule for nonTerm. Other rule components are
 * initialized to NIL. 
 */
static Index 
newRule(nonTerm)
  Index nonTerm;
{
  CONST Index r= A_NEXT(rules);
  RULES(r).lhsNonTerm= nonTerm; 
  RULES(r).rhsIndexLo= RULES(r).rhsIndexHi= RULES(r).assocPrec= NIL;
  RULES(r).actN= NIL; RULES(r).look= 2;
  RULES(r).lineNum= 0;
  return r;
}

Index 
ruleNonTerm(ruleN)
  Index ruleN;
{
  assert(ruleN < A_NENTRIES(rules));
  return RULES(ruleN).lhsNonTerm;
}

Index 
ruleLineNum(ruleN)
  Index ruleN;
{
  assert(ruleN < A_NENTRIES(rules));
  return RULES(ruleN).lineNum;
}


/*			RHS INFORMATION.				*/

/* We store the RHS symbols here.  For each rule, not only do we store its
 * RHS symbols in consecutive locations of RHS_SYMS[], but we terminate
 * the sequence by a RULE_SYM with number set to the rule #.  Hence even
 * an empty rule uses 1 RHS_SYMS[] location.
 */

#if 0	/* Defined in "gram.h". Retained here for documentation. */

typedef struct {	/* All identical RHS symbols chained together. */
  Sym sym;		/* The symbol stored in this RHS position. */
  Index2 succ;		/* Chain occurrences.  Wasted for TERM_SYMs. */
} RHSSym;

#endif 

static AREA (rhsSyms, RHSSym, 9, UINT2_MAX);
#define RHS_SYMS(i) A_ACCESS(rhsSyms, RHSSym, i)

static Index 
addRHSSym(symType, num, succ)
  Index symType;
  Index num;
  Index succ;
{
  Index i= A_NEXT(rhsSyms);
  RHS_SYMS(i).sym= MAKE_SYM(symType, num);
  RHS_SYMS(i).succ= succ;
  return i;
}

Sym 
getRHSSym(rhsIndex)
  Index rhsIndex;
{
  assert(rhsIndex < A_NENTRIES(rhsSyms));
  return RHS_SYMS(rhsIndex).sym;
}

/* Return the first occurrence in the head of the rhsChain for term. */
Index 
termRHSIndex(term)
  Index term;
{
  return TERMS(term).rhsChain;
}

/* Return the first occurrence in the head of the rhsChain for nonTerm. */
Index 
nonTermRHSIndex(nonTerm)
  Index nonTerm;
{
  return NON_TERMS(nonTerm).rhsChain;
}

/* Return the index of the first RHS symbol in the head of 
 * the rulesChain for nonTerm. 
 */
Index 
nonTermRHSRule(nonTerm)
  Index nonTerm;
{
  return NON_TERMS(nonTerm).rulesChain;
}


/*			    SEMANTIC TESTS.				*/

typedef struct {
  Index2 ruleN;		/* Test rule #. */
} TestInfo;

static AREA(testInfo, TestInfo, 4, UINT2_MAX);
#define TEST_INFO(i) A_ACCESS(testInfo, TestInfo, i)

/* Return a # for a new test.  0 is reserved for marking end of test-sets. */
static Index
newTest(ruleN)
  Index ruleN;
{
  Index t= A_NEXT(testInfo);
  if (t == 0) t= A_NEXT(testInfo);
  assert(t > 0);
  TEST_INFO(t).ruleN= ruleN;
  return t;
}

Index 
testRuleN(testNum)
  Index testNum;
{
  assert(testNum < A_NENTRIES(testInfo));
  return TEST_INFO(testNum).ruleN;
}

Count 
nTests()
{
  return A_NENTRIES(testInfo);
}


/*			RULE BUILDING.					*/

/* Add a dummy non-terminal for an action within the RHS.  Create a rule for
 * it, tho the empty RHS for the rule will be filled in later.
 */
Index 
internalActRule()
{
  Index n= getNonTerm(NIL, TRUE);	/* Create a new non-terminal. */
  Index r= newRule(n);			/* Create a new rule for it. */	
  setActReduction(currentRule.lastActN, r); /* Connect rule with action. */
  RULES(r).lineNum= currentRule.lastActLineNum;
  currentRule.lastActN= NIL;
  return NON_TERMS(n).rhsChain= addRHSSym(NON_TERM_SYM, n, NIL);
}

/* Add a dummy non-terminal for a %test within the RHS.  Create a rule for
 * it, tho the empty RHS for the rule will be filled in later.
 */
Index 
addBodyTest()
{
  CONST Index n= getNonTerm(NIL, TRUE);	/* Create a new non-terminal. */
  CONST Index ruleN= newRule(n);	/* Create a new rule for it. */	
  CONST Index testN= newTest(ruleN);	/* Create a new test. */
  NON_TERMS(n).testNum= testN;
  RULES(ruleN).lineNum= errorLineN();
/*  if (currentRule.lastActN != NIL) internalActRule(); */
  return NON_TERMS(n).rhsChain= addRHSSym(NON_TERM_SYM, n, NIL);
}


/* If id has been declared to be a terminal, then add it to RHS_SYMS[]
 * as a terminal.  Otherwise create a non-terminal entry and add it to
 * RHS_SYMS[] as a non-terminal.
 */
static Index 
addBodyID(id)
  Index id;
{
  Index i= getTerm(id, FALSE, FALSE);
  SymType type;
  Index2 *chainRoot;
  if (i == NIL) {
    i= getNonTerm(id, TRUE);
    if (NON_TERMS(i).id == NIL) { /* New entry. */
      NON_TERMS(i).id= id; 
    }
    chainRoot= &NON_TERMS(i).rhsChain;  
    type= NON_TERM_SYM;
  }
  else {
    type= TERM_SYM; currentRule.lastTerm= i; 
    chainRoot= &TERMS(i).rhsChain;
  }
/*  if (currentRule.lastActN != NIL) internalActRule(); */
  return *chainRoot= addRHSSym(type, i, *chainRoot);
}

/* Add literal litID with value val to RHS_SYMS as a terminal.  Create a
 * terminal entry for it with default values if it hasn't been seen 
 * previously.
 */
static Index 
addBodyLit(litID, val)
  Index litID;
  Index val;
{
  Index t= getTerm(litID, TRUE, TRUE);
  assert(t != NIL);
  if (TERMS(t).ids[LIT_NAME] == NIL) { /* Not seen previously. */
    TermInfo *CONST p= &TERMS(t);
    p->ids[LIT_NAME]= litID;  
    if ((val == NIL) && checkLitAsCID(getIDString(litID))) {
      p->flags|= LIT_NAME_TF;
    }
    val= setTermValue(t, val);
    p->typeID= p->assocPrec= NIL;
  }
/*  if (currentRule.lastActN != NIL) internalActRule(); */
  currentRule.lastTerm= t;
  return TERMS(t).rhsChain= addRHSSym(TERM_SYM, t, TERMS(t).rhsChain);
}

Index
addBodySym(id, val)
  Index id;
  Index val;
{
  return IS_LIT(id) ? addBodyLit(id, val) : addBodyID(id);
}

Index 
addBodyAct(actN, lineNum)
  Index actN;
  Index lineNum;
{
/*  if (currentRule.lastActN != NIL) internalActRule(); */
  /* action #s assigned sequentially. */
  assert(nErrors() > 0 || actN == G.nActs);
  currentRule.lastActN= actN; currentRule.lastActLineNum= lineNum;
  G.nActs++;
  return actN;
}

/* Start a new rule LHS.  Create a non-terminal for id if it doesn't 
 * already exist.  Return the nonTerm.
 */
Index 
ruleLHS(id)
  Index id;
{
  Index n= getNonTerm(id, TRUE);
  assert(n != NIL);
  if (NON_TERMS(n).id == NIL) { /* New non-term. */
    NON_TERMS(n).id= id; 
  }
  currentRule.lhsNonTerm= n; 
  return n;
}

/* Start a new rule. Reset currentRule global struct. */
Index 
beginRule()
{
  Index r= newRule(currentRule.lhsNonTerm);
  currentRule.ruleN= r; 
  currentRule.assocPrec= currentRule.lastActN= currentRule.lastTerm= NIL;
  currentRule.look= 2;
  RULES(r).rhsIndexLo= A_NENTRIES(rhsSyms);
  return r;
}

/* Complete current rule ruleN by adding a RULE_SYM to RHS_SYMS[].  Chain
 * it onto X_NON_TERMS[].  Return index where RULE_SYM added to RHS_SYMS[].
 */
static Index 
completeRule(nonTerm, ruleN, nRHS)
  Index nonTerm;
  Index ruleN; 
  Count nRHS;
{
  Boolean startsWithNonTerm=  
    (nRHS > 0) && 
    SYM_TYPE(RHS_SYMS(RULES(ruleN).rhsIndexLo).sym) == NON_TERM_SYM;
  XNonTerm *CONST p= &X_NON_TERMS(nonTerm);
  CONST Index i= addRHSSym(RULE_SYM, ruleN, NIL);
  if (nRHS > G.maxRuleLen) G.maxRuleLen= nRHS;
  if (p->rulesStart[startsWithNonTerm] == NIL) {
    p->rulesStart[startsWithNonTerm]= i;
  }
  else {
    assert(p->rulesEnd[startsWithNonTerm] != NIL);
    RHS_SYMS(p->rulesEnd[startsWithNonTerm]).succ= i;
  }
  RULES(ruleN).assocPrec= currentRule.assocPrec;
  RULES(ruleN).look= currentRule.look;
  return p->rulesEnd[startsWithNonTerm]= RULES(ruleN).rhsIndexHi= i;
}

/* End the current rule.  Fill in the associativity/precedence of the 
 * current rule and check for default action typing errors.  If new rules 
 * have been created since the current rule, then they correspond to 
 * rules for internal acts --- complete those empty rules.
 */
Index 
endRule(nRHS)
  Count nRHS;
{  
  CONST Index r= currentRule.ruleN;
  CONST Index lastSym= A_NENTRIES(rhsSyms); /* Index 1 beyond last RHS sym. */
  CONST Count nRules= A_NENTRIES(rules);
  Index i;
  nRHS-= currentRule.lastActN != NIL;
  assert(lastSym >= nRHS);

  /* Check typing. */
  if (currentRule.lastActN != NIL) {	/* Not a default action. */
    RULES(r).actN= currentRule.lastActN;
    RULES(r).lineNum= currentRule.lastActLineNum;
    setActReduction(currentRule.lastActN, r);
  }
  else { /* Check typing for default action. */
    Index lhsType= NON_TERMS(currentRule.lhsNonTerm).typeID;
    RULES(r).lineNum= errorLineN();    
    if (G.isTyped && lhsType != NIL) { /* Rule is typed. */
      Boolean typeErr= TRUE;
      if (nRHS > 0) {
	Sym sym1= RHS_SYMS(lastSym - nRHS).sym;	/* 1st RHS symbol. */
	typeErr= (SYM_TYPE(sym1) == TERM_SYM) 
	         ? TERMS(SYM_NUM(sym1)).typeID != lhsType
		 : NON_TERMS(SYM_NUM(sym1)).typeID != lhsType;
      }
      if (typeErr) {
	warn("Type error on implicit action.");
      }
    }
  }

  /* Set assoc/prec. to that of last RHS terminal if not explicit. */
  if (currentRule.assocPrec == NIL && currentRule.lastTerm != NIL) { 
    currentRule.assocPrec= TERMS(currentRule.lastTerm).assocPrec;
  }

  /* Complete rule. */
  i= completeRule(currentRule.lhsNonTerm, r, nRHS);
  assert(lastSym == i);

  /* Complete empty rules for internal actions. */
  for (i= r + 1; i < nRules; i++) {
    RULES(i).rhsIndexLo= completeRule(RULES(i).lhsNonTerm, i, 0);
    if (NON_TERMS(RULES(i).lhsNonTerm).testNum == NIL) {
      RULES(i).actN= G.nActs - (currentRule.lastActN != NIL) - (nRules - r);
    }
  }

  return r;
}

/* Set the rule precedence of the current rule to that of the terminal
 * given by termID.  Error if current rule already has precedence,
 * termID is not a terminal or has no precedence.
 */
VOID 
setRulePrec(termID)
  Index termID;
{
  if (currentRule.assocPrec != NIL) {
    error("multiple %%prec declarations for a single rule");
  }
  else {
    Boolean isLit= IS_LIT(termID);
    Index i= getTerm(termID, FALSE, isLit);
    if (i == NIL) {
      error("`%s' not declared as a terminal", getIDString(termID));
    }
    else {
      CONST Index assocPrec= TERMS(i).assocPrec;
      TERMS(i).flags|= PREC_USED_TF;
      if (assocPrec == NIL) {
	error("`%s' has no declared associativity/precedence", 
	      getIDString(termID));
      }
      else {
	currentRule.assocPrec= assocPrec;
      }
    }
  }
  VOID_RET();
}

VOID
setRuleLook(isLook1)
  BooleanX isLook1;
{
  currentRule.look= isLook1 ? 1 : 0;
}



/*		ASSOCIATIVITY-PRECEDENCE CONFLICT RESOLUTION.		*/

/* Resolve the conflict between terminal term and reduction for ruleN
 * dictated by associativity/precedence considerations.  If the
 * resolution is for a reduction, return reduceResolved; if the
 * resolution is for a shift, return shiftResolved.  Otherwise NIL
 * is returned: in this case, isNonAssoc is set TRUE iff the resolution
 * failed because the precedences were equal but declared %nonassoc.
 */
Index 
assocPrecResolve(term, ruleN, reduceResolved, shiftResolved, isNonAssoc)
  Index term;
  Index ruleN; 
  Index reduceResolved;
  Index shiftResolved;
  Boolean *isNonAssoc;
{
  CONST Index2 ruleAssocPrec= RULES(ruleN).assocPrec;
  CONST Index2 tokenAssocPrec= TERMS(term).assocPrec;
  if (ruleAssocPrec != NIL && tokenAssocPrec != NIL) {
    CONST Index2 rulePrec= PREC(ruleAssocPrec);
    CONST Index2 tokenPrec= PREC(tokenAssocPrec);
    *isNonAssoc= TRUE;
    if (rulePrec > tokenPrec) {
      return reduceResolved;
    }
    else if (rulePrec < tokenPrec) {
      return shiftResolved;
    }
    else {	/* Equal prec. */
      CONST AssocType assoc= ASSOC(ruleAssocPrec);
      assert(assoc == ASSOC(tokenAssocPrec)); /* = prec --> = assoc. */
      assert(assoc == LEFT_ASSOC || assoc == RIGHT_ASSOC|| assoc == NON_ASSOC);
      return (assoc == LEFT_ASSOC)
	     ? reduceResolved
	     : ((assoc == RIGHT_ASSOC) ? shiftResolved : NIL);
    }
  }
  *isNonAssoc= FALSE; return NIL;
}


/*				ITEMS.					*/

/* An item is merely an index into RHS_SYMS[]. */

/* Return the rule in which item is in. */
Index 
getItemRuleN(item)
  Index item;
{
  Index i;
  assert(item < A_NENTRIES(rhsSyms));
  for (i= item; SYM_TYPE(RHS_SYMS(i).sym) != RULE_SYM; i++) {
    assert(i < A_NENTRIES(rhsSyms));
  }
  return SYM_NUM(RHS_SYMS(i).sym);
}
	 
VOID 
printRule(outFile, ruleN, item)
  FILE *outFile; 
  Index ruleN; 
  Index item;
{
  Index i;
  CONST Index hi= RULES(ruleN).rhsIndexHi;
  fputs(getNonTermString(RULES(ruleN).lhsNonTerm), outFile); 
  fputs(": ", outFile);
  for (i= RULES(ruleN).rhsIndexLo; i < hi; i++) {
    CONST Sym sym= RHS_SYMS(i).sym;
    CONST Index num= SYM_NUM(sym);
    if (i == item) fputs(". ", outFile);
    fputs((SYM_TYPE(sym) == TERM_SYM) 
	  ? getTermString(num) 
	  : getNonTermString(num),
	  outFile);
    fputs(" ", outFile);
  }
  if (i == item) fputs(". ", outFile);
  VOID_RET();
}


/*			      NUMBERS.					*/

Count 
gramNTerms()
{
  return A_NENTRIES(terms);
}

Count 
gramNNonTerms()
{
  return A_NENTRIES(nonTerms);
}

Count 
gramNRules()
{ 
  return A_NENTRIES(rules);
}

Count 
gramNRHSSyms()
{
  return A_NENTRIES(rhsSyms);
}



/*			  @-LOCATION VARIABLES.				*/

/* Return the offset for a @n variable.  This must be called before
 * the constituent which contains this @-var is put into RHS_SYMS[].
 */
int 
translateLocVar(varNum)
  int varNum;
{
  Index lastSym= A_NENTRIES(rhsSyms);
  Int nRHS= lastSym - RULES(currentRule.ruleN).rhsIndexLo;
  Int offset= 1;
  if (varNum > nRHS) {
    error("@%d in rule does not refer to previous consituent", varNum);
  }
  else {
    offset= varNum - nRHS - 1;
  }
  return offset;
}
    

/*			  $-NUMERIC VARIABLES.				*/

/* Fill in the type (if varTypeID != NIL) and offset for $n variable. 
 * This must be called before the constituent which contains this $-var
 * is put into RHS_SYMS[].
 */
VOID 
translateNumVar(varNum, varTypeID, numVarP)
  int varNum;
  Index varTypeID;
  NumVar *numVarP;
{
  Index lastSym= A_NENTRIES(rhsSyms);
  Int nRHS= lastSym - RULES(currentRule.ruleN).rhsIndexLo;
  Int offset= 1;
  Index type= varTypeID;
  if (varNum > nRHS) {
    error("$%d in rule does not refer to previous consituent", varNum);
  }
  else {
    offset= varNum - nRHS - 1;
  }
  if (type == NIL && G.isTyped) {
    if (varNum > 0) {
      CONST Sym sym= RHS_SYMS(lastSym + offset).sym;
      Index num= SYM_NUM(sym);
      type= (SYM_TYPE(sym) == TERM_SYM) 
	? TERMS(num).typeID 
	  : NON_TERMS(num).typeID;
    }
    if (type == NIL) {
      warn("$%d of `%s' has no type", varNum, 
	   getNonTermString(currentRule.lhsNonTerm));
    }
  }
  numVarP->offset= offset; numVarP->type= type;
  VOID_RET();
}
    
VOID 
translateLHSVar(varTypeID, numVarP)
  Index varTypeID;
  NumVar *numVarP;
{
  numVarP->offset= 0; 
  if ((numVarP->type= NON_TERMS(currentRule.lhsNonTerm).typeID) == NIL && 
      G.isTyped) {
    warn("$$ of `%s' has no type", getNonTermString(currentRule.lhsNonTerm));
  }
  VOID_RET();
}



/*			NON-TERM REACHABILITY				*/

/* Recursive depth-first search of non-terminals accessible from 
 * non-terminal n.  stat[nx] is set TRUE if non-terminal nx is reachable.
 * Returns # of non-terminals which were not accessible before.
 */
static Count 
reachableTraverse(n, stat)
  Index n;
  Boolean stat[];
{
  Index nChanged= 0;
  Index i;
  assert(!stat[n]);
  stat[n]= TRUE; nChanged++;
  for (i= NON_TERMS(n).rulesChain; i != NIL; i= RHS_SYMS(i).succ) {
    CONST Index ruleN= SYM_NUM(RHS_SYMS(i).sym);
    Index j;
    assert(SYM_TYPE(RHS_SYMS(i).sym) == RULE_SYM);
    for (j= RULES(ruleN).rhsIndexLo; j < RULES(ruleN).rhsIndexHi; j++) {
      if (SYM_TYPE(RHS_SYMS(j).sym) == NON_TERM_SYM) {
	Index n0= SYM_NUM(RHS_SYMS(j).sym);
	if (!stat[n0]) {	/* Never reached before. */
	  nChanged+= reachableTraverse(n0, stat);
	}
      }
    } /* for (j= RULES(ruleN).rhsIndexLo; ...) */
  } /* for (i= NON_TERMS(nonTerm).rulesChain; ...) */
  return nChanged;
}

/* Ensure that all non-terminals are reachable from the start-symbol.
 * Print error messages for those which are not.
 */
static VOID 
checkReachableNonTerms() 
{
  CONST Count nNonTerms= A_NENTRIES(nonTerms);
  Boolean *stat= 		/* stat[n] tracks non-terminal n's status. */
    (Boolean *)CALLOC(nNonTerms, sizeof(Boolean)); 
  Index i;
  assert(FALSE == 0);	/* CALLOC() inits stat[] to FALSE. */
  if (reachableTraverse(0, stat) != nNonTerms) {
    for (i= 0; i < nNonTerms; i++) {
      /* Print error messages for unreachable non-terminals. */
      if (!stat[i]) {
	warn("Nonterminal `%s' not accessible from start-symbol.",
	      getNonTermString(i));
      }
    }
  }
  FREE(stat);
  VOID_RET();
}

/*			CHECK NON-TERM DERIVES TERMINALS		*/

/* Ensure that all non-terminals derive terminals.  Print error messages 
 * for those which do not.
 * Bottom-up iterative algorithm: first identify those non-terminals which 
 * directly derive terminal strings; put them in a toDo set.  Then for each 
 * non-terminal in toDo, check whether its RHS occurrences makes some 
 * other non-terminal derive a terminal string --- if so, put into toDo.
 * Finally iterate thru all non-terminals, printing error messages for 
 * those which do not derive terminal strings.
 */
static VOID 
checkTerminalNonTerms() 
{
  CONST Count nNonTerms= A_NENTRIES(nonTerms);
#if 0
  CONST Count nRules= A_NENTRIES(rules);
#endif
  enum { /* Current status of a non-terminal. */
    UNKNOWN= 0, TERMINAL_TO_DO, TERMINAL_DONE
  };
  Index i;
  char *stat= 		/* stat[n] tracks non-terminal n's status. */
    (char *)CALLOC(nNonTerms, sizeof(char));
  Index minToDo= nNonTerms;
  for (i= 0; i < nNonTerms; i++) {
    Index j;
    for (j= NON_TERMS(i).rulesChain; j != NIL; j= RHS_SYMS(j).succ) {
      CONST Index ruleN= SYM_NUM(RHS_SYMS(j).sym);
      CONST Index lastItem= RULES(ruleN).rhsIndexHi;
      Index k;
      assert(SYM_TYPE(RHS_SYMS(j).sym) == RULE_SYM);
      for (k= RULES(ruleN).rhsIndexLo; k < lastItem; k++) {
	if (SYM_TYPE(RHS_SYMS(k).sym) == NON_TERM_SYM) break;
      }
      if (k == lastItem) {
	stat[i]= TERMINAL_TO_DO; 
	if (i < minToDo) minToDo= i;
	break;
      }
    } /* for (j= NON_TERMS(i).rulesChain; j != NIL; j= RHS_SUCC(j)) */
  } /* for (i= 0; i < nNonTerms; i++) */
  while (minToDo < nNonTerms) {
    /* For each nonTerm in toDo, add those nonTerms it makes terminal. */
    Index nonTerm= minToDo;
    Index i;
    assert(stat[nonTerm] == TERMINAL_TO_DO);
    stat[nonTerm]= TERMINAL_DONE;
    for (i= minToDo + 1; i < nNonTerms; i++) {	
      /* Search forward for new minToDo. */
      if (stat[i] == TERMINAL_TO_DO) break;
    }
    minToDo= i;
    for (i= NON_TERMS(nonTerm).rhsChain; i != NIL; i= RHS_SYMS(i).succ) {
      CONST Index ruleN= getItemRuleN(i);
      CONST Index newNonTerm= RULES(ruleN).lhsNonTerm;
      if (stat[newNonTerm] == UNKNOWN) {
	CONST Index lastItem= RULES(ruleN).rhsIndexHi;
	Index j;
	for (j= RULES(ruleN).rhsIndexLo; j < lastItem; j++) {
	  if (SYM_TYPE(RHS_SYMS(j).sym) == NON_TERM_SYM && 
	      stat[SYM_NUM(RHS_SYMS(j).sym)] == UNKNOWN)
	    break;
	}
	if (j == lastItem) {
	  stat[newNonTerm]= TERMINAL_TO_DO;
	  if (newNonTerm < minToDo) minToDo= newNonTerm;
	}
      } /* if (stat[newNonTerm] == UNKNOWN) */
    } /* for (i= NON_TERMS(nonTerm).rhsChain; ...) */
  } /* while (minToDo < nNonTerms) */
  for (i= 0; i < nNonTerms; i++) {
    /* Print error messages for non-terms which do not derive terminals. */
    if (stat[i] != TERMINAL_DONE) {
      assert(stat[i] == UNKNOWN);
      error("Nonterminal `%s' does not derive any terminal string.",
	    getNonTermString(i));
    }
  }
  FREE(stat);
  VOID_RET();
}


/*			MAKE START RULES.				*/

/* Ensure that each start symbol has a rule.  If the # of start symbols
 * > 1, then for each start symbol with i create a rule $SS: $i i where
 * $SS is a new non-terminal and $i is a new terminal.
 */
static Index 
makeStartRules()
{
  assert(G.startIDs != BAD_ISET);
  if (sizeISet(G.startIDs) == 1) {
    ISetIter isIter= G.startIDs;
    CONST Index id= nextElementISet(G.startIDs, id, &isIter);
    CONST Index startNonTerm= getNonTerm(id, FALSE);
    if (startNonTerm == NIL) {
      error("no rules for start non-terminal `%s'", getIDString(id));
      return 0;
    }
    return startNonTerm;
  }
  else {
    CONST Index dummyNonTermID= getID("$SS", 3);
    CONST Index dummyNonTerm= dclNonTermAttribs(dummyNonTermID, NIL); 
    Index i= 1;
    Index id;
    ISetIter isIter;
    FOR_EACH_IN_ISET(id, G.startIDs, isIter) {
      CONST Index startNonTerm= getNonTerm(id, FALSE);
      Char scratch[10];
      sprintf(scratch, "$%d", i++);
      if (startNonTerm == NIL) {
	error("no rules for start non-terminal `%s'", getIDString(id));
	continue;
      }
      { CONST Index termID= getID(scratch, strlen(scratch));
        dclTermAttribs(termID, NIL, NIL, NIL, NIL, NIL, 0, FALSE);
	beginRuleAttribs();
	addRHSAttribs(termID, NIL); addRHSAttribs(id, NIL);
	endRuleAttribs(2);
      }
    }
    return dummyNonTerm;
  }
}


/*			COMPLETE GRAMMAR.				*/

CONST RHSSym *gramRHSSyms= 0;
CONST Rule *gramRules= 0;

static VOID 
completeGram()
{
  /* Complete initial rule messily "by hand". */
  enum {
    PATCH_INDEX= 0,		/* The RHS symbol which must be patched. */
    FIRST_USER_RULE= 1,		/* The number of the first user rule. */
    PSEUDO_START_NON_TERM= 0	/* The non-terminal for "$S". "  */
  };
  Index startNonTerm= (G.startIDs == BAD_ISET)
                      ? RULES(FIRST_USER_RULE).lhsNonTerm
                      : makeStartRules();
  assert(RHS_SYMS(PATCH_INDEX).succ == NIL);
  assert(NON_TERMS(PSEUDO_START_NON_TERM).rhsChain == PATCH_INDEX);
  RHS_SYMS(PATCH_INDEX).sym= MAKE_SYM(NON_TERM_SYM, startNonTerm);
  RHS_SYMS(PATCH_INDEX).succ= NON_TERMS(startNonTerm).rhsChain;
  NON_TERMS(startNonTerm).rhsChain= PATCH_INDEX;
  NON_TERMS(PSEUDO_START_NON_TERM).rhsChain= NIL;

  /* Chain rules for non-terminal together. */
  { Index n;
    CONST Count nNonTerms= A_NENTRIES(nonTerms);
    assert(nNonTerms == A_NENTRIES(xNonTerms));
    for (n= 0; n < nNonTerms; n++) {
      if (X_NON_TERMS(n).rulesStart[1] != NIL) {
	NON_TERMS(n).rulesChain= X_NON_TERMS(n).rulesStart[1];
	assert(X_NON_TERMS(n).rulesEnd[1] != NIL);
	RHS_SYMS(X_NON_TERMS(n).rulesEnd[1]).succ=
	  X_NON_TERMS(n).rulesStart[0];
      }
      else {
	NON_TERMS(n).rulesChain= X_NON_TERMS(n).rulesStart[0];
      }
    }
  }

  /* Check if terminals used on RHS of rules. */
  checkTerms();

  /* Check if grammar is reduced. */
  checkReachableNonTerms();  checkTerminalNonTerms();

  
  { /* Add special error rule for non-term $Err, with empty RHS. */
    Index errID= getID("$Err", 4);
    ruleLHS(errID); beginRule();  endRule(0);
  }

  { /* Add special tokens. */
    G.badTok= dclTerm(getID("$yyBadTok", 9), NIL, NIL, NIL, NIL, NIL, TRUE);
    G.emptyTok= 
      dclTerm(getID("$yyEmptyTok", 11), NIL, NIL, NIL, NIL, NIL, TRUE);
  }

  delArea(&xNonTerms);

  gramRHSSyms= &RHS_SYMS(0); gramRules= &RULES(0);
  VOID_RET();
}

/*			INITIALIZATION.					*/

static VOID 
initGram() 
{
  enum { TOK_VALS_START= ERR_TOK_VAL };
  G.maxTokVal= 0; G.nextTokVal= TOK_VALS_START;
  initTerms();
  initNonTerms();
  G.startIDs= BAD_ISET;
  { /* Build special symbols and initial rule. */
    Index eofID= getID("<EOF>", 5);
    Index startID= getID("$S", 2);
    Index s= dclNonTerm(startID, NIL);
    G.errTokVal= ERR_TOK_VAL;
    G.errTok= dclTerm(getID("error", 5), NIL, NIL, NIL, NIL, NIL, TRUE);
    dclTerm(eofID, NIL, NIL, 0, NIL, NIL, FALSE);
    assert(s == 0);
    /* Create $S: $S <EOF>.  1st RHS symbol replaced by %start later. */
    ruleLHS(startID); beginRule();
    addBodyID(startID); addBodyID(eofID);
    endRule(2);
  }
  VOID_RET();
}   



/*			TOP-LEVEL GRAMMAR DRIVER.			*/

VOID 
doGram()
{
  initISet();
  initGram();
  if (yyparse()) { fatal("could not recover from parse errors"); }
  completeGram();
  VOID_RET();
}


/*			OUTPUT REFERENCE GRAMMAR.			*/

static Boolean 
isErrorRule(ruleN)
  Index ruleN;
/* Returns true iff rule ruleN RHS has `error' token. */
{
  CONST Index hi= RULES(ruleN).rhsIndexHi;
  Index i;
  for (i= RULES(ruleN).rhsIndexLo; i < hi; i++) {
    CONST Sym sym= RHS_SYMS(i).sym;
    if (SYM_TYPE(sym) == TERM_SYM && SYM_NUM(sym) == G.errTok) return TRUE;
  }
  return FALSE;
}

/* Output the RHS symbols for ruleN (ignoring non-terminals created for
 * intra-body actions or %tests).  Indent any new lines by indent.  
 * Assumes that column # is indent at entry.  Does not output newline
 * at the end of the rule.
 */
static VOID
outRefGramRuleRHS(f, ruleN, indent)
  FILE *f;
  Index ruleN;
  Index indent;
{
  enum {
    LINE_WIDTH= 72
  };
  Index colN= indent;
  CONST Index hi= RULES(ruleN).rhsIndexHi;
  Count nRHSOut= 0;	/* Number of symbols output from RHS. */
  Index i;
  for (i= RULES(ruleN).rhsIndexLo; i < hi; i++) {
    CONST Sym sym= RHS_SYMS(i).sym;
    if (SYM_TYPE(sym) != NON_TERM_SYM || NON_TERMS(SYM_NUM(sym)).id != NIL) {
      CONST Index n= SYM_NUM(sym);
      CONST Index id= 
	( SYM_TYPE(sym) == NON_TERM_SYM )
	? NON_TERMS(n).id 
	: ( TERMS(n).ids[LIT_NAME] == NIL ? TERMS(n).ids[ID_NAME]
	                                  : TERMS(n).ids[LIT_NAME] );
      CONST Index idLen= getIDLen(id);
      if (colN + idLen + 1 >= LINE_WIDTH) {
	fprintf(f, "\n%*s", indent, ""); colN= indent;
      }
      fprintf(f, "%s ", getIDString(id)); colN+= idLen + 1;
      nRHSOut++;
    }
  }
  if (nRHSOut == 0) {
    fputs("/* EMPTY */", f);
  }
  VOID_RET();
}

/* Output formatted non-error reference rules for non-terminal n
 * provided it is not a special non-terminal introduced for intra-body
 * actions or %tests.
 */
static VOID 
outRefNonTermRules(f, n)
  FILE *f;
  Index n;
{
  if (NON_TERMS(n).id != NIL) { /* Not a special non-term. */
    Count nRules= 0;	/* Number of rules output so far. */
    Index rChain;
    fprintf(f, "%s\n", getIDString(NON_TERMS(n).id));
    for (rChain= NON_TERMS(n).rulesChain; 
	 rChain != NIL;
	 rChain= RHS_SYMS(rChain).succ) {
      CONST Index ruleN= SYM_NUM(RHS_SYMS(rChain).sym);
      assert(SYM_TYPE(RHS_SYMS(rChain).sym) == RULE_SYM);
      if (!isErrorRule(ruleN)) {
	fprintf(f, "  %c ", nRules ? '|' : ':');
	outRefGramRuleRHS(f, ruleN, 4); fputs("\n", f);  
	nRules++;
      }
    } /* for (rChain= NON_TERMS(n).rulesChain; ... ) */
    if (nRules == 0) { 
      /* Something weird: either a non-term with only error rules or
       * a non-reduced grammar. 
       */
      fputs("  : /* ERROR */\n", f);
    }
    fputs("  ;\n", f);
  } /* if (NON_TERMS(n).id != NIL) */
  VOID_RET();
}

/* Output a reasonably well-formatted reference grammar to f.  Do not
 * output nullable non-terminals created for intra-RHS actions or for
 * tests or error rules.  Do not output any assoc-prec. information.
 * Hence the resulting grammar may be ambiguous.  Output rule for
 * start-non-term first.  Then output rules essentially in order they
 * were in source file, except that all the rules for a non-terminal
 * are output together.
 */
VOID
outRefGram(f)
  FILE *f;
{
  Index r;
  ISet s= newISet();	/* Non-terminals whose rules have been output. */
  CONST Count nRules= A_NENTRIES(rules) - 1; /* Don't include $Err rule. */
  if (G.startIDs != BAD_ISET) {
    Index id;
    ISetIter isIter;
    FOR_EACH_IN_ISET(id, G.startIDs, isIter) {
      CONST Index n= getNonTerm(id, FALSE);
      outRefNonTermRules(f, n);  addISet(s, n);
    }
  }
  for (r= 1; r < nRules; r++) {
    if (!inISet(RULES(r).lhsNonTerm, s)) {
      outRefNonTermRules(f, RULES(r).lhsNonTerm); 
      addISet(s, RULES(r).lhsNonTerm);
    }
  }
  delISet(s);
  VOID_RET();
}

/*			    TEST ROUTINES.				*/	

#ifdef TEST_GRAM

static VOID dumpTerms PROTO((VOID_ARGS));
static VOID dumpNonTerms PROTO((VOID_ARGS));
static VOID dumpRules PROTO((VOID_ARGS));
static VOID dumpChains PROTO((VOID_ARGS));
int main PROTO((int argc, char **argv));




static VOID 
dumpTerms()
{
  CONST Count nTerms= A_NENTRIES(terms);
  Index t;
  printf("TERMINALS\n");
  for (t= 0; t < nTerms; t++) {
    printf("%s", getTermString(t));
    printf("= %d", TERMS(t).val);
    if (TERMS(t).typeID != NIL) 
      printf(" <%s>", getIDString(TERMS(t).typeID));
    printAssocPrec(TERMS(t).assocPrec);
    printf("\n");
  }
  printf("\n");
  VOID_RET();
}

static VOID 
dumpNonTerms()
{ 
  CONST Count nNonTerms= A_NENTRIES(nonTerms);
  Index n;
  printf("NON-TERMINALS\n");
  for (n= 0; n < nNonTerms; n++) {
    printf("%s", getNonTermString(n));
    if (NON_TERMS[n].typeID != NIL) 
      printf(" <%s>", getIDString(NON_TERMS(n).typeID));
    printf("\n");
  }
  printf("\n");
  VOID_RET();
}

static VOID 
dumpRules(VOID_ARGS)
{
  CONST Count nRules= A_NENTRIES(rules);
  Index r;
  printf("RULES\n");
  for (r= 0; r < nRules; r++) {
    printf("%3d: ", r); printRule(stdout, r, NIL); printf("\n");
  }
  printf("\n");
  VOID_RET();
}

static VOID 
dumpChains(VOID_ARGS) 
{
  CONST Count nNonTerms= A_NENTRIES(nonTerms);
  CONST Count nTerms= A_NENTRIES(terms);
  Index i;

  printf("NON-TERMINAL RULES\n");
  for (i= 0; i < nNonTerms; i++) {
    Index j;
    printf("%s: ", getNonTermString(i));
    for (j= nonTermRHSRule(i); j != NIL; j= RHS_SUCC(j)) {
      assert(SYM_TYPE(RHS_SYM(j)) == RULE_SYM);
      printf("%d ", SYM_NUM(RHS_SYM(j)));
    }
    printf("\n");
  }
  printf("\n");

  printf("TERMINAL RHS OCCURRENCES\n");
  for (i= 0; i < nTerms; i++) {
    Index j;
    printf("%s: ", getTermString(i));
    for (j= termRHSIndex(i); j != NIL; j= RHS_SUCC(j)) {
      Index r= getItemRuleN(j);
      assert(SYM_TYPE(RHS_SYM(j)) == TERM_SYM);
      assert(SYM_NUM(RHS_SYM(j)) == i);
      printf("(%d, %d) ", r, j - RULES(r).rhsIndexLo + 1);
    }
    printf("\n");
  }
  printf("\n");

  printf("NON-TERMINAL RHS OCCURRENCES\n");
  for (i= 0; i < nNonTerms; i++) {
    Index j;
    printf("%s: ", getNonTermString(i));
    for (j= nonTermRHSIndex(i); j != NIL; j= RHS_SUCC(j)) {
      Index r= getItemRuleN(j);
      assert(SYM_TYPE(RHS_SYM(j)) == NON_TERM_SYM);
      assert(SYM_NUM(RHS_SYM(j)) == i);
      printf("(%d, %d) ", r, j - RULES(r).rhsIndexLo + 1);
    }
    printf("\n");
  }
  printf("\n");

  VOID_RET();
}

int 
main(argc, argv)
  int argc;
  char **argv;
{
  initScan(argc > 1 ? argv[1] : NULL);
  doGram();
  dumpTerms();
  dumpNonTerms();
  dumpRules();
  dumpChains();
  return 0;
}

#endif
