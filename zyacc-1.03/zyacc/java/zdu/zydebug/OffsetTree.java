/*

File:	 OffsetTree.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/07/27 09:12:05 zdu"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.zydebug;

import zdu.zydebug.Tree;

/** Trees where each kid maintains an offset from its parent, which
 * can be used to draw the tree.
 */

/* 

This is adapted from "Tidier Drawing of Trees", by E. M. Reingold and
J. S. Tilford, IEEETSE, Vol. SE-7, Number 2, March 1981, pp. 223--228.

This algorithm needs to be incremental, so that subtrees can be
inserted at arbitrary nodes.  Hence for a straightforward adaptation,
each node would need to store information about its lowest level
leftmost and rightmost nodes (EXTREME information, in the terminology
of the paper).  Since we are also allowing general m-ary (rather than
merely binary) trees, we would also need 3 offset fields per node, if
we followed the suggestion in the paper (I could not understand how to
determine which offset field corresponded to which thread pointing to
a node).  This would be quite a lot of auxiliary storage per node.

Instead in the algorithm used here, we store the left and right
contours of each subtree at the root of the subtree.  If we use an
array for the contour, this requires additional storage of 2*h, where
h is the height of the node.  This will win over the above adaptation
when h is less than about 5; it will loose when h >= 5, but the
implementation appears simpler, without bothering about updating
threads incrementally.

If the height of a subtree is h, then left (right) contour[i] contains
the absolute displacement from the subtree root of the left (right)
contour of the tree at level (i + 1) (the root is level 0).

The main action used here is adding a kid subtree to a node.  When a
subtree is added, its contour is used to compute its separation from
its left sibling, then the separation for its right sibling is
computed.  Finally, the contours for the parent are recomputed; if
they have changed, then the its parent is recomputed, and so on.  The
worst case (which may happen often) occurs when the subtree is added
to the lowest level of the tree, in which case all nodes between the
point of addition and the root (inclusive) need their contours
recomputed.

When checking for collisions, it is not sufficient to check for
collisions only with immediate siblings.  It is necessary to check for
collisions with non-immediate siblings too, when the height of the
immediate sibling is less than the height of non-immediate siblings.

Note that the implementation distinguishes between computing the
separation of a newly added kid and existing kids.  To minimize the
computation when a new kid is added, it separates the kid from its
left siblings and its right siblings to ensure they do not collide; to
do this, it merely needs to check all the levels in the new kid versus
its left siblings and right siblings.  However, when two siblings are
brought closer together after a node is removed, it is necessary to
check all levels to the minimum of the height of the left siblings and
the right siblings.

*/

class OffsetTree extends Tree {

  /** Nullary OffsetTree constructor. 
   * @param info	Information stored in tree node being constructed.
   */
  OffsetTree(Object info) {
    super(info);
  }

  /** Unary OffsetTree constructor. 
   * @param info	Information stored in tree node being constructed.
   * @param kid0	Single child of constructed OffsetTree node.
   */
  OffsetTree(Object info, OffsetTree kid0) {
    super(info, kid0);
    kid0.offset= 0;
    sepToOffset();
    updateParent();
  }

  /** Binary OffsetTree constructor. 
   * @param info	Information stored in tree node being constructed.
   * @param kid0	Left child of constructed OffsetTree node.
   * @param kid1	Right child of constructed OffsetTree node.
   */
  OffsetTree(Object info, OffsetTree kid0, OffsetTree kid1) {
    super(info);
    addKid(kid0); addKid(kid1);
  }

  /** Ternary OffsetTree constructor.
   * @param info	Information stored in tree node being constructed.
   * @param kid0	Leftmost child of constructed OffsetTree node.
   * @param kid1	Middle child of constructed OffsetTree node.
   * @param kid2	Rightmost child of constructed OffsetTree node.
   */
  OffsetTree(Object info, OffsetTree kid0, OffsetTree kid1, OffsetTree kid2) {
    super(info);
    addKid(kid0); addKid(kid1); addKid(kid2);
  }

  /** General n-ary OffsetTree constructor.
   * @param info	Information stored in tree node being constructed.
   * @param kids	Array of OffsetTrees which will be kids 
   *			of constructed node.
   */
  OffsetTree(Object info, OffsetTree kids[]) {
    super(info);
    for (int i= 0; i < kids.length; i++) {
      addKid(kids[i]);
    }
  }

  /** Add a kid as the last kid for this OffsetTree-node.
   * @param kid		kid to be added to this OffsetTree-node.
   */
  public void addKid(OffsetTree kid) {
    addKid(kid, nKids());
  }

  /** Add a kid after the k'th current kid for this offsetTree-node.  
   * @param kid		The kid to be added.
   * @param k		The existing kid after which the new kid should be
   *			added.  If k is >= the # of existing kids, then the
   *			new kid is added as the last kid.  If k < 0, then
   *			the new kid is added as the first kid.
   */
  public void addKid(OffsetTree kid, int k) {
    offsetToSep();
    super.addKid(kid, k);
    separation(kid, k + 1);
    sepToOffset();
    updateParent();
  }

  public int getOffset() {
    return offset;
  }

  /** Remove kth kid.   No action if this does not have a kid k.
   * @return		kid if it is removed; null if not.
   */
  public Tree rmKid(int k) {
    int n= nKids();
    OffsetTree kid= null;
    if (0 <= k && k < n) {
      offsetToSep();
      OffsetTree right= (OffsetTree)kid(k + 1);
      kid= (OffsetTree)super.rmKid(k);
      kid.offset= 0;
      if (right != null) {
	right.offset= (k == 0) ? 0 : fullSep(right);
      }
      sepToOffset();
      updateParent();
    }
    return kid;
  }

  /** Remove kid.       No action if this does not have kid.
   * @return		kid if it is removed; null if not.
   */
  public Tree rmKid(OffsetTree kid) {
    int k= getKidN(kid);
    return rmKid(k);
  }

  //Instead of calculating this here, we could cache the value for each
  //node within the node (the calculation would be done just once while
  //doing computeContours().
  public int leftWidth() {
    int n= contours[LEFT].length;
    int w= 0;
    for (int i= 0; i < n; i++) {
      if (contours[LEFT][i] < w) w= contours[LEFT][i];
    }
    return -w;
  }

  //Instead of calculating this here, we could cache the value for each
  //node within the node (the calculation would be done just once while
  //doing computeContours().
  public int rightWidth() {
    int n= contours[RIGHT].length;
    int w= 0;
    for (int i= 0; i < n; i++) {
      if (contours[RIGHT][i] > w) w= contours[RIGHT][i];
    }
    return w;
  }

  public boolean getExpand() {
    return isExpand;
  }
  
  public void setExpand(boolean isExpand) {
    this.isExpand= isExpand;
    updateParent();
  }

  private void updateParent() {
    boolean isChanged= computeContours();
    if (isChanged) {
      OffsetTree p= (OffsetTree)parent();
      if (p != null && p.isExpand) {
	int k= p.getKidN(this);
	p.updateKid(this, k);
      }
    }
  }

  private void updateKid(OffsetTree kid, int k) {
    offsetToSep();
    separation(kid, k);
    sepToOffset();
    updateParent();
  }


  private void offsetToSep() {
    if (!isLeaf()) {
      //Convert offsets to separations.
      OffsetTree firstKid= (OffsetTree)kid();
      int off= firstKid.offset;
      firstKid.offset= 0;
      for (OffsetTree t= (OffsetTree)firstKid.nextSib(); t != null; 
	   t= (OffsetTree)(t.nextSib())) {
	int oldOff= t.offset;
	t.offset-= off;
	off= oldOff;
      }
    }
  }

  private void sepToOffset() {
    int nKids= nKids();
    if (nKids > 0 && isExpand) {
      OffsetTree firstKid= (OffsetTree)(kid());
      int off= 0;
      for (OffsetTree kid= (OffsetTree)firstKid.nextSib(); kid != null; 
	   kid= (OffsetTree)kid.nextSib()) {
	kid.offset+= off;
	off= kid.offset;
      }
      int rootOffset= 
	(nKids % 2 == 0) 
        ? (((OffsetTree)kid(nKids/2 - 1)).offset +
	   ((OffsetTree)kid(nKids/2)).offset)/2
        : ((OffsetTree)kid(nKids/2)).offset;
      for (OffsetTree kid= firstKid; kid != null; 
	   kid= (OffsetTree)kid.nextSib()) {
	kid.offset-= rootOffset;
      }
    }
  }

  private final boolean updateContour(int contour[], int index, int value) {
    if (contour[index] != value) {
      contour[index]= value;
      return true;
    }
    else {
      return false;
    }
  }

  /** (Re)compute the contours for the tree rooted at this.
   * @return			true iff the new contours differ from the
   *				previously computed one.
   */
  private boolean computeContours() {
    boolean isChanged= false;
    int nKids1= nKids();
    if (nKids1 > 0) { 
      if (!isExpand) {
	isChanged= contours[LEFT].length > 0;
	if (isChanged) contours[LEFT]= contours[RIGHT]= new int[0];
      }
      else {
	int contourN, startKidN, kidNInc;
	for (contourN= LEFT, startKidN= 0, kidNInc= 1; 
	     contourN <= RIGHT; 
	     contourN++, startKidN= (nKids1 - 1), kidNInc= -1) {
	  int h;
	  if (contourN == LEFT) {
	    //Compute max. ht. of all subtrees.
	    int kidsHMax= 0;
	    for (OffsetTree kid= (OffsetTree)kid(); 
		 kid != null; kid= (OffsetTree)kid.nextSib()) {
	      if (kid.contours[contourN].length > kidsHMax) {
		kidsHMax= kid.contours[contourN].length;
	      }
	    }
	    h= kidsHMax + 1;
	  }
	  else {
	    h= contours[LEFT].length;
	  }
	  if (contours[contourN].length != h) {
	    contours[contourN]= new int[h];
	    isChanged= true;
	  }
	  OffsetTree kid= (OffsetTree)kid(startKidN);
	  isChanged|= updateContour(contours[contourN], 0, kid.offset);
	  for (int i= 1; i < h; i++) {
	    while (i - 1 >= kid.contours[contourN].length) {
	      //Find next kid of sufficient height.	Must exist.
	      kid= (OffsetTree)kid.nextSib(kidNInc);
	    } 
	    isChanged|= updateContour(contours[contourN], i,
				      kid.offset + 
				      kid.contours[contourN][i - 1]);
	  } /* for (int i= 1; i < h; i++) */
	} /* for (contourN= LEFT, ...) */
      } /* if (nKids1 > 0 && isExpand) */
    }
    return isChanged;
  }
	
  /** Calculate minimum separation between kid and its immediate
   * left sibling to prevent any collisions.
   * @param kid		kid whose separation is to be calculated.
   * @return		returns separation.
   */
  private int leftSep(OffsetTree kid) {
    int h= kid.contours[RIGHT].length; //height of kid.
    OffsetTree left= (OffsetTree)kid.nextSib(-1); 
    OffsetTree t= left;	//will iterate thru kid's left siblings.
    int tRootOffset= 0;	//offset of current t from left.
    int sep= MIN_SEP;		//separation needed from kid and left.
    for (int i= 0; i < h; i++) {//for each level of this.
      while (t != null && t.contours[RIGHT].length <= i) {
	tRootOffset-= t.offset; t= (OffsetTree)t.nextSib(-1); 
      }
      if (t == null) break;	//kid higher than anything to its left.
      //t does have a node at level i; compute offsets at level i.
      int tOffset= tRootOffset + t.contours[RIGHT][i];
      int kidOffset= sep + kid.contours[LEFT][i];
      if (kidOffset - tOffset < MIN_SEP) { //move kid to the right.
	sep+= tOffset - kidOffset + MIN_SEP;
      }
    }
    return sep;
  }

  /** Calculate minimum separation between kid and its immediate
   * right sibling to prevent any collisions.
   * @param kid		kid whose separation is to be calculated.
   * @return		returns separation.
   */
  private int rightSep(OffsetTree kid) {
    int h= kid.contours[RIGHT].length; //height of kid.
    OffsetTree right= (OffsetTree)kid.nextSib(); 
    OffsetTree t= right;	//will iterate thru kid's right siblings.
    int tRootOffset= 0;	        //offset of current t from right.
    int sep= MIN_SEP;		//separation needed from kid and right.
  separate:
    for (int i= 0; i < h; i++) {//for each level of this.
      while (t != null && t.contours[LEFT].length <= i) {
	t= (OffsetTree)t.nextSib(); 
	if (t == null) break separate; //kid higher than anything to right.
	tRootOffset+= t.offset; 
      }
      //t does have a node at level i; compute offsets at level i.
      int tOffset= tRootOffset + t.contours[LEFT][i];
      int kidOffset= kid.contours[RIGHT][i] - sep;
      if (tOffset - kidOffset < MIN_SEP) { //move right to the right.
	sep+= kidOffset - tOffset + MIN_SEP;
      }
    }
    return sep;
  }

  /** Calculate minimum separation between kid # k and its immediate
   * siblings to prevent any collisions.  
   * @param kid		kid whose separation is to be updated..
   * @param k		the index of kid among the kids to this.
   */
  private void separation(OffsetTree kid, int k) {
    int n= nKids();
    if (k == 0) {
      kid.offset= 0;
    }
    else if (k > 0) {
      //compute separation between kid and its left sibling.
      kid.offset= leftSep(kid);
    }
    if (k < n - 1) {
      //compute separation between kid's right sibling and kid.
      ((OffsetTree)kid.nextSib()).offset= rightSep(kid);
    }
  }

  /** Calculate minimum separation between kid and its immediate
   * left sibling to prevent any collisions.
   * @param kid		kid whose separation is to be calculated.
   * @return		returns separation.
   *
   * Unlike leftSep(), this ensures no collisions between all right
   * siblings of kid and all left siblings of kid.
   *
   * Probably untested.
   */
  private int fullSep(OffsetTree kid) {
    int sep= MIN_SEP;
    OffsetTree left= (OffsetTree)kid.nextSib(-1);
    OffsetTree tLeft= left;
    int leftRootOffset= 0;
    OffsetTree tRight= kid;
    int rightRootOffset= 0;
  separate:
    for (int i= 0; true; i++) {
      while (tRight != null && tRight.contours[LEFT].length <= i) {
	tRight= (OffsetTree)tRight.nextSib();
	if (tRight == null) break separate;
	rightRootOffset+= tRight.offset;
      }
      while (tLeft != null && tLeft.contours[RIGHT].length <= i) {
	leftRootOffset-= tLeft.offset; tLeft= (OffsetTree)tLeft.nextSib(-1);
      }
      if (tRight == null || tLeft == null) break;
      int rightOffset= sep + rightRootOffset + tRight.contours[LEFT][i];
      int leftOffset= tLeft.contours[RIGHT][i] - leftRootOffset;
      if (rightOffset - leftOffset < MIN_SEP) {
	sep+= leftOffset - rightOffset + MIN_SEP;
      }
    }
    return sep;
  }

  public int height() {
    return contours[LEFT].length;
  }

  public String toString() { 
    StringBuffer b= new StringBuffer();
    b.append(super.toString() + "@" + offset);
    if (debug != 0) {
      for (int i= LEFT; i <= RIGHT; i++) {
	b.append(" [");
	for (int j= 0; j < contours[i].length; j++) {
	  b.append(contours[i][j] + " ");
	}
	b.append("]");
	if (i == LEFT) b.append(" /");
      }
    }
    return b.toString();
  }

  private static final int LEFT= 0;
  private static final int RIGHT= 1;
  private static final int MIN_SEP= 1;
  private static final int debug= 1;
  private int[][] contours= new int[2][0];
  private int[] widths= new int[2];
  private int offset= 0;	//Offset of this node from its parent.
  private boolean isExpand= true;

  /* Simple test program, basically tests addKid() and constructors. */
  static public void main(String args[]) {
    OffsetTree treeX = new OffsetTree("A",
				      new OffsetTree("0"),
				      new OffsetTree("1"),
				      new OffsetTree("2"));
    System.out.println(treeX.treeString());

    OffsetTree t000= new OffsetTree("A");
    OffsetTree t001= new OffsetTree("B");
    OffsetTree t002= new OffsetTree("C");
    OffsetTree t00= new OffsetTree("D", t000, t001, t002);
    t00.addKid(new OffsetTree("E"));
    t00.addKid(new OffsetTree("F", new OffsetTree("G")), 1);
    OffsetTree OffsetTrees[]= { t00, 
		    new OffsetTree("H", new OffsetTree("I"), 
				   new OffsetTree("J")), 
		    new OffsetTree("K"), 
		    new OffsetTree("L")
                  };
    OffsetTree t0= new OffsetTree("M", OffsetTrees);
    System.out.println(t0.treeString());

    if (false) {
    OffsetTree prg= new OffsetTree("program");
    OffsetTree stmts= new OffsetTree("stmts");
    prg.addKid(stmts);
    OffsetTree assgnStmt= new OffsetTree("assgn");
    stmts.addKid(assgnStmt);
    assgnStmt.addKid(new OffsetTree("a"));
    assgnStmt.addKid(new OffsetTree(":="));
    OffsetTree expr= new OffsetTree("expr");
    assgnStmt.addKid(expr);
    OffsetTree term= new OffsetTree("term");
    expr.addKid(term);
    OffsetTree factor= new OffsetTree("factor");
    term.addKid(factor);
    factor.addKid(new OffsetTree("1"));
    OffsetTree stmtsRest= new OffsetTree("stmtsRest");
    stmts.addKid(stmtsRest);
    stmtsRest.addKid(new OffsetTree(";"));
    System.out.println(prg.treeString());

    OffsetTree tStmts= new OffsetTree("stmts");
    OffsetTree tAssgn= new OffsetTree("assgn");
    tStmts.addKid(tAssgn);
    tAssgn.addKid(new OffsetTree("a")); tAssgn.addKid(new OffsetTree(":="));
    OffsetTree tRestStmts= new OffsetTree("restStmts");
    tStmts.addKid(tRestStmts);
    tRestStmts.addKid(new OffsetTree(";"));
    System.out.println(tStmts.treeString());
   

    OffsetTree x= 
      new OffsetTree("stmts",
        new OffsetTree("assgnStmt", 
	  new OffsetTree("a"),
          new OffsetTree(":="),
          new OffsetTree("expr",
            new OffsetTree("term",
              new OffsetTree("factor",
                new OffsetTree("1"))))),
        new OffsetTree(";"),
        new OffsetTree("stmts",
          new OffsetTree("assgnStmt",
            new OffsetTree("b"),
            new OffsetTree(":="),
	    new OffsetTree("expr",
              new OffsetTree("expr",
                new OffsetTree("term",
                  new OffsetTree("factor",
                    new OffsetTree("1")))),
              new OffsetTree("+"),
              new OffsetTree("term",
                new OffsetTree("factor",
                  new OffsetTree("2")))))));
    System.out.println(x.treeString());
    }
  }

}
