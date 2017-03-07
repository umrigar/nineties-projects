#include "plops.h"
#include "scan.h"

#include "area.h"

#include <assert.h>

/* Operator properties of a name: 0 means no operators. */
typedef UChar OpProp;	

enum {	/* Constants used for encoding/decoding operators into a OpProp. */
  OP_MASK= ((1 << NBITS_PER_OP_FIX) - 1)
};

#define OP_TYPE_TO_OP_FIX(opType)					\
  (((opType) <= FY_OP) ? PREFIX_OP : 					\
    ((opType) >= XF_OP) ? SUFFIX_OP : INFIX_OP)

#define OP_CODE_FIX(code, fix)						\
  ((code) & (OP_MASK << (NBITS_PER_OP_FIX * (fix))))

/*			OPERATOR HASH TABLE.				*/

typedef struct {
  Index2 name;		/* Operator name. */
  Index2 succ;		/* Next entry in hash-chain. */
  Index2 levels[3];	/* Level for PREFIX, INFIX and SUFFIX ops. */
  OpProp opProp;	/* Encoded operator properties for name. */
} OpStruct;

static AREA (ops, OpStruct, 6, UINT2_MAX);
#define OPS(i) A_ACCESS(ops, OpStruct, i)

#define OPS_HASH_SIZE 32
static Index2 opsHashTab[OPS_HASH_SIZE];

static VOID initOpsHash(VOID_ARGS)
{
  Index i;
  for (i= 0; i < OPS_HASH_SIZE; i++) opsHashTab[i]= NIL;
}

static Index getOp(Index id, Boolean doCreate)
{
  CONST Index h= id % OPS_HASH_SIZE;
  Index i;
  for (i= opsHashTab[h]; i != NIL; i= OPS(i).succ) {
    if (OPS(i).name == id) break;
  }
  if (i == NIL && doCreate) {
    i= A_NEXT(ops); 
    OPS(i).name= id; OPS(i).opProp= 0; 
    OPS(i).levels[PREFIX_OP]= OPS(i).levels[INFIX_OP]= 
      OPS(i).levels[SUFFIX_OP]= 0;
    OPS(i).succ= opsHashTab[h]; opsHashTab[h]= i;
  }
  return i;
}

Index opLevel(Index id, OpType opType)
{
  Index i= getOp(id, FALSE);
  Index opFix= OP_TYPE_TO_OP_FIX(opType);
  return (i == NIL || OP_CODE_FIX(OPS(i).opProp, opFix) != opType)
         ? 0 
	 : OPS(i).levels[opFix];
}

VOID addOp(Index id, OpType type, Index level)
{
  CONST Index i= getOp(id, TRUE);
  CONST OpFix opFix= OP_TYPE_TO_OP_FIX(type);
  CONST Index mask= ~(OP_MASK << (opFix * NBITS_PER_OP_FIX));
  OPS(i).levels[opFix]= level;
  OPS(i).opProp&= mask;
  assert((mask & type) == 0);
  if (level > 0) {
    OPS(i).opProp|= type;
  }
}

Boolean isInfixOrSuffixOp(Index id, int n)
/* Return TRUE iff:
 * 1. id is a infix operator with level >= n. 
 * 2. id is not a infix operator but is a suffix operator with level >= n. 
 */
{
  Index i= getOp(id, FALSE);
  return 
    ( (i != NIL) && 
      ( (  (OPS(i).levels[INFIX_OP] > 0) 
           ? OPS(i).levels[INFIX_OP] 
           : OPS(i).levels[SUFFIX_OP])
         >= n)
    );
}

VOID initOps(VOID_ARGS)
{
  initOpsHash();
  addOp(getName(":-", 2), XFX_OP, 1200);
  addOp(getName("-->", 3), XFX_OP, 1200);
  addOp(getName(":-", 2), FX_OP, 1200);
  addOp(getName("?-", 2), FX_OP, 1200);
  addOp(getName("dynamic", 7), FX_OP, 1150);
  addOp(getName("multifile", 9), FX_OP, 1150);
  addOp(getName("meta_predicate", 14), FX_OP, 1150);
  addOp(getName("initialization", 14), FX_OP, 1150);
  addOp(getName("volatile", 8), FX_OP, 1150);
  addOp(getName(";", 1), XFY_OP, 1100);
  addOp(getName("->", 2), XFY_OP, 1050);
  addOp(getName(",", 1), XFY_OP, 1000);
  addOp(getName("\\+", 2), FY_OP, 900);
  addOp(getName("spy", 3), FY_OP, 900);
  addOp(getName("nospy", 2), FY_OP, 900);
  addOp(getName("=", 1), XFX_OP, 700);
  addOp(getName("is", 2), XFX_OP, 700);
  addOp(getName("=..", 3), XFX_OP, 700);
  addOp(getName("==", 2), XFX_OP, 700);
  addOp(getName("\\==", 3), XFX_OP, 700);
  addOp(getName("@<", 2), XFX_OP, 700);
  addOp(getName("@>", 2), XFX_OP, 700);
  addOp(getName("@=<", 3), XFX_OP, 700);
  addOp(getName("@>=", 3), XFX_OP, 700);
  addOp(getName("=:=", 3), XFX_OP, 700);
  addOp(getName("=\\=", 3), XFX_OP, 700);
  addOp(getName("<", 1), XFX_OP, 700);
  addOp(getName(">", 1), XFX_OP, 700);
  addOp(getName("=<", 2), XFX_OP, 700);
  addOp(getName(">=", 2), XFX_OP, 700);
  addOp(getName(":", 1), XFY_OP, 600);
  addOp(getName("+", 1), YFX_OP, 500);
  addOp(getName("-", 1), YFX_OP, 500);
  addOp(getName("\\/", 2), YFX_OP, 500);
  addOp(getName("/\\", 2), YFX_OP, 500);
  addOp(getName("+", 1), FX_OP, 500);
  addOp(getName("-", 1), FX_OP, 500);
  addOp(getName("/", 1), YFX_OP, 400);
  addOp(getName("//", 2), YFX_OP, 400);
  addOp(getName("*", 1), YFX_OP, 400);
  addOp(getName("<<", 2), YFX_OP, 400);
  addOp(getName(">>", 2), YFX_OP, 400);
  addOp(getName("div", 3), YFX_OP, 400);
  addOp(getName("mod", 3), XFX_OP, 300);
  addOp(getName("^", 1), XFY_OP, 200);
}

