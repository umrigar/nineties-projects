/*

File:	 gcolor.h
Purpose: Graph coloring interface.

Last Update Time-stamp: "97/07/10 18:07:21 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#ifndef _GCOLOR
#define _GCOLOR

#include "port.h"

/* The following types are opaque to clients. */

typedef VOIDP Graph;
typedef VOIDP NodePairs;
typedef VOIDP Coloring;

typedef Index2 Node;
typedef Index NodeI;	/* For parameter passing. */

NodePairs newPairs PROTO((VOID_ARGS));
NodePairs addPair PROTO((NodePairs p, NodeI node1, NodeI node2));
VOID freeNodePairs PROTO((NodePairs np));

Graph newGraph PROTO((Count nNodes));
Graph addEdge PROTO((Graph g, NodeI node1, NodeI node2));
Coloring colorGraph PROTO((Graph g, NodePairs similarNodes));
VOID freeGraph PROTO((Graph g));

Count nColors PROTO((Coloring cP));
Index nodeColor PROTO((Coloring cP, NodeI n));
Index nextColoredNode PROTO((Coloring cP, Index color, Index node));
VOID freeColors PROTO((Coloring cP));

#endif /* ifndef _GCOLOR */
