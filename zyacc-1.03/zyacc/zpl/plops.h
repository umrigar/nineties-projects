#ifndef _PLOPS_H
#define _PLOPS_H

#include "ccport.h"

/* Private type used for encoding operators. */
typedef enum { /* MUST be contiguous, starting at 0. */
  PREFIX_OP, 
  INFIX_OP, 
  SUFFIX_OP 
} OpFix;


/* Private constant used for encoding operators. */
enum { NBITS_PER_OP_FIX= 2 }; 

#define ENCODE_OP(fix, disp)	/* disp != 0. */			\
  (disp << (fix * NBITS_PER_OP_FIX))

/* Public type for Prolog's operator-types. */
typedef enum { 
  FX_OP= ENCODE_OP(PREFIX_OP, 1),
  FY_OP= ENCODE_OP(PREFIX_OP, 2),
  XFX_OP= ENCODE_OP(INFIX_OP, 1),
  XFY_OP= ENCODE_OP(INFIX_OP, 2),
  YFX_OP= ENCODE_OP(INFIX_OP, 3),
  XF_OP= ENCODE_OP(SUFFIX_OP, 1),
  YF_OP= ENCODE_OP(SUFFIX_OP, 2)
} OpType;

/* Add operator id of OpType type and precedence level. */
VOID addOp(Index id, OpType type, Index level);

/* Return TRUE iff:
 * 1. id is a infix operator with level >= n. 
 * 2. id is not a infix operator but is a suffix operator with level >= n. 
 */
Boolean isInfixOrSuffixOp(Index id, int n);

/* Returns the level of operator id having type opType.  0 if none. */
Index opLevel(Index id, OpType opType);

/* Initialize operators. */
VOID initOps(VOID_ARGS);


#endif /* ifndef _PLOPS_H */
