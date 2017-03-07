/*

File:	 outvec.h
Purpose: Output vector interface.

Last Update Time-stamp: "97/07/10 18:11:48 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _OUTVEC
#define _OUTVEC

#include <stdio.h>

#include "port.h"

/* Output elements of vector vec, separated by ', 's with no more than
 * a certain number per line.  No ', ' after last element.  Return
 * count of '\n's output.  Precede vector elements with static
 * linkage and string hdr followed by [nElements]. 
 */
Count outIValVec PROTO((FILE *outFile, ConstString hdr, Count nEntries,
			CONST Index2 access[]));
Count outIFnVec PROTO((FILE *outFile, ConstString hdr, Count nEntries,
		       Index (*access)(Index)));
Count outSFnVec PROTO((FILE *outFile, ConstString hdr, Count nEntries,
		       ConstString (*access)(Index)));



#endif
