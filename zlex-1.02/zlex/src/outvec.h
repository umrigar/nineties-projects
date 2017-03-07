/*

File:	 outvec.h
Purpose: Scanner table output interface.

Copyright (C) 1995 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _OUTVEC_H
#define _OUTVEC_H


#include "port.h"

/* Output elements of a vector, with indexes in the range [0,
 * nEntries), separated by ', 's with no comma after the last element.
 * Each element is output by calling fn with the index of the element.
 * Indents each line by indent and outputs no more than nPerLine
 * elements per line. Returns without newline being output after
 * last element.
 */
Count outVec PROTO((FILE *outFile, 
		    Index indent, Count nPerLine, Count nEntries, 
		    VOID (*fn)(FILE *outFile, Index index)));

/* Output elements of a Index2 vector with indexes in the range [0,
 * nEntries), separated by ', 's with no comma after the last element.
 * Each element is output using "%5d" format.  Indents each line by
 * indent and outputs no more than nPerLine elements per line. Returns
 * without newline being output after last element.
 */
Count outIndex2Vec PROTO((FILE *outFile, 
			  Index indent, Count nPerLine, Count nEntries, 
			  Index2 vec[]));


#endif /* ifndef _OUTVEC_H */
