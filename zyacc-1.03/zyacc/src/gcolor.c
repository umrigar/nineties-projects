/*

File:	 gcolor.c
Purpose: Graph coloring routines.

Last Update Time-stamp: "97/07/31 10:27:14 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

#include "gcolor.h"

#include "memalloc.h"
#include "error.h"

/* System includes via port.h. */

/*

Heuristic graph coloring based on R. D. Dutton & R. C. Brigham, "A new
graph colouring algorithm", Computer Journal, (24) 1, 1981,
pp. 85-86.

The algorithm is based on Zykov's theorem which states the following:

    Given a graph G, and two non-adjacent vertices x, y in G.  Then
    the min. # of colors required to color G is the min. of the min. #
    of colors required to color graphs G1 and G2 where G1 is G with
    vertices x & y merged together and G2 is G with edge (x, y)
    added.

The theorem is fairly obvious: in any minimal coloring of G two
possibilities arise: (1) x & y are colored identically; (2) x & y
receive different colors.  G1 accounts for possibility (1), G2
accounts for possibility (2).

An exponential time algorithm can be based on a direct application of
Zykov's theorem.  Dutton and Brigham came up with a polynomial-time
algorithm by ignoring G2, and using a heuristic to choose the two
nodes (x, y) to merge at each step of their algorithm.

The heuristic is based on the following idea: the merging of
non-adjacent nodes will continue until a complete graph is formed.
Hence to get close to a minimal coloring, the formation of the
complete graph should be put off for as long as possible.  The chosen
heuristic does this by attempting to minimize the number of edges in
each reduced graph.  This is done by merging the pair of non-adjacent
nodes (x, y) which have the largest number of common neighbors. 

We maintain a triangular matrix of adjacencies: for every pair of
nodes n1 and n2 with n1 < n2, we maintain a magnitude mag(n1, n2).
mag(n1, n2) == 0 if n1 and n2 are adjacent; otherwise it is 1 greater
than the number of common neighbors for n1 & n2.  The non-adjacent
entries are linked into doubly-linked lists, sorted (>=) by
magnitude.  To facilitate quick insertion of a specific magnitude,
there is a perfect hash-table indexed by magnitude (the ordered list
is threaded thru these hash-table entries).

When two nodes n1 and n2 are merged together, the adjacencies of n2
are first added to n1: n2 is then effectively deleted (handled via a
Boolean vector isDeleted[]).  A non-adjacent pair of nodes is affected
by the merge if at least one node in the pair is adjacent or
coincident to the updated n1.  The magnitude for all such affected
non-adjacent pairs is recomputed, and the updated value inserted onto
the ordered magnitude list. 

*/

/*			      NODE PAIRS.				*/

/*

Externally-built data-structure used to indicate same color
preferences.  In any coloring, these preferences are satisfied if
permitted by the graph adjacency relation.

*/

enum { NODE_PAIR_BLOCK_SIZE= 16 };

typedef struct NodePairStruct {	
  struct NodePairStruct *succ;	/* Next node-pair. */
  Node node1, node2;		/* Nodes in node-pair. */
} NodePairType;

typedef NodePairType *NodePairsP;

typedef struct NPBlockStruct {	/* Blocks for NodePairs allocation. */
  struct NPBlockStruct *pred;	/* Previous node-pair block. */
  NodePairType	 		/* Block of node-pairs. */
    nodePairs[NODE_PAIR_BLOCK_SIZE];
} NodePairBlock;

static struct {
  NodePairType dummy;	/* Dummy node pair for list header. */
  NodePairBlock *blockP;/* Block of free node pairs. */
  Index2 index;		/* Index of next free  nodepair in current block. */
} nodePairAlloc;  

static NodePairsP newNodePair PROTO((VOID_ARGS));

/* Initialize the NodePairs data-structures. */
NodePairs 
newPairs()
{
  nodePairAlloc.index= NODE_PAIR_BLOCK_SIZE; /* Force new block. */
  return (NodePairs) &nodePairAlloc.dummy;
}

/* Free all memory occupied by NodePairs data-structures. */
VOID 
freeNodePairs(np)
  NodePairs np;
{
  NodePairBlock *p= nodePairAlloc.blockP;
  while (p != NULL) {
    NodePairBlock *previous= p->pred;
    FREE(p); p= previous;
  }
  VOID_RET();
}

/* Allocate a new node-pair (starting a new NodePairBlock if necessary). */
static NodePairsP 
newNodePair() 
{
  if (nodePairAlloc.index == NODE_PAIR_BLOCK_SIZE) {
    NodePairBlock *blockP= (NodePairBlock *)MALLOC(sizeof(NodePairBlock));
    blockP->pred= nodePairAlloc.blockP;
    nodePairAlloc.index= 0; nodePairAlloc.blockP= blockP;
  }
  return &(nodePairAlloc.blockP->nodePairs[nodePairAlloc.index++]);
}
  

/* Add the node-pair (node1, node2) to the NodePairs data-structure if
 * it isn't already there.  The list of NodePairs is maintained in
 * lexicographic order (>) by node-pair.  The > order is chosen to
 * accomodate those applications which may generate node-pairs in
 * a < order.
 */
NodePairs 
addPair(nodePairs, node1, node2)
  NodePairs nodePairs;
  NodeI node1;
  NodeI node2;
{
  Node n1= (node1 < node2) ? node1 : node2;
  Node n2= (node1 < node2) ? node2 : node1;
  Boolean found;
  NodePairsP nodePairsP= (NodePairsP) nodePairs;
  NodePairsP p;
  assert(node1 != node2);
  assert(n1 < n2);
  assert(nodePairsP == &nodePairAlloc.dummy);
  for (p= nodePairsP; TRUE; p= p->succ) {
    assert(p != NULL);
    if (p->succ == NULL) { /* Reached end of list. */	
      found= FALSE; break;
    }
    else { /* p->succ != NULL */
      NodePairsP s= p->succ;
      if (s->node1 == n1 && s->node2 == n2) {
	found= TRUE; break;
      }
      else { /* Check if we've reached a possible insertion point. */
	if (n1 < s->node1 || (n1 == s->node1 && n2 < s->node2)) {
	  found= FALSE; break;
	}
      } /* else !(s->node1 == n1 && s->node2 == n2) */
    } /* else p->succ != NULL */
  } /* for (p= nodePairsP; TRUE; p= p->succ) */
  if (!found) { /* Need to insert right after p. */
    NodePairsP newP= newNodePair();
    newP->node1= n1; newP->node2= n2; newP->succ= p->succ;
    p->succ= newP;
  }
  return nodePairs;
}


/*			 GRAPH CONSTRUCTION.	 			*/

typedef struct {
  Boolean **adj;	/* Upper non-diagonal half of adjacency matrix. */
  Count2 nNodes;	/* Number of nodes. */
  Count2 nEdges;	/* Number of edges. */
} GraphStruct;

typedef GraphStruct *GraphP;

#define ADJ_ELEM(g, i, j)  /* Assume i != j. */				\
  (((i) < (j)) ? &g->adj[i][(j) - (i) - 1] : &g->adj[j][(i) - (j) - 1])
#define ADJ(g, i, j)	   /* Assume i != j. */				\
  (((i) < (j)) ? g->adj[i][(j) - (i) - 1] : g->adj[j][(i) - (j) - 1])


/* Add edge (n1, n2) to adjacency matrix for graph g. */
Graph 
addEdge(g0, n1, n2)
  Graph g0;
  NodeI n1;
  NodeI n2;
{
  GraphP g= (GraphP) g0;
  Boolean *CONST p= ADJ_ELEM(g, n1, n2);
  assert(n1 != n2);
  assert(n1 < g->nNodes);
  assert(n2 < g->nNodes);
  if (!(*p)) {
    *p= TRUE; g->nEdges++;
  }
  return g0;
}

Graph 
newGraph(nNodes)
  Count nNodes;
{
  GraphP g= (GraphStruct *)MALLOC(sizeof(GraphStruct));
  if (nNodes < 2) {
    g->adj= NULL;
  }
  else { /* nNodes >= 2. */
    Boolean **adj= (Boolean **)MALLOC((nNodes - 1) * sizeof(Boolean *));
    Index i;
    for (i= 0; i < nNodes - 1; i++) {
      adj[i]= (Boolean *)CALLOC((nNodes - i - 1), sizeof(Boolean));
    }
    g->adj= adj; 
  }
  g->nNodes= nNodes; g->nEdges= 0;
  return (Graph)g;
}

VOID 
freeGraph(g0)
  Graph g0;
{
  GraphP g= (GraphP)g0;
  CONST Count nNodes= g->nNodes;
  Index i;
  if (nNodes > 1) {
    for (i= 0; i < nNodes - 1; i++) FREE(g->adj[i]);
  }
  FREE(g->adj);
  FREE(g);
  VOID_RET();
}

#ifdef TEST_GCOLOR

/* These routines are only required for random-graph testing. */

static GraphP completeGraph PROTO((GraphP g));
static GraphP delEdge PROTO((GraphP g, NodeI n1, NodeI n2));

/* Make graph g complete (all possible edges). */
static GraphP 
completeGraph(g)
  GraphP g;
{
  CONST Count nNodes= g->nNodes;
  Index i;
  for (i= 0; i < nNodes; i++) {
    Index j;
    for (j= i + 1; j < nNodes; j++) {
      addEdge(g, i, j);
    }
  }
  assert(g->nEdges == nNodes * (nNodes - 1)/2);
  return g;
}

/* Remove edge (n1, n2) from adjacency matrix for graph g (if it was
 * there). 
 */
static GraphP 
delEdge(g, n1, n2)
  GraphP g;
  NodeI n1;
  NodeI n2;
{
  Boolean *CONST p= ADJ_ELEM(g, n1, n2);
  assert(n1 < g->nNodes);
  assert(n2 < g->nNodes);
  assert(n1 != n2);
  if ((*p)) {
    *p= FALSE; g->nEdges--;
  }
  return g;
}

#endif /* ifdef TEST_GCOLOR */


/*			    COLORING MAGNITUDES.			*/

/* 

For each node-pair maintain a magnitude with value 0 if the nodes are
adjacent else 1 greater than the number of common neigbors for the
pair.  The magnitudes for non-adjacent nodes are maintained in a
doubly-linked list sorted (>=) by magnitude.  The runs of equal
magnitudes in the chain are interrupted by dummy magnitude nodes (with
magnitude == NIL) which serve as perfect hash-table entries to find
the start of a particular run. 

*/
typedef struct {
  Count2 mag;	/* 0: adjacent.  > 0: Non-adjacent magnitude, NIL dummy. */
  Index2 succ;	/* Next entry on magnitude chain. */
  Index2 pred;	/* Previous entry on magnitude chain. */
} MagStruct;

typedef struct {
  Int2 *rowBases;	/* rowBases[i] is biased index of 1st row i entry. */
  MagStruct *mags;	/* Vector indexed via rowBases[]. */
  Index2 magHeader0;	/* Biased (-1) index for magnitude 1. */
  Count2 nNodes;	/* Number of nodes in graph replicated here. */
} InternalStruct;

typedef InternalStruct *InternalP;

/* Since We only store entries for pairs (n1, n2) where n1 < n2, the
 * following macros facilitate access to these entries. 
 */

#define COLOR_ADJ(c, i, j)	/* i != j */				\
   ( (((i) < (j)) ? (c).mags[(c).rowBases[i] + (Int) (j)].mag		\
                  : (c).mags[(c).rowBases[j] + (Int) (i)].mag)		\
      == 0)


#define COLOR_ADJ_P(c, i, j)	/* i != j */				\
  (((i) < (j)) ? &(c).mags[(c).rowBases[i] + (Int) (j)]			\
               : &(c).mags[(c).rowBases[j] + (Int) (i)])


#define COLOR_ADJ_I(c, i, j)	/* i != j */				\
  (((i) < (j)) ? ((c).rowBases[i] + (Int) (j))				\
               : ((c).rowBases[j] + (Int) (i)))


/*	  COLOR-ADJACENCY INDEX TO MATRIX COORDINATE CONVERSION.	*/

static VOID indexToCoord PROTO((Count nNodes, Index index, 
				Node *row, Node *col));

/* Return (*row, *col) as the coordinates corresponding to index in 
 * mags[]. 
 */
static VOID 
indexToCoord(nNodes, index, row, col)
  Count nNodes;
  Index index;
  Node *row;
  Node *col;
{
  Index base= 0;
  Index r= 0;
  Size rowSize= nNodes - 1;
  while (base + rowSize <= index) { 
    base+= rowSize; rowSize--; r++; 
    assert(r < nNodes);
  }
  *row= r; *col= index - base + r + 1;
  assert(*row < *col);
  assert(*row < nNodes);
  assert(*col < nNodes);
  VOID_RET();
}


/*		      MAGNITUDE CHAIN MAINTENANCE.			*/

static VOID insertMagChain PROTO((CONST InternalStruct *iP, Index insert));
static VOID unlinkMagChain PROTO((CONST InternalStruct *iP, Index link));
static VOID changeMagChain PROTO((CONST InternalStruct *iP, 
				  Index change, Count newMag));
static Index getMaxMagIndex PROTO((Count nNodes, CONST InternalStruct *iP, 
				   Boolean deleted[], Node *n1P, Node *n2P));

/* Insert mag-entry at index insert in iP->mags[] into proper magnitude
 * chain. 
 */
static VOID 
insertMagChain(iP, insert)
  CONST InternalStruct *iP;
  Index insert;
{
  MagStruct *mags= iP->mags;
  CONST Index i= iP->magHeader0 + mags[insert].mag;
  assert(insert <= iP->magHeader0);
  mags[insert].pred= i; mags[insert].succ= mags[i].succ;
  if (mags[i].succ != NIL) mags[mags[i].succ].pred= insert;
  mags[i].succ= insert;
  VOID_RET();
}

/* Remove iP->mags[link] from appropriate magnitude chain. */     
static VOID 
unlinkMagChain(iP, link)
  CONST InternalStruct *iP;
  Index link;
{
  MagStruct *mags= iP->mags;
  assert(link <= iP->magHeader0);
  assert(mags[link].pred != NIL);
  mags[mags[link].pred].succ= mags[link].succ;
  if (mags[link].succ != NIL) mags[mags[link].succ].pred= mags[link].pred;
  mags[link].succ= mags[link].pred= NIL;  
  VOID_RET();
}

/* Change magnitude-value iP->mags[change] to newMag, and put into proper 
 * chain.
 */
static VOID changeMagChain(iP, change, newMag)
  CONST InternalStruct *iP;
  Index change;
  Count newMag;
{
  assert(change <= iP->magHeader0);
  unlinkMagChain(iP, change);
  iP->mags[change].mag= newMag;
  insertMagChain(iP, change);
  VOID_RET();
}

/* Search the magnitude chain (rooted in the dummy header at
 * (magHeader0 + nNodes - 1) for a non-dummy magnitude pair both of
 * whose nodes have not been deleted.  Remove any pairs which are
 * encountered which have deleted nodes.  Return the index of the
 * candidate pair, along with its row & column index in *rowP and 
 * *colP.  Return NIL if there is no candidate pair.
 */
static Index getMaxMagIndex(nNodes, iP, deleted, n1P, n2P)
  Count nNodes;
  CONST InternalStruct *iP;
  Boolean deleted[];
  Node *n1P;
  Node *n2P;
{
  MagStruct *mags= iP->mags;
  Index i, next;
  for (i= iP->magHeader0 + nNodes - 1; i != NIL; i= next) {
    assert(mags[i].mag != 0); /* Nodes better not be adjacent. */
    next= mags[i].succ;
    if (mags[i].mag != NIL) { /* Not a header node. */
      indexToCoord(nNodes, i, n1P, n2P);
      if (deleted[*n1P] || deleted[*n2P]) {
	unlinkMagChain(iP, i); /* Dump to facilitate next search. */
      }
      else {
	break; /* Found the maximum undeleted magnitude pair. */
      }
    }
  }
  return i;
}


/*			MAKE COLOR STRUCT.				*/

static InternalP makeInternalStruct PROTO((GraphP g));
static VOID freeInternalStruct PROTO((InternalP iP));

/* Build the internal structure for graph g, to facilitate coloring.
 * Create magnitude hashing dummy nodes and link them into a chain.
 * Create magnitude pairs for each pair of nodes, linking the pairs
 * for non-adjacent nodes into a linearly ordered (>= by magnitude) list.
 */
static InternalP 
makeInternalStruct(g)
  GraphP g;
{
  CONST Count nNodes= g->nNodes;
  CONST Count nPairs= nNodes * (nNodes - 1) / 2; /* # of node-pairs. */
  CONST Index nMags= (nNodes - 2) - 0 + 1; /* # of magnitude values. */
  CONST Count magHeader0= nPairs - 1; /* Bias, as magnitude has +1. */
  CONST Count nEntries= nPairs + nMags;
  MagStruct *mags= (MagStruct *)MALLOC(nEntries * sizeof(MagStruct));
  Int2 *rowBases= (Int2 *)MALLOC((nNodes - 1) * sizeof(Int2));
  InternalP iP= (InternalP)MALLOC(sizeof(InternalStruct));
  Index magsIndex= 0;
  Index i;
  iP->mags= mags; iP->rowBases= rowBases; iP->magHeader0= magHeader0;
  iP->nNodes= nNodes;
  for (i= 0; i <= nNodes - 2; i++) { 
    /* Initialize magnitude hash-table entries. */
    mags[magHeader0 + i + 1].mag= NIL;
    mags[magHeader0 + i + 1].pred= magHeader0 + (i + 1) + 1;
    mags[magHeader0 + i + 1].succ= magHeader0 + (i + 1) - 1;
  }
  mags[magHeader0 + 0 + 1].succ= 
    mags[magHeader0 + (nNodes - 2) + 1].pred= NIL;
  /* ! Bug fix: replaced i < nNodes by i < nNodes - 1 */
  for (i= 0; i < nNodes - 1; i++) { /* Build mags. */
    Index j;
    assert(magsIndex == (nNodes*(nNodes-1) - (nNodes-i)*(nNodes-i-1))/2);
    assert(magsIndex >= i);
    rowBases[i]= (Int)(magsIndex - i) - 1; /* Bias to avoid addition. */
    for (j= i + 1; j < nNodes; j++) {
      Index a= magsIndex++;
      if (ADJ(g, i, j)) {
	mags[a].mag= 0; 
      }
      else {
	Index k;
	Count mag= 1; /* Bias by 1 to distinguish from adjacent nodes. */
	for (k= 0; k < nNodes; k++) {
	  mag+= (i != k) && ADJ(g, i, k) && (k != j) && ADJ(g, k, j);
	}
	mags[a].mag= mag;
	insertMagChain(iP, a);
      } /* else !ADJ(g, i, j) */
    } /* for (j= i + 1; j < nNodes; j++) */
  } /* for (i= 0; i < nNodes; i++) */
  assert(magsIndex == nPairs);
  return iP;
}

static VOID freeInternalStruct(iP)
  InternalP iP;
{
  FREE(iP->mags); FREE(iP->rowBases); FREE(iP);
  VOID_RET();
}

#ifndef NDEBUG

static Boolean internalStructInvariant PROTO((InternalStruct *iP, 
					      Boolean deleted[]));

/* Invariant on internal-structure to be checked using assertions. */
static Boolean 
internalStructInvariant(iP, deleted)
  InternalStruct *iP;
  Boolean deleted[];
{
  CONST Count nNodes= iP->nNodes;
  Index i;
  Boolean isOk= TRUE;
  for (i= 0; i < nNodes; i++) {
    if (!deleted[i]) {
      Index j;
      for (j= i + 1; j < nNodes; j++) {
	if (!deleted[j] && !COLOR_ADJ(*iP, i, j)) { /* j !adj i. */
	  CONST Index c= COLOR_ADJ_I(*iP, i, j);
	  Count mag= 1;
	  Index k;
	  for (k= 0; k < nNodes; k++) {
	    mag+= !deleted[k] && (i != k) && COLOR_ADJ(*iP, i, k) && 
	          (k != j) && COLOR_ADJ(*iP, k, j);
	  }
	  if (iP->mags[c].mag != mag) { 
	    isOk= FALSE; 
	    fprintf(stderr, "Failing internalStruct invariant on (%d, %d).\n", 
		    i, j);
	    break; 
	  }
	}
      }
      if (!isOk) break;
    }
  }
  return isOk;
}

#endif


/*			    NODE EQUIVALENCES.				*/

typedef struct {
  Node class;		/* Smallest node with same color. */
  Node succ;		/* Successor in circular-list for color equiv. class. */
} NodeEquivs;

static NodeEquivs *makeEquivs PROTO((Count nNodes));
static VOID freeEquivs PROTO((NodeEquivs *equivs));
static VOID mergeEquivs PROTO((NodeEquivs equivs[], NodeI n1, NodeI n2));
static Node equivClass PROTO((NodeEquivs equivs[], NodeI n));


static NodeEquivs *
makeEquivs(nNodes)
  Count nNodes;
{
  NodeEquivs *equivs= (NodeEquivs *)MALLOC(nNodes * sizeof(NodeEquivs));
  Index i;
  for (i= 0; i < nNodes; i++) {
    equivs[i].class= equivs[i].succ= i;
  }
  return equivs;
}

static VOID 
freeEquivs(equivs)
  NodeEquivs *equivs;
{
  FREE(equivs);
  VOID_RET();
}

static VOID mergeEquivs(equivs, n1, n2)
  NodeEquivs equivs[];
  NodeI n1;
  NodeI n2;
{
  CONST Index n1Class= equivs[n1].class;
  CONST Index n2Class= equivs[n2].class;
  CONST Boolean isN1Class= (n1Class < n2Class); 
  CONST Index newClass= (isN1Class) ? n1Class : n2Class;
  CONST Index target1= (isN1Class) ? n2 : n1;
  CONST Index j= (isN1Class) ? n1 : n2;
  CONST Index target2= equivs[j].succ;
  Index i;
  assert(n1 < n2);
  for (i= target1; equivs[i].succ != target1; i= equivs[i].succ) {
    equivs[i].class= newClass;
  }
  equivs[target1].class= newClass; 
  /* i is predecessor of target1, j predecessor of target2. */
  /* Splice two circular chains together. */
  equivs[i].succ= target2; equivs[j].succ= target1;
  VOID_RET();
}

static Node 
equivClass(equivs, n)
  NodeEquivs equivs[];
  NodeI n;
{
  return equivs[n].class;
}



/*		        COLORING STRUCTURE.			*/

typedef struct {
  NodeEquivs *equivs;	/* Array of node-equivalence classes. */
  Index2 *colorClasses;	/* Index of element within color-class. */
  Count2 nColors;	/* Number of colors. */
} ColorStruct;

typedef ColorStruct *ColorP;

static ColorP makeColorStruct PROTO((Count nNodes));
static VOID completeColoring PROTO((ColorP cP, Count nNodes));

static ColorP makeColorStruct(nNodes)
  Count nNodes;
{
  ColorP cP= (ColorP)MALLOC(sizeof(ColorStruct));
  NodeEquivs *equivs= makeEquivs(nNodes);
  cP->equivs= equivs; cP->colorClasses= NULL; cP->nColors= NIL;
  return cP;
}

/* Change the class of each equivalence class from its representative
 * element (the smallest member) to a sequentially assigned color.
 */
static VOID completeColoring(cP, nNodes)
  ColorP cP;
  Count nNodes;
{
  NodeEquivs *equivs= cP->equivs;
  Index color= 0;
  Index2 *colorClasses= (Index2 *)MALLOC(cP->nColors * sizeof(Index2));
  Index i= 0;
  if (nNodes > 0) {
    do {
      CONST Index class= i;
      CONST Index c= color++;
      Index j= class;
      assert(equivs[i].class == i);
      assert(c <= i);
      colorClasses[c]= i;
      do { equivs[j].class= c; j= equivs[j].succ; } while (j != class);
      do { i++; } while (i < nNodes && equivs[i].class != i);
    } while (i < nNodes);
  }
  assert(color == cP->nColors);
  cP->colorClasses= colorClasses;
  VOID_RET();
}

VOID freeColors(cP0)
  Coloring cP0;
{
  ColorP cP= (ColorP)cP0;
  freeEquivs(cP->equivs); FREE(cP->colorClasses); FREE(cP);
  VOID_RET();
}


/*			EXTERNAL COLORING INTERFACE.			*/
Count nColors(cP)
  Coloring cP;
{
  return ((ColorP)cP)->nColors;
}

Index nodeColor(cP, n)
  Coloring cP; 
  NodeI n;
{
  return ((ColorP)cP)->equivs[n].class;
}

Index 
nextColoredNode(cP0, color, node)
  Coloring cP0; 
  Index color;
  NodeI node;
{
  ColorP cP= (ColorP)cP0;
  assert(color < cP->nColors);
  if (node == NIL) {
    return cP->colorClasses[color];
  }
  else {
    assert(cP->equivs[node].class == color);
    if (cP->equivs[node].succ == cP->colorClasses[color]) {
      return NIL;
    }
    else {
      assert(cP->equivs[cP->equivs[node].succ].class == color);
      return cP->equivs[node].succ;
    }
  }
}


/*			   MERGE NON-ADJACENT NODES.			*/

/* Merge nodes n1 & n2 in coloring-structure *cP.  Label the merged node
 * by n1.  If i is adjacent to either n1 or n2, then for all non-adjacent
 * pairs (i, j) recompute their magnitudes.  Add n2 to the deleted nodes.
 */
static VOID mergeNodes PROTO((InternalStruct *iP, Boolean deleted[], 
			      NodeI n1, NodeI n2));
static VOID 
mergeNodes(iP, deleted, n1, n2)
  InternalStruct *iP;
  Boolean deleted[];
  NodeI n1;
  NodeI n2;
{
  CONST Count nNodes= iP->nNodes;
  MagStruct *CONST mags= iP->mags;
  Index i;
  assert(!deleted[n1]);
  assert(!deleted[n2]);
  deleted[n2]= TRUE; 
  for (i= 0; i < nNodes; i++) { /* Make n2 adjacencies adjacent to n1. */
    if (!deleted[i] && (i != n1) && 
	COLOR_ADJ(*iP, i, n2) && !COLOR_ADJ(*iP, i, n1)) {
      CONST Index n1iIndex= COLOR_ADJ_I(*iP, i, n1);
      unlinkMagChain(iP, n1iIndex);
      mags[n1iIndex].mag= 0; /* Make n1 & i adjacent. */
    }
  }
  for (i= 0; i < nNodes; i++) {
    if (!deleted[i]) {
      Index j;
      for (j= i + 1; j < nNodes; j++) {
	if (!deleted[j] && !COLOR_ADJ(*iP, i, j) &&
	    ( (i == n1) || (j == n1) || 
	     COLOR_ADJ(*iP, i, n1) || COLOR_ADJ(*iP, j, n1) )
	    ) { /* Need to recompute magnitude for pair (i, j). */
	  CONST Index ijIndex= COLOR_ADJ_I(*iP, i, j);
	  Count mag= 1;
	  Index k;
	  for (k= 0; k < nNodes; k++) {
	    mag+= !deleted[k] && (i != k) && COLOR_ADJ(*iP, i, k) && 
	      (k != j) && COLOR_ADJ(*iP, k, j);
	  }
	  if (mag != mags[ijIndex].mag) changeMagChain(iP, ijIndex, mag);
	}
      } /* for (j= i + 1; j < nNodes; j++) */
    } /* if (!deleted[i]) */
  } /* for (i= 0; i < nNodes; i++) */
  assert(internalStructInvariant(iP, deleted));
  VOID_RET();
}

#if 0

/* Merge nodes n1 & n2 in internal-structure *iP.  Label the merged node
 * by n1.  Recompute the magnitude for *ALL* non-adjacent pair of nodes.  
 * Add n2 to the deleted nodes.  This used to work; has not been tested
 * after several changes were made.
 */
static VOID mergeNodes(InternalStruct *iP, Boolean deleted[], 
		       Node n1, Node n2)
{
  CONST Count nNodes= iP->nNodes;
  MagStruct *CONST mags= iP->mags;
  Index i;
  assert(!deleted[n1]);
  assert(!deleted[n2]);
  deleted[n2]= TRUE; 
  for (i= 0; i < nNodes; i++) { /* Make n2 adjacencies adjacent to n1. */
    if (!deleted[i] && (i != n1) && 
	COLOR_ADJ(*iP, i, n2) && !COLOR_ADJ(*iP, i, n1)) {
      CONST Index n1iIndex= COLOR_ADJ_I(*iP, i, n1);
      unlinkMagChain(iP, n1iIndex);
      mags[n1iIndex].mag= 0; /* Make n1 & i adjacent. */
    }
  }
  for (i= 0; i < nNodes; i++) {
    if (!deleted[i]) {
      Index j;
      for (j= i + 1; j < nNodes; j++) {
	if (!deleted[j] && !COLOR_ADJ(*iP, i, j)) { /* j !adj i. */
	  CONST Index ijIndex= COLOR_ADJ_I(*iP, i, j);
	  Count mag= 1;
	  Index k;
	  for (k= 0; k < nNodes; k++) {
	    mag+= !deleted[k] && (i != k) && COLOR_ADJ(*iP, i, k) && 
	          (k != j) && COLOR_ADJ(*iP, k, j);
	  }
	  if (mags[ijIndex].mag != mag) changeMagChain(iP, ijIndex, mag);
	}
      } /* for (j= i + 1; j < nNodes; j++) */
      
    }
  } /* for (i= 0; i < nNodes; i++) */
  assert(internalStructInvariant(iP, deleted));
  VOID_RET();
}

#endif


/*			    GRAPH COLORING.				*/

#ifndef NDEBUG

static Boolean goodColoring PROTO((GraphP g, ColorP cP));

/* Return TRUE iff the equivalence-classes in equivs[] represents a correct
 * coloring for graph g.
 */
static Boolean goodColoring(g, cP)
  GraphP g;
  ColorP cP;
{
  CONST Count nNodes= g->nNodes;
  CONST NodeEquivs *equivs= cP->equivs;
  Index i= 0;
  Index color= 0;
  if (nNodes == 0) return TRUE;
  do {  /* i gets values of the smallest element in each equiv. class. */
    CONST Index class= i;
    Index j= class;
    CONST Index c= color++;
    assert(equivs[i].class == c); /* i is smallest element with color c. */
    assert(cP->colorClasses[c] == i);
    do { /* j iterates thru all nodes in class. */
      Index k;
      for (k= equivs[j].succ; k != class; k= equivs[k].succ) {
	/* Confirm j not adjacent to remaining nodes in its class. */
	if (ADJ(g, j, k)) {
	  fprintf(stderr, "Coloring error for (%d, %d).\n", j, k);
	  return FALSE;
	}
      }
      j= equivs[j].succ;
    } while (j != class);
    do { i++; } while (i < nNodes && equivs[i].class != color);
  } while (i < nNodes);
  assert(color == cP->nColors);
  return TRUE;  
}

#endif

Coloring 
colorGraph(g0, similarNodes0)
  Graph g0;
  NodePairs similarNodes0;
{
  GraphP g= (GraphP)g0;
  NodePairsP similarNodes= (NodePairsP)similarNodes0;
  CONST Count nNodes= g->nNodes;
  Count nColors= nNodes;
  ColorP cP= makeColorStruct(nNodes);
  if (nNodes > 1) { /* Possibility of non-trivial coloring. */
    Boolean *isDeleted= (Boolean *)CALLOC(nNodes, sizeof(Boolean));
    InternalP iP= makeInternalStruct(g);
    NodeEquivs *equivs= cP->equivs;
    NodePairsP p;
    Index i;
    Index2 n1, n2;
    assert(similarNodes != NULL);
    assert(internalStructInvariant(iP, isDeleted));
    for (p= similarNodes->succ; p != NULL; p= p->succ) {
      /* Accomodate coloring preferences if possible. */
      CONST Node n1= equivClass(equivs, p->node1);
      CONST Node n2= equivClass(equivs, p->node2);
      if (n1 != n2 && !COLOR_ADJ(*iP, n1, n2)) {
	mergeNodes(iP, isDeleted, n1, n2);
	mergeEquivs(equivs, n1, n2);	
	nColors--;
      }
    }
    while ((i= getMaxMagIndex(nNodes, iP, isDeleted, &n1, &n2)) != NIL) {
      /* Do coloring by merging non-adjacent nodes. */
      mergeNodes(iP, isDeleted, n1, n2);
      mergeEquivs(equivs, n1, n2);
      nColors--;
    }
    freeInternalStruct(iP);
    FREE(isDeleted);
  } /* if (nNodes > 1) */
  cP->nColors= nColors; 
  completeColoring(cP, nNodes);
  assert(goodColoring(g, cP));
  return (Coloring) cP;
}

#ifdef TEST_GCOLOR

/*			TESTING GRAPHS DEFINED IN FILES. 		*/

/*

Graphs are specified using the following format:

Graphs->     Graphs Graph
           | EMPTY
Graph->	     GraphName NNodes Edges Terminator
GraphName->  LINE 
NNodes->     POS_INT
Edges->      Edges Edge
           | EMPTY
Edge->	     '(' NON_NEG_INT ',' NON_NEG_INT ')'
Terminator-> NEG_INT

LINE is any sequence of characters not containing newline.  All other
terminals are as defined by scanf().

*/

#include <math.h>
#include <stdio.h>

enum { MAX_LINE_LENGTH= 120 };

static Graph readGraph PROTO((FILE *gFile, char *graphName));
static VOID fileTest PROTO((ConstString fName));

/* Hardly any error checking here. */
static Graph 
readGraph(gFile, graphName)
  FILE *gFile;
  char *graphName;
{
  Count nNodes;
  Graph g;
  Char c;
  Int dummy;
  fgets(graphName, MAX_LINE_LENGTH, gFile); 
  graphName[strlen(graphName) - 1]= '\0'; /* Clobber possible newline. */
  fscanf(gFile, "%d", &nNodes);
  g= newGraph(nNodes);
  while (1) {
    int n1, n2;
    fscanf(gFile, " %c", &c);
    if (c != '(') break;
    if (fscanf(gFile, "%d , %d )", &n1, &n2) != 2) 
      fatal("bad graph file 0.");
    if (n1 >= nNodes || n2 >= nNodes) {
      fatal("bad graph edge (%d, %d).", n1, n2);
    }
    addEdge(g, n1, n2);
  }
  if (c != '-') fatal("bad graph file 1.");
  fscanf(gFile, "%d ", &dummy);
  return g;
}

static VOID 
fileTest(fName)
  ConstString fName;
{
  FILE *gFile;
  NodePairs p= newPairs();
  if (!(gFile= fopen(fName, "r"))) FILE_ERROR(fName);
  while (!feof(gFile)) {
    char name[MAX_LINE_LENGTH];
    Graph g= readGraph(gFile, name);
    Coloring cP= colorGraph(g, p);
    CONST Count nC= nColors(cP);
    Index c;
    printf("%s\n# of colors= %d.\n", name, nC);
    for (c= 0; c < nC; c++) {
      Index i;
      printf("{ ");
      for (i= nextColoredNode(cP, c, NIL); i != NIL; 
	   i= nextColoredNode(cP, c, i)) {
	printf("%d ", i);
      }
      printf("}\n");
    }
    printf("\n");
    freeGraph(g); freeColors(cP);
  }
  VOID_RET();
}


/*			RANDOM GRAPH TESTING.				*/

static Graph randomGraph PROTO((Count nNodes, double density));
static VOID randTest PROTO((VOID_ARGS));

/* Build a random graph with nNodes nodes and number of edges given by
 * density. 
 */
static Graph randomGraph(nNodes, density)
  Count nNodes;
  double density;
{
  Count nEdges= (nNodes * (nNodes - 1) / 2) * density + 0.5;
  Graph g0= newGraph(nNodes);
  GraphP g= (GraphP) g0;
  CONST double randFactor= (nNodes - 1)/(double)RAND_MAX;
  if (density > 0.5) { /* Build down from complete graph. */
    completeGraph(g);
    while (g->nEdges > nEdges) {
      CONST Node n1= rand()*randFactor;
      CONST Node n2= rand()*randFactor;
      if (n1 != n2) delEdge(g, n1, n2);
    }
  }
  else { /* Build up from empty graph. */
    while (g->nEdges < nEdges) {
      CONST Node n1= rand()*randFactor;
      CONST Node n2= rand()*randFactor;
      if (n1 != n2) addEdge(g, n1, n2);
    }
  }
  return g0;
}

static VOID randTest()
{
  enum { NNODES_LO= 10, NNODES_INC= 10, NNODES_HI= 150,
         NTRIES= 120
       };
#define DENS_LO 0.0
#define DENS_INC 0.1
#define DENS_HI 1.0
#define EPSILON 0.01
  Count n;
  NodePairs p= newPairs();
  printf("%10s%10s%10s\n", "# NODES", "DENSITY", "# COLORS");
  for (n= NNODES_LO; n <= NNODES_HI; n+= NNODES_INC) {
    double d;
    for (d= DENS_LO; d <= DENS_HI; d+= DENS_INC) {
      CONST Count 
	nTries= (fabs(d - 0.0) < EPSILON || fabs(d - 1.0) < EPSILON)
	        ? 1 
		: NTRIES;
      Index t;
      for (t= 0; t < nTries; t++) {
	Graph g= randomGraph(n, d);
	Coloring cP= colorGraph(g, p);
	printf("%10d%10.1g%10d\n", n, d, nColors(cP));
	freeGraph(g); freeColors(cP);
      }
    }
  }
  VOID_RET();
}


/*			   MAIN TEST PROGRAM.				*/	

int main(int argc, char *argv[]);

/* No arguments-> random graph testing; 1 argument-> name of file containing
 * graphs to be tested.
 */     
int main(argc, argv)
  int argc;
  char *argv[];
{
  if (argc > 2) {
    fprintf(stderr, "Usage: %s [graphFile]\n", argv[0]); exit(1);
  }
  if (argc == 1) { /* No arguments: do a random test. */
    randTest();
  }
  else {
    fileTest(argv[1]);
  }
  return 0;
}




#endif /* ifdef TEST_GCOLOR */
