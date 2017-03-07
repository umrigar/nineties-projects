/*

File:	 TreeCanvas.java
Author:  zerksis d. umrigar (zdu@acm.org)
Copyright (C) 1997 Zerksis D. Umrigar
Last Update Time-stamp: "97/06/27 20:41:04 umrigar"

This code is distributed under the terms of the GNU General Public License.
See the file COPYING with this distribution, or

		http://www.fsf.org/copyleft/gpl.html

THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

package zdu.parsdemo;

import zdu.parsdemo.OffsetForest;
import zdu.parsdemo.ScrollableCanvas;

import java.applet.*;
import java.awt.*;
import java.io.*;
import java.util.Vector;

class TreeCanvas extends ScrollableCanvas implements Selectable {

  TreeCanvas(int width, int height, String mouseHint) {
    super(width, height, mouseHint == null ? null : "");
    hintMsg= mouseHint; forest= null;
    /* why doesn't this work here?  Putting it in updateScrollbars(). 
    FontMetrics fontMetrics= getFontMetrics(getFont());
    hScroll.setLineIncrement(fontMetrics.stringWidth("M"));
    hScroll.setPageIncrement(COL_SEP); 
    vScroll.setLineIncrement(fontMetrics.getHeight());
    vScroll.setPageIncrement(ROW_SEP); 
    */
  }

  TreeCanvas(int width, int height) {
    this(width, height, null);
  }

  public void setFont(Font f) {
    super.setFont(f); 
    FontMetrics fontMetrics= getFontMetrics(f);
    hScroll.setLineIncrement(fontMetrics.stringWidth("M"));
    vScroll.setLineIncrement(fontMetrics.getHeight());
    repaint();
  }

  public void drawTree(Graphics g, OffsetTree t, int x, int y) {
    ParseNode n= (ParseNode)t.getInfo();
    Color c= n.isClosed() ? CLOSED_COLOR : OPEN_COLOR;
    FontMetrics m= g.getFontMetrics(g.getFont());
    String lab= n.getLabel();
    boolean isSelected= (t == selectedNode);
    int w= m.stringWidth(lab);
    int h= m.getHeight();
    int a= m.getAscent();
    int w1= (w < MIN_NODE_WIDTH) ? MIN_NODE_WIDTH : w;
    int xR= x - (int)((w1/2)*EXTRA_WIDTH);
    int yR= y;
    int wR= (int)(w1*EXTRA_WIDTH);
    int hR= (int)(h*EXTRA_HEIGHT);
    g.setColor(RECT_SHADOW_COLOR);
    g.fillRect(xR + (xR - SOURCE_X)*NODE_Z/(SOURCE_Z - NODE_Z), 
	       yR + (yR - SOURCE_Y)*NODE_Z/(SOURCE_Z - NODE_Z), 
	       wR, hR);
    if (isSelected) {
      g.setColor(SELECT_COLOR);
      g.fillRect(xR - SELECT_WIDTH, yR - SELECT_WIDTH,
		 wR + 2*SELECT_WIDTH, hR + 2*SELECT_WIDTH);
    }
    g.setColor(c);
    g.fillRect(xR, yR, wR, hR);
    g.setColor(TEXT_COLOR);
    g.drawString(lab, x - w/2, y + (int)(a * EXTRA_HEIGHT));
    for (int i= 0; i < t.nKids(); i++) {
      OffsetTree kid= (OffsetTree)t.kid(i);
      ParseNode kidNode= (ParseNode)kid.getInfo();
      Color cKid= kidNode.isClosed() ? CLOSED_COLOR : OPEN_COLOR;
      int eX0= x;
      int eY0= y + (int)(h*EXTRA_HEIGHT);
      int eX1= x + kid.getOffset()*COL_SEP;
      int eY1= y + ROW_SEP;
      int eX0Offset= (eX0 - SOURCE_X)*EDGE_Z/(SOURCE_Z - EDGE_Z);
      int eY0Offset= (eY0 - SOURCE_Y)*EDGE_Z/(SOURCE_Z - EDGE_Z);
      int eX1Offset= (eX1 - SOURCE_X)*EDGE_Z/(SOURCE_Z - EDGE_Z);
      int eY1Offset= (eY1 - SOURCE_Y)*EDGE_Z/(SOURCE_Z - EDGE_Z);
      g.setColor(LINE_SHADOW_COLOR);
      g.drawLine(eX0 + eX0Offset, eY0 + eY0Offset, 
		 eX1 + eX1Offset, eY1 + eY1Offset);
      g.setColor(cKid);
      g.drawLine(eX0, eY0, eX1, eY1);
      g.drawLine(eX0 + 1, eY0, eX1 + 1, eY1);
      drawTree(g, kid, x + kid.getOffset()*COL_SEP, y + ROW_SEP);
    }
  }

  public void reset() {
    rowLo= colLo= 0;
    if (forest != null) forest.removeAllElements();
  }

  public void select(Object xSpec, Object ySpec) {
    selectedNode= (OffsetTree)xSpec;
    didSelect= true; 
  }

  public void paint(Graphics g) {
    Dimension s= size();
    if (forest != null && forest.size() > 0) {
      int n= forest.size();
      int wLeft= forest.leftWidth();
      int wRight= forest.rightWidth();
      int h= forest.height();
      int x= hScroll.getValue();
      int y= vScroll.getValue();
      if (didSelect) {
	x= wLeft;
	y= 0;
	for (OffsetTree t= selectedNode; t != null; 
	     t= (OffsetTree)t.parent()) {
	  y++; x+= t.getOffset();
	}
	x= x > 1 ? (x - 1)*COL_SEP : 0; 
	y= y > 2 ? (y - 2)*ROW_SEP : 0;
	didSelect= false;
      }
      updateScrollbars(x, y, 
		       (wLeft + wRight + 1) * COL_SEP, (h + 1) * ROW_SEP);
      g.translate(-hScroll.getValue(), -vScroll.getValue());
      for (int i= 0; i < n; i++) {
	OffsetTree tree= forest.elementAt(i);
	drawTree(g, tree, (wLeft + tree.getOffset())*COL_SEP + COL_SEP/2, 10);
      }
    }
    else {
      updateScrollbars(0, 0, s.width, s.height);
    }
  }

  void setForest(OffsetForest forest) {
    this.forest= forest;
  }

  private void updateScrollbars(int x, int y, int width, int height) {
    Dimension s= size();
    int xMax= s.width > width ? s.width : width;
    int w1= width - s.width + SLOP;
    int x1= (x <= width) ? x : width;
    hScroll.setValues(x1, s.width, 0, xMax - s.width + SLOP);
    int yMax= s.height > height ? s.height : height;
    int y1= (y <= height) ? y : height;
    vScroll.setValues(y1, s.height, 0, yMax - s.height + SLOP);
    //following lines should really be in constructor. 
    FontMetrics fontMetrics= getFontMetrics(getFont());
    hScroll.setLineIncrement(fontMetrics.stringWidth("M"));
    hScroll.setPageIncrement(COL_SEP); 
    vScroll.setLineIncrement(fontMetrics.getHeight());
    vScroll.setPageIncrement(ROW_SEP); 
  }

  Image getHintImage() {
    FontMetrics m= getFontMetrics(getFont());
    Dimension s= size();
    if (hintMsg != null &&
	(hintBuf == null || hintBufDim.width != s.width || 
	 hintBufDim.height != m.getHeight() + 2*HINT_Y_INSET)) {
      int h= m.getHeight();
      hintBufDim= new Dimension(s.width, h + 2*HINT_Y_INSET);
      hintBuf= createImage(hintBufDim.width, hintBufDim.height);
      Graphics g= hintBuf.getGraphics();
      int xMsg= SLOP;
      int xStacked= xMsg + m.stringWidth(hintMsg) + HINT_SPACE;
      int xUnstacked= xStacked + m.stringWidth(STACKED_HINT) + HINT_SPACE;
      g.setColor(HINT_BG_COLOR);
      g.fillRect(0, 0, s.width, h + 2*HINT_Y_INSET);
      g.setColor(OPEN_COLOR); 
      g.fillRect(xStacked, HINT_Y_INSET, m.stringWidth(STACKED_HINT), h);
      g.setColor(CLOSED_COLOR);
      g.fillRect(xUnstacked, HINT_Y_INSET, m.stringWidth(UNSTACKED_HINT), h);
      g.setColor(HINT_FG_COLOR);
      int a= m.getAscent();
      g.drawString(hintMsg, xMsg, HINT_Y_INSET + a);
      g.drawString(STACKED_HINT, xStacked, HINT_Y_INSET + a); 
      g.drawString(UNSTACKED_HINT, xUnstacked, HINT_Y_INSET + a);
    }
    return hintBuf;
  }

  private Dimension minSize= new Dimension(200, 80);
  private Dimension prefSize= new Dimension(400, 1600);
  private int rowLo= 0;	
  private int colLo= 0;
  private OffsetForest forest;
  private OffsetTree selectedNode;
  private String hintMsg;		//hint msg. to output on top of canvas.
  private Image hintBuf;		//buffer for hint message.
  private Dimension hintBufDim;		//dimensions of allocated hint buf.
  private boolean didSelect;
  private static final String STACKED_HINT= "stacked";
  private static final String UNSTACKED_HINT= "unstacked";
  private static final Color HINT_BG_COLOR= Color.yellow;
  private static final Color HINT_FG_COLOR= Color.black;
  private static final int HINT_SPACE= 20;
  private static final int HINT_Y_INSET= 3;
  private static final int FONTSIZE= 14;
  private static final int ROW_SEP= 50;
  private static final int COL_SEP= 100;
  private static final int MIN_NODE_WIDTH= 20;
  private static final int TREE_SEP= 0;
  private static final double EXTRA_WIDTH= 1.2;
  private static final double EXTRA_HEIGHT= 1.2;
  private static final Color OPEN_COLOR= Color.green;
  private static final Color CLOSED_COLOR= Color.red;
  private static final Color SELECT_COLOR= Color.yellow;
  private static final int SELECT_WIDTH= 3;
  private static final Color TEXT_COLOR= Color.black;

  //Right-hand axes with Z increasing towards viewer.  Canvas is at Z= 0.  
  //Ensure light source is real far away, relative to object
  //distances so that all parts of the object have similar sized shadows.
  //Coordinates of light source.
  private static final int SOURCE_X= -10000;
  private static final int SOURCE_Y= -10000; //must be < 0 to overlay shadows.
  private static final int SOURCE_Z= 10000;
  private static final int NODE_Z= 8;        //Nodes at plane z= NODE_Z
  private static final int EDGE_Z= 7;        //Edges at plane z= EDGE_Z

  private static final int RECT_GRAY= 220;
  private static final int LINE_GRAY= 200;
  private static final Color RECT_SHADOW_COLOR= 
    new Color(RECT_GRAY, RECT_GRAY, RECT_GRAY);
  private static final Color LINE_SHADOW_COLOR= 
    new Color(LINE_GRAY, LINE_GRAY, LINE_GRAY);


  /* Simple test program, basically tests addKid() and constructors. */
  static public void main(String args[]) {
    OffsetForest forest= new OffsetForest();

    OffsetTree t000= new OffsetTree(new ParseNode("A", true));
    OffsetTree t001= new OffsetTree(new ParseNode("B", true));
    OffsetTree t002= new OffsetTree(new ParseNode("C", true));
    OffsetTree t00= new OffsetTree(new ParseNode("D", true), t000, t001, t002);
    t00.addKid(new OffsetTree(new ParseNode("E", true)));
    t00.addKid(new OffsetTree(new ParseNode("F", true), 
			      new OffsetTree(new ParseNode("G", true))), 1);
    OffsetTree OffsetTrees[]= { t00, 
		    new OffsetTree(new ParseNode("H", true), 
				   new OffsetTree(new ParseNode("I", true)), 
				   new OffsetTree(new ParseNode("J", true))), 
		    new OffsetTree(new ParseNode("K", true)), 
		    new OffsetTree(new ParseNode("L", true))
                  };
    OffsetTree t0= new OffsetTree(new ParseNode("M", true), OffsetTrees);
    forest.addElement(t0);

    OffsetTree prg= new OffsetTree(new ParseNode("program", true));
    OffsetTree stmts= new OffsetTree(new ParseNode("stmts", true));
    prg.addKid(stmts);
    OffsetTree assgnStmt= new OffsetTree(new ParseNode("assgn", true));
    stmts.addKid(assgnStmt);
    assgnStmt.addKid(new OffsetTree(new ParseNode("a", true)));
    assgnStmt.addKid(new OffsetTree(new ParseNode(":=", true)));
    OffsetTree expr= new OffsetTree(new ParseNode("expr", true));
    assgnStmt.addKid(expr);
    OffsetTree term= new OffsetTree(new ParseNode("term", true));
    expr.addKid(term);
    OffsetTree factor= new OffsetTree(new ParseNode("factor", true));
    term.addKid(factor);
    factor.addKid(new OffsetTree(new ParseNode("1", true)));
    OffsetTree stmtsRest= new OffsetTree(new ParseNode("stmtsRest", true));
    stmts.addKid(stmtsRest);
    stmtsRest.addKid(new OffsetTree(new ParseNode(";", true)));
    forest.addElement(prg);

    OffsetTree tStmts= new OffsetTree(new ParseNode("stmts", false));
    OffsetTree tAssgn= new OffsetTree(new ParseNode("assgn", false));
    tStmts.addKid(tAssgn);
    tAssgn.addKid(new OffsetTree(new ParseNode("a", false))); 
    tAssgn.addKid(new OffsetTree(new ParseNode(":=", false)));
    OffsetTree tRestStmts= new OffsetTree(new ParseNode("restStmts", false));
    tStmts.addKid(tRestStmts);
    tRestStmts.addKid(new OffsetTree(new ParseNode(";", false)));
    forest.addElement(tStmts); 

    OffsetTree x= 
      new OffsetTree(new ParseNode("stmts", false),
        new OffsetTree(new ParseNode("assgnStmt", false), 
	  new OffsetTree(new ParseNode("a", false)),
          new OffsetTree(new ParseNode(":=", false)),
          new OffsetTree(new ParseNode("expr", false),
            new OffsetTree(new ParseNode("term", false),
              new OffsetTree(new ParseNode("factor", false),
                new OffsetTree(new ParseNode("1", false)))))),
        new OffsetTree(new ParseNode(";", false)),
        new OffsetTree(new ParseNode("stmts", false),
          new OffsetTree(new ParseNode("assgnStmt", false),
            new OffsetTree(new ParseNode("b", false)),
            new OffsetTree(new ParseNode(":=", false)),
	    new OffsetTree(new ParseNode("expr", false),
              new OffsetTree(new ParseNode("expr", false),
                new OffsetTree(new ParseNode("term", false),
                  new OffsetTree(new ParseNode("factor", false),
                    new OffsetTree(new ParseNode("1", false))))),
              new OffsetTree(new ParseNode("+", false)),
              new OffsetTree(new ParseNode("term", false),
                new OffsetTree(new ParseNode("factor", false),
                  new OffsetTree(new ParseNode("2", false))))))));
    forest.addElement(x);

    TreeCanvas treeCanvas= new TreeCanvas(200, 200, "Test tree");
    treeCanvas.setForest(forest);
    Frame frame= new Frame("TreeCanvas Test");
    frame.setLayout(new FlowLayout());
    frame.add(treeCanvas.getComponent());
    frame.resize(400, 400);
    frame.show();
    treeCanvas.repaint();

  }

    
}



