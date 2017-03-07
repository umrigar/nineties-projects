/*

File:	 Tree.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:41:00 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

/** General n-ary trees.  Allows accessing nth kid and parent.  Kids 
 *  are referred to by their index (0-origin).  The tree nodes can 
 *  contain any object.  Each leaf node  can have upto one thread
 *  associated with it (thread as in  tree-thread, not concurrent
 *  processing thread).  
 */
class Tree {

  /** Nullary tree constructor. 
   * @param info	Information stored in tree node being constructed.
   */
  Tree(Object info) {
    this.info= info;
    flags= (short)(IS_LEAF|IS_LAST_SIB);
    sibs= kids= null;
  }

  /** Unary tree constructor. 
   * @param info	Information stored in tree node being constructed.
   * @param kid0	Single child of constructed tree node.
   */
  Tree(Object info, Tree kid0) {
    this.info= info;
    flags= (IS_LAST_SIB);
    sibs= null; kids= kid0;
    kid0.flags|= IS_LAST_SIB; kid0.sibs= this;
    this.info= info;
  }

  /** Binary tree constructor. 
   * @param info	Information stored in tree node being constructed.
   * @param kid0	Left child of constructed tree node.
   * @param kid1	Right child of constructed tree node.
   */
  Tree(Object info, Tree kid0, Tree kid1) {
    this.info= info;
    flags= (IS_LAST_SIB);
    sibs= null; kids= kid0;
    kid0.flags&= ~IS_LAST_SIB; kid1.flags|= IS_LAST_SIB;
    kid0.sibs= kid1; kid1.sibs= this;
  }

  /** Ternary tree constructor.
   * @param info	Information stored in tree node being constructed.
   * @param kid0	Leftmost child of constructed tree node.
   * @param kid1	Middle child of constructed tree node.
   * @param kid2	Rightmost child of constructed tree node.
   */
  Tree(Object info, Tree kid0, Tree kid1, Tree kid2) {
    this.info= info;
    flags= (IS_LAST_SIB);
    sibs= null; kids= kid0;
    kid0.flags&= ~IS_LAST_SIB; kid1.flags&= ~IS_LAST_SIB; 
    kid2.flags|= IS_LAST_SIB;
    kid0.sibs= kid1; kid1.sibs= kid2; kid2.sibs= this;
  }

  /** General n-ary tree constructor.
   * @param info	Information stored in tree node being constructed.
   * @param kids	Array of Trees which will be kids of constructed node.
   */
  Tree(Object info, Tree kids[]) {
    this.info= info;
    int n= kids.length;
    sibs= null; 
    this.kids= (n > 0) ? kids[0] : null;
    flags= (short)(IS_LAST_SIB|(n > 0 ? 0 : IS_LEAF));
    for (int i= 0; i < n - 1; i++) {
      kids[i].flags&= ~IS_LAST_SIB; kids[i].sibs= kids[i + 1];
    }
    kids[n - 1].flags|= IS_LAST_SIB; kids[n - 1].sibs= this;
  }

  /** @return	# of kids of this tree-node. */
  public int nKids() {
    int n= 0;
    if ((flags & IS_LEAF) == 0) {
      n= 1;
      for (Tree t= kids; (t.flags & IS_LAST_SIB) == 0; t= t.sibs) n++;
    }
    return n;
  }

  /** Add a kid as the last kid for this tree-node.
   * @param kid		kid to be added to this tree-node.
   */
  public void addKid(Tree kid) {
    addKid(kid, nKids());
  }

  /** Add a kid after the k'th current kid for this tree-node.  
   * @param kid		The kid to be added.
   * @param k		The existing kid after which the new kid should be
   *			added.  If k is >= the # of existing kids, then the
   *			new kid is added as the last kid.  If k < 0, then
   *			the new kid is added as the first kid.
   */
  public void addKid(Tree kid, int k) {
    if ((flags & IS_LEAF) != 0) { //No existing kid. 
      flags&= ~IS_LEAF;
      kids= kid;
      kid.flags|= (IS_LAST_SIB); kid.sibs= this;
    }
    else if (k < 0) { //Add as first kid.
      kid.sibs= kids; 
      kids= kid;
      kid.flags&= ~(IS_LAST_SIB);
    }
    else { //Add after kth existing kid.
      int i;
      Tree t;
      for (i= 0, t= kids; i < k && (t.flags & IS_LAST_SIB) == 0; 
	   i++, t= t.sibs) {
      }
      //Insert after t.
      kid.sibs= t.sibs; 
      kid.flags&= ~IS_LAST_SIB; kid.flags|= (t.flags & IS_LAST_SIB);
      t.sibs= kid; t.flags&= ~IS_LAST_SIB;
    }
  }

  /** Remove specified kid.  No action if kid is not a kid of this.
   * @return		kid if it is removed; null if not.
   */
  public Tree rmKid(Tree kid) {
    Tree ret= null;
    if ((flags & IS_LEAF) == 0) {
      if (kids == kid) { // Kid is first kid. 
	ret= kid;
	if ((kid.flags & IS_LAST_SIB) != 0) {
	  kids= null; flags|= IS_LEAF;
	}
        else {
	  kids= kid.sibs;
        }
      }
      else {
        Tree t;
        Tree t0;	//Lags t.
	for (t0= kids, t= kids.sibs; t != kid && (t.flags & IS_LAST_SIB) == 0; 
	     t0= t, t= t.sibs) {
	}
        if (t == kid) {
          t0.flags|= (t.flags & IS_LAST_SIB);
          t0.sibs= t.sibs;
          ret= kid;
	}
      }
      if (ret != null) {
	ret.flags|= IS_LAST_SIB; ret.sibs= null;
      }
    }
    return ret;
  }

  /** Remove nth kid.  No action if this does not have a kid n.
   * @return		kid if it is removed; null if not.
   */
  public Tree rmKid(int n) {
    Tree ret= null;
    if ((flags & IS_LEAF) == 0) {
      if (n == 0) { // Kid is first kid. 
	ret= kids;
	if ((ret.flags & IS_LAST_SIB) != 0) {
	  kids= null; flags|= IS_LEAF;
	}
        else {
	  kids= ret.sibs;
        }
      }
      else {
	Tree t;
        Tree t0;	//Lags t.
        int i;		//Tracks t.
	
	for (t0= kids, t= t0.sibs, i= 1; 
	     i != n && (t.flags & IS_LAST_SIB) == 0; 
	     t0= t, t= t.sibs, i++) {
	}
        if (i == n) {
          t0.flags|= (t.flags & IS_LAST_SIB);
          t0.sibs= t.sibs;
          ret= t;
	}
      }
      if (ret != null) {
	ret.flags|= IS_LAST_SIB; ret.sibs= null;
      }
    }
    return ret;
  }

  /** @return		parent of this node.  null if this node is root.
   */
  public Tree parent() {
    Tree p= null;
    if (sibs != null) {
      Tree t;
      for (t= this; (t.flags & IS_LAST_SIB) == 0; t= t.sibs) {
      }
      p= t.sibs;
    }
    return p;
  }

  /** @return		first kid of this (null if none). */
  public Tree kid() {
    return ((flags & IS_LEAF) == 0) ? kids : null;
  }
   
  /** Return specified kid.
   * @param k		specifies the kid to be returned (0-origin).
   * @return		specified kid; null if no such kid.
   */
  public Tree kid(int k) {
    Tree kid= null;
    if ((flags & IS_LEAF) == 0) {
      Tree t;
      int i;
      for (t= kids, i= 0; i < k && (t.flags & IS_LAST_SIB) == 0; 
	   i++, t= t.sibs) {
      }
      if (i == k) kid= t;
    }
    return kid;
  }

  /** @return		array of Tree's representing kids of this node. */
  public Tree[] getKids() {
    int n= nKids();
    Tree[] ret= new Tree[n];
    int i;
    Tree t;
    for (t= kids, i= 0; i < n; t= t.sibs, i++) {
      ret[i]= t;
    }
    return ret;
  }

  /** @param kid 	to be looked for in this.
   * @return		# of kid in this (0 is leftmost kid).  -1 if not 
   *                    found.
   */
  public int getKidN(Tree kid) {
    int kidN= -1;
    if ((flags & IS_LEAF) == 0) {
      Tree t;
      for (t= kids, kidN= 0; t != kid && (t.flags & IS_LAST_SIB) == 0; 
	   kidN++, t= t.sibs) {
      }
      if (t != kid) kidN= -1;
    }
    return kidN;
  }

  /** @return		Next youngest sibling; null if none. */
  public Tree nextSib() {
    return ((flags & IS_LAST_SIB) == 0) ? sibs : null;
  }

  /** Return specified sibling.
   * @param n:		1==> return next youngest sibling;
   *			-1==> return next oldest sibling;
   *			anything else==> return null.
   * @return		specified sibling; null if none. 
   */
  public Tree nextSib(int n) {
    if (n == 1) {
      return ((flags & IS_LAST_SIB) == 0) ? sibs : null;
    }
    else if (n == -1) {
      Tree p= parent();
      if (p == null || p.kids == this) {
	return null;
      }
      else {
	Tree t;
	for (t= p.kids; t.sibs != this; t= t.sibs) {
	}
	return t;
      }
    }
    else {
      return null;
    }
  }

  /** @return		true iff this is a leaf node. */
  public boolean isLeaf() {
    return (flags & IS_LEAF) != 0;
  }

  /** Set a thread in a leaf node. 
   *  @param thread	thread to be added
   *  @return		this if ok; null if not (this was not a leaf).
   */
  public Tree setThread(Tree thread) {
    Tree ret= null;
    if ((flags & IS_LEAF) != 0) {
      kids= thread; 
      ret= this;
    }
    return ret;
  }

  /** @return		thread associated with this node; null if none. */
  public Tree getThread() {
    Tree ret= null;
    if ((flags & IS_LEAF) != 0) {
      ret= kids;
    }
    return ret;
  }

  /** @return 		height of this tree (root is height 0). */
  public int height() {
    if (isLeaf()) {
      return 0;
    }
    else {
      int kidMaxHeight= 0;
      for (Tree t= kid(); t != null; t= t.nextSib()) {
	int h= t.height();
	if (h > kidMaxHeight) kidMaxHeight= h;
      }
      return kidMaxHeight + 1;
    }
  }


  /** @return		info associated with this node. */
  public Object getInfo() {
    return info;
  }

  /** Set info associated with this node.
   * @param info	New info to be associated with this node.
   * @return		Old info associated with this node.
   */
  public Object setInfo(Object info) {
    Object ret= info;
    this.info= info;
    return ret;
  }

  /** String representation of this node. 
   * @return		String representation of info in this node.
   */
  public String toString() {
    return (info == null) ? "@Tree@" : info.toString();
  }

  /** String representation of entire tree. 
   * @return 		String representation of entire tree in indented 
   * 			multi-line notation.
   */
  public String treeString() {
    return appendToStringBuffer(new StringBuffer(), 0, INDENT_INC).toString();
  }

  /** String representation of entire tree.
   * @param indent	Starting indent at which tree should be printed.
   * @return 		String representation of entire tree in indented
   * 			multi-line notation.
   */
  public String treeString(int indent) {
    return 
      appendToStringBuffer(new StringBuffer(), indent, INDENT_INC).toString();
  }

  /** String representation of entire tree.
   * @param indent	Starting indent at which tree should be printed.
   * @param indentInc	Increment in indent for printing kids.
   * @return 		String representation of entire tree in indented 
   * 			multi-line notation.
   */
  public String treeString(int indent, int indentInc) {
    return 
      appendToStringBuffer(new StringBuffer(), indent, indentInc).toString();
  }

  /** Appends string representation of this to StringBuffer b.
   * @param b		StringBuffer to which string representation of 
   *			this should be appended.
   * @param indent	Starting indent at which tree should be printed.
   * @param indentInc	Increment in indent for printing kids.
   * @return		updated StringBuffer b.
   * @see		Tree#toString()
   */
  public StringBuffer appendToStringBuffer(StringBuffer b, 
					   int indent, int indentInc) {
    for (int i= 0; i < indent; i++) b.append(' ');
    b.append(toString()); b.append('\n');
    if ((flags & IS_LEAF) == 0) {
      int indent1= indent + indentInc;
      Tree t;
      for (t= kids; (t.flags & IS_LAST_SIB) == 0; t= t.sibs) {
	t.appendToStringBuffer(b, indent1, indentInc);
      }
      t.appendToStringBuffer(b, indent1, indentInc);
    }
    return b;
  }

  private final static short IS_LAST_SIB= 0x1;
  private final static short IS_LEAF= 0x2;
  private final static int INDENT_INC= 2;
  private Object info= null;	//Arbitrary user information.
  private short flags= 0;	//Combination of above flags.
  private Tree sibs= null;	//Next sibling.  Links thru sibs to parent.
  private Tree kids= null;	//Oldest kid or thread pointer (if leaf).


  /* Simple test program, basically tests addKid() and constructors. */
  static public void main(String args[]) {
    {
      Tree t000= new Tree("2");
      Tree t001= new Tree("3");
      Tree t002= new Tree("6");
      Tree t00= new Tree("1", t000, t001, t002);
      t00.addKid(new Tree("7"));
      t00.addKid(new Tree("4", new Tree("5")), 1);
      Tree trees[]= { t00, 
		      new Tree("8", new Tree("9"), 
			       new Tree("10")), 
		      new Tree("11"), 
		      new Tree("12")
      };
      Tree t0= new Tree("0", trees);
      System.out.println(t0.treeString());
    }

    {
      Tree t000= new Tree("A");
      Tree t001= new Tree("B");
      Tree t002= new Tree("C");
      Tree t00= new Tree("D", t000, t001, t002);
      t00.addKid(new Tree("E"));
      t00.addKid(new Tree("F", new Tree("G")), 1);
      System.out.println(t00.treeString());
    }
  }    

}
